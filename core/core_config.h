#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include "device/device_list.h"

// device config struct
struct DeviceConfig {
    QString name;
    DeviceTypeId deviceType = DeviceTypeId::RelayGD427;
    CommTypeId commType = CommTypeId::Can;

    // option
    int node_id = -1;
    QString bus = "can0";

    QJsonObject params;
};

struct RelayNodeConfig {
    int nodeId = 1;
    bool enabled = true;
    int channels = 4;
    QString name;
};

struct CommCanConfig {
    QString canIfname = "can0";
    int canBitrate = 125000;
    bool canTripleSampling = true;
    bool canFd = false;
};

struct MainConfig {
    quint16 rpcPort = 12345;
};

// Device group config
struct DeviceGroupConfig {
    int groupId = 0;
    QString name;
    QList<int> deviceNodes;  // list of device node IDs in this group
    bool enabled = true;
};

class CoreConfig
{
public:
    MainConfig core_;
    CommCanConfig can_;

    QList<DeviceConfig> devices_;
    QList<DeviceGroupConfig> groups_;

    bool loadFromFile(const QString& path, QString* err=nullptr);
    bool saveToFile(const QString& path, QString* err=nullptr) const;
    static CoreConfig makeDefault();
};

#endif // CORE_CONFIG_H
