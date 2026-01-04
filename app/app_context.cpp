#include "app_context.h"
#include "rpc/json_rpc_client.h"
#include <QDebug>

AppContext& AppContext::instance()
{
    static AppContext ctx;
    return ctx;
}

void AppContext::init(QObject* parentForServices)
{
    if (initialized_) {
        qDebug() << "[AppContext] Already initialized, skipping";
        return;
    }
    
    qDebug() << "[AppContext] Initializing...";
    
    // Create RPC client
    rpc_ = new JsonRpcClient(parentForServices);
    if (!rpc_) {
        qCritical() << "[AppContext] Failed to create RPC client";
        return;
    }
    
    // Connect RPC status signals for logging
    connect(rpc_, &JsonRpcClient::connected, this, []() {
        qDebug() << "[AppContext] RPC client connected";
    });
    
    connect(rpc_, &JsonRpcClient::disconnected, this, []() {
        qDebug() << "[AppContext] RPC client disconnected";
    });
    
    connect(rpc_, &JsonRpcClient::transportError, this, [](const QString& error) {
        qWarning() << "[AppContext] RPC transport error:" << error;
    });
    
    initialized_ = true;
    qDebug() << "[AppContext] Initialization complete";
}

bool AppContext::isInitialized() const
{
    return initialized_;
}
