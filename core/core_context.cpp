#include "core_context.h"
#include "core_config.h"
#include "config/system_settings.h"
#include "comm/comm_can.h"
#include "device/can/can_device_manager.h"
#include "device/can/device_relay_can_f427.h"
#include "utils/logger.h"

#include <QDebug>

static const char* LOG_SOURCE = "核心上下文";

CoreContext::CoreContext(QObject* parent) : QObject(parent) {}

/**
 * @brief 使用默认配置初始化核心上下文
 */
bool CoreContext::init()
{
    LOG_INFO(LOG_SOURCE, "正在初始化核心上下文(默认配置)...");
    if (!initSystemSettings()) {
        LOG_ERROR(LOG_SOURCE, "初始化系统设置失败");
        return false;
    }
    if (!initCan()) {
        LOG_ERROR(LOG_SOURCE, "初始化CAN总线失败");
        return false;
    }
    if (!initDevices()) {
        LOG_ERROR(LOG_SOURCE, "初始化设备失败");
        return false;
    }
    LOG_INFO(LOG_SOURCE, "核心上下文初始化完成");
    return true;
}

/**
 * @brief 使用配置文件初始化核心上下文
 * @param cfg 核心配置对象
 */
bool CoreContext::init(const CoreConfig& cfg)
{
    LOG_INFO(LOG_SOURCE, "正在使用配置初始化核心上下文...");
    LOG_DEBUG(LOG_SOURCE, QString("RPC端口: %1, CAN接口: %2, 波特率: %3")
              .arg(cfg.core_.rpcPort).arg(cfg.can_.canIfname).arg(cfg.can_.canBitrate));
    
    rpcPort = cfg.core_.rpcPort;
    canIfname = cfg.can_.canIfname;
    canBitrate = cfg.can_.canBitrate;
    tripleSampling = cfg.can_.canTripleSampling;

    if (!initSystemSettings()) {
        LOG_ERROR(LOG_SOURCE, "初始化系统设置失败");
        return false;
    }
    if (!initCan()) {
        LOG_ERROR(LOG_SOURCE, "初始化CAN总线失败");
        return false;
    }

    // 从配置初始化设备
    if (!initDevices(cfg)) {
        LOG_ERROR(LOG_SOURCE, "从配置初始化设备失败");
        return false;
    }
    
    LOG_INFO(LOG_SOURCE, "核心上下文初始化完成");
    return true;
}

/**
 * @brief 初始化系统设置
 */
bool CoreContext::initSystemSettings()
{
    LOG_DEBUG(LOG_SOURCE, "正在初始化系统设置...");
    sys = new SystemSettings(this);

    // 连接信号
    connect(sys, &SystemSettings::commandOutput, this, [](const QString& s){
        LOG_DEBUG("系统设置", QString("[输出] %1").arg(s));
    });

    connect(sys, &SystemSettings::errorOccurred, this, [](const QString& s){
        LOG_WARNING("系统设置", QString("[错误] %1").arg(s));
    });

    connect(sys, &SystemSettings::candumpLine, this, [](const QString& line){
        LOG_DEBUG("CAN抓包", line);
    });

    LOG_INFO(LOG_SOURCE, QString("设置CAN波特率: 接口=%1, 波特率=%2, 三重采样=%3")
             .arg(canIfname).arg(canBitrate).arg(tripleSampling));
    sys->setCanBitrate(canIfname, canBitrate, tripleSampling);
    return true;
}

/**
 * @brief 初始化CAN总线通讯
 */
bool CoreContext::initCan()
{
    LOG_DEBUG(LOG_SOURCE, "正在初始化CAN总线...");
    CanConfig cfg;
    cfg.ifname = canIfname;
    cfg.canFd = false;

    canBus = new CommCan(cfg, this);
    connect(canBus, &CommCan::errorOccurred, this, [](const QString& e){
        LOG_ERROR("CAN", QString("错误: %1").arg(e));
    });

    if (!canBus->open()) {
        LOG_WARNING(LOG_SOURCE, "CAN打开失败，RPC服务仍将启动，但CAN方法将无法使用!");
        // 不返回false，允许RPC服务正常启动
    } else {
        LOG_INFO(LOG_SOURCE, QString("CAN总线打开成功: %1").arg(canIfname));
    }

    canMgr = new CanDeviceManager(canBus, this);
    LOG_DEBUG(LOG_SOURCE, "CAN设备管理器已创建");
    return true;
}

/**
 * @brief 使用默认配置初始化设备
 */
bool CoreContext::initDevices()
{
    LOG_DEBUG(LOG_SOURCE, "正在初始化设备(默认模式)...");
    const QList<quint8> nodes = {0x01};
    for (auto node : nodes) {
        auto* d = new RelayCanDeviceGD427(node, canBus, this);
        d->init();
        canMgr->addDevice(d);
        relays.insert(node, d);
        LOG_INFO(LOG_SOURCE, QString("继电器设备已添加: 节点=0x%1").arg(node, 2, 16, QChar('0')));
    }
    return true;
}

/**
 * @brief 使用配置文件初始化设备
 * @param cfg 核心配置对象
 */
bool CoreContext::initDevices(const CoreConfig& cfg)
{
    LOG_DEBUG(LOG_SOURCE, "正在从配置初始化设备...");
    // 清除旧设备
    relays.clear();
    if (!cfg.devices_.isEmpty()) {
        LOG_INFO(LOG_SOURCE, QString("在配置中发现 %1 个设备").arg(cfg.devices_.size()));
        
        for (const auto& dcfg : cfg.devices_) {

            // 检查设备是否启用
            const bool enabled = dcfg.params.value("enabled").toBool(true);
            if (!enabled) {
                LOG_DEBUG(LOG_SOURCE, QString("设备 '%1' 已禁用，跳过").arg(dcfg.name));
                continue;
            }

            // RelayGD427 + CAN 设备类型
            if (dcfg.deviceType == DeviceTypeId::RelayGD427 &&
                dcfg.commType == CommTypeId::Can) {

                if (dcfg.node_id < 1 || dcfg.node_id > 255) {
                    LOG_WARNING(LOG_SOURCE, QString("配置中节点ID无效: %1, 名称=%2")
                                .arg(dcfg.node_id).arg(dcfg.name));
                    continue;
                }

                const quint8 node = quint8(dcfg.node_id);

                // 防止重复添加
                if (relays.contains(node)) {
                    LOG_WARNING(LOG_SOURCE, QString("配置中继电器节点重复: %1, 跳过").arg(int(node)));
                    continue;
                }

                auto* dev = new RelayCanDeviceGD427(node, canBus, this);
                dev->init();
                canMgr->addDevice(dev);
                relays.insert(node, dev);

                LOG_INFO(LOG_SOURCE, QString("RelayGD427已添加: 节点=0x%1, 名称=%2")
                         .arg(node, 2, 16, QChar('0')).arg(dcfg.name));
            } else {
                LOG_WARNING(LOG_SOURCE, QString("不支持的设备类型/通讯类型: %1/%2, 名称=%3")
                            .arg(int(dcfg.deviceType)).arg(int(dcfg.commType)).arg(dcfg.name));
            }
        }

        // 加载设备组
        deviceGroups.clear();
        groupNames.clear();
        deviceGroups.reserve(cfg.groups_.size());
        groupNames.reserve(cfg.groups_.size());
        
        LOG_INFO(LOG_SOURCE, QString("正在加载 %1 个设备组...").arg(cfg.groups_.size()));
        
        for (const auto& gcfg : cfg.groups_) {
            if (!gcfg.enabled) {
                LOG_DEBUG(LOG_SOURCE, QString("设备组 '%1' 已禁用，跳过").arg(gcfg.name));
                continue;
            }
            
            QList<quint8> nodes;
            nodes.reserve(gcfg.deviceNodes.size());
            for (int nodeId : gcfg.deviceNodes) {
                if (nodeId >= 1 && nodeId <= 255) {
                    nodes.append(quint8(nodeId));
                }
            }
            
            deviceGroups.insert(gcfg.groupId, nodes);
            groupNames.insert(gcfg.groupId, gcfg.name);
            
            LOG_INFO(LOG_SOURCE, QString("设备组已添加: ID=%1, 名称=%2, 设备数=%3")
                     .arg(gcfg.groupId).arg(gcfg.name).arg(nodes.size()));
        }

        return true;
    }
    // 没有配置设备时的警告
    LOG_WARNING(LOG_SOURCE, "未配置任何设备(devices_为空)");
    return true;
}

QStringList CoreContext::methodGroups() const
{
    return {"rpc.*", "sys.*", "can.*", "relay.*", "group.*"};
}








