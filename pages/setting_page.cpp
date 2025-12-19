#include "setting_page.h"
#include "ui_setting_page.h"

SettingPage::SettingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingPage)
{
    ui->setupUi(this);
}


SettingPage::~SettingPage() {
    delete ui;
}
