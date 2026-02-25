/**
 * =============================================================================
 * Orders Plugin - 插件主类实现
 * =============================================================================
 * 
 * 这是 MPF 插件主类的实现模板，展示了完整的插件生命周期管理。
 * 
 * 【插件生命周期】
 * 1. 构造函数 - 插件对象创建（由 Qt 插件系统调用）
 * 2. initialize() - 初始化阶段，创建服务、注册类型
 * 3. start() - 启动阶段，注册路由、菜单、加载数据
 * 4. stop() - 停止阶段，保存数据、清理资源
 * 5. 析构函数 - 插件对象销毁
 * 
 * 【创建新插件时需要修改的地方】
 * 1. 包含的头文件
 * 2. 命名空间
 * 3. 元数据 JSON 内容
 * 4. 路由注册的路径
 * 5. 菜单项配置
 * 6. QML 类型注册
 * =============================================================================
 */

#include "orders_plugin.h"
#include "orders_service.h"
#include "order_model.h"

// MPF SDK 头文件
#include <mpf/service_registry.h>        // 服务注册表
#include <mpf/interfaces/inavigation.h>  // 导航服务接口
#include <mpf/interfaces/imenu.h>        // 菜单服务接口
#include <mpf/logger.h>                  // 日志宏

#include <QJsonDocument>
#include <QQmlEngine>
#include <QCoreApplication>
#include <QDir>
#include <QUrl>
#include <QFile>

// 【修改点1】命名空间
namespace orders {

// =============================================================================
// 构造/析构
// =============================================================================

OrdersPlugin::OrdersPlugin(QObject* parent)
    : QObject(parent)
{
    // 构造函数中不要做太多初始化工作
    // 主要初始化放在 initialize() 中
}

OrdersPlugin::~OrdersPlugin() = default;

// =============================================================================
// 初始化阶段
// =============================================================================

bool OrdersPlugin::initialize(mpf::ServiceRegistry* registry)
{
    m_registry = registry;
    
    // -------------------------------------------------------------------------
    // 【日志使用示例】
    // MPF 提供统一的日志宏，支持不同级别：
    // - MPF_LOG_DEBUG: 调试信息
    // - MPF_LOG_INFO: 一般信息
    // - MPF_LOG_WARNING: 警告
    // - MPF_LOG_ERROR: 错误
    // 第一个参数是标签（通常用插件名），第二个是消息
    // -------------------------------------------------------------------------
    MPF_LOG_INFO("OrdersPlugin", "Initializing...");
    
    // -------------------------------------------------------------------------
    // 【调试】检查 qrc 资源是否可访问
    // -------------------------------------------------------------------------
    QStringList resourcesToCheck = {
        ":/YourCo/Orders/qml/OrdersPage.qml",
        "qrc:/YourCo/Orders/qml/OrdersPage.qml"
    };
    for (const QString& res : resourcesToCheck) {
        QFile f(res);
        MPF_LOG_DEBUG("OrdersPlugin", 
            QString("Resource check: %1 exists=%2").arg(res).arg(f.exists() ? "YES" : "NO").toStdString().c_str());
    }
    
    // -------------------------------------------------------------------------
    // 【服务创建】
    // 在初始化阶段创建业务服务实例
    // 服务通常是整个插件生命周期内唯一的实例
    // -------------------------------------------------------------------------
    m_ordersService = std::make_unique<OrdersService>(this);
    
    // -------------------------------------------------------------------------
    // 【QML 类型注册】
    // 必须在 QML 引擎加载任何使用这些类型的文件之前完成
    // 所以放在 initialize() 而不是 start() 中
    // -------------------------------------------------------------------------
    registerQmlTypes();
    
    MPF_LOG_INFO("OrdersPlugin", "Initialized successfully");
    return true;
}

// =============================================================================
// 启动阶段
// =============================================================================

bool OrdersPlugin::start()
{
    MPF_LOG_INFO("OrdersPlugin", "Starting...");
    
    // -------------------------------------------------------------------------
    // 【路由和菜单注册】
    // 在启动阶段注册，因为此时所有依赖的服务都已就绪
    // -------------------------------------------------------------------------
    registerRoutes();
    
    // -------------------------------------------------------------------------
    // 【示例数据】
    // 这里创建一些演示用的示例数据
    // 实际项目中，数据通常从配置、数据库或 API 加载
    // 
    // 【修改点2】删除或替换为你的初始数据加载逻辑
    // -------------------------------------------------------------------------
    m_ordersService->createOrder({
        {"customerName", "John Doe"},
        {"productName", "Widget Pro"},
        {"quantity", 2},
        {"price", 99.99},
        {"status", "pending"}
    });
    
    m_ordersService->createOrder({
        {"customerName", "Jane Smith"},
        {"productName", "Gadget X"},
        {"quantity", 1},
        {"price", 149.99},
        {"status", "processing"}
    });
    
    m_ordersService->createOrder({
        {"customerName", "Bob Wilson"},
        {"productName", "Tool Kit"},
        {"quantity", 3},
        {"price", 49.99},
        {"status", "shipped"}
    });
    
    MPF_LOG_INFO("OrdersPlugin", "Started with sample orders");
    return true;
}

// =============================================================================
// 停止阶段
// =============================================================================

void OrdersPlugin::stop()
{
    MPF_LOG_INFO("OrdersPlugin", "Stopping...");
    
    // -------------------------------------------------------------------------
    // 【清理工作】
    // 在此保存数据、断开连接、释放资源
    // 服务实例会在析构函数中自动销毁（unique_ptr）
    // -------------------------------------------------------------------------
}

// =============================================================================
// 元数据
// =============================================================================

QJsonObject OrdersPlugin::metadata() const
{
    // -------------------------------------------------------------------------
    // 【插件元数据】
    // 这里的内容应该与 orders_plugin.json 文件保持一致
    // 
    // 【修改点3】修改所有字段为你的插件信息：
    // - id: 插件唯一标识符，格式 "com.company.plugin"
    // - name: 插件显示名称
    // - version: 插件版本号
    // - description: 插件描述
    // - vendor: 开发者/公司名称
    // - requires: 依赖的服务或插件列表
    // - provides: 此插件提供的服务列表
    // - qmlModules: 提供的 QML 模块 URI 列表
    // - priority: 加载优先级（数字越小越先加载）
    // -------------------------------------------------------------------------
    return QJsonDocument::fromJson(R"({
        "id": "com.yourco.orders",
        "name": "Orders Plugin",
        "version": "1.0.0",
        "description": "Order management functionality",
        "vendor": "YourCo",
        "requires": [
            {"type": "service", "id": "INavigation", "min": "1.0"}
        ],
        "provides": ["OrdersService"],
        "qmlModules": ["YourCo.Orders"],
        "priority": 10
    })").object();
}

// =============================================================================
// 路由和菜单注册
// =============================================================================

void OrdersPlugin::registerRoutes()
{
    // -------------------------------------------------------------------------
    // 【导航路由注册】
    // 使用 INavigation 服务注册插件主页面
    // 
    // 新架构（Loader-based）：
    // - 只注册一个主页面 URL
    // - 插件内部导航使用 Popup/Dialog
    // - 避免跨 DLL 动态加载 QML 组件的问题
    // -------------------------------------------------------------------------
    auto* nav = m_registry->get<mpf::INavigation>();
    if (nav) {
        // 构建 QML 搜索路径列表（优先级从高到低）
        QStringList searchPaths;
        QString appDir = QCoreApplication::applicationDirPath();
        
        // 1. QML_IMPORT_PATH 环境变量（dev.json 开发路径优先）
        QString qmlImportPaths = qEnvironmentVariable("QML_IMPORT_PATH");
        searchPaths << qmlImportPaths.split(QDir::listSeparator(), Qt::SkipEmptyParts);

        // 2. MPF_SDK_ROOT 环境变量（SDK 兜底）
        QString sdkRoot = qEnvironmentVariable("MPF_SDK_ROOT");
        if (!sdkRoot.isEmpty()) {
            searchPaths << QDir::cleanPath(sdkRoot + "/qml");
        }
        
        // 3. 应用程序相对路径（标准 SDK 安装布局）
        searchPaths << QDir::cleanPath(appDir + "/../qml");
        
        // 4. 应用程序同级 qml 目录（开发模式）
        searchPaths << QDir::cleanPath(appDir + "/qml");
        
        // 查找 QML 模块目录
        QString qmlBase;
        QString qmlFile;
        for (const QString& basePath : searchPaths) {
            QString candidate = QDir::cleanPath(basePath + "/YourCo/Orders/OrdersPage.qml");
            if (QFile::exists(candidate)) {
                qmlBase = QDir::cleanPath(basePath + "/YourCo/Orders");
                qmlFile = candidate;
                break;
            }
        }
        
        if (qmlFile.isEmpty()) {
            MPF_LOG_ERROR("OrdersPlugin", "Could not find YourCo/Orders/OrdersPage.qml!");
            MPF_LOG_ERROR("OrdersPlugin", QString("Searched paths: %1").arg(searchPaths.join("; ")).toStdString().c_str());
            return;
        }
        
        QString ordersPage = QUrl::fromLocalFile(qmlFile).toString();
        
        MPF_LOG_INFO("OrdersPlugin", QString("QML base path: %1").arg(qmlBase).toStdString().c_str());
        MPF_LOG_INFO("OrdersPlugin", QString("Orders page URL: %1").arg(ordersPage).toStdString().c_str());
        
        // 注册主页面（内部导航使用 Popup）
        nav->registerRoute("orders", ordersPage);
        
        MPF_LOG_INFO("OrdersPlugin", "Registered route: orders");
    }
    
    // -------------------------------------------------------------------------
    // 【菜单项注册】
    // 使用 IMenu 服务注册侧边栏菜单项
    // 
    // MenuItem 结构体字段说明：
    // - id: 菜单项唯一标识符
    // - label: 显示文本（支持 tr() 国际化）
    // - icon: 图标（可以是 emoji 或图标路径）
    // - route: 点击时导航的路由名称
    // - pluginId: 所属插件 ID
    // - order: 排序权重（数字越小越靠前）
    // - group: 分组名称（可选，用于菜单分组）
    // 
    // 【修改点5】修改菜单项配置
    // -------------------------------------------------------------------------
    auto* menu = m_registry->get<mpf::IMenu>();
    if (menu) {
        mpf::MenuItem item;
        item.id = "orders";                    // 菜单项 ID
        item.label = tr("Orders");             // 显示文本
        item.icon = "📦";                      // 图标
        item.route = "orders";                 // 对应的路由
        item.pluginId = "com.yourco.orders";   // 插件 ID
        item.order = 10;                       // 排序（10, 20, 30...）
        item.group = "Business";               // 分组
        
        bool registered = menu->registerItem(item);
        if (!registered) {
            MPF_LOG_WARNING("OrdersPlugin", "Failed to register menu item");
            return;
        }
        
        // ---------------------------------------------------------------------
        // 【菜单徽章】
        // 在菜单项上显示数字徽章（如未读数、待处理数）
        // ---------------------------------------------------------------------
        menu->setBadge("orders", QString::number(m_ordersService->getOrderCount()));
        
        // ---------------------------------------------------------------------
        // 【信号连接】
        // 当数据变化时自动更新徽章
        // 这是 Qt 信号槽机制的典型应用
        // ---------------------------------------------------------------------
        connect(m_ordersService.get(), &OrdersService::ordersChanged, this, [this, menu]() {
            menu->setBadge("orders", QString::number(m_ordersService->getOrderCount()));
        });
        
        MPF_LOG_DEBUG("OrdersPlugin", "Registered menu item");
    } else {
        MPF_LOG_WARNING("OrdersPlugin", "Menu service not available");
    }
}

// =============================================================================
// QML 类型注册
// =============================================================================

void OrdersPlugin::registerQmlTypes()
{
    // -------------------------------------------------------------------------
    // 【QML 单例注册】
    // qmlRegisterSingletonInstance 将 C++ 对象注册为 QML 单例
    // 
    // 参数说明：
    // - URI: QML 模块 URI（与 CMakeLists.txt 中一致）
    // - major/minor: 版本号
    // - qmlName: 在 QML 中使用的名称
    // - instance: C++ 对象指针
    // 
    // QML 中使用: import YourCo.Orders 1.0
    //            OrdersService.getAllOrders()
    // 
    // 【修改点6】修改 URI 和类型名称
    // -------------------------------------------------------------------------
    qmlRegisterSingletonInstance("YourCo.Orders", 1, 0, "OrdersService", m_ordersService.get());
    
    // -------------------------------------------------------------------------
    // 【QML 类型注册】
    // qmlRegisterType 注册可在 QML 中实例化的类型
    // 
    // 适用于：
    // - 列表模型 (QAbstractListModel 子类)
    // - 可重用组件的后端类
    // 
    // QML 中使用: import YourCo.Orders 1.0
    //            OrderModel { service: OrdersService }
    // -------------------------------------------------------------------------
    qmlRegisterType<OrderModel>("YourCo.Orders", 1, 0, "OrderModel");
    
    MPF_LOG_DEBUG("OrdersPlugin", "Registered QML types");
}

} // namespace orders
