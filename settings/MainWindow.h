#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <string>
#include <Windows.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);


private slots:
    void updateWindowTreeWidget();
    void configWindow(const QModelIndex& index);
    void modifyBootReg(bool checked);

private:
    struct Window {
        HWND hwnd;
        std::wstring windowTitle;
        std::wstring windowClass;
        std::wstring processName;
        DWORD processId;
        bool isShown;
        int interruptOpinion;
    };

private:
    Ui::MainWindowClass ui;
    QList<Window> windowList;

private:
    static BOOL enumWindowCallback(HWND hWnd, LPARAM lParam);
};
