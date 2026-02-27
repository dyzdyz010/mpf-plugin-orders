/**
 * =============================================================================
 * 样例 01: 通过 EventBus 发布事件（C++ 层）
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * 当订单插件执行业务操作时（如创建订单），
 * 如何通过 EventBus 发布事件通知其他插件。
 *
 * 【为什么要发布事件】
 * 订单创建后，其他插件可能需要响应：
 * - rules 插件需要检查业务规则
 * - 通知插件需要发送提醒
 * - 统计插件需要更新数据
 *
 * 通过 EventBus，orders 插件不需要知道谁在监听，
 * 只管发布事件，实现彻底的解耦。
 *
 * 【编译时依赖】
 * 这段代码只依赖 mpf-sdk 中的 IEventBus 接口，
 * 不依赖 Host 中的 EventBusService 实现。
 * =============================================================================
 */

#include <mpf/interfaces/ieventbus.h>
#include <mpf/service_registry.h>
#include <mpf/logger.h>

/**
 * 样例：在 OrdersService 中集成 EventBus 事件发布
 *
 * 以下代码展示如何修改 OrdersService::createOrder() 方法，
 * 在创建订单后通过 EventBus 发布事件。
 */

// ---- 以下为修改后的代码片段 ----

/*
class OrdersService : public QObject {
    Q_OBJECT
public:
    // 构造函数中接收 registry，获取 EventBus
    OrdersService(mpf::ServiceRegistry* registry, QObject* parent = nullptr)
        : QObject(parent)
    {
        // 获取 EventBus 服务
        // 注意：需要在 start() 阶段调用（所有服务已注册）
        m_eventBus = registry->get<mpf::IEventBus>();
        if (!m_eventBus) {
            MPF_LOG_WARNING("OrdersService", "EventBus not available, events won't be published");
        }
    }

    Q_INVOKABLE QString createOrder(const QVariantMap& data)
    {
        // ... 正常的创建逻辑 ...
        Order order = Order::fromVariantMap(data);
        order.id = generateId();
        order.createdAt = QDateTime::currentDateTime();
        m_orders.append(order);

        // =====================================================================
        // 【关键】通过 EventBus 发布"订单创建"事件
        // =====================================================================
        //
        // topic 命名约定：
        // - 使用 / 分隔的层级结构
        // - 第一级是模块名（orders）
        // - 第二级是动作（created, updated, deleted）
        //
        // data 内容：
        // - 包含其他插件可能需要的信息
        // - 不要包含敏感数据
        // - 使用 QVariantMap（QML 可直接访问）
        //
        // senderId：
        // - 插件的唯一标识符
        // - 用于订阅者过滤自己发送的事件

        if (m_eventBus) {
            int notified = m_eventBus->publish(
                "orders/created",                    // 主题
                {                                    // 事件数据
                    {"orderId",      order.id},
                    {"customerName", order.customerName},
                    {"productName",  order.productName},
                    {"quantity",     order.quantity},
                    {"price",        order.price},
                    {"totalAmount",  order.quantity * order.price},
                    {"status",       order.status}
                },
                "com.yourco.orders"                  // 发送者 ID
            );

            MPF_LOG_DEBUG("OrdersService",
                QString("Published orders/created, notified %1 subscribers")
                    .arg(notified));
        }

        emit orderCreated(order.id);
        emit ordersChanged();
        return order.id;
    }

    Q_INVOKABLE bool updateStatus(const QString& id, const QString& newStatus)
    {
        // ... 更新状态逻辑 ...
        
        // 发布状态变更事件
        if (m_eventBus) {
            m_eventBus->publish(
                "orders/status_changed",
                {
                    {"orderId",  id},
                    {"oldStatus", oldStatus},
                    {"newStatus", newStatus}
                },
                "com.yourco.orders"
            );
        }
        
        return true;
    }

    Q_INVOKABLE bool deleteOrder(const QString& id)
    {
        // ... 删除逻辑 ...
        
        // 发布删除事件
        if (m_eventBus) {
            m_eventBus->publish(
                "orders/deleted",
                {{"orderId", id}},
                "com.yourco.orders"
            );
        }
        
        return true;
    }

private:
    mpf::IEventBus* m_eventBus = nullptr;
};
*/

/**
 * 样例：在插件 start() 中注册 EventBus
 *
 * EventBus 应该在 start() 阶段获取，因为此时所有服务已注册。
 */

/*
bool OrdersPlugin::start()
{
    // 获取 EventBus（在 start 阶段，所有服务已就绪）
    auto* eventBus = m_registry->get<mpf::IEventBus>();
    
    // 将 EventBus 传递给 OrdersService
    m_ordersService->setEventBus(eventBus);
    
    // 订阅其他插件的事件（如果需要）
    if (eventBus) {
        // 订阅规则检查完成事件
        eventBus->subscribe(
            "rules/check/completed",     // 订阅的主题
            "com.yourco.orders",         // 订阅者 ID
            nullptr,                     // 使用信号而非回调
            mpf::SubscriptionOptions{}   // 默认选项
        );
    }
    
    registerRoutes();
    return true;
}

void OrdersPlugin::stop()
{
    // 【重要】停止时取消所有订阅
    auto* eventBus = m_registry->get<mpf::IEventBus>();
    if (eventBus) {
        eventBus->unsubscribeAll("com.yourco.orders");
    }
}
*/
