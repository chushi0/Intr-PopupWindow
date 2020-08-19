#include "pch.h"
#include "sqlite3.h"
#include <ShlObj.h>
#include <assert.h>

sqlite3* db;
sqlite3_stmt* insert, * removeStmt, * queryStmt;

inline void prepareStmts() {
	int res;
	// insert
	const wchar_t psql_insert[] = L"insert into intr_config (type, title, class, process) values(?, ?, ?, ?)";
	res = sqlite3_prepare16(db, psql_insert, sizeof(psql_insert), &insert, NULL);
	assert(res == SQLITE_OK);
	// remove
	const wchar_t psql_remove[] = L"delete from intr_config where title=? and class=? and process=?";
	res = sqlite3_prepare16(db, psql_remove, sizeof(psql_remove), &removeStmt, NULL);
	assert(res == SQLITE_OK);
	// query
	const wchar_t psql_query[] = L"select type, title, class, process from intr_config";
	res = sqlite3_prepare16(db, psql_query, sizeof(psql_query), &queryStmt, NULL);
	assert(res == SQLITE_OK);
}

inline void freeStmts() {
	sqlite3_finalize(insert);
	sqlite3_finalize(removeStmt);
	sqlite3_finalize(queryStmt);
}

void openDatabase() {
	WCHAR szPath[MAX_PATH];
	SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath);
	int res;
	res = sqlite3_open16((std::wstring(szPath) + L"\\intr-popupwindow.db").c_str(), &db);
	assert(res == SQLITE_OK);
	res = sqlite3_exec(db, "create table if not exists intr_config(type, title, class, process, unique(title, class, process) on conflict replace)", NULL, NULL, NULL);
	assert(res == SQLITE_OK);

	prepareStmts();
}

void closeDatabase() {
	freeStmts();
	sqlite3_close(db);
}

#define WSTRING_BYTES(wstr) (((wstr).length() + 1) * sizeof(wchar_t))

bool insertNewConfig(LpIntrConfig config) {
	sqlite3_reset(insert);
	sqlite3_bind_int(insert, 1, config->type);
	sqlite3_bind_text16(insert, 2, config->windowTitle.c_str(), WSTRING_BYTES(config->windowTitle), NULL);
	sqlite3_bind_text16(insert, 3, config->windowClass.c_str(), WSTRING_BYTES(config->windowClass), NULL);
	sqlite3_bind_text16(insert, 4, config->process.c_str(), WSTRING_BYTES(config->process), NULL);
	int res = sqlite3_step(insert);
	return res == SQLITE_DONE;
}

bool removeConfig(LpIntrConfig config) {
	sqlite3_reset(removeStmt);
	sqlite3_bind_text16(removeStmt, 1, config->windowTitle.c_str(), WSTRING_BYTES(config->windowTitle), NULL);
	sqlite3_bind_text16(removeStmt, 2, config->windowClass.c_str(), WSTRING_BYTES(config->windowClass), NULL);
	sqlite3_bind_text16(removeStmt, 3, config->process.c_str(), WSTRING_BYTES(config->process), NULL);
	int res = sqlite3_step(removeStmt);
	return res == SQLITE_DONE;
}

std::vector<IntrConfig> readAllConfig() {
	std::vector<IntrConfig> result;

	sqlite3_reset(queryStmt);
	while ((sqlite3_step(queryStmt) == SQLITE_ROW)) {
		IntrConfig config;
		config.type = sqlite3_column_int(queryStmt, 0);
		config.windowTitle = (LPCWSTR)sqlite3_column_text16(queryStmt, 1);
		config.windowClass = (LPCWSTR)sqlite3_column_text16(queryStmt, 2);
		config.process = (LPCWSTR)sqlite3_column_text16(queryStmt, 3);
		result.push_back(config);
	}

	return result;
}