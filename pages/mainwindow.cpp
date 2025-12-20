#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPushButton>
#include <QScreen>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // ui->setupUi(this);
    // initUi();
}

void MainWindow::initUi() {
    comm_page = new CommPage(this);
    ctrl_page = new CtrlPage(this);
    home_page = new HomePage(this);
    log_page = new LogPage(this);
    setting_page = new SettingPage(this);

    // init stackedwidget
    ui->stackedWidget->addWidget(home_page);
    ui->stackedWidget->addWidget(ctrl_page);
    ui->stackedWidget->addWidget(setting_page);
    ui->stackedWidget->addWidget(comm_page);
    ui->stackedWidget->addWidget(log_page);
    ui->stackedWidget->setCurrentIndex(PAGE_HOME);

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
    if (id < 0 || id >= ui->stackedWidget->count())
            return;

    ui->stackedWidget->setCurrentIndex(id);
    page_id = static_cast<PageIndex>(id);
    // TODO : log add
    qDebug() << "[Debug] MainWindow::setPage = " << id;

}

MainWindow::~MainWindow()
{
    delete ui;
}

