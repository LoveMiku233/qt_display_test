# Qt Display Test - 设备控制系统

## 项目简介
这是一个基于Qt的设备控制和监控系统，支持CAN总线通信，提供设备管理、分组控制和自动化功能。

## 主要功能

### 1. 设备管理
- 支持多个继电器设备节点管理
- 通过CAN总线通信
- 实时设备状态监控

### 2. 设备组管理 (新增)
- 创建和管理设备组
- 将多个设备添加到组中
- 批量控制组内所有设备
- 组列表查看和编辑

### 3. 自动控制 (新增)
- 定时自动执行控制任务
- 可配置执行间隔
- 运行日志记录
- 启动/停止自动控制

### 4. 参数设置 (优化)
- 系统参数配置（RPC端口、日志级别）
- 通讯参数配置（CAN接口、波特率）
- 设备列表查看
- 分标签页管理不同类型设置

### 5. RPC命令接口

#### 设备组相关命令
- `group.list` - 列出所有设备组
- `group.create` - 创建新组 (参数: groupId, name)
- `group.delete` - 删除组 (参数: groupId)
- `group.addDevice` - 添加设备到组 (参数: groupId, node)
- `group.removeDevice` - 从组移除设备 (参数: groupId, node)
- `group.control` - 控制组内所有设备 (参数: groupId, ch, action)

#### 继电器控制命令
- `relay.control` - 控制单个继电器 (参数: node, ch, action)
- `relay.query` - 查询继电器状态 (参数: node, ch)
- `relay.status` - 获取继电器详细状态 (参数: node, ch)
- `relay.statusAll` - 获取节点所有通道状态 (参数: node)
- `relay.nodes` - 列出所有继电器节点

## 配置文件格式

配置文件支持设备和组的定义：

```json
{
  "main": {
    "rpcPort": 12345
  },
  "can": {
    "ifname": "can0",
    "bitrate": 125000,
    "tripleSampling": true,
    "canFd": false
  },
  "devices": [
    {
      "name": "relay01",
      "type": 1,
      "commType": 2,
      "nodeId": 1,
      "bus": "can0",
      "params": {
        "channels": 4,
        "enabled": true
      }
    }
  ],
  "groups": [
    {
      "groupId": 1,
      "name": "主控组",
      "enabled": true,
      "devices": [1, 2, 3]
    }
  ]
}
```

## 编译和运行

### 依赖项
- Qt 5.x (Core, GUI, Widgets, Network)
- C++11 或更高版本
- Linux系统（用于CAN总线支持）

### 编译步骤
```bash
qmake qt_display_yk_1.pro
make
```

### 运行
```bash
# 启动核心服务器
cd core
./core_server

# 启动客户端应用
cd app
./qt_display_yk_1
```

## 界面导航

左侧菜单包含以下页面：
1. **主监控** - 主要监控界面
2. **设备控制** - 设备手动控制
3. **参数设置** - 系统和设备参数配置
4. **设备组** - 设备组管理（新增）
5. **自动控制** - 自动化控制配置（新增）
6. **通讯状态** - 通讯状态监控
7. **系统日志** - 系统日志查看

## 代码优化

本次更新包含以下优化：
1. 修复了 RPC helpers 中 getU8 的类型转换错误
2. 在加载配置时使用 reserve() 预分配内存提高性能
3. 改进了错误处理和用户反馈
4. 添加了更多的输入验证
5. 优化了 UI 布局和用户体验

## 许可证
请查看项目许可证文件
