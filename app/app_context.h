#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <QObject>
#include "app_config.h"

class JsonRpcClient;

/**
 * @brief 应用程序上下文单例类
 * 
 * 提供对共享服务的集中访问，如RPC客户端和应用配置，
 * 具有正确的初始化状态跟踪和错误处理功能
 */
class AppContext : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 获取单例实例
     */
    static AppContext& instance();

    /**
     * @brief 初始化应用程序上下文
     * @param parentForServices 服务对象的父对象
     */
    void init(QObject* parentForServices = nullptr);

    /**
     * @brief 检查上下文是否已初始化
     */
    bool isInitialized() const;
    
    /**
     * @brief 获取RPC客户端
     * @return RPC客户端指针（未初始化时可能为nullptr）
     */
    JsonRpcClient* rpc() const { return rpc_; }
    
    /**
     * @brief 获取应用配置
     * @return 应用配置引用
     */
    const AppConfig& config() const { return config_; }
    
    /**
     * @brief 获取可修改的应用配置
     * @return 应用配置引用
     */
    AppConfig& config() { return config_; }
    
    /**
     * @brief 重新加载配置
     * @return 是否成功
     */
    bool reloadConfig();
    
    /**
     * @brief 保存当前配置
     * @return 是否成功
     */
    bool saveConfig();

private:
    AppContext() = default;

    JsonRpcClient* rpc_ = nullptr;
    AppConfig config_;
    bool initialized_ = false;
};

#endif // APP_CONTEXT_H
