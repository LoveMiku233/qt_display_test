#include "home_page.h"
#include "ui_home_page.h"

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    initUi();
}

void HomePage::initUi() {
    if (!ui->scroll->layout()) {
            auto *layout = new QVBoxLayout(ui->scroll);
            layout->setContentsMargins(10, 10, 10, 10);
            layout->setSpacing(10);
            ui->scrollArea->widget()->setLayout(layout);
        }
        ui->scrollArea->setWidgetResizable(true);
#ifdef HOME_PAGE_CARD_TEST
    connect(ui->pushButton, &QPushButton::clicked, this, [=]{
        testAddCardWidget("test", "9999", QPixmap(":/resources/temp.png"));
    });
#endif
}

#ifdef HOME_PAGE_CARD_TEST

void HomePage::testAddCardWidget(const QString& title, const QString& value, const QPixmap& icon) {
    CardWidget* card = new CardWidget();
    card->setTitle(title);
    card->setValue(value);
    card->setIcon(icon);
    ui->scrollArea->widget()->layout()->addWidget(card);
    // ui->device_card_scroll->addWidget(card);
    // ui->device_card_scroll->addWidget(card);
}

#endif


HomePage::~HomePage() {
    delete ui;
}
