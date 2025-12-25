#include "home_page.h"
#include "ui_home_page.h"

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    initUi();
}

void HomePage::initUi()
{


}


HomePage::~HomePage()
{
    delete ui;
}
