#include "home_page.h"
#include "ui_home_page.h"

/**
 * @brief 主监控页面构造函数
 * @param parent 父窗口指针
 */
HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    initUi();
}

/**
 * @brief 初始化界面
 * 
 * 设置主监控页面的布局和样式
 */
void HomePage::initUi()
{
    // 设置页面背景样式
    setStyleSheet(R"(
        QWidget {
            background-color: #0f3460;
            color: #e8e8e8;
        }
    )");
}


/**
 * @brief 主监控页面析构函数
 */
HomePage::~HomePage()
{
    delete ui;
}
