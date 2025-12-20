#include "debug_page.h"
#include "ui_debug_page.h"

DebugPage::DebugPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DebugPage)
{
    ui->setupUi(this);
}


DebugPage::~DebugPage() {
    delete ui;
}
