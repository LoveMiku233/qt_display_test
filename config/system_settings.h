#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

class QProcess;

class SystemSettings : public QObject
{
    Q_OBJECT
public:
    explicit SystemSettings(QObject* parent=nullptr);

    // 运行一次命令（阻塞等待结束），返回 stdout；失败时返回空并发 errorOccurred
    QString runCommand(const QString& program,
                       const QStringList& args,
                       int timeoutMs = 5000);

    // 1) CAN up/down
    bool canDown(const QString& ifname);
    bool canUp(const QString& ifname);

    // 2) 设置 CAN 波特率
    bool setCanBitrate(const QString& ifname, int bitrate, bool tripleSampling = false);

    // 3) 发送一帧TEST
    bool sendCanFrame(const QString& ifname, quint32 canId, const QByteArray& data, bool extended = false);

    // 4) 启动 candump
    bool startCanDump(const QString& ifname, const QStringList& extraArgs = {});
    void stopCanDump();

signals:
    void commandOutput(const QString& line);
    void errorOccurred(const QString& error);
    void candumpLine(const QString& line);

private:
    QProcess* dumpProc_ = nullptr;

    static QString toCanSendArg(quint32 canId, const QByteArray& data, bool extended);
};
#endif // SYSTEM_SETTINGS_H
