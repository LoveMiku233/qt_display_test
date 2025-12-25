#ifndef CTRL_PAGE_H
#define CTRL_PAGE_H

#include <QWidget>

namespace Ui {
class CtrlPage;
}

class DeviceCardWidget;
class JsonRpcClient;

class CtrlPage : public QWidget
{
    Q_OBJECT
public:
    explicit CtrlPage(QWidget *parent = nullptr);

    ~CtrlPage();

private:
    void loadCards();
    void refreshCards();

    Ui::CtrlPage *ui;
    JsonRpcClient* rpc_ = nullptr;
    QList<DeviceCardWidget*> cards_;

    int refreshIndex_ = 0;
    bool refreshInFlight_ = false;   // 防止上一条没回来又发

};

#endif // CTRL_PAGE_H
