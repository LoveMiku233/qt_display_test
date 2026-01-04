#include "core_config.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


static DeviceTypeId deviceTypeFromInt(int v)
{
    return static_cast<DeviceTypeId>(v);
}

static CommTypeId commTypeFromInt(int v)
{
    return static_cast<CommTypeId>(v);
}


static bool writeTextFile(const QString& path, const QByteArray& data, QString* err)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (err) *err = QString("open for write failed: %1").arg(f.errorString());
        return false;
    }
    if (f.write(data) != data.size()) {
        if (err) *err = QString("write failed: %1").arg(f.errorString());
        return false;
    }
    return true;
}

static int toInt(DeviceTypeId t) { return static_cast<int>(t); }
static int toInt(CommTypeId t)   { return static_cast<int>(t); }

CoreConfig CoreConfig::makeDefault()
{
    CoreConfig c;
    c.core_.rpcPort = 12345;

    c.can_.canIfname = "can0";
    c.can_.canBitrate = 125000;
    c.can_.canTripleSampling = true;
    c.can_.canFd = false;

    DeviceConfig d1;
    d1.name = "relay01";
    d1.deviceType = DeviceTypeId::RelayGD427;
    d1.commType = CommTypeId::Can;
    d1.node_id = 1;
    d1.bus = "can0";
    d1.params = QJsonObject{{"channels", 4}};
    c.devices_.append(d1);

    return c;
}

bool CoreConfig::loadFromFile(const QString& path, QString* err)
{
    QFile f(path);
    if (!f.exists()) {
        if (err) *err = "config file not found";
        return false;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        if (err) *err = QString("open failed: %1").arg(f.errorString());
        return false;
    }

    const auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) {
        if (err) *err = "invalid json root (not object)";
        return false;
    }
    const QJsonObject root = doc.object();

    if (root.contains("main") && root["main"].isObject()) {
        const auto main = root["main"].toObject();
        if (main.contains("rpcPort")) core_.rpcPort = quint16(main["rpcPort"].toInt(int(core_.rpcPort)));
    }

    // -------- can --------
    if (root.contains("can") && root["can"].isObject()) {
        const auto can = root["can"].toObject();
        if (can.contains("ifname")) can_.canIfname = can["ifname"].toString(can_.canIfname);
        if (can.contains("bitrate")) can_.canBitrate = can["bitrate"].toInt(can_.canBitrate);
        if (can.contains("tripleSampling")) can_.canTripleSampling = can["tripleSampling"].toBool(can_.canTripleSampling);
        if (can.contains("canFd")) can_.canFd = can["canFd"].toBool(can_.canFd);
    }

    // devices (array)
    devices_.clear();
    if (root.contains("devices") && root["devices"].isArray()) {
        const auto arr = root["devices"].toArray();
        for (const auto& v : arr) {
            if (!v.isObject()) continue;
            const auto o = v.toObject();

            DeviceConfig d;
            d.name = o.value("name").toString();
            d.deviceType = deviceTypeFromInt(o.value("type").toInt(toInt(d.deviceType)));
            d.commType = commTypeFromInt(o.value("commType").toInt(toInt(d.commType)));
            d.node_id = o.value("nodeId").toInt(d.node_id);
            d.bus = o.value("bus").toString(d.bus);
            if (o.contains("params") && o["params"].isObject()) d.params = o["params"].toObject();

            devices_.append(d);
        }
    }

    // groups (array)
    groups_.clear();
    if (root.contains("groups") && root["groups"].isArray()) {
        const auto arr = root["groups"].toArray();
        for (const auto& v : arr) {
            if (!v.isObject()) continue;
            const auto o = v.toObject();

            DeviceGroupConfig g;
            g.groupId = o.value("groupId").toInt(0);
            g.name = o.value("name").toString();
            g.enabled = o.value("enabled").toBool(true);
            
            if (o.contains("devices") && o["devices"].isArray()) {
                const auto devArr = o["devices"].toArray();
                for (const auto& dv : devArr) {
                    g.deviceNodes.append(dv.toInt());
                }
            }

            groups_.append(g);
        }
    }

    return true;
}

bool CoreConfig::saveToFile(const QString& path, QString* err) const
{
    QJsonObject root;

    // main
    QJsonObject main;
    main["rpcPort"] = int(core_.rpcPort);
    root["main"] = main;

    // can
    QJsonObject can;
    can["ifname"] = can_.canIfname;
    can["bitrate"] = can_.canBitrate;
    can["tripleSampling"] = can_.canTripleSampling;
    can["canFd"] = can_.canFd;
    root["can"] = can;

    // devices
    QJsonArray devArr;
    for (const auto& d : devices_) {
        QJsonObject o;
        o["name"] = d.name;
        o["type"] = toInt(d.deviceType);
        o["commType"] = toInt(d.commType);
        if (d.node_id >= 0) o["nodeId"] = d.node_id;
        if (!d.bus.isEmpty()) o["bus"] = d.bus;
        if (!d.params.isEmpty()) o["params"] = d.params;
        devArr.append(o);
    }
    root["devices"] = devArr;

    // groups
    QJsonArray groupArr;
    for (const auto& g : groups_) {
        QJsonObject o;
        o["groupId"] = g.groupId;
        o["name"] = g.name;
        o["enabled"] = g.enabled;
        
        QJsonArray devNodes;
        for (int nodeId : g.deviceNodes) {
            devNodes.append(nodeId);
        }
        o["devices"] = devNodes;
        
        groupArr.append(o);
    }
    root["groups"] = groupArr;

    QJsonDocument doc(root);
    const QByteArray data = doc.toJson(QJsonDocument::Indented);
    return writeTextFile(path, data, err);
}
