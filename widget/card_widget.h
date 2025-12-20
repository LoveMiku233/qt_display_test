#ifndef CARD_WIDGET_H
#define CARD_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

class CardWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CardWidget(QWidget* parent = nullptr);

    void setTitle(const QString& title);
    void setValue(const QString& value);
    void setIcon(const QPixmap& icon);

private:
    QLabel* lblIcon;
    QLabel* lblTitle;
    QLabel* lblValue;
};

#endif // CARD_WIDGET_H
