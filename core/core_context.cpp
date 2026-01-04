#include "core_context.h"
#include "core_config.h"
#include "config/system_settings.h"
#include "comm/comm_can.h"
#include "device/can/can_device_manager.h"
#include "device/can/device_relay_can_f427.h"



#include <QDebug>

CoreContext::CoreContext(QObject* parent) : QObject(parent) {}

bool CoreContext::init()
{
    if (!initSystemSettings()) return false;
    if (!initCan()) return false;
    if (!initDevices()) return false;
    return true;
}


// config
bool CoreContext::init(const CoreConfig& cfg)
{
    rpcPort = cfg.core_.rpcPort;
    canIfname = cfg.can_.canIfname;
    canBitrate = cfg.can_.canBitrate;
    tripleSampling = cfg.can_.canTripleSampling;

    if (!initSystemSettings()) return false;
    if (!initCan()) return false;

    // initDevicesFromConfig(cfg.devices_ / cfg.relayNodes)
    return initDevices(cfg);
}


bool CoreContext::initSystemSettings()
{
    sys = new SystemSettings(this);

    // connect
    connect(sys, &SystemSettings::commandOutput, this, [](const QString& s){
        qInfo().noquote() << "[SYS][OUT]" << s;
    });

    connect(sys, &SystemSettings::errorOccurred, this, [](const QString& s){
        qWarning().noquote() << "[SYS][ERR]" << s;
    });

    connect(sys, &SystemSettings::candumpLine, this, [](const QString& line){
        qInfo().noquote() << "[CANDUMP]" << line;
    });

    sys->setCanBitrate(canIfname, canBitrate, tripleSampling);
    return true;
}


bool CoreContext::initCan()
{
    CanConfig cfg;
    cfg.ifname = canIfname;
    cfg.canFd = false;

    canBus = new CommCan(cfg, this);
    connect(canBus, &CommCan::errorOccurred, this, [](const QString& e){
        qWarning().noquote() << "[CAN] Error:" << e;
    });

    if (!canBus->open()) {
        qWarning() << "CAN Open failed, RPC still starts, but CAN methods will feil!";
        // return false
    }

    canMgr = new CanDeviceManager(canBus, this);
    return true;
}

bool CoreContext::initDevices()
{
    const QList<quint8> nodes = {0x01};
    for (auto node : nodes) {
        auto* d = new RelayCanDeviceGD427(node, canBus, this);
        d->init();
        canMgr->addDevice(d);
        relays.insert(node, d);
    }
    return true;
}


bool CoreContext::initDevices(const CoreConfig& cfg)
{
    // clear old devices
    relays.clear();
    if (!cfg.devices_.isEmpty()) {
        for (const auto& dcfg : cfg.devices_) {

            // custom : params["enabled"]
            const bool enabled = dcfg.params.value("enabled").toBool(true);
            if (!enabled) continue;

            // RelayGD427 + CAN
            if (dcfg.deviceType == DeviceTypeId::RelayGD427 &&
                dcfg.commType == CommTypeId::Can) {

                if (dcfg.node_id < 1 || dcfg.node_id > 255) {
                    qWarning() << "Invalid node_id in config:" << dcfg.node_id << "name=" << dcfg.name;
                    continue;
                }

                const quint8 node = quint8(dcfg.node_id);

                // 防重复
                if (relays.contains(node)) {
                    qWarning() << "Duplicate relay node in config:" << int(node) << "skip";
                    continue;
                }

                auto* dev = new RelayCanDeviceGD427(node, canBus, this);
                dev->init();
                canMgr->addDevice(dev);
                relays.insert(node, dev);

                qInfo().noquote() << "RelayGD427 added: node=0x"
                                  << QString::number(node, 16)
                                  << " name=" << dcfg.name;
            } else {
                qWarning() << "Unsupported device type/commType:"
                           << int(dcfg.deviceType) << int(dcfg.commType)
                           << "name=" << dcfg.name;
            }
        }

        // Load groups
        deviceGroups.clear();
        groupNames.clear();
        deviceGroups.reserve(cfg.groups_.size());
        groupNames.reserve(cfg.groups_.size());
        
        for (const auto& gcfg : cfg.groups_) {
            if (!gcfg.enabled) continue;
            
            QList<quint8> nodes;
            nodes.reserve(gcfg.deviceNodes.size());
            for (int nodeId : gcfg.deviceNodes) {
                if (nodeId >= 1 && nodeId <= 255) {
                    nodes.append(quint8(nodeId));
                }
            }
            
            deviceGroups.insert(gcfg.groupId, nodes);
            groupNames.insert(gcfg.groupId, gcfg.name);
            
            qInfo().noquote() << "Group added: id=" << gcfg.groupId 
                              << "name=" << gcfg.name 
                              << "devices=" << nodes.size();
        }

        return true;
    }
    // err
    qWarning() << "No devices configured (devices_ and relayNodes are empty).";
    return true;
}

QStringList CoreContext::methodGroups() const
{
    return {"rpc.*", "sys.*", "can.*", "relay.*", "group.*"};
}








