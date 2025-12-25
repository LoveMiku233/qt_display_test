#ifndef CORE_CONTEXT_H
#define CORE_CONTEXT_H

#include <QObject>
#include <QHash>
#include "core/core_config.h"

// per def
class SystemSettings;
class CommCan;
class CanDeviceManager;
class RelayCanDeviceGD427;

struct CanConfig;

class CoreContext : public QObject
{
    Q_OBJECT
public:
    explicit CoreContext(QObject* parent = nullptr);
    // init system, dev, can, serial
    bool init();
    bool init(const CoreConfig& cfg);
    // get method groups
    QStringList methodGroups() const;
    SystemSettings* sys = nullptr;
    CommCan* canBus = nullptr;
    CanDeviceManager* canMgr = nullptr;

    // device reg table: nodeId -> device
    QHash<quint8, RelayCanDeviceGD427*> relays;

    QString canIfname = "can0";
    int canBitrate = 125000;
    bool tripleSampling = true;

    // server
    quint16 rpcPort = 12345;


private:
    bool initSystemSettings();
    bool initCan();

    bool initDevices();     // OLD TEST
    bool initDevices(const CoreConfig& cfg);
};

#endif // CORE_CONTEXT_H
