#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pages/comm_page.h"
#include "pages/ctrl_page.h"
#include "pages/home_page.h"
#include "pages/log_page.h"
#include "pages/setting_page.h"
#include "pages/group_page.h"
#include "pages/auto_ctrl_page.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// page index
enum PageIndex {
    PAGE_HOME = 0,
    PAGE_CTRL,
    PAGE_SETTING,
    PAGE_GROUP,
    PAGE_AUTO_CTRL,
    PAGE_COMM,
    PAGE_LOG
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    void initUi();
    // set page
    void setPage(int id);

private:
    Ui::MainWindow *ui;
    PageIndex page_id = PAGE_HOME;

    // rpc
    JsonRpcClient* rpc_;

    // widgets
    CommPage *comm_page;
    CtrlPage *ctrl_page;
    HomePage *home_page;
    LogPage *log_page;
    SettingPage *setting_page;
    GroupPage *group_page;
    AutoCtrlPage *auto_ctrl_page;
};
#endif // MAINWINDOW_H
