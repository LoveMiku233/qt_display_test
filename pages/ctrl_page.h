#ifndef CTRL_PAGE_H
#define CTRL_PAGE_H

#include <QWidget>

namespace Ui {
class CtrlPage;
}


class CtrlPage : public QWidget
{
    Q_OBJECT
public:
    explicit CtrlPage(QWidget *parent = nullptr);
    ~CtrlPage();

private:
    Ui::CtrlPage *ui;

signals:

};

#endif // CTRL_PAGE_H
