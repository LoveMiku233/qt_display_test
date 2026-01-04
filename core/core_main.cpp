#include <QCoreApplication>
#include <QHostAddress>
#include <QDebug>
#include <QDir>

#include "rpc/json_rpc_server.h"
#include "rpc/json_rpc_dispatcher.h"
#include "rpc/rpc_registry.h"

#include "core/core_context.h"
#include "core/core_config.h"
#include "utils/logger.h"

static const char* LOG_SOURCE = "CoreMain";
static const QString DEFAULT_LOG_PATH = "/var/log/fanzhou_core/core.log";

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

    // Initialize logger
    Logger::instance().init(DEFAULT_LOG_PATH, LogLevel::Debug);
    LOG_INFO(LOG_SOURCE, "Core server starting...");

    // 1) 加载配置
    const QString cfgPath = pickConfigPath(app);
    LOG_INFO(LOG_SOURCE, QString("Loading configuration from: %1").arg(cfgPath));

    CoreConfig cfg = CoreConfig::makeDefault();
    QString err;
    if (!cfg.loadFromFile(cfgPath, &err)) {
        LOG_WARNING(LOG_SOURCE, QString("Load config failed: %1 -> write default config").arg(err));

        QString mkErr;
        if (!ensureParentDir(cfgPath, &mkErr)) {
            LOG_ERROR(LOG_SOURCE, QString("Create config dir failed: %1").arg(mkErr));
        } else {
            QString err2;
            if (!cfg.saveToFile(cfgPath, &err2)) {
                LOG_ERROR(LOG_SOURCE, QString("Write default config failed: %1").arg(err2));
            } else {
                LOG_INFO(LOG_SOURCE, QString("Default config written to: %1").arg(cfgPath));
            }
        }
    } else {
        LOG_INFO(LOG_SOURCE, "Configuration loaded successfully");
    }

    CoreContext ctx;
    LOG_INFO(LOG_SOURCE, "Initializing CoreContext...");
    if (!ctx.init(cfg)) {
        LOG_CRITICAL(LOG_SOURCE, "CoreContext init failed");
        return 1;
    }
    LOG_INFO(LOG_SOURCE, "CoreContext initialized successfully");

    // 3) 注册 RPC 方法
    LOG_INFO(LOG_SOURCE, "Registering RPC methods...");
    JsonRpcDispatcher dispatcher;
    RpcRegistry reg(&ctx, &dispatcher);
    reg.registerAll();
    LOG_INFO(LOG_SOURCE, "RPC methods registered");

    // 4)
    JsonRpcServer server(&dispatcher);
    const quint16 port = ctx.rpcPort;
    LOG_INFO(LOG_SOURCE, QString("Starting JSON-RPC server on port %1...").arg(port));
    if (!server.listen(QHostAddress::Any, port)) {
        LOG_CRITICAL(LOG_SOURCE, QString("Listen failed: %1").arg(server.errorString()));
        return 1;
    }

    LOG_INFO(LOG_SOURCE, QString("Core server started successfully. JSON-RPC listening on port %1, config=%2")
             .arg(port).arg(cfgPath));

    return app.exec();
}
