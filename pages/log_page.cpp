#include "log_page.h"
#include "ui_log_page.h"

LogPage::LogPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogPage)
{
    ui->setupUi(this);
}

LogPage::~LogPage() {
    delete ui;
}
