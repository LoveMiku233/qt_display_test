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

signals:

};

#endif // HOME_PAGE_H
