#include "ctrl_page.h"
#include "ui_ctrl_page.h"

CtrlPage::CtrlPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CtrlPage)
{
    ui->setupUi(this);
}


CtrlPage::~CtrlPage() {
    delete ui;
}
