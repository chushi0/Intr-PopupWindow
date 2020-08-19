#include "ConfigDialog.h"
#include <qscreen.h>
#include <Windows.h>

ConfigDialog::ConfigDialog(QWidget* parent, HWND hwnd) :QDialog(parent) {
	ui.setupUi(this);

	if (hwnd) {
		auto screen = QGuiApplication::primaryScreen();
		auto pixel = screen->grabWindow((WId)hwnd);
		QGraphicsScene* scene = new QGraphicsScene(ui.graphicsView);
		scene->addPixmap(pixel);
		ui.graphicsView->setScene(scene);

		connect(ui.hideButton, &QPushButton::clicked, [this, hwnd] {
			ShowWindow(hwnd, SW_HIDE);
			});
		connect(ui.showButton, &QPushButton::clicked, [this, hwnd] {
			ShowWindow(hwnd, SW_SHOW);
			});
		connect(ui.switchButton, &QPushButton::clicked, [this, hwnd] {
			SwitchToThisWindow(hwnd, TRUE);
			});
	}
}

void ConfigDialog::setOpinion(int n) {
	QRadioButton* intrOpinion[] = {
		ui.intr_no,
		ui.intr_title,
		ui.intr_class,
		ui.intr_title_all,
		ui.intr_class_all,
		ui.intr_exe
	};
	intrOpinion[n]->setChecked(true);
}

int ConfigDialog::getOpinion() {
	QRadioButton* intrOpinion[] = {
		ui.intr_no,
		ui.intr_title,
		ui.intr_class,
		ui.intr_title_all,
		ui.intr_class_all,
		ui.intr_exe
	};
	for (int i = 0; i < sizeof(intrOpinion) / sizeof(QRadioButton*); i++) {
		if (intrOpinion[i]->isChecked()) return i;
	}
	return 0;
}