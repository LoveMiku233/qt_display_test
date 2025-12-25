#ifndef DEVICE_DIALOG_H
#define DEVICE_DIALOG_H

#include <QDialog>

class QLabel;
class QComboBox;
class QPushButton;
class QTimer;

class JsonRpcClient; // 你需要有一个客户端（下面说明）

class DeviceDialog : public QDialog
{
    Q_OBJECT
public:
    DeviceDialog(int nodeId, JsonRpcClient* rpc, QWidget* parent=nullptr);

private:
    void refresh();
    void sendControl(const QString& action);

    int nodeId_;
    JsonRpcClient* rpc_;
    QLabel* statusLabel_;
    QComboBox* chBox_;
    QTimer* timer_;

    bool statusInFlight_ = false;
    int lastStatusCh_ = -1;
    qint64 lastStatusSetMs_ = 0;

protected:
    void showEvent(QShowEvent* e) override;
};

#endif // DEVICE_DIALOG_H
