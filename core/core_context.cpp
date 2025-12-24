#include "core_context.h"
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

QStringList CoreContext::methodGroups() const
{
    return {"rpc.*", "sys.*", "can.*", "relay.*"};
}








