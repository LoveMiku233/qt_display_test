#include "home_page.h"
#include "ui_home_page.h"
#include "utils/glass_style.h"

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
 * 设置主监控页面的极简留白风格布局和样式
 */
void HomePage::initUi()
{
    // 设置页面极简留白风格样式
    setStyleSheet(GlassStyle::getFullPageStyle());
}


/**
 * @brief 主监控页面析构函数
 */
HomePage::~HomePage()
{
    delete ui;
}
