# RPC API 参考文档

## 概述
本文档描述了设备组管理相关的RPC命令接口。所有命令使用JSON-RPC 2.0协议。

## 基本格式

### 请求格式
```json
{
  "jsonrpc": "2.0",
  "method": "方法名",
  "params": { 参数对象 },
  "id": 请求ID
}
```

### 成功响应格式
```json
{
  "jsonrpc": "2.0",
  "result": { 结果对象 },
  "id": 请求ID
}
```

### 错误响应格式
```json
{
  "jsonrpc": "2.0",
  "error": {
    "code": 错误代码,
    "message": "错误消息"
  },
  "id": 请求ID
}
```

## 设备组管理命令

### group.list
列出所有设备组。

**参数:** 无

**请求示例:**
```json
{
  "jsonrpc": "2.0",
  "method": "group.list",
  "params": {},
  "id": 1
}
```

**响应示例:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "ok": true,
    "groups": [
      {
        "groupId": 1,
        "name": "主控组",
        "devices": [1, 2, 3],
        "deviceCount": 3
      },
      {
        "groupId": 2,
        "name": "备用组",
        "devices": [4],
        "deviceCount": 1
      }
    ]
  },
  "id": 1
}
```

---

### group.create
创建一个新的设备组。

**参数:**
- `groupId` (integer, 必需): 组ID，必须大于0且唯一
- `name` (string, 必需): 组名称

**请求示例:**
```json
{
  "jsonrpc": "2.0",
  "method": "group.create",
  "params": {
    "groupId": 1,
    "name": "主控组"
  },
  "id": 2
}
```

**成功响应:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "ok": true,
    "groupId": 1
  },
  "id": 2
}
```

**错误情况:**
- 组ID已存在
- 组ID无效（小于等于0）
- 参数缺失或类型错误

---

### group.delete
删除一个设备组。

**参数:**
- `groupId` (integer, 必需): 要删除的组ID

**请求示例:**
```json
{
  "jsonrpc": "2.0",
  "method": "group.delete",
  "params": {
    "groupId": 1
  },
  "id": 3
}
```

**成功响应:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "ok": true
  },
  "id": 3
}
```

**错误情况:**
- 组不存在
- 参数缺失或类型错误

---

### group.addDevice
向设备组中添加一个设备。

**参数:**
- `groupId` (integer, 必需): 组ID
- `node` (integer, 必需): 设备节点ID (1-255)

**请求示例:**
```json
{
  "jsonrpc": "2.0",
  "method": "group.addDevice",
  "params": {
    "groupId": 1,
    "node": 2
  },
  "id": 4
}
```

**成功响应:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "ok": true
  },
  "id": 4
}
```

**错误情况:**
- 组不存在
- 设备不存在
- 设备节点ID无效
- 参数缺失或类型错误

**注意:** 如果设备已经在组中，不会重复添加。

---

### group.removeDevice
从设备组中移除一个设备。

**参数:**
- `groupId` (integer, 必需): 组ID
- `node` (integer, 必需): 设备节点ID

**请求示例:**
```json
{
  "jsonrpc": "2.0",
  "method": "group.removeDevice",
  "params": {
    "groupId": 1,
    "node": 2
  },
  "id": 5
}
```

**成功响应:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "ok": true
  },
  "id": 5
}
```

**错误情况:**
- 组不存在
- 参数缺失或类型错误

**注意:** 如果设备不在组中，操作仍然成功。

---

### group.control
批量控制设备组中的所有设备。

**参数:**
- `groupId` (integer, 必需): 组ID
- `ch` (integer, 必需): 通道号 (0-3)
- `action` (string, 必需): 动作，可选值:
  - `"stop"` 或 `"0"` - 停止
  - `"fwd"` 或 `"forward"` 或 `"1"` - 正转
  - `"rev"` 或 `"reverse"` 或 `"2"` - 反转

**请求示例:**
```json
{
  "jsonrpc": "2.0",
  "method": "group.control",
  "params": {
    "groupId": 1,
    "ch": 0,
    "action": "fwd"
  },
  "id": 6
}
```

**成功响应:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "ok": true,
    "total": 3,
    "success": 3,
    "failed": 0
  },
  "id": 6
}
```

**响应字段说明:**
- `total`: 组内设备总数
- `success`: 成功控制的设备数量
- `failed`: 失败的设备数量

**错误情况:**
- 组不存在
- 通道号无效（不在0-3范围内）
- 动作参数无效
- 参数缺失或类型错误

---

## 错误代码

| 代码 | 名称 | 说明 |
|------|------|------|
| 1 | MissingParameter | 缺少必需参数 |
| 2 | BadParameterType | 参数类型错误 |
| 3 | BadParameterValue | 参数值无效 |
| 4 | InvalidState | 系统状态无效 |

## 使用流程示例

### 创建并管理设备组的完整流程

```javascript
// 1. 创建设备组
rpc.call("group.create", {groupId: 1, name: "主控组"})

// 2. 添加设备到组
rpc.call("group.addDevice", {groupId: 1, node: 1})
rpc.call("group.addDevice", {groupId: 1, node: 2})
rpc.call("group.addDevice", {groupId: 1, node: 3})

// 3. 列出所有组查看结果
rpc.call("group.list", {})

// 4. 批量控制组内设备
rpc.call("group.control", {groupId: 1, ch: 0, action: "fwd"})

// 5. 从组中移除一个设备
rpc.call("group.removeDevice", {groupId: 1, node: 3})

// 6. 删除整个组
rpc.call("group.delete", {groupId: 1})
```

## 与其他命令的配合使用

### 查询设备状态
```javascript
// 先获取组内的设备列表
const groupInfo = await rpc.call("group.list", {})
const devices = groupInfo.result.groups[0].devices

// 然后查询每个设备的状态
for (const node of devices) {
  const status = await rpc.call("relay.statusAll", {node: node})
  console.log(status)
}
```

### 条件控制
```javascript
// 查询设备状态，根据条件控制组
const status = await rpc.call("relay.status", {node: 1, ch: 0})
if (status.result.currentA > 5.0) {
  // 电流过大，停止整个组
  await rpc.call("group.control", {
    groupId: 1,
    ch: 0,
    action: "stop"
  })
}
```

## 注意事项

1. **并发控制**: group.control会依次控制组内所有设备，如果设备较多可能需要一些时间
2. **错误处理**: 即使部分设备控制失败，命令仍会继续执行其他设备
3. **组ID管理**: 组ID由用户管理，建议使用有意义的编号
4. **设备验证**: 添加设备到组时会验证设备是否存在
5. **持久化**: 通过RPC创建的组不会自动保存到配置文件，需要手动更新配置

## 客户端示例

### Python客户端
```python
import json
import socket

class RPCClient:
    def __init__(self, host='127.0.0.1', port=12345):
        self.host = host
        self.port = port
        self.id = 0
    
    def call(self, method, params):
        self.id += 1
        request = {
            "jsonrpc": "2.0",
            "method": method,
            "params": params,
            "id": self.id
        }
        
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((self.host, self.port))
        sock.send(json.dumps(request).encode() + b'\n')
        response = sock.recv(4096)
        sock.close()
        
        return json.loads(response)

# 使用示例
client = RPCClient()
result = client.call("group.list", {})
print(result)
```

### JavaScript客户端 (Node.js)
```javascript
const net = require('net')

class RPCClient {
  constructor(host = '127.0.0.1', port = 12345) {
    this.host = host
    this.port = port
    this.id = 0
  }
  
  call(method, params) {
    return new Promise((resolve, reject) => {
      const client = net.connect(this.port, this.host)
      const request = {
        jsonrpc: '2.0',
        method: method,
        params: params,
        id: ++this.id
      }
      
      client.write(JSON.stringify(request) + '\n')
      
      client.on('data', (data) => {
        resolve(JSON.parse(data.toString()))
        client.end()
      })
      
      client.on('error', reject)
    })
  }
}

// 使用示例
const client = new RPCClient()
client.call('group.list', {}).then(result => {
  console.log(result)
})
```

## 版本历史

- v1.0 (2026-01-04): 初始版本，添加group.*命令族
