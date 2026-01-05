#include "comm_page.h"
#include "ui_comm_page.h"
#include "utils/glass_style.h"

CommPage::CommPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CommPage)
{
    ui->setupUi(this);
    
    // 应用玻璃拟态风格样式
    setStyleSheet(GlassStyle::getFullPageStyle());
}


CommPage::~CommPage() {
    delete ui;
}
