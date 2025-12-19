#ifndef COMM_PAGE_H
#define COMM_PAGE_H

#include <QWidget>


// 前置声明
namespace Ui {
class CommPage;
}


class CommPage : public QWidget
{
    Q_OBJECT
public:
    explicit CommPage(QWidget *parent = nullptr);
    ~CommPage();

private:
    Ui::CommPage *ui;

signals:

};

#endif // COMM_PAGE_H
