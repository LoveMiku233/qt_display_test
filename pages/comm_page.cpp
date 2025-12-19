#include "comm_page.h"
#include "ui_comm_page.h"

CommPage::CommPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CommPage)
{
    ui->setupUi(this);
}


CommPage::~CommPage() {
    delete ui;
}
