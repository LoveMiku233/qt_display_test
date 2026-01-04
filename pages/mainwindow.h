#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractAnimation>
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

/**
 * @brief 页面索引枚举
 * 
 * 定义各功能页面的索引值
 */
enum PageIndex {
    PAGE_HOME = 0,      // 主监控页
    PAGE_CTRL,          // 设备控制页
    PAGE_SETTING,       // 参数设置页
    PAGE_GROUP,         // 设备组页
    PAGE_AUTO_CTRL,     // 自动控制页
    PAGE_COMM,          // 通讯状态页
    PAGE_LOG            // 系统日志页
};

/**
 * @brief 主窗口类
 * 
 * 大棚智能控制系统的主窗口，包含导航菜单和内容区域
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MainWindow();

    /**
     * @brief 初始化界面
     */
    void initUi();
    
    /**
     * @brief 切换页面（带动画效果）
     * @param id 目标页面ID
     */
    void setPage(int id);
    
    /**
     * @brief 应用现代化样式
     */
    void applyStyles();

private:
    /**
     * @brief 执行页面切换动画
     * @param from 源页面
     * @param to 目标页面
     * @param newIndex 新页面索引
     */
    void animatePageTransition(QWidget* from, QWidget* to, int newIndex);
    
    /**
     * @brief 更新导航按钮选中状态
     * @param activePageId 当前激活的页面ID
     */
    void updateNavButtonStates(int activePageId);
    
    /**
     * @brief 启动时间更新定时器
     */
    void startTimeUpdater();
    
    Ui::MainWindow *ui;
    PageIndex page_id = PAGE_HOME;

    // RPC客户端
    JsonRpcClient* rpc_;
    
    // 页面切换动画
    QAbstractAnimation* pageTransitionAnim_;

    // 各功能页面指针
    CommPage *comm_page;
    CtrlPage *ctrl_page;
    HomePage *home_page;
    LogPage *log_page;
    SettingPage *setting_page;
    GroupPage *group_page;
    AutoCtrlPage *auto_ctrl_page;
};
#endif // MAINWINDOW_H
