/**
 * =============================================================================
 * 样例 03: 通过 ServiceRegistry 注册和获取服务
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * 插件如何使用 ServiceRegistry 实现服务注册与发现。
 *
 * 【两种角色】
 * 1. 服务消费者：获取 Host 或其他插件提供的服务（get<T>）
 * 2. 服务提供者：注册自己的服务供其他插件使用（add<T>）
 *
 * 【编译时依赖】
 * 插件只依赖 mpf-sdk 中的接口定义。
 * 如果要注册服务供其他插件使用，需要在 SDK 层定义公共接口。
 * =============================================================================
 */

#include <mpf/service_registry.h>
#include <mpf/interfaces/inavigation.h>
#include <mpf/interfaces/imenu.h>
#include <mpf/interfaces/isettings.h>
#include <mpf/interfaces/ieventbus.h>
#include <mpf/logger.h>

/**
 * 样例：作为服务消费者 —— 获取 Host 提供的服务
 */

/*
bool OrdersPlugin::initialize(mpf::ServiceRegistry* registry)
{
    m_registry = registry;
    
    // =========================================================================
    // 获取系统服务（Host 在启动时注册）
    // =========================================================================
    
    // 【导航服务】注册页面路由
    auto* nav = registry->get<mpf::INavigation>();
    // nav 可能为 nullptr（如果 Host 没有提供导航服务）
    // 始终检查返回值！
    
    // 【菜单服务】注册侧边栏菜单项
    auto* menu = registry->get<mpf::IMenu>();
    
    // 【设置服务】读写持久化设置
    auto* settings = registry->get<mpf::ISettings>();
    if (settings) {
        // 从设置中恢复上次的配置
        int pageSize = settings->value("com.yourco.orders", "pageSize", 20).toInt();
        MPF_LOG_INFO("Orders", QString("Page size: %1").arg(pageSize));
    }
    
    // 【事件总线】跨插件通信
    auto* eventBus = registry->get<mpf::IEventBus>();
    
    // =========================================================================
    // 版本检查
    // =========================================================================
    // 如果插件需要某个服务的特定版本：
    auto* nav3 = registry->get<mpf::INavigation>(3);  // 要求 v3+
    if (!nav3) {
        MPF_LOG_ERROR("Orders", "Navigation service v3+ required!");
        return false;  // 初始化失败
    }
    
    // 也可以先检查再获取：
    if (registry->has<mpf::IEventBus>(3)) {
        auto* eb = registry->get<mpf::IEventBus>();
        // 使用 v3 的 API...
    }
    
    return true;
}
*/

/**
 * 样例：作为服务提供者 —— 注册自己的服务
 *
 * 如果 orders 插件想让其他插件直接调用自己的服务（不通过 EventBus），
 * 可以将服务注册到 ServiceRegistry。
 *
 * 【前提】需要在 mpf-sdk 中定义公共接口。
 */

/*
// ---- 在 mpf-sdk 中定义公共接口 ----
// 文件：mpf-sdk/include/mpf/interfaces/iorders.h

namespace mpf {

class IOrders {
public:
    virtual ~IOrders() = default;
    
    virtual QVariantList getAllOrders() const = 0;
    virtual QVariantMap getOrder(const QString& id) const = 0;
    virtual int getOrderCount() const = 0;
    
    static constexpr int apiVersion() { return 1; }
};

} // namespace mpf


// ---- 在 orders 插件中实现并注册 ----

// OrdersService 需要同时继承 QObject 和 IOrders
class OrdersService : public QObject, public mpf::IOrders {
    Q_OBJECT
public:
    // ... 实现 IOrders 接口 ...
    QVariantList getAllOrders() const override { ... }
    QVariantMap getOrder(const QString& id) const override { ... }
    int getOrderCount() const override { ... }
};

bool OrdersPlugin::initialize(mpf::ServiceRegistry* registry)
{
    m_ordersService = std::make_unique<OrdersService>(this);
    
    // 注册到 ServiceRegistry
    // 其他插件可以通过 registry->get<mpf::IOrders>() 获取
    registry->add<mpf::IOrders>(
        m_ordersService.get(),       // 服务实例
        mpf::IOrders::apiVersion(),  // API 版本
        "com.yourco.orders"          // 提供者 ID
    );
    
    return true;
}


// ---- 在 rules 插件中获取和使用 ----

bool RulesPlugin::start()
{
    // 获取 orders 插件的服务
    auto* orders = m_registry->get<mpf::IOrders>();
    if (orders) {
        int count = orders->getOrderCount();
        MPF_LOG_INFO("Rules", QString("Found %1 orders to check").arg(count));
        
        // 获取所有订单进行规则检查
        QVariantList allOrders = orders->getAllOrders();
        for (const QVariant& v : allOrders) {
            QVariantMap order = v.toMap();
            checkRules(order);  // 执行规则检查
        }
    } else {
        MPF_LOG_WARNING("Rules", "Orders service not available");
    }
    
    return true;
}
*/

/**
 * 样例：ServiceRegistry vs EventBus 的选择
 *
 * 什么时候用 ServiceRegistry，什么时候用 EventBus？
 */
void example_when_to_use_what()
{
    // =========================================================================
    // 使用 ServiceRegistry 的场景
    // =========================================================================
    // 1. 同步调用：需要立即获取返回值
    //    auto* orders = registry->get<IOrders>();
    //    int count = orders->getOrderCount();  // 立即返回
    //
    // 2. 紧耦合的服务调用：调用者需要知道服务的存在
    //    if (!registry->has<IOrders>()) return false;
    //
    // 3. 需要版本控制：
    //    registry->get<IOrders>(2);  // 需要 v2+ 的 API

    // =========================================================================
    // 使用 EventBus 的场景
    // =========================================================================
    // 1. 松耦合通知：发送者不关心谁在监听
    //    eventBus->publish("orders/created", data, senderId);
    //
    // 2. 一对多广播：多个插件需要响应同一事件
    //    // rules 订阅、audit 订阅、notification 订阅...
    //
    // 3. 异步处理：不需要等待处理结果
    //    eventBus->publish("orders/created", data, senderId);
    //    // 继续执行，不等待订阅者处理完成
    //
    // 4. 可选依赖：即使没有订阅者也不影响功能
    //    int notified = eventBus->publish(...);
    //    // notified == 0 也没关系
}
