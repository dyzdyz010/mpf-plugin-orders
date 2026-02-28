# MPF Orders Plugin

> 📖 **[MPF 开发环境完整教程](https://github.com/QMPF/mpf-dev/blob/main/docs/USAGE.md)** — 安装指南、命令参考、开发流程、IDE 配置、常见问题

Qt Modular Plugin Framework - 订单管理插件（示例）

## 概述

这是一个完整的业务插件示例，展示如何：
- 实现 `IPlugin` 接口（initialize / start / stop 生命周期）
- 注册路由和菜单项
- 使用 MPF UI 组件（`import MPF.Components 1.0`）
- QML 与 C++ 交互（Service 单例 + ListModel）
- 使用 Theme 主题适配

## 依赖

- Qt 6.8+（Core, Gui, Qml, Quick, Network）
- MPF foundation-sdk
- MPF http-client
- MPF ui-components（运行时通过 QML import 加载，**不链接**）

## 构建

```bash
# 1. 确保已安装 mpf-dev 和 SDK
mpf-dev setup

# 2. 初始化项目（自动生成 CMakeUserPresets.json）
mpf-dev init

# 3. 配置和构建
cmake --preset dev
cmake --build build
```

## 开发调试

```bash
# 注册插件到 dev.json
mpf-dev link plugin orders ./build

# 运行（使用 SDK 的 Host + 你的插件）
mpf-dev run

# 或在 Qt Creator 中：
# 1. 打开 CMakeLists.txt，选择 dev preset
# 2. 构建
# 3. 通过 mpf-dev run 运行，或直接运行 Host（Host 自动读取 dev.json）
```

## 功能

- 订单列表页面（ListView + OrderCard）
- 订单详情弹窗（Popup）
- 创建订单对话框（MPFDialog）
- 侧边栏菜单项 + 数量徽章
- 状态筛选（ComboBox filter）
- 统计卡片（Total Orders / Revenue）

## 项目结构

```
mpf-plugin-orders/
├── CMakeLists.txt           # 构建配置（find_package(MPF)）
├── include/
│   ├── orders_plugin.h      # IPlugin 接口实现
│   ├── orders_service.h     # 业务服务（Q_INVOKABLE 方法）
│   └── order_model.h        # QAbstractListModel 子类
├── src/
│   ├── orders_plugin.cpp    # 插件生命周期、路由/菜单注册
│   ├── orders_service.cpp   # CRUD 业务逻辑
│   └── order_model.cpp      # 列表数据模型
└── qml/
    ├── OrdersPage.qml       # 主页面
    ├── OrderCard.qml         # 列表项卡片
    └── CreateOrderDialog.qml # 创建对话框
```

## License

MIT
