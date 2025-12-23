#include "json_rpc_server.h"
#include "json_rpc_dispatcher.h"

#include <QTcpSocket>
#include <QJsonDocument>



JsonRpcServer::JsonRpcServer(JsonRpcDispatcher* dispatcher, QObject* parent)
    : QTcpServer(parent), dispatcher_(dispatcher)
{
    connect(this, &QTcpServer::newConnection, this, &JsonRpcServer::onNewConnection);
}

void JsonRpcServer::onNewConnection()
{
    // has panding connections
    while (hasPendingConnections()) {
        auto* s = nextPendingConnection();
        buffers_[s] = QByteArray{};
        connect(s, &QTcpSocket::readyRead, this, &JsonRpcServer::onReadyRead);
        connect(s, &QTcpSocket::disconnected, this, &JsonRpcServer::onDisconnected);
    }
}

// when TCP receives data
void JsonRpcServer::onReadyRead()
{
    // determine which socket send the data
    // sender(); return QObject*, 'who triggered this slot'
    // qobject_cast; QT dynamic_cast, if it is not a QTcpSocktet -> return nullptr
    auto* s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;

    buffers_[s].append(s->readAll());
    // try to analyze "complete message"
    processLines(s);
}

void JsonRpcServer::processLines(QTcpSocket *s)
{
    // get buffer
    auto& buf = buffers_[s];

    for (;;) {
        // the end of the first complete message
        const int nl = buf.indexOf('\n');
        if (nl < 0) break;
        // get this line and delete
        const QByteArray line = buf.left(nl);
        buf.remove(0, nl + 1);

        // remove spaces
        const QByteArray trimmed = line.trimmed();
        // skip the blank line
        if (trimmed.isEmpty()) continue;

        // parse json
        QJsonParseError err {};
        const auto doc = QJsonDocument::fromJson(trimmed, &err);
        // handling parsing errors
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            QJsonObject resp {
                {"jsonrpc", "2.0"},
                {"id", QJsonValue(QJsonValue::Null)},
                {"error", QJsonObject{{"code", -32700}, {"message", "Parse error"}}}
            };
            s->write(toLine(resp));
            continue;
        }

        // handle the request normal
        const QJsonObject req = doc.object();
        const QJsonObject resp = dispatcher_->handle(req);
        if (!resp.isEmpty()) {
            s->write(toLine(resp));
        }
    }
}

QByteArray JsonRpcServer::toLine(const QJsonObject& obj)
{
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

void JsonRpcServer::onDisconnected()
{
    auto* s = qobject_cast<QTcpSocket*>(sender());
    if (!s) return;
    buffers_.remove(s);
    s->deleteLater();
}
