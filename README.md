# MPF Orders Plugin

Qt Modular Plugin Framework - Orders Plugin (Example)

## 概述

这是一个示例业务插件，展示如何：
- 实现 IPlugin 接口
- 注册路由和菜单
- 使用 MPF UI 组件
- QML 与 C++ 交互

## 依赖

- Qt 6.8+ (Core, Gui, Qml, Quick, Network)
- mpf-sdk
- mpf-http-client
- mpf-ui-components

## 构建

```bash
export QT_DIR=/path/to/qt6
export MPF_SDK=/path/to/mpf-sdk

# 使用 CMake Presets
cmake --preset linux-debug
cmake --build build/linux-debug

# 或手动配置
cmake -B build -G Ninja \
    -DCMAKE_PREFIX_PATH="$QT_DIR;$MPF_SDK" \
    -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 功能

- 订单列表页面
- 订单详情页面
- 创建订单对话框
- 菜单项和徽章

## EventBus 通信

通过 EventBus 实现与其他插件的松耦合通信：

```cpp
// 发布订单创建事件
bus->publish("orders/created", {{"id", orderId}}, pluginId);

// 处理其他插件的数据请求
bus->registerHandler("orders/getById", pluginId, [this](const Event& e) -> QVariantMap {
    return getOrder(e.data["id"].toString()).toVariantMap();
});
```

## 许可证

MIT License
