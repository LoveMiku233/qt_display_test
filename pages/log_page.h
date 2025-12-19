#ifndef LOG_PAGE_H
#define LOG_PAGE_H

#include <QWidget>

namespace Ui {
    class LogPage;
}

class LogPage : public QWidget
{
    Q_OBJECT
public:
    explicit LogPage(QWidget *parent = nullptr);
    ~LogPage();

private:
    Ui::LogPage *ui;

signals:

};

#endif // LOG_PAGE_H
