#include <QCoreApplication>
#include <QHostAddress>
#include <QDebug>
#include <QDir>

#include "rpc/json_rpc_server.h"
#include "rpc/json_rpc_dispatcher.h"
#include "rpc/rpc_registry.h"

#include "core/core_context.h"
#include "core/core_config.h"

static QString pickConfigPath(const QCoreApplication& app)
{
    Q_UNUSED(app);
    // TODO params
    return "/var/lib/fanzhou_core/core.json";
}


static bool ensureParentDir(const QString& filePath, QString* err=nullptr)
{
    const QString dirPath = QFileInfo(filePath).absolutePath();
    QDir d;
    if (d.exists(dirPath)) return true;
    if (!d.mkpath(dirPath)) {
        if (err) *err = QString("mkpath failed: %1").arg(dirPath);
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // 1) 加载配置
    const QString cfgPath = pickConfigPath(app);

    CoreConfig cfg = CoreConfig::makeDefault();
    QString err;
    if (!cfg.loadFromFile(cfgPath, &err)) {
        qWarning().noquote() << "Load config failed:" << err << "-> write default config to" << cfgPath;

        QString mkErr;
        if (!ensureParentDir(cfgPath, &mkErr)) {
            qWarning().noquote() << "Create config dir failed:" << mkErr;
        } else {
            QString err2;
            if (!cfg.saveToFile(cfgPath, &err2)) {
                qWarning().noquote() << "Write default config failed:" << err2;
            } else {
                qInfo().noquote() << "Default config written to" << cfgPath;
            }
        }
    }

    CoreContext ctx;
    if (!ctx.init(cfg)) {
        qCritical() << "CoreContext init failed";
        return 1;
    }

    // 3) 注册 RPC 方法
    JsonRpcDispatcher dispatcher;
    RpcRegistry reg(&ctx, &dispatcher);
    reg.registerAll();

    // 4)
    JsonRpcServer server(&dispatcher);
    const quint16 port = ctx.rpcPort;
    if (!server.listen(QHostAddress::Any, port)) {
        qCritical() << "listen failed:" << server.errorString();
        return 1;
    }

    qInfo() << "core_main JSON-RPC listening on" << port
            << "config =" << cfgPath;

    return app.exec();
}
