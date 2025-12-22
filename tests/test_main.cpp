#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

#include "comm/comm_serial.h"
#include "comm/comm_can.h"

// 用法：
// 1) 串口 + CAN 一起测：
//    ./test_comm_both <dev> <baud> [rs485] [canif]
//
// 示例：
//    ./test_comm_both /dev/ttyS0 115200 can0
//    ./test_comm_both /dev/ttyS2 9600 rs485 can0
//    ./test_comm_both /dev/ttyUSB1 115200 can0
//
// 说明：
// - 程序启动后会 open 串口、open CAN（如果提供 canif）
// - 串口：每 1 秒发送一次 "ping <ms>\r\n"，收到数据打印 hex + ascii
// - CAN：每 1 秒发送一次 id=0x123 data=11 22 33 44 55 66 77 88，收到帧打印
// - Ctrl+C 结束
//
// 注意：CAN 的 bitrate/up/down 推荐在程序外配置：
//   sudo ip link set can0 down
//   sudo ip link set can0 up type can bitrate 500000
//   candump can0

static QString fmtCanId(quint32 id, bool extended)
{
    return extended ? QString("0x%1").arg(id, 8, 16, QLatin1Char('0')).toUpper()
                    : QString("0x%1").arg(id, 3, 16, QLatin1Char('0')).toUpper();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    const QStringList args = app.arguments();
    if (args.size() < 3) {
        qWarning().noquote()
            << "Usage:\n"
            << "  " << args.value(0) << " <dev> <baud> [rs485] [canif]\n"
            << "Examples:\n"
            << "  " << args.value(0) << " /dev/ttyS0 115200 can0\n"
            << "  " << args.value(0) << " /dev/ttyS2 9600 rs485 can0\n"
            << "  " << args.value(0) << " /dev/ttyUSB1 115200 can0\n";
        return 2;
    }

    // -------- Serial --------
    SerialConfig scfg;
    scfg.dev = args.at(1);
    scfg.baud = args.at(2).toInt();

    // 参数规则：
    // - 第3个参数如果是 rs485 => 开 rs485
    // - 其余非 rs485 的参数，如果存在，当作 canif（例如 can0）
    QString canIfName;

    for (int i = 3; i < args.size(); ++i) {
        const QString a = args.at(i).toLower();
        if (a == "rs485") {
            scfg.rs485 = true;
            scfg.rs485DelayBeforeUs = 0;
            scfg.rs485DelayAfterUs = 0;
        } else {
            canIfName = args.at(i); // 比如 "can0"
        }
    }

    auto serial = new CommSerial(scfg, &app);

    QObject::connect(serial, &CommSerial::opened, [&](){
        qInfo().noquote() << "[SER] Opened:" << scfg.dev << "baud=" << scfg.baud
                          << (scfg.rs485 ? " (rs485)" : "");
    });

    QObject::connect(serial, &CommSerial::closed, [&](){
        qInfo() << "[SER] Closed";
    });

    QObject::connect(serial, &CommSerial::errorOccurred, [&](const QString& e){
        qWarning().noquote() << "[SER] Error:" << e;
    });

    QObject::connect(serial, &CommSerial::bytesReceived, [&](const QByteArray& data){
        qInfo().noquote()
            << "[SER] RX len=" << data.size()
            << " hex=" << data.toHex(' ')
            << " ascii=" << QString::fromUtf8(data);
    });

    if (!serial->open()) {
        qWarning() << "[SER] open() failed, exit.";
        return 1;
    }

    QTimer serTx;
    serTx.setInterval(1000);
    QObject::connect(&serTx, &QTimer::timeout, [&](){
        const QByteArray msg = QByteArray("ping ")
                               + QByteArray::number(QDateTime::currentMSecsSinceEpoch())
                               + "\r\n";
        const int64_t n = serial->wirteBytes(msg); // 注意：你的函数名是 wirteBytes
        qInfo().noquote() << "[SER] TX n=" << n << " data=" << msg.trimmed();
    });
    serTx.start();

    // -------- CAN (optional) --------
    CommCan* can = nullptr;
    QTimer canTx;

    if (!canIfName.isEmpty()) {
        CanConfig ccfg;
        ccfg.ifname = canIfName;
        ccfg.canFd = false;

        can = new CommCan(ccfg, &app);

        QObject::connect(can, &CommCan::opened, [&](){
            qInfo().noquote() << "[CAN] Opened:" << canIfName;
        });

        QObject::connect(can, &CommCan::closed, [&](){
            qInfo() << "[CAN] Closed";
        });

        QObject::connect(can, &CommCan::errorOccurred, [&](const QString& e){
            qWarning().noquote() << "[CAN] Error:" << e;
        });

        QObject::connect(can, &CommCan::canFrameReceived,
                         [&](quint32 canId, const QByteArray& payload, bool extended, bool rtr){
            qInfo().noquote()
                << "[CAN] RX"
                << "id=" << fmtCanId(canId, extended)
                << (extended ? "EFF" : "SFF")
                << (rtr ? "RTR" : "DATA")
                << "dlc=" << payload.size()
                << "data=" << payload.toHex(' ');
        });


        if (!can->open()) {
            qWarning() << "[CAN] open() failed (continue with serial only).";
            can->deleteLater();
            can = nullptr;
        } else {
            canTx.setInterval(1000);
            QObject::connect(&canTx, &QTimer::timeout, [&](){
                const quint32 id = 0x123;
                const QByteArray data = QByteArray::fromHex("1122334455667788");
                const bool ok = can->sendFrame(id, data, /*extended=*/false, /*rtr=*/false);

                qInfo().noquote()
                    << "[CAN] TX"
                    << "id=" << fmtCanId(id, false)
                    << "dlc=" << data.size()
                    << "data=" << data.toHex(' ')
                    << (ok ? "OK" : "FAIL");
            });
            canTx.start();
        }
    } else {
        qInfo() << "[CAN] Not enabled (no canif argument).";
    }

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&](){
        if (can) can->close();
        serial->close();
    });

    return app.exec();
}
