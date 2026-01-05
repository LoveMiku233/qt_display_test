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
#include <QTimer>
#include <QTime>
#include <QDateTime>

#include "rpc/json_rpc_client.h"
#include "app_context.h"
#include "utils/animation_utils.h"
#include "utils/glass_style.h"

/**
 * @brief 主窗口构造函数
 * @param parent 父窗口指针
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pageTransitionAnim_(nullptr)
{
    ui->setupUi(this);
    
    // 初始化RPC客户端
    rpc_ = AppContext::instance().rpc();
    
    // 使用配置中的RPC设置
    const auto& config = AppContext::instance().config();
    rpc_->setEndpoint(config.rpc_.serverHost, config.rpc_.serverPort);
    rpc_->connectToServer();

    initUi();
    applyStyles();
    
    // 启动时间更新定时器
    startTimeUpdater();
}

/**
 * @brief 启动时间更新定时器
 */
void MainWindow::startTimeUpdater()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (ui->labTime) {
            ui->labTime->setText(QTime::currentTime().toString("HH:mm:ss"));
        }
    });
    timer->start(1000);  // 每秒更新一次
    
    // 立即更新一次
    if (ui->labTime) {
        ui->labTime->setText(QTime::currentTime().toString("HH:mm:ss"));
    }
}

/**
 * @brief 初始化界面
 * 
 * 创建各页面实例并添加到堆叠窗口，绑定导航按钮事件
 */
void MainWindow::initUi() {
    // 创建各功能页面
    comm_page = new CommPage(this);
    ctrl_page = new CtrlPage(this);
    home_page = new HomePage(this);
    log_page = new LogPage(this);
    setting_page = new SettingPage(this);
    group_page = new GroupPage(this);
    auto_ctrl_page = new AutoCtrlPage(this);

    // 初始化堆叠窗口，添加各页面
    ui->contentStackedWidget->addWidget(home_page);
    ui->contentStackedWidget->addWidget(ctrl_page);
    ui->contentStackedWidget->addWidget(setting_page);
    ui->contentStackedWidget->addWidget(group_page);
    ui->contentStackedWidget->addWidget(auto_ctrl_page);
    ui->contentStackedWidget->addWidget(comm_page);
    ui->contentStackedWidget->addWidget(log_page);
    ui->contentStackedWidget->setCurrentIndex(PAGE_HOME);

    // 绑定导航按钮点击事件
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

    // 输出屏幕信息用于调试
    QScreen *screen = QGuiApplication::primaryScreen();
    qDebug() << "[主窗口] 屏幕DPI:" << screen->logicalDotsPerInch()
             << "设备像素比:" << screen->devicePixelRatio()
             << "分辨率:" << screen->size();

    qDebug() << "[主窗口] 当前样式:" << QApplication::style()->objectName();
}

/**
 * @brief 切换页面
 * @param id 目标页面ID
 */
void MainWindow::setPage(int id) {
    if (id < 0 || id >= ui->contentStackedWidget->count())
        return;
    
    // 如果是同一页面则不处理
    if (id == static_cast<int>(page_id))
        return;

    // 获取当前和目标页面控件
    QWidget* currentWidget = ui->contentStackedWidget->currentWidget();
    QWidget* nextWidget = ui->contentStackedWidget->widget(id);
    
    if (!currentWidget || !nextWidget) {
        ui->contentStackedWidget->setCurrentIndex(id);
        page_id = static_cast<PageIndex>(id);
        return;
    }
    
    // 创建平滑过渡动画
    animatePageTransition(currentWidget, nextWidget, id);
    
    page_id = static_cast<PageIndex>(id);
    qDebug() << "[主窗口] 页面切换至:" << id;
}

/**
 * @brief 执行页面切换动画
 * @param from 源页面控件
 * @param to 目标页面控件
 * @param newIndex 新页面索引
 */
void MainWindow::animatePageTransition(QWidget* from, QWidget* to, int newIndex)
{
    // 停止任何现有动画
    if (pageTransitionAnim_) {
        pageTransitionAnim_->stop();
        pageTransitionAnim_->deleteLater();
        pageTransitionAnim_ = nullptr;
    }
    
    // 确保透明度效果存在
    auto* fromEffect = AnimationUtils::ensureOpacityEffect(from);
    auto* toEffect = AnimationUtils::ensureOpacityEffect(to);
    
    // 设置初始状态
    fromEffect->setOpacity(1.0);
    toEffect->setOpacity(0.0);
    
    // 切换到新页面
    ui->contentStackedWidget->setCurrentIndex(newIndex);
    
    // 创建并行动画组
    auto* group = new QParallelAnimationGroup(this);
    pageTransitionAnim_ = group;
    
    // 新页面淡入动画
    auto* fadeIn = new QPropertyAnimation(toEffect, "opacity");
    fadeIn->setDuration(200);  // 200ms快速切换
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::OutCubic);
    group->addAnimation(fadeIn);
    
    // 连接清理回调
    connect(group, &QAbstractAnimation::finished, this, [this, fromEffect]() {
        // 重置源页面透明度
        if (fromEffect) {
            fromEffect->setOpacity(1.0);
        }
        pageTransitionAnim_ = nullptr;
    });
    
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

/**
 * @brief 应用极简留白风格样式
 * 
 * 使用GlassStyle命名空间中定义的极简留白样式
 */
void MainWindow::applyStyles()
{
    // 设置主窗口背景样式
    setStyleSheet(GlassStyle::MAIN_BACKGROUND);
    
    // 设置顶栏样式
    ui->topBar->setStyleSheet(GlassStyle::TOPBAR_GLASS);
    
    // 设置左侧导航栏样式
    ui->menuBar->setStyleSheet(GlassStyle::SIDEBAR_GLASS);
    
    // 设置内容区样式
    ui->contentStackedWidget->setStyleSheet(GlassStyle::CONTENT_GLASS);
    
    // 应用导航按钮样式
    QList<QPushButton*> navButtons = {
        ui->btnMain, ui->btnControl, ui->btnParam,
        ui->btnGroup, ui->btnAutoCtrl, ui->btnComm, ui->btnLog
    };
    
    for (auto* btn : navButtons) {
        if (btn) {
            btn->setStyleSheet(GlassStyle::NAV_BUTTON_GLASS);
            btn->setCheckable(true);
            btn->setCursor(Qt::PointingHandCursor);
        }
    }
    
    // 设置初始选中状态
    if (ui->btnMain) {
        ui->btnMain->setChecked(true);
    }
    
    qDebug() << "[主窗口] 极简留白风格样式已应用";
}

/**
 * @brief 更新导航按钮选中状态
 * @param activePageId 当前激活的页面ID
 */
void MainWindow::updateNavButtonStates(int activePageId)
{
    // 页面ID到按钮的映射
    QMap<int, QPushButton*> pageButtons = {
        {PAGE_HOME, ui->btnMain},
        {PAGE_CTRL, ui->btnControl},
        {PAGE_SETTING, ui->btnParam},
        {PAGE_GROUP, ui->btnGroup},
        {PAGE_AUTO_CTRL, ui->btnAutoCtrl},
        {PAGE_COMM, ui->btnComm},
        {PAGE_LOG, ui->btnLog}
    };
    
    // 更新所有按钮的选中状态
    for (auto it = pageButtons.begin(); it != pageButtons.end(); ++it) {
        if (it.value()) {
            it.value()->setChecked(it.key() == activePageId);
        }
    }
}

/**
 * @brief 主窗口析构函数
 */
MainWindow::~MainWindow()
{
    delete ui;
}

