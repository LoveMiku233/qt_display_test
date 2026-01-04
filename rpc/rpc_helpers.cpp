#include "rpc_helpers.h"

namespace RpcHelpers {
bool getU8(const QJsonObject& p, const char* key, quint8& out)
{
    if (!p.contains(key) || !p.value(key).isDouble()) return false;
    const int v = p.value(key).toInt(-1);
    if (v < 0 || v > 255) return false;
    out = quint8(v);
    return true;
}

bool getBool(const QJsonObject& p, const char* key, bool& out, bool def)
{
    if (!p.contains(key)) { out = def; return true; }
    if (!p.value(key).isBool()) return false;
    out = p.value(key).toBool();
    return true;
}

bool getI32(const QJsonObject& p, const char* key, qint32& out)
{
    if (!p.contains(key) || !p.value(key).isDouble()) return false;
    out = p.value(key).toInt();
    return true;
}

bool getString(const QJsonObject& p, const char* key, QString& out)
{
    if (!p.contains(key) || !p.value(key).isString()) return false;
    out = p.value(key).toString();
    return true;
}

bool getHexBytes(const QJsonObject& p, const char* key, QByteArray& out)
{
    if (!p.contains(key) || !p.value(key).isString()) return false;
    const QString s = p.value(key).toString().trimmed();
    const QByteArray b = QByteArray::fromHex(s.toLatin1());
    if (b.isEmpty() && !s.isEmpty()) return false;
    out = b;
    return true;
}

QJsonObject ok(bool v)
{
    return QJsonObject{{"ok", v}};
}

QJsonObject err(int code, const QString& msg)
{
    return QJsonObject{
        {"ok", false},
        {"code", code},
        {"message", msg},
    };
}


}
