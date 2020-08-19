#include <QSettings>
#include <QDir>
#include "MainWindow.h"
#include "ConfigDialog.h"
#include <tlhelp32.h>
#include "../dbms/dbms.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);
	QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	if (settings.contains(QString::fromWCharArray(L"intr_popupwindow"))) {
		ui.bootCheckBox->setChecked(true);
	}
	connect(ui.updateButton, &QPushButton::clicked, this, &MainWindow::updateWindowTreeWidget);
	connect(ui.windowTreeWidget, &QTreeWidget::doubleClicked, this, &MainWindow::configWindow);
	connect(ui.bootCheckBox, &QCheckBox::toggled, this, &MainWindow::modifyBootReg);
	updateWindowTreeWidget();
}

void MainWindow::updateWindowTreeWidget() {
	windowList.clear();
	// 正在显示的窗口
	EnumWindows(MainWindow::enumWindowCallback, (LPARAM)&windowList);
	// 已经拦截的窗口
	auto intrWindows = readAllConfig();
	for (auto& config : intrWindows) {
		Window window;
		window.hwnd = NULL;
		window.interruptOpinion = config.type;
		window.windowTitle = config.windowTitle;
		window.windowClass = config.windowClass;
		window.processName = config.process;
		window.processId = 0;
		window.isShown = false;
		windowList.append(window);
	}
	// 排序
	qSort(windowList.begin(), windowList.end(), [](Window& w1, Window& w2) -> bool {
		if (w1.isShown != w2.isShown) return w1.isShown;
		if (w1.interruptOpinion ^ w2.interruptOpinion) return w1.isShown || w1.interruptOpinion;
		if (w1.processId != w2.processId) return w1.processId < w2.processId;
		if (w1.windowClass != w2.windowClass) return w1.windowClass.compare(w2.windowClass) < 0;
		return w1.windowTitle.compare(w2.windowTitle) < 0;
		}
	);
	ui.windowTreeWidget->clear();
	for (int i = 0; i < windowList.size(); i++) {
		auto window = windowList[i];
		QTreeWidgetItem* item = new QTreeWidgetItem(ui.windowTreeWidget);
		item->setText(0, QString::fromStdWString(window.windowTitle));
		item->setText(1, QString::fromStdWString(window.windowClass));
		item->setText(2, QString::fromStdWString(window.processName));
		item->setText(3, QString("%1").arg(window.processId));
		item->setText(4, QString::fromWCharArray(window.interruptOpinion ? L"已拦截" : (window.isShown ? L"正在显示" : L"已隐藏")));
		ui.windowTreeWidget->insertTopLevelItem(i, item);
	}
	return;
}

void MainWindow::configWindow(const QModelIndex& index) {
	int row = index.row();
	auto window = windowList[row];
	ConfigDialog dialog(this, window.hwnd);
	if (window.processId == 0) {
		dialog.setOpinion(window.interruptOpinion);
	}
	int result = dialog.exec();
	if (result == QDialog::Accepted) {
		int opinion = dialog.getOpinion();
		IntrConfig config;
		config.type = opinion;
		config.windowTitle = window.windowTitle;
		config.windowClass = window.windowClass;
		config.process = window.processName;
		if (opinion != 0) {
			insertNewConfig(&config);
		} else if (window.processId == 0) {
			removeConfig(&config);
		}
	}
}

void MainWindow::modifyBootReg(bool checked) {
	if (checked) {
		int ret = MessageBoxW((HWND) winId(), L"系统完成引导后，窗口拦截程序将立即启动。若您设置拦截了系统窗口，可能会导致电脑无法使用。此时，请进入系统安全模式或其他管理员账户，删除当前用户 %AppData% 文件夹下的 intr-popupwindow.db 文件，即可恢复正常。\n\n如果您已经了解并接受以上风险，请点击“是”。否则，请点击“否”。", L"警告：开机自启", MB_YESNO | MB_ICONINFORMATION);
		if (ret == IDNO) {
			ui.bootCheckBox->setChecked(false);
			return;
		}
	}
	QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	if (checked) {
		settings.setValue(QString::fromWCharArray(L"intr_popupwindow"), QString::fromStdWString(L"\"" + QDir::currentPath().replace(QString::fromWCharArray(L"/"), QString::fromWCharArray(L"\\")).toStdWString() + L"\\拦截弹窗.exe\" /b"));
	} else {
		settings.remove(QString::fromWCharArray(L"intr_popupwindow"));
	}
}

BOOL MainWindow::enumWindowCallback(HWND hwnd, LPARAM lParam) {
	Window window;
	window.hwnd = hwnd;
	window.interruptOpinion = 0;
	WCHAR szBuf[512];
	DWORD processId;
	// 窗口标题
	GetWindowTextW(hwnd, szBuf, 512);
	window.windowTitle = szBuf;
	// 窗口类
	GetClassNameW(hwnd, szBuf, 512);
	window.windowClass = szBuf;
	// pid
	GetWindowThreadProcessId(hwnd, &processId);
	window.processId = processId;
	// 进程名
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processId);
	PROCESSENTRY32W pe = { sizeof(pe) };
	BOOL ret = Process32FirstW(hSnapshot, &pe);
	while (ret) {
		if (pe.th32ProcessID == processId) {
			CloseHandle(hSnapshot);
			window.processName = pe.szExeFile;
			break;
		}
		ret = Process32NextW(hSnapshot, &pe);
	}
	// 是否可见
	window.isShown = IsWindowVisible(hwnd);

	auto windowList = (QList<Window>*)lParam;
	windowList->append(window);
	return TRUE;
}