#ifndef COMM_CAN_H
#define COMM_CAN_H

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <QByteArray>

#include "base/comm_adapter.h"

struct CanConfig {
    QString ifname = "can0";
    bool canFd = false;
};


class CommCan : public CommAdapter
{
    Q_OBJECT
public:
    explicit CommCan(CanConfig cfg, QObject* parent = nullptr)
        : CommAdapter(parent), cfg_(std::move(cfg)) {}

    bool open() override;
    void close() override;

    // 约定：data = [4字节can_id(大端)] + payload(0~8字节)
    int64_t wirteBytes(const QByteArray& data) override;
    bool sendFrame(quint32 canId, const QByteArray& payload, bool extended = false, bool rtr = false);

signals:
    void canFrameReceived(quint32 canId, QByteArray payload, bool extended, bool rtr);

private slots:
    void onReadable();

private:
    CanConfig cfg_;
    int s_ = -1;
    QSocketNotifier* rd_ = nullptr;

};

#endif // COMM_CAN_H
