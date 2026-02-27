# MPF Plugin Orders 样例代码

本目录展示订单插件如何使用 MPF 框架的全部能力，重点是跨插件通信。

## 文件说明

| 文件 | 展示内容 |
|------|----------|
| `01_eventbus_publish.cpp` | C++ 层通过 EventBus 发布事件 |
| `02_eventbus_subscribe.cpp` | C++ 层订阅和接收事件 |
| `03_service_registry_usage.cpp` | 通过 ServiceRegistry 注册和获取服务 |
| `04_http_client_usage.cpp` | 使用 mpf-http-client 发起网络请求 |
| `05_eventbus_qml.qml` | QML 层的 EventBus 使用 |
| `06_full_plugin_example.cpp` | 完整的插件启动流程（含 EventBus 集成） |

## 关键依赖关系

```
编译时依赖：
  mpf-plugin-orders → mpf-sdk (C++ include，接口定义)
  mpf-plugin-orders → mpf-http-client (link，HTTP 请求)

运行时依赖：
  OrdersPage.qml → MPF.Components (import，UI 组件)
  OrdersPage.qml → Theme (context property，主题)
  OrdersPage.qml → EventBus (context property，事件总线)
```

## 跨插件通信流程

```
orders 插件创建订单
    ↓ EventBus.publish("orders/created", {...})
rules 插件收到事件
    ↓ Connections { target: EventBus; onEventPublished: ... }
rules 插件执行规则检查
    ↓ EventBus.publish("rules/check/completed", {...})
orders 插件收到检查结果
```
