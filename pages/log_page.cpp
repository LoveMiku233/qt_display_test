#include "log_page.h"
#include "ui_log_page.h"
#include "utils/glass_style.h"

LogPage::LogPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogPage)
{
    ui->setupUi(this);
    
    // 应用极简留白风格样式
    setStyleSheet(GlassStyle::getFullPageStyle());
}

LogPage::~LogPage() {
    delete ui;
}
