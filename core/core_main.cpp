// core/core_main.cpp name=core/core_main.cpp
#include <QCoreApplication>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QHostAddress>

#include "config/system_settings.h"

#include "rpc/json_rpc_server.h"
#include "rpc/json_rpc_dispatcher.h"
#include "rpc/rpc_error_codes.h"

// CAN + Device
#include "comm/comm_can.h"
#include "device/can/can_device_manager.h"
#include "device/can/device_relay_can_f427.h"
#include "device/can/relay_can_protocol.h"

static bool getU8(const QJsonObject& p, const char* key, quint8& out)
{
    if (!p.contains(key) || !p.value(key).isDouble()) return false;
    const int v = p.value(key).toInt(-1);
    if (v < 0 || v > 255) return false;
    out = quint8(v);
    return true;
}

static bool getBool(const QJsonObject& p, const char* key, bool& out, bool def=false)
{
    if (!p.contains(key)) { out = def; return true; }
    if (!p.value(key).isBool()) return false;
    out = p.value(key).toBool();
    return true;
}

static bool getHexBytes(const QJsonObject& p, const char* key, QByteArray& out)
{
    if (!p.contains(key) || !p.value(key).isString()) return false;
    const QString s = p.value(key).toString().trimmed();
    const QByteArray b = QByteArray::fromHex(s.toLatin1());
    if (b.isEmpty() && !s.isEmpty()) return false;
    out = b;
    return true;
}

static RelayCanProtocol::Action parseAction(const QString& s, bool* ok=nullptr)
{
    const QString a = s.trimmed().toLower();
    if (ok) *ok = true;
    if (a == "stop" || a == "0") return RelayCanProtocol::Action::Stop;
    if (a == "fwd"  || a == "forward" || a == "1") return RelayCanProtocol::Action::Forward;
    if (a == "rev"  || a == "reverse" || a == "2") return RelayCanProtocol::Action::Reverse;
    if (ok) *ok = false;
    return RelayCanProtocol::Action::Stop;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);


     auto* sys = new SystemSettings(&app);
     // 监听输出/错误
     QObject::connect(sys, &SystemSettings::commandOutput, [](const QString& s){
         qInfo().noquote() << "[SYS][OUT]" << s;
     });
     QObject::connect(sys, &SystemSettings::errorOccurred, [](const QString& e){
         qWarning().noquote() << "[SYS][ERR]" << e;
     });
     QObject::connect(sys, &SystemSettings::candumpLine, [](const QString& line){
         qInfo().noquote() << "[CANDUMP]" << line;
     });

     sys->setCanBitrate("can0", 125000, /*tripleSampling=*/true);

    // ---------------- CAN init ----------------
    CanConfig canCfg;
    canCfg.ifname = "can0";
    canCfg.canFd = false;

    auto* canBus = new CommCan(canCfg, &app);
    QObject::connect(canBus, &CommCan::errorOccurred, [&](const QString& e){
        qWarning().noquote() << "[CAN] Error:" << e;
    });

    if (!canBus->open()) {
        qWarning() << "CAN open failed. RPC still starts, but CAN methods will fail.";
    }

    auto* canMgr = new CanDeviceManager(canBus, &app);
    // add device
    auto* relay01 = new RelayCanDeviceGD427(0x01, canBus, &app);
    auto* relay02 = new RelayCanDeviceGD427(0x02, canBus, &app);
    auto* relay03 = new RelayCanDeviceGD427(0x03, canBus, &app);

    relay01->init(); relay02->init(); relay03->init();

    canMgr->addDevice(relay01);
    canMgr->addDevice(relay02);
    canMgr->addDevice(relay03);

    // ---------------- JSON-RPC ----------------
    JsonRpcDispatcher dispatcher;

    dispatcher.registerMethod("rpc.list", [&](const QJsonObject&){
        QJsonArray arr;
        for (const auto& m : dispatcher.methods()) arr.append(m);
        return QJsonValue(arr);
    });

    dispatcher.registerMethod("echo", [&](const QJsonObject& params){
        return QJsonValue(params);
    });

    // params: { "id": 291, "dataHex": "112233", "extended": false }
    dispatcher.registerMethod("can.send", [&](const QJsonObject& params){
        if (!canBus) return QJsonObject{{"ok", false}};

        quint32 canId = 0;
        if (!params.contains("id") || !params.value("id").isDouble())
            return QJsonObject{{"ok", false}, {"error", "missing id"}};

        canId = quint32(params.value("id").toInt());
        QByteArray data;
        if (!getHexBytes(params, "dataHex", data))
            return QJsonObject{{"ok", false}, {"error", "missing/invalid dataHex"}};

        bool extended = false;
        if (!getBool(params, "extended", extended, false))
            return QJsonObject{{"ok", false}, {"error", "invalid extended"}};

        if (data.size() > 8)
            return QJsonObject{{"ok", false}, {"error", "payload too long (>8)"}};

        const bool ok = canBus->sendFrame(canId, data, extended, false);
        return QJsonObject{{"ok", ok}};
    });

    auto pickRelay = [&](quint8 nodeId) -> RelayCanDeviceGD427* {
        if (nodeId == 0x01) return relay01;
        if (nodeId == 0x02) return relay02;
        if (nodeId == 0x03) return relay03;
        return nullptr;
    };

    // 控制继电器
    // params: { "node":2, "ch":0, "action":"fwd" }  action: stop/fwd/rev
    dispatcher.registerMethod("relay.control", [&](const QJsonObject& params){
        quint8 node=0, ch=0;
        if (!getU8(params, "node", node))
            return QJsonObject{{"ok", false}, {"code", RpcError::MissingParameter}, {"message", "missing/invalid node"}};
        if (!getU8(params, "ch", ch) || ch > 3)
            return QJsonObject{{"ok", false}, {"code", RpcError::BadParameterValue}, {"message", "missing/invalid ch(0..3)"}};
        if (!params.contains("action") || !params.value("action").isString())
            return QJsonObject{{"ok", false}, {"code", RpcError::MissingParameter}, {"message", "missing action"}};

        bool okAction = false;
        const auto action = parseAction(params.value("action").toString(), &okAction);
        if (!okAction)
            return QJsonObject{{"ok", false}, {"code", RpcError::BadParameterValue}, {"message", "invalid action (stop/fwd/rev)"}};

        auto* dev = pickRelay(node);
        if (!dev)
            return QJsonObject{{"ok", false}, {"code", RpcError::BadParameterValue}, {"message", "unknown node"}};

        const bool ok = dev->control(ch, action);
        return QJsonObject{{"ok", ok}};
    });

    // 查询某通道（发 QueryStatus）
    // params: { "node":2, "ch":0 }
    dispatcher.registerMethod("relay.query", [&](const QJsonObject& params){
        quint8 node=0, ch=0;
        if (!getU8(params, "node", node))
            return QJsonObject{{"ok", false}, {"code", RpcError::MissingParameter}, {"message", "missing/invalid node"}};
        if (!getU8(params, "ch", ch) || ch > 3)
            return QJsonObject{{"ok", false}, {"code", RpcError::BadParameterValue}, {"message", "missing/invalid ch(0..3)"}};

        auto* dev = pickRelay(node);
        if (!dev)
            return QJsonObject{{"ok", false}, {"code", RpcError::BadParameterValue}, {"message", "unknown node"}};

        const bool ok = dev->query(ch);
        return QJsonObject{{"ok", ok}};
    });

    // params: { "node":2, "ch":0 }
    dispatcher.registerMethod("relay.status", [&](const QJsonObject& params){
        quint8 node=0, ch=0;
        if (!getU8(params, "node", node))
            return QJsonObject{{"ok", false}, {"code", RpcError::MissingParameter}, {"message", "missing/invalid node"}};
        if (!getU8(params, "ch", ch) || ch > 3)
            return QJsonObject{{"ok", false}, {"code", RpcError::BadParameterValue}, {"message", "missing/invalid ch(0..3)"}};

        auto* dev = pickRelay(node);
        if (!dev)
            return QJsonObject{{"ok", false}, {"code", RpcError::BadParameterValue}, {"message", "unknown node"}};

        const auto st = dev->lastStatus(ch);

        QJsonObject out;
        out["ok"] = true;
        out["channel"] = int(st.channel);
        out["statusByte"] = int(st.statusByte);
        out["currentA"] = double(st.currentA);
        out["mode"] = int(RelayCanProtocol::modeBits(st.statusByte));     // 0..3
        out["phaseLost"] = RelayCanProtocol::phaseLost(st.statusByte);
        return out;
    });

    JsonRpcServer server(&dispatcher);
    const quint16 port = 12345;
    if (!server.listen(QHostAddress::Any, port)) {
        qCritical() << "listen failed:" << server.errorString();
        return 1;
    }

    qInfo() << "core_main JSON-RPC listening on" << port;
    return app.exec();
}
