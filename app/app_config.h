#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <QString>
#include <QJsonObject>

/**
 * @brief 应用界面配置结构体
 * 
 * 管理应用程序界面相关的配置参数
 */
struct UiConfig {
    QString theme = "dark";                               // 主题: "dark" 或 "light"
    QString language = "zh_CN";                           // 语言设置
    int fontSize = 14;                                    // 字体大小
    bool enableAnimations = true;                         // 是否启用动画效果
    int animationDuration = 250;                          // 动画持续时间(毫秒)
};

/**
 * @brief RPC连接配置结构体
 */
struct RpcConfig {
    QString serverHost = "127.0.0.1";                     // 服务器地址
    quint16 serverPort = 12345;                           // 服务器端口
    int reconnectInterval = 5000;                         // 重连间隔(毫秒)
    bool autoReconnect = true;                            // 是否自动重连
};

/**
 * @brief 应用日志配置结构体
 */
struct AppLogConfig {
    bool logToConsole = true;                             // 是否输出到终端
    bool logToFile = false;                               // 是否输出到文件
    QString logFilePath = "logs/app.log";                 // 日志文件路径
    int logLevel = 0;                                     // 日志级别
};

/**
 * @brief 应用配置类
 * 
 * 管理大棚控制系统客户端应用的所有配置，
 * 包括界面设置、RPC连接参数和日志配置等
 */
class AppConfig
{
public:
    UiConfig ui_;                                         // 界面配置
    RpcConfig rpc_;                                       // RPC连接配置
    AppLogConfig log_;                                    // 日志配置

    /**
     * @brief 从文件加载配置
     * @param path 配置文件路径
     * @param err 错误信息输出
     * @return 是否成功
     */
    bool loadFromFile(const QString& path, QString* err = nullptr);
    
    /**
     * @brief 保存配置到文件
     * @param path 配置文件路径
     * @param err 错误信息输出
     * @return 是否成功
     */
    bool saveToFile(const QString& path, QString* err = nullptr) const;
    
    /**
     * @brief 生成默认配置
     * @return 默认配置对象
     */
    static AppConfig makeDefault();
    
    /**
     * @brief 获取默认配置文件路径
     * @return 配置文件路径
     */
    static QString defaultConfigPath();
};

#endif // APP_CONFIG_H
