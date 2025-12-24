#include "rpc_registry.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "rpc/json_rpc_dispatcher.h"
#include "rpc/rpc_helpers.h"
#include "rpc/rpc_error_codes.h"
#include "rpc/rpc_registry.h"

#include "comm/comm_can.h"

#include "device/can/device_relay_can_f427.h"

#include "core/core_context.h"
#include "config/system_settings.h"



RpcRegistry::RpcRegistry(CoreContext* ctx, JsonRpcDispatcher* disp, QObject* parent)
    : QObject(parent), ctx_(ctx), disp_(disp) {}


void RpcRegistry::registerAll()
{
    registerBase();
    registerSys();
    registerCan();
    registerRelay();
}

// register base methods
void RpcRegistry::registerBase()
{
    disp_->registerMethod("rpc.list", [&](const QJsonObject&){
        QJsonArray arr;
        for (const auto& m : disp_->methods()) arr.append(m);
        return QJsonValue(arr);
    });

    disp_->registerMethod("rpc.ping", [&](const QJsonObject&){
        return QJsonObject{{"ok", true}};
    });

    disp_->registerMethod("echo", [&](const QJsonObject& params){
        return QJsonValue(params);
    });
    // TODO:
}

void RpcRegistry::registerSys()
{
    disp_->registerMethod("sys.can.setBitrate", [&](const QJsonObject& params){
        // ifname
        QString ifname;
        qint32 bitrate = 0;
        bool ts = false;

        // get param
        if (!RpcHelpers::getString(params, "ifname", ifname))
            return RpcHelpers::err(RpcError::MissingParameter, "missing ifname");
        if (!RpcHelpers::getI32(params, "bitrate", bitrate) || bitrate <= 0)
            return RpcHelpers::err(RpcError::BadParameterValue, "missing/invalid bitrate");
        if (!RpcHelpers::getBool(params, "tripleSampling", ts, false))
            return RpcHelpers::err(RpcError::BadParameterType, "invalid tripleSampling");

        if (!ctx_->sys) return RpcHelpers::err(RpcError::InvalidState, "SystemSettings not ready");
        const bool ok = ctx_->sys->setCanBitrate(ifname, bitrate, ts);
        return QJsonObject{{"ok", ok}};
    });

    // sys.can.dump.start: {ifname:"can0"}
    disp_->registerMethod("sys.can.dump.start", [&](const QJsonObject& params){
        QString ifname;
        if (!RpcHelpers::getString(params, "ifname", ifname))
            return RpcHelpers::err(RpcError::MissingParameter, "missing ifname");
        const bool ok = ctx_->sys && ctx_->sys->startCanDump(ifname);
        return QJsonObject{{"ok", ok}};
    });

    // sys.can.dump.stop: {}
    disp_->registerMethod("sys.can.dump.stop", [&](const QJsonObject&){
        if (ctx_->sys) ctx_->sys->stopCanDump();
        return RpcHelpers::ok(true);
    });
}


void RpcRegistry::registerCan()
{
    disp_->registerMethod("can.send", [&](const QJsonObject& params){
        if (!ctx_->canBus) return RpcHelpers::err(RpcError::InvalidState, "CAN not ready");

        qint32 id = 0;
        QByteArray data;
        bool ext = false;

        if (!RpcHelpers::getI32(params, "id", id) || id < 0)
            return RpcHelpers::err(RpcError::MissingParameter, "missing/invalid id");
        if (!RpcHelpers::getHexBytes(params, "dataHex", data))
            return RpcHelpers::err(RpcError::MissingParameter, "missing/invalid dataHex");
        if (!RpcHelpers::getBool(params, "extended", ext, false))
            return RpcHelpers::err(RpcError::BadParameterType, "invalid extended");
        if (data.size() > 8)
            return RpcHelpers::err(RpcError::BadParameterValue, "payload too long (>8)");

        const bool ok = ctx_->canBus->sendFrame(quint32(id), data, ext, false);
        return QJsonObject{{"ok", ok}};
    });
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

void RpcRegistry::registerRelay()
{
    // relay.control: {node:2, ch:0, action:"fwd"}
    disp_->registerMethod("relay.control", [&](const QJsonObject& params){
        quint8 node=0, ch=0;
        QString actionStr;

        if (!RpcHelpers::getU8(params, "node", node))
            return RpcHelpers::err(RpcError::MissingParameter, "missing/invalid node");
        if (!RpcHelpers::getU8(params, "ch", ch) || ch > 3)
            return RpcHelpers::err(RpcError::BadParameterValue, "missing/invalid ch(0..3)");
        if (!RpcHelpers::getString(params, "action", actionStr))
            return RpcHelpers::err(RpcError::MissingParameter, "missing action");

        bool okAction=false;
        const auto action = parseAction(actionStr, &okAction);
        if (!okAction)
            return RpcHelpers::err(RpcError::BadParameterValue, "invalid action (stop/fwd/rev)");

        auto* dev = ctx_->relays.value(node, nullptr);
        if (!dev) return RpcHelpers::err(RpcError::BadParameterValue, "unknown node");

        return QJsonObject{{"ok", dev->control(ch, action)}};
    });

    // relay.query: {node:2, ch:0}
    disp_->registerMethod("relay.query", [&](const QJsonObject& params){
        quint8 node = 0, ch = 0;

        if (!RpcHelpers::getU8(params, "node", node))
            return RpcHelpers::err(RpcError::MissingParameter, "missing/invalid node");
        if (!RpcHelpers::getU8(params, "ch", ch) || ch > 3)
            return RpcHelpers::err(RpcError::BadParameterValue, "missing/invalid ch(0..3)");

        auto* dev = ctx_->relays.value(node, nullptr);
        if (!dev) return RpcHelpers::err(RpcError::BadParameterValue, "unknown node");

        return QJsonObject{{"ok", dev->query(ch)}};
    });

    // relay.status: {node:2, ch:0}
    disp_->registerMethod("relay.status", [&](const QJsonObject& params){
        quint8 node=0, ch=0;
        if (!RpcHelpers::getU8(params, "node", node))
            return RpcHelpers::err(RpcError::MissingParameter, "missing/invalid node");
        if (!RpcHelpers::getU8(params, "ch", ch) || ch > 3)
            return RpcHelpers::err(RpcError::BadParameterValue, "missing/invalid ch(0..3)");

        auto* dev = ctx_->relays.value(node, nullptr);
        if (!dev) return RpcHelpers::err(RpcError::BadParameterValue, "unknown node");

        const auto st = dev->lastStatus(ch);
        return QJsonObject{
            {"ok", true},
            {"channel", int(st.channel)},
            {"statusByte", int(st.statusByte)},
            {"currentA", double(st.currentA)},
            {"mode", int(RelayCanProtocol::modeBits(st.statusByte))},
            {"phaseLost", RelayCanProtocol::phaseLost(st.statusByte)},
        };
    });

    // relay.node: {}
    disp_->registerMethod("relay.nodes", [&](const QJsonObject&){
        QJsonArray arr;
        for (auto it = ctx_->relays.begin(); it != ctx_->relays.end(); ++it) {
            arr.append(int(it.key()));
        }
        return QJsonObject{{"ok", true}, {"nodes", arr}};
    });
}


