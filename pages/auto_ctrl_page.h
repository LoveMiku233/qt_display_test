#ifndef AUTO_CTRL_PAGE_H
#define AUTO_CTRL_PAGE_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class AutoCtrlPage;
}

class AutoCtrlPage : public QWidget
{
    Q_OBJECT
public:
    explicit AutoCtrlPage(QWidget *parent = nullptr);
    ~AutoCtrlPage();

private slots:
    void onStartAuto();
    void onStopAuto();
    void onAutoTick();

private:
    Ui::AutoCtrlPage *ui;
    QTimer* autoTimer;
    bool autoRunning = false;

    void updateStatus();
};

#endif // AUTO_CTRL_PAGE_H
