#pragma once
#include <QDialog>
#include "ui_ConfigDialog.h"

class ConfigDialog : public QDialog
{
public:
	ConfigDialog(QWidget* parent, HWND hwnd);

	void setOpinion(int n);
	int getOpinion();

private:
	Ui_ConfigDialog ui;

};

