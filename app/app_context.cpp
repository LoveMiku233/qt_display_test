#include "app_context.h"
#include "app_config.h"
#include "rpc/json_rpc_client.h"
#include <QDebug>

/**
 * @brief 获取单例实例
 */
AppContext& AppContext::instance()
{
    static AppContext ctx;
    return ctx;
}

/**
 * @brief 初始化应用程序上下文
 * @param parentForServices 服务对象的父对象
 */
void AppContext::init(QObject* parentForServices)
{
    if (initialized_) {
        qDebug() << "[应用上下文] 已初始化，跳过";
        return;
    }
    
    qDebug() << "[应用上下文] 正在初始化...";
    
    // 加载应用配置
    const QString configPath = AppConfig::defaultConfigPath();
    QString err;
    if (!config_.loadFromFile(configPath, &err)) {
        qDebug() << "[应用上下文] 加载配置失败:" << err << "-> 使用默认配置";
        config_ = AppConfig::makeDefault();
        
        // 尝试保存默认配置
        QString saveErr;
        if (!config_.saveToFile(configPath, &saveErr)) {
            qWarning() << "[应用上下文] 保存默认配置失败:" << saveErr;
        } else {
            qDebug() << "[应用上下文] 已保存默认配置到:" << configPath;
        }
    } else {
        qDebug() << "[应用上下文] 配置加载成功:" << configPath;
    }
    
    // 创建RPC客户端
    rpc_ = new JsonRpcClient(parentForServices);
    if (!rpc_) {
        qCritical() << "[应用上下文] 创建RPC客户端失败";
        return;
    }
    
    // 连接RPC状态信号用于日志记录
    connect(rpc_, &JsonRpcClient::connected, this, []() {
        qDebug() << "[应用上下文] RPC客户端已连接";
    });
    
    connect(rpc_, &JsonRpcClient::disconnected, this, []() {
        qDebug() << "[应用上下文] RPC客户端已断开";
    });
    
    connect(rpc_, &JsonRpcClient::transportError, this, [](const QString& error) {
        qWarning() << "[应用上下文] RPC传输错误:" << error;
    });
    
    initialized_ = true;
    qDebug() << "[应用上下文] 初始化完成";
}

/**
 * @brief 检查上下文是否已初始化
 */
bool AppContext::isInitialized() const
{
    return initialized_;
}

/**
 * @brief 重新加载配置
 */
bool AppContext::reloadConfig()
{
    const QString configPath = AppConfig::defaultConfigPath();
    QString err;
    if (!config_.loadFromFile(configPath, &err)) {
        qWarning() << "[应用上下文] 重新加载配置失败:" << err;
        return false;
    }
    qDebug() << "[应用上下文] 配置重新加载成功";
    return true;
}

/**
 * @brief 保存当前配置
 */
bool AppContext::saveConfig()
{
    const QString configPath = AppConfig::defaultConfigPath();
    QString err;
    if (!config_.saveToFile(configPath, &err)) {
        qWarning() << "[应用上下文] 保存配置失败:" << err;
        return false;
    }
    qDebug() << "[应用上下文] 配置保存成功";
    return true;
}
