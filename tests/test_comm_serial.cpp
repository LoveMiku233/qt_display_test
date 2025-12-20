#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

#include "comm/comm_serial.h"

// 用法：
// 1) ./test_comm_serial /dev/ttyS0 115200
// 2) ./test_comm_serial /dev/ttyS2 9600 rs485
// 3) ./test_comm_serial /dev/ttyUSB1 115200
//
// 说明：
// - 程序启动后会 open 串口
// - 每 1 秒发送一次 "ping <ms>\r\n"
// - 收到的数据会打印 hex + ascii
// - Ctrl+C 结束

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    const QStringList args = app.arguments();
    if (args.size() < 3) {
        qWarning().noquote()
            << "Usage:\n"
            << "  " << args.value(0) << " <dev> <baud> [rs485]\n"
            << "Examples:\n"
            << "  " << args.value(0) << " /dev/ttyS0 115200\n"
            << "  " << args.value(0) << " /dev/ttyS2 9600 rs485\n"
            << "  " << args.value(0) << " /dev/ttyUSB1 115200\n";
        return 2;
    }

    SerialConfig cfg;
    cfg.dev = args.at(1);
    cfg.baud = args.at(2).toInt();

    if (args.size() >= 4 && args.at(3).toLower() == "rs485") {
        cfg.rs485 = true;
        cfg.rs485DelayBeforeUs = 0;
        cfg.rs485DelayAfterUs = 0;
    }

    auto serial = new CommSerial(cfg, &app);

    QObject::connect(serial, &CommSerial::opened, [&](){
        qInfo().noquote() << "Opened:" << cfg.dev << "baud=" << cfg.baud
                          << (cfg.rs485 ? " (rs485)" : "");
    });

    QObject::connect(serial, &CommSerial::closed, [&](){
        qInfo() << "Closed";
    });

    QObject::connect(serial, &CommSerial::errorOccurred, [&](const QString& e){
        qWarning().noquote() << "Error:" << e;
    });

    QObject::connect(serial, &CommSerial::bytesReceived, [&](const QByteArray& data){
        qInfo().noquote()
            << "RX len=" << data.size()
            << " hex=" << data.toHex(' ')
            << " ascii=" << QString::fromUtf8(data);
    });

    if (!serial->open()) {
        qWarning() << "open() failed, exit.";
        return 1;
    }

    // 定时发送测试数据
    QTimer txTimer;
    txTimer.setInterval(1000);
    QObject::connect(&txTimer, &QTimer::timeout, [&](){
        const QByteArray msg = QByteArray("ping ") + QByteArray::number(QDateTime::currentMSecsSinceEpoch()) + "\r\n";
        const int64_t n = serial->wirteBytes(msg);
        qInfo().noquote() << "TX n=" << n << " data=" << msg.trimmed();
    });
    txTimer.start();

    // 退出时关闭串口
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&](){
        serial->close();
    });

    return app.exec();
}
