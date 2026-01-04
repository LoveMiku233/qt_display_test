#ifndef CORE_CONTEXT_H
#define CORE_CONTEXT_H

#include <QObject>
#include <QHash>
#include "core/core_config.h"

// 前向声明
class SystemSettings;
class CommCan;
class CanDeviceManager;
class RelayCanDeviceGD427;

struct CanConfig;

/**
 * @brief 核心上下文类
 * 
 * 管理大棚控制系统的核心组件，包括系统设置、CAN总线通讯、
 * 设备管理器以及继电器设备等
 */
class CoreContext : public QObject
{
    Q_OBJECT
public:
    explicit CoreContext(QObject* parent = nullptr);
    
    /**
     * @brief 使用默认配置初始化
     * @return 是否成功
     */
    bool init();
    
    /**
     * @brief 使用指定配置初始化
     * @param cfg 核心配置对象
     * @return 是否成功
     */
    bool init(const CoreConfig& cfg);
    
    /**
     * @brief 获取RPC方法组列表
     * @return 方法组名称列表
     */
    QStringList methodGroups() const;
    
    // 系统组件指针
    SystemSettings* sys = nullptr;          // 系统设置
    CommCan* canBus = nullptr;              // CAN总线通讯
    CanDeviceManager* canMgr = nullptr;     // CAN设备管理器

    // 设备注册表: 节点ID -> 设备
    QHash<quint8, RelayCanDeviceGD427*> relays;

    // 设备组: 组ID -> 节点ID列表
    QHash<int, QList<quint8>> deviceGroups;
    QHash<int, QString> groupNames;

    // CAN总线配置
    QString canIfname = "can0";             // CAN接口名称
    int canBitrate = 125000;                // 波特率
    bool tripleSampling = true;             // 三重采样

    // 服务器配置
    quint16 rpcPort = 12345;                // RPC服务端口


private:
    /**
     * @brief 初始化系统设置
     */
    bool initSystemSettings();
    
    /**
     * @brief 初始化CAN总线
     */
    bool initCan();

    /**
     * @brief 使用默认配置初始化设备
     */
    bool initDevices();
    
    /**
     * @brief 使用配置文件初始化设备
     * @param cfg 核心配置对象
     */
    bool initDevices(const CoreConfig& cfg);
};

#endif // CORE_CONTEXT_H
