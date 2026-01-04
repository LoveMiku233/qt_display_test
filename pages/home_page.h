#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <QWidget>
#include "widget/card_widget.h"

#include <QString>
#include <QPixmap>


#define HOME_PAGE_CARD_TEST 1

namespace Ui {
class HomePage;
}

/**
 * @brief 主监控页面类
 * 
 * 显示大棚系统的概览信息，包括环境数据、设备状态等
 */
class HomePage : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit HomePage(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~HomePage();

    /**
     * @brief 初始化界面
     */
    void initUi();

private:
    Ui::HomePage *ui;

signals:

};

#endif // HOME_PAGE_H
