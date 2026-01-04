#include "core_context.h"
#include "core_config.h"
#include "config/system_settings.h"
#include "comm/comm_can.h"
#include "device/can/can_device_manager.h"
#include "device/can/device_relay_can_f427.h"
#include "utils/logger.h"

#include <QDebug>

static const char* LOG_SOURCE = "CoreContext";

CoreContext::CoreContext(QObject* parent) : QObject(parent) {}

bool CoreContext::init()
{
    LOG_INFO(LOG_SOURCE, "Initializing CoreContext (default config)...");
    if (!initSystemSettings()) {
        LOG_ERROR(LOG_SOURCE, "Failed to initialize system settings");
        return false;
    }
    if (!initCan()) {
        LOG_ERROR(LOG_SOURCE, "Failed to initialize CAN bus");
        return false;
    }
    if (!initDevices()) {
        LOG_ERROR(LOG_SOURCE, "Failed to initialize devices");
        return false;
    }
    LOG_INFO(LOG_SOURCE, "CoreContext initialization complete");
    return true;
}


// config
bool CoreContext::init(const CoreConfig& cfg)
{
    LOG_INFO(LOG_SOURCE, "Initializing CoreContext with config...");
    LOG_DEBUG(LOG_SOURCE, QString("RPC Port: %1, CAN Interface: %2, Bitrate: %3")
              .arg(cfg.core_.rpcPort).arg(cfg.can_.canIfname).arg(cfg.can_.canBitrate));
    
    rpcPort = cfg.core_.rpcPort;
    canIfname = cfg.can_.canIfname;
    canBitrate = cfg.can_.canBitrate;
    tripleSampling = cfg.can_.canTripleSampling;

    if (!initSystemSettings()) {
        LOG_ERROR(LOG_SOURCE, "Failed to initialize system settings");
        return false;
    }
    if (!initCan()) {
        LOG_ERROR(LOG_SOURCE, "Failed to initialize CAN bus");
        return false;
    }

    // initDevicesFromConfig(cfg.devices_ / cfg.relayNodes)
    if (!initDevices(cfg)) {
        LOG_ERROR(LOG_SOURCE, "Failed to initialize devices from config");
        return false;
    }
    
    LOG_INFO(LOG_SOURCE, "CoreContext initialization complete");
    return true;
}


bool CoreContext::initSystemSettings()
{
    LOG_DEBUG(LOG_SOURCE, "Initializing system settings...");
    sys = new SystemSettings(this);

    // connect
    connect(sys, &SystemSettings::commandOutput, this, [](const QString& s){
        LOG_DEBUG("SystemSettings", QString("[OUT] %1").arg(s));
    });

    connect(sys, &SystemSettings::errorOccurred, this, [](const QString& s){
        LOG_WARNING("SystemSettings", QString("[ERR] %1").arg(s));
    });

    connect(sys, &SystemSettings::candumpLine, this, [](const QString& line){
        LOG_DEBUG("CANDump", line);
    });

    LOG_INFO(LOG_SOURCE, QString("Setting CAN bitrate: interface=%1, bitrate=%2, tripleSampling=%3")
             .arg(canIfname).arg(canBitrate).arg(tripleSampling));
    sys->setCanBitrate(canIfname, canBitrate, tripleSampling);
    return true;
}


bool CoreContext::initCan()
{
    LOG_DEBUG(LOG_SOURCE, "Initializing CAN bus...");
    CanConfig cfg;
    cfg.ifname = canIfname;
    cfg.canFd = false;

    canBus = new CommCan(cfg, this);
    connect(canBus, &CommCan::errorOccurred, this, [](const QString& e){
        LOG_ERROR("CAN", QString("Error: %1").arg(e));
    });

    if (!canBus->open()) {
        LOG_WARNING(LOG_SOURCE, "CAN Open failed, RPC still starts, but CAN methods will fail!");
        // return false
    } else {
        LOG_INFO(LOG_SOURCE, QString("CAN bus opened successfully: %1").arg(canIfname));
    }

    canMgr = new CanDeviceManager(canBus, this);
    LOG_DEBUG(LOG_SOURCE, "CAN device manager created");
    return true;
}

bool CoreContext::initDevices()
{
    LOG_DEBUG(LOG_SOURCE, "Initializing devices (default mode)...");
    const QList<quint8> nodes = {0x01};
    for (auto node : nodes) {
        auto* d = new RelayCanDeviceGD427(node, canBus, this);
        d->init();
        canMgr->addDevice(d);
        relays.insert(node, d);
        LOG_INFO(LOG_SOURCE, QString("Relay device added: node=0x%1").arg(node, 2, 16, QChar('0')));
    }
    return true;
}


bool CoreContext::initDevices(const CoreConfig& cfg)
{
    LOG_DEBUG(LOG_SOURCE, "Initializing devices from config...");
    // clear old devices
    relays.clear();
    if (!cfg.devices_.isEmpty()) {
        LOG_INFO(LOG_SOURCE, QString("Found %1 device(s) in config").arg(cfg.devices_.size()));
        
        for (const auto& dcfg : cfg.devices_) {

            // custom : params["enabled"]
            const bool enabled = dcfg.params.value("enabled").toBool(true);
            if (!enabled) {
                LOG_DEBUG(LOG_SOURCE, QString("Device '%1' is disabled, skipping").arg(dcfg.name));
                continue;
            }

            // RelayGD427 + CAN
            if (dcfg.deviceType == DeviceTypeId::RelayGD427 &&
                dcfg.commType == CommTypeId::Can) {

                if (dcfg.node_id < 1 || dcfg.node_id > 255) {
                    LOG_WARNING(LOG_SOURCE, QString("Invalid node_id in config: %1, name=%2")
                                .arg(dcfg.node_id).arg(dcfg.name));
                    continue;
                }

                const quint8 node = quint8(dcfg.node_id);

                // 防重复
                if (relays.contains(node)) {
                    LOG_WARNING(LOG_SOURCE, QString("Duplicate relay node in config: %1, skipping").arg(int(node)));
                    continue;
                }

                auto* dev = new RelayCanDeviceGD427(node, canBus, this);
                dev->init();
                canMgr->addDevice(dev);
                relays.insert(node, dev);

                LOG_INFO(LOG_SOURCE, QString("RelayGD427 added: node=0x%1, name=%2")
                         .arg(node, 2, 16, QChar('0')).arg(dcfg.name));
            } else {
                LOG_WARNING(LOG_SOURCE, QString("Unsupported device type/commType: %1/%2, name=%3")
                            .arg(int(dcfg.deviceType)).arg(int(dcfg.commType)).arg(dcfg.name));
            }
        }

        // Load groups
        deviceGroups.clear();
        groupNames.clear();
        deviceGroups.reserve(cfg.groups_.size());
        groupNames.reserve(cfg.groups_.size());
        
        LOG_INFO(LOG_SOURCE, QString("Loading %1 device group(s)...").arg(cfg.groups_.size()));
        
        for (const auto& gcfg : cfg.groups_) {
            if (!gcfg.enabled) {
                LOG_DEBUG(LOG_SOURCE, QString("Group '%1' is disabled, skipping").arg(gcfg.name));
                continue;
            }
            
            QList<quint8> nodes;
            nodes.reserve(gcfg.deviceNodes.size());
            for (int nodeId : gcfg.deviceNodes) {
                if (nodeId >= 1 && nodeId <= 255) {
                    nodes.append(quint8(nodeId));
                }
            }
            
            deviceGroups.insert(gcfg.groupId, nodes);
            groupNames.insert(gcfg.groupId, gcfg.name);
            
            LOG_INFO(LOG_SOURCE, QString("Group added: id=%1, name=%2, devices=%3")
                     .arg(gcfg.groupId).arg(gcfg.name).arg(nodes.size()));
        }

        return true;
    }
    // err
    LOG_WARNING(LOG_SOURCE, "No devices configured (devices_ and relayNodes are empty)");
    return true;
}

QStringList CoreContext::methodGroups() const
{
    return {"rpc.*", "sys.*", "can.*", "relay.*", "group.*"};
}








