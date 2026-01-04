#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPushButton>
#include <QScreen>
#include <QStyle>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QMap>

#include "rpc/json_rpc_client.h"
#include "app_context.h"
#include "utils/animation_utils.h"

// Modern color scheme constants
namespace AppStyle {
    const QString PRIMARY_COLOR = "#3498db";
    const QString PRIMARY_DARK = "#2980b9";
    const QString ACCENT_COLOR = "#2ecc71";
    const QString BACKGROUND_DARK = "#2c3e50";
    const QString BACKGROUND_LIGHT = "#34495e";
    const QString TEXT_PRIMARY = "#ecf0f1";
    const QString TEXT_SECONDARY = "#bdc3c7";
    const QString BORDER_COLOR = "#465c6e";
    
    const QString BUTTON_STYLE = R"(
        QPushButton {
            background-color: %1;
            color: %2;
            border: none;
            border-radius: 8px;
            padding: 12px 20px;
            font-size: 14px;
            font-weight: 500;
            min-width: 100px;
        }
        QPushButton:hover {
            background-color: %3;
        }
        QPushButton:pressed {
            background-color: %4;
        }
        QPushButton:checked {
            background-color: %5;
            border: 2px solid %6;
        }
    )";
    
    const QString SIDEBAR_BUTTON_STYLE = R"(
        QPushButton {
            background-color: transparent;
            color: %1;
            border: none;
            border-radius: 6px;
            padding: 10px 16px;
            font-size: 13px;
            text-align: left;
        }
        QPushButton:hover {
            background-color: %2;
        }
        QPushButton:checked {
            background-color: %3;
            color: %4;
            font-weight: 600;
        }
    )";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pageTransitionAnim_(nullptr)
{
    ui->setupUi(this);
    // rpc client
    rpc_ = AppContext::instance().rpc();
    rpc_->setEndpoint("127.0.0.1", 12345);
    rpc_->connectToServer();

    initUi();
    applyStyles();
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

    // bind btn with button group behavior
    connect(ui->btnMain, &QPushButton::clicked, this, [this]{
        updateNavButtonStates(PAGE_HOME);
        setPage(PAGE_HOME);
    });

    connect(ui->btnControl, &QPushButton::clicked, this, [this]{
        updateNavButtonStates(PAGE_CTRL);
        setPage(PAGE_CTRL);
    });

    connect(ui->btnParam, &QPushButton::clicked, this, [this]{
        updateNavButtonStates(PAGE_SETTING);
        setPage(PAGE_SETTING);
    });

    connect(ui->btnGroup, &QPushButton::clicked, this, [this]{
        updateNavButtonStates(PAGE_GROUP);
        setPage(PAGE_GROUP);
    });

    connect(ui->btnAutoCtrl, &QPushButton::clicked, this, [this]{
        updateNavButtonStates(PAGE_AUTO_CTRL);
        setPage(PAGE_AUTO_CTRL);
    });

    connect(ui->btnComm, &QPushButton::clicked, this, [this]{
        updateNavButtonStates(PAGE_COMM);
        setPage(PAGE_COMM);
    });

    connect(ui->btnLog, &QPushButton::clicked, this, [this]{
        updateNavButtonStates(PAGE_LOG);
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
    
    // Don't animate if same page
    if (id == static_cast<int>(page_id))
        return;

    // Get current and next widgets
    QWidget* currentWidget = ui->contentStackedWidget->currentWidget();
    QWidget* nextWidget = ui->contentStackedWidget->widget(id);
    
    if (!currentWidget || !nextWidget) {
        ui->contentStackedWidget->setCurrentIndex(id);
        page_id = static_cast<PageIndex>(id);
        return;
    }
    
    // Create smooth transition animation
    animatePageTransition(currentWidget, nextWidget, id);
    
    page_id = static_cast<PageIndex>(id);
    qDebug() << "[MainWindow] Page changed to:" << id;
}

void MainWindow::animatePageTransition(QWidget* from, QWidget* to, int newIndex)
{
    // Stop any existing animation
    if (pageTransitionAnim_) {
        pageTransitionAnim_->stop();
        pageTransitionAnim_->deleteLater();
        pageTransitionAnim_ = nullptr;
    }
    
    // Ensure opacity effects
    auto* fromEffect = AnimationUtils::ensureOpacityEffect(from);
    auto* toEffect = AnimationUtils::ensureOpacityEffect(to);
    
    // Set initial states
    fromEffect->setOpacity(1.0);
    toEffect->setOpacity(0.0);
    
    // Switch to new page
    ui->contentStackedWidget->setCurrentIndex(newIndex);
    
    // Create parallel animation group
    auto* group = new QParallelAnimationGroup(this);
    pageTransitionAnim_ = group;
    
    // Fade in new page
    auto* fadeIn = new QPropertyAnimation(toEffect, "opacity");
    fadeIn->setDuration(250);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::OutCubic);
    group->addAnimation(fadeIn);
    
    // Connect cleanup
    connect(group, &QAbstractAnimation::finished, this, [this, fromEffect]() {
        // Reset from widget opacity
        if (fromEffect) {
            fromEffect->setOpacity(1.0);
        }
        pageTransitionAnim_ = nullptr;
    });
    
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::applyStyles()
{
    // Apply modern styling to main window
    setStyleSheet(QString(R"(
        QMainWindow {
            background-color: %1;
        }
        QStackedWidget {
            background-color: %2;
            border-radius: 8px;
        }
        QLabel {
            color: %3;
        }
    )").arg(AppStyle::BACKGROUND_DARK)
       .arg(AppStyle::BACKGROUND_LIGHT)
       .arg(AppStyle::TEXT_PRIMARY));
    
    // Style sidebar navigation buttons
    QString sidebarStyle = AppStyle::SIDEBAR_BUTTON_STYLE
        .arg(AppStyle::TEXT_SECONDARY)     // normal text
        .arg(AppStyle::BACKGROUND_LIGHT)    // hover bg
        .arg(AppStyle::PRIMARY_COLOR)       // checked bg
        .arg(AppStyle::TEXT_PRIMARY);       // checked text
    
    // Apply to all navigation buttons
    QList<QPushButton*> navButtons = {
        ui->btnMain, ui->btnControl, ui->btnParam,
        ui->btnGroup, ui->btnAutoCtrl, ui->btnComm, ui->btnLog
    };
    
    for (auto* btn : navButtons) {
        if (btn) {
            btn->setStyleSheet(sidebarStyle);
            btn->setCheckable(true);
            btn->setCursor(Qt::PointingHandCursor);
        }
    }
    
    // Set initial checked state
    if (ui->btnMain) {
        ui->btnMain->setChecked(true);
    }
    
    qDebug() << "[MainWindow] Modern styles applied";
}

void MainWindow::updateNavButtonStates(int activePageId)
{
    // Map page IDs to buttons
    QMap<int, QPushButton*> pageButtons = {
        {PAGE_HOME, ui->btnMain},
        {PAGE_CTRL, ui->btnControl},
        {PAGE_SETTING, ui->btnParam},
        {PAGE_GROUP, ui->btnGroup},
        {PAGE_AUTO_CTRL, ui->btnAutoCtrl},
        {PAGE_COMM, ui->btnComm},
        {PAGE_LOG, ui->btnLog}
    };
    
    // Update all button checked states
    for (auto it = pageButtons.begin(); it != pageButtons.end(); ++it) {
        if (it.value()) {
            it.value()->setChecked(it.key() == activePageId);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

