#include "json_rpc_client.h"

#include <functional>
#include <QJsonDocument>
#include <QEventLoop>
#include <QTimer>


JsonRpcClient::JsonRpcClient(QObject* parent) : QObject(parent)
{
    connect(&sock_, &QTcpSocket::readyRead, this, &JsonRpcClient::onReadyRead);
    connect(&sock_, &QTcpSocket::connected, this, &JsonRpcClient::connected);
    connect(&sock_, &QTcpSocket::disconnected, this, &JsonRpcClient::disconnected);
    // fix: 5.12
    connect(&sock_,
               QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
               this,
               &JsonRpcClient::onSocketError);
}


void JsonRpcClient::setEndpoint(const QString& host, quint16 port)
{
    host_ = host;
    port_ = port;
}

bool JsonRpcClient::connectToServer(int timeoutMs)
{
    if (sock_.state() == QAbstractSocket::ConnectedState) return true;

    sock_.connectToHost(host_, port_);
    if (!sock_.waitForConnected(timeoutMs)) {
        emit transportError(QString("connectToHost failed: %1").arg(sock_.errorString()));
        return false;
    }
    return true;
}

void JsonRpcClient::disconnectFromServer()
{
    sock_.disconnectFromHost();
}

bool JsonRpcClient::isConnected() const
{
    return sock_.state() == QAbstractSocket::ConnectedState;
}


QJsonObject JsonRpcClient::makeError(int code, const QString& message) const
{
    return QJsonObject{
        {"code", code},
        {"message", message},
    };
}

QByteArray JsonRpcClient::packRequest(int id, const QString& method, const QJsonObject& params) const
{
    QJsonObject req;
    req["jsonrpc"] = "2.0";
    req["id"] = id;
    req["method"] = method;
    req["params"] = params;

    // 一行一个 JSON
    QJsonDocument doc(req);
    return doc.toJson(QJsonDocument::Compact) + "\n";
}

int JsonRpcClient::callAsync(const QString& method, const QJsonObject& params)
{
    if (!connectToServer()) return -1;

    const int id = nextId_++;
    pending_.insert(id, method);

    const QByteArray payload = packRequest(id, method, params);
    const qint64 n = sock_.write(payload);
    if (n != payload.size()) {
        emit transportError(QString("write failed: %1").arg(sock_.errorString()));
        pending_.remove(id);
        return -1;
    }
    sock_.flush();
    return id;
}

int JsonRpcClient::callAsync(const QString& method, const QJsonObject& params, Callback cb, int timeoutMs)
{
    const int id = callAsync(method, params);
    if (id < 0) {
        if (cb) cb(QJsonValue(), makeError(-32000, "transport write/connect failed"));
        return -1;
    }

    if (cb) callbacks_.insert(id, std::move(cb));

    if (timeoutMs > 0) {
        QTimer::singleShot(timeoutMs, this, [this, id](){
            if (!pending_.contains(id)) return;
            pending_.remove(id);

            auto cbIt = callbacks_.find(id);
            if (cbIt != callbacks_.end()) {
                auto cb = cbIt.value();
                callbacks_.erase(cbIt);
                if (cb) cb(QJsonValue(), makeError(-32001, "timeout"));
            }
        });
    }

    return id;
}

void JsonRpcClient::dispatchCallback(int id, const QJsonValue& result, const QJsonObject& error)
{
    auto it = callbacks_.find(id);
    if (it == callbacks_.end()) return;

    auto cb = it.value();
    callbacks_.erase(it);
    if (cb) cb(result, error);
}



QJsonValue JsonRpcClient::call(const QString& method, const QJsonObject& params, int timeoutMs)
{
    if (!connectToServer(timeoutMs)) {
        return QJsonObject{{"ok", false}, {"error", "not connected"}};
    }

    const int id = nextId_++;
    pending_.insert(id, method);

    const QByteArray payload = packRequest(id, method, params);
    if (sock_.write(payload) != payload.size()) {
        pending_.remove(id);
        return QJsonObject{{"ok", false}, {"error", QString("write failed: %1").arg(sock_.errorString())}};
    }
    sock_.flush();

    // wait id
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QJsonValue outResult;
    QJsonObject outError;
    bool got = false;

    const auto conn = connect(this, &JsonRpcClient::callFinished, this,
                              [&](int rid, const QJsonValue& result, const QJsonObject& error){
        if (rid != id) return;
        got = true;
        outResult = result;
        outError = error;
        loop.quit();
    });

    connect(&timer, &QTimer::timeout, &loop, [&](){
        loop.quit();
    });

    timer.start(timeoutMs);
    loop.exec();

    disconnect(conn);

    pending_.remove(id);

    if (!got) {
        return QJsonObject{{"ok", false}, {"error", "timeout"}};
    }
    if (!outError.isEmpty()) {
        return QJsonObject{{"ok", false}, {"rpcError", outError}};
    }
    return outResult;
}

void JsonRpcClient::onReadyRead()
{
    rxBuf_ += sock_.readAll();

    while (true) {
        const int idx = rxBuf_.indexOf('\n');
        if (idx < 0) break;

        const QByteArray line = rxBuf_.left(idx).trimmed();
        rxBuf_.remove(0, idx + 1);

        if (!line.isEmpty()) handleLine(line);
    }
}

void JsonRpcClient::handleLine(const QByteArray& line)
{
    QJsonParseError pe;
    const auto doc = QJsonDocument::fromJson(line, &pe);
    if (pe.error != QJsonParseError::NoError || !doc.isObject()) {
        emit transportError(QString("parse response failed: %1 | line=%2")
                            .arg(pe.errorString(), QString::fromUtf8(line)));
        return;
    }

    const QJsonObject obj = doc.object();
    const int id = obj.value("id").toInt(-1);

    QJsonValue result;
    QJsonObject error;

    if (obj.contains("error") && obj.value("error").isObject()) {
        error = obj.value("error").toObject();
    } else {
        result = obj.value("result");
    }

    emit callFinished(id, result, error);
    dispatchCallback(id, result, error);
    pending_.remove(id);
}

void JsonRpcClient::onSocketError(QAbstractSocket::SocketError)
{
    qWarning() << "RPC socket error:" << sock_.errorString();
    emit transportError(sock_.errorString());
}





