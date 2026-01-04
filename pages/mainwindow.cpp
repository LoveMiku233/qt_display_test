#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPushButton>
#include <QScreen>
#include <QStyle>

#include "rpc/json_rpc_client.h"
#include "app_context.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // rpc client
    rpc_ = AppContext::instance().rpc();
    rpc_->setEndpoint("127.0.0.1", 12345);
    rpc_->connectToServer();

    initUi();
}

void MainWindow::initUi() {
    comm_page = new CommPage(this);
    ctrl_page = new CtrlPage(this);
    home_page = new HomePage(this);
    log_page = new LogPage(this);
    setting_page = new SettingPage(this);
    group_page = new GroupPage(this);
    auto_ctrl_page = new AutoCtrlPage(this);

    // init stackedwidget
    ui->contentStackedWidget->addWidget(home_page);
    ui->contentStackedWidget->addWidget(ctrl_page);
    ui->contentStackedWidget->addWidget(setting_page);
    ui->contentStackedWidget->addWidget(group_page);
    ui->contentStackedWidget->addWidget(auto_ctrl_page);
    ui->contentStackedWidget->addWidget(comm_page);
    ui->contentStackedWidget->addWidget(log_page);
    ui->contentStackedWidget->setCurrentIndex(PAGE_HOME);

    // bind btn
    connect(ui->btnMain, &QPushButton::clicked, this, [=]{
        setPage(PAGE_HOME);
    });

    connect(ui->btnControl, &QPushButton::clicked, this, [=]{
            setPage(PAGE_CTRL);
        });

    connect(ui->btnParam, &QPushButton::clicked, this, [=]{
        setPage(PAGE_SETTING);
    });

    connect(ui->btnGroup, &QPushButton::clicked, this, [=]{
        setPage(PAGE_GROUP);
    });

    connect(ui->btnAutoCtrl, &QPushButton::clicked, this, [=]{
        setPage(PAGE_AUTO_CTRL);
    });

    connect(ui->btnComm, &QPushButton::clicked, this, [=]{
        setPage(PAGE_COMM);
    });

    connect(ui->btnLog, &QPushButton::clicked, this, [=]{
        setPage(PAGE_LOG);
    });

    QScreen *screen = QGuiApplication::primaryScreen();
    qDebug() << "DPI:" << screen->logicalDotsPerInch()
             << "Device pixel ratio:" << screen->devicePixelRatio()
             << "Size:" << screen->size();

    qDebug() << "Current style:" << QApplication::style()->objectName();
}

void MainWindow::setPage(int id) {
    if (id < 0 || id >= ui->contentStackedWidget->count())
            return;

    ui->contentStackedWidget->setCurrentIndex(id);
    page_id = static_cast<PageIndex>(id);
    // TODO : log add
    qDebug() << "[Debug] MainWindow::setPage = " << id;

}

MainWindow::~MainWindow()
{
    delete ui;
}

