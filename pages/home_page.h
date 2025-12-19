#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <QWidget>

namespace Ui {
class HomePage;
}

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

private:
    Ui::HomePage *ui;

signals:

};

#endif // HOME_PAGE_H
