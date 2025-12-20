#include "card_widget.h"

CardWidget::CardWidget(QWidget *parent)
    : QFrame(parent)
{

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setStyleSheet(
        "QFrame {"
        "border-radius: 10px;"
        "background-color: #ffffff;"
        "border: 1px solid #cccccc;"
        "}"
    );

    lblIcon = new QLabel(this);
    lblTitle = new QLabel("Title", this);
    lblValue = new QLabel("0", this);

    lblTitle->setAlignment(Qt::AlignCenter);
    lblValue->setAlignment(Qt::AlignCenter);

    lblTitle->setAlignment(Qt::AlignCenter);
    lblValue->setAlignment(Qt::AlignCenter);

    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    lblTitle->setFont(titleFont);

    QFont valueFont;
    valueFont.setPointSize(16);
    valueFont.setBold(true);
    lblValue->setFont(valueFont);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(lblIcon, 0, Qt::AlignCenter);
    layout->addWidget(lblTitle);
    layout->addWidget(lblValue);
    setLayout(layout);
}

void CardWidget::setTitle(const QString& title)
{
    lblTitle->setText(title);
}

void CardWidget::setValue(const QString& value)
{
    lblValue->setText(value);
}

void CardWidget::setIcon(const QPixmap& icon)
{
    lblIcon->setPixmap(icon.scaled(40,40,Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
