#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <QWidget>
#include "widget/card_widget.h"

#include <QString>
#include <QPixmap>


#define HOME_PAGE_CARD_TEST 1

namespace Ui {
class HomePage;
}

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

    void initUi();

private:
    Ui::HomePage *ui;
    // Group
    QVector<CardWidget*> device_cards;
    QVector<CardWidget*> env_cards;

#ifdef HOME_PAGE_CARD_TEST
    void testAddCardWidget(const QString& title, const QString& value, const QPixmap& icon);
#endif

signals:

};

#endif // HOME_PAGE_H
