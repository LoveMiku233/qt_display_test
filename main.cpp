/**
 * @file main.cpp
 * @brief 大棚智能控制系统 - 客户端应用入口
 * 
 * 本程序是大棚控制系统的图形界面客户端，
 * 通过RPC与核心服务通讯，实现设备监控和控制功能。
 */

#include "pages/mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

#include "app_context.h"

/**
 * @brief 应用程序入口
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 应用程序退出码
 */
int main(int argc, char *argv[])
{
    // 设置Fusion风格以获得跨平台一致的外观
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);

    // 初始化应用程序上下文（加载配置、创建RPC客户端等）
    AppContext::instance().init(&a);

    // 创建并显示主窗口
    MainWindow w;    
    w.show();
    
    return a.exec();
}
