#include "json_rpc_dispatcher.h"


void JsonRpcDispatcher::registerMethod(const QString &method, Handler handler)
{
    handlers_[method] = std::move(handler);
}


QStringList JsonRpcDispatcher::methods() const {
    auto keys = handlers_.keys();
    std::sort(keys.begin(), keys.end());
    return keys;
}

QJsonObject JsonRpcDispatcher::makeError(const QJsonValue &id, int code, const QString &message)
{
    return QJsonObject {
        {"jsonrpc", "2.0"},
        {"id", id.isUndefined() ? QJsonValue(QJsonValue::Null) : id},
        {"error", QJsonObject{{"code", code}, {"message", message}}}
    };
}


QJsonObject JsonRpcDispatcher::makeResult(const QJsonValue &id, const QJsonValue &result)
{
    return QJsonObject {
        {"jsonrpc", "2.0"},
        {"id", id},
        {"result", result}
    };
}


QJsonObject JsonRpcDispatcher::handle(const QJsonObject &req) const
{
    if (req.value("jsonrpc").toString() != "2.0")
        return makeError(req.value("id"), -32600, "Invalid Request: jsonrpc must be '2.0'");

    const bool isNotification = !req.contains("id");
    const QJsonValue id = req.value("id");

    const QString method = req.value("method").toString();
    if (method.isEmpty())
        return makeError(id, -32600, "Invalid Request: method missing");

    const auto it = handlers_.find(method);
    // a notification is a requset object without an "id" member
    if (it == handlers_.end())
        return isNotification ? QJsonObject{} : makeError(id, -32601, "Method not found");

    QJsonObject paramsObj;
    if (req.contains("params")) {
        if (!req.value("params").isObject())
            return isNotification ? QJsonObject{} : makeError(id, -32602, "Invalid params: must be object");
        paramsObj = req.value("params").toObject();
    }

    // callback
    const QJsonValue result = it.value()(paramsObj);
    return isNotification ? QJsonObject{} : makeResult(id, result);
}
