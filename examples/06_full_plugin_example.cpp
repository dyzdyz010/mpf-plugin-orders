/**
 * =============================================================================
 * 样例 06: 完整的插件启动流程（含 EventBus 集成）
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * 一个完整的插件如何集成所有 MPF 框架能力：
 * - ServiceRegistry 获取系统服务
 * - EventBus 发布和订阅事件
 * - INavigation 注册路由
 * - IMenu 注册菜单项
 * - HTTP Client 网络请求
 * - QML 类型注册
 *
 * 这是一个"理想"的插件实现模板，展示最佳实践。
 * =============================================================================
 */

#include <mpf/interfaces/iplugin.h>
#include <mpf/interfaces/ieventbus.h>
#include <mpf/interfaces/inavigation.h>
#include <mpf/interfaces/imenu.h>
#include <mpf/interfaces/isettings.h>
#include <mpf/service_registry.h>
#include <mpf/logger.h>

#include <QObject>

/*

// =============================================================================
// 完整的插件主类
// =============================================================================

class ExamplePlugin : public QObject, public mpf::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MPF_IPlugin_iid FILE "../example_plugin.json")
    Q_INTERFACES(mpf::IPlugin)

public:
    explicit ExamplePlugin(QObject* parent = nullptr) : QObject(parent) {}
    ~ExamplePlugin() override = default;

    // =========================================================================
    // 阶段一：initialize — 创建服务、注册类型
    // =========================================================================
    bool initialize(mpf::ServiceRegistry* registry) override
    {
        m_registry = registry;
        MPF_LOG_INFO("Example", "Initializing...");
        
        // 1. 创建业务服务
        m_service = std::make_unique<ExampleService>(this);
        
        // 2. 注册 QML 类型（必须在 QML 引擎加载前完成）
        qmlRegisterSingletonInstance("Example.Module", 1, 0,
                                     "ExampleService", m_service.get());
        
        // 3. [可选] 注册服务到 Registry，供其他插件使用
        // 如果其他插件需要直接调用本插件的服务：
        // registry->add<IExampleService>(m_service.get(), 1, "com.example.plugin");
        
        MPF_LOG_INFO("Example", "Initialized successfully");
        return true;
    }

    // =========================================================================
    // 阶段二：start — 注册路由、菜单、订阅事件
    // =========================================================================
    bool start() override
    {
        MPF_LOG_INFO("Example", "Starting...");
        
        // ----- 1. 获取系统服务 -----
        auto* nav      = m_registry->get<mpf::INavigation>();
        auto* menu     = m_registry->get<mpf::IMenu>();
        auto* settings = m_registry->get<mpf::ISettings>();
        m_eventBus     = m_registry->get<mpf::IEventBus>();
        
        // ----- 2. 注册路由 -----
        if (nav) {
            // QML 文件由 qt_add_qml_module 嵌入 DLL 的 qrc 资源
            // qrc 路径 = RESOURCE_PREFIX "/" + URI 转目录 + 文件名
            nav->registerRoute("example", "qrc:/Example/Module/MainPage.qml");
        }
        
        // ----- 3. 注册菜单项 -----
        if (menu) {
            mpf::MenuItem item;
            item.id       = "example";
            item.label    = tr("Example");
            item.icon     = "🔧";
            item.route    = "example";
            item.pluginId = "com.example.plugin";
            item.order    = 30;
            item.group    = "Tools";
            menu->registerItem(item);
        }
        
        // ----- 4. 恢复设置 -----
        if (settings) {
            QString lastMode = settings->value(
                "com.example.plugin", "viewMode", "list").toString();
            m_service->setViewMode(lastMode);
        }
        
        // ----- 5. 订阅 EventBus 事件 -----
        if (m_eventBus) {
            // 注册订阅关系
            m_eventBus->subscribe("orders/**", "com.example.plugin",
                                  nullptr, mpf::SubscriptionOptions{});
            
            // 连接信号以接收事件
            QObject* busObj = dynamic_cast<QObject*>(m_eventBus);
            if (busObj) {
                connect(busObj, SIGNAL(eventPublished(QString,QVariantMap,QString)),
                        this,   SLOT(handleEvent(QString,QVariantMap,QString)));
            }
        }
        
        // ----- 6. 发布"插件就绪"事件 -----
        if (m_eventBus) {
            m_eventBus->publish("example/ready", {
                {"version", "1.0.0"},
                {"capabilities", QStringList{"feature1", "feature2"}}
            }, "com.example.plugin");
        }
        
        MPF_LOG_INFO("Example", "Started successfully");
        return true;
    }

    // =========================================================================
    // 阶段三：stop — 保存设置、取消订阅、清理资源
    // =========================================================================
    void stop() override
    {
        MPF_LOG_INFO("Example", "Stopping...");
        
        // 1. 保存设置
        auto* settings = m_registry->get<mpf::ISettings>();
        if (settings) {
            settings->setValue("com.example.plugin", "viewMode",
                              m_service->viewMode());
            settings->sync();
        }
        
        // 2. 取消所有 EventBus 订阅
        if (m_eventBus) {
            m_eventBus->unsubscribeAll("com.example.plugin");
        }
        
        // 3. 发布"插件停止"事件
        if (m_eventBus) {
            m_eventBus->publish("example/stopped", {}, "com.example.plugin");
        }
        
        MPF_LOG_INFO("Example", "Stopped");
    }

    QJsonObject metadata() const override
    {
        return QJsonDocument::fromJson(R"({
            "id": "com.example.plugin",
            "name": "Example Plugin",
            "version": "1.0.0",
            "description": "Complete plugin example",
            "vendor": "Example",
            "requires": [
                {"type": "service", "id": "INavigation", "min": "1.0"}
            ],
            "provides": [],
            "qmlModules": ["Example.Module"],
            "priority": 30
        })").object();
    }

    QString qmlModuleUri() const override { return "Example.Module"; }

private slots:
    // =========================================================================
    // 事件处理
    // =========================================================================
    void handleEvent(const QString& topic,
                     const QVariantMap& data,
                     const QString& senderId)
    {
        if (topic == "orders/created") {
            MPF_LOG_INFO("Example",
                QString("New order: %1 from %2")
                    .arg(data.value("orderId").toString())
                    .arg(senderId));
            
            // 响应订单创建：执行业务逻辑...
            // 然后发布处理结果
            if (m_eventBus) {
                m_eventBus->publish("example/processed", {
                    {"orderId", data.value("orderId")},
                    {"result", "success"}
                }, "com.example.plugin");
            }
        }
    }

private:
    mpf::ServiceRegistry* m_registry = nullptr;
    mpf::IEventBus* m_eventBus = nullptr;
    std::unique_ptr<ExampleService> m_service;
    
    QString findQmlFile(const QString& relativePath) {
        // 简化版本，完整实现见 orders_plugin.cpp
        QStringList searchPaths;
        QString qmlImportPaths = qEnvironmentVariable("QML_IMPORT_PATH");
        searchPaths << qmlImportPaths.split(QDir::listSeparator(), Qt::SkipEmptyParts);
        searchPaths << QCoreApplication::applicationDirPath() + "/../qml";
        
        for (const QString& base : searchPaths) {
            QString full = QDir::cleanPath(base + "/" + relativePath);
            if (QFile::exists(full)) return full;
        }
        return {};
    }
};

*/
