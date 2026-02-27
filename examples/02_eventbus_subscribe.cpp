/**
 * =============================================================================
 * 样例 02: 通过 EventBus 订阅事件（C++ 层）
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * 订单插件如何在 C++ 层订阅和处理其他插件发布的事件。
 *
 * 【EventBus 的事件接收机制】
 * 当前实现中，EventBus 使用 Qt 信号进行事件投递：
 * 1. subscribe() 注册订阅关系（pattern + subscriberId）
 * 2. publish() 时，EventBus 匹配订阅并发射 eventPublished 信号
 * 3. 接收方通过 connect() 连接 eventPublished 信号
 * 4. 在 slot 中过滤 topic 并处理
 *
 * 【为什么需要 subscribe + connect 两步】
 * - subscribe() 告诉 EventBus "我对这个主题感兴趣"
 *   → 影响 publish() 返回的 notified 计数
 *   → 影响 subscriberCount() 查询结果
 * - connect() 实际接收信号
 *   → eventPublished 信号携带 topic/data/senderId
 *   → 接收方需要自己过滤 topic
 * =============================================================================
 */

#include <mpf/interfaces/ieventbus.h>
#include <mpf/service_registry.h>
#include <mpf/logger.h>
#include <QObject>

/**
 * 样例：订阅并处理事件
 *
 * 展示 orders 插件如何监听 rules 插件发布的规则检查结果。
 */

/*
class OrdersPlugin : public QObject, public mpf::IPlugin {
    Q_OBJECT
    
    // ...

    bool start() override
    {
        auto* eventBus = m_registry->get<mpf::IEventBus>();
        if (!eventBus) {
            MPF_LOG_WARNING("OrdersPlugin", "EventBus not available");
            return true;  // 非致命错误，插件仍可工作
        }
        
        // =====================================================================
        // 第一步：注册订阅
        // =====================================================================
        // 使用通配符订阅 rules 模块的所有事件
        m_rulesSubId = eventBus->subscribe(
            "rules/**",                  // 通配符：匹配 rules/ 下所有层级
            "com.yourco.orders",         // 订阅者 ID
            nullptr,                     // 使用信号而非回调
            mpf::SubscriptionOptions{}   // 默认选项
        );
        
        MPF_LOG_INFO("OrdersPlugin", 
            QString("Subscribed to rules/**, subId: %1").arg(m_rulesSubId));
        
        // =====================================================================
        // 第二步：连接 eventPublished 信号
        // =====================================================================
        // EventBus 实现类是 QObject，需要 dynamic_cast 获取 QObject*
        QObject* eventBusObj = dynamic_cast<QObject*>(eventBus);
        if (eventBusObj) {
            // 使用 lambda 连接信号，方便处理
            connect(eventBusObj, SIGNAL(eventPublished(QString,QVariantMap,QString)),
                    this,        SLOT(onEventReceived(QString,QVariantMap,QString)));
        }
        
        return true;
    }
    
    void stop() override
    {
        // 【重要】插件停止时必须取消订阅
        auto* eventBus = m_registry->get<mpf::IEventBus>();
        if (eventBus) {
            eventBus->unsubscribeAll("com.yourco.orders");
        }
    }

private slots:
    // =========================================================================
    // 事件处理 Slot
    // =========================================================================
    void onEventReceived(const QString& topic,
                         const QVariantMap& data,
                         const QString& senderId)
    {
        // 【重要】需要手动过滤 topic
        // 因为 eventPublished 信号会携带所有事件
        
        if (topic == "rules/check/completed") {
            // 规则检查完成
            QString orderId = data.value("orderId").toString();
            bool passed = data.value("passed").toBool();
            QString reason = data.value("reason").toString();
            
            MPF_LOG_INFO("OrdersPlugin",
                QString("Rule check for order %1: %2 (%3)")
                    .arg(orderId)
                    .arg(passed ? "PASSED" : "FAILED")
                    .arg(reason));
            
            if (!passed) {
                // 规则检查失败，更新订单状态
                m_ordersService->updateStatus(orderId, "rejected");
            }
        }
        else if (topic == "rules/updated") {
            // 规则被更新，可能需要重新检查
            MPF_LOG_DEBUG("OrdersPlugin", "Rules updated, may need rechecking");
        }
        // 其他 rules/** 事件 → 忽略
    }

private:
    QString m_rulesSubId;  // 保存订阅 ID，用于取消订阅
};
*/

/**
 * 样例：使用 SubscriptionOptions 控制事件接收行为
 */
void example_subscription_options()
{
    // =========================================================================
    // 高优先级订阅
    // =========================================================================
    // 确保某些处理器先于其他处理器执行
    //
    // mpf::SubscriptionOptions highPriority;
    // highPriority.priority = 100;  // 数字越大越先执行
    //
    // eventBus->subscribe("orders/created", "com.audit.plugin", nullptr, highPriority);

    // =========================================================================
    // 接收自己发送的事件
    // =========================================================================
    // 默认情况下，不会收到自己发送的事件（防止消息循环）
    // 如果确实需要：
    //
    // mpf::SubscriptionOptions selfRecv;
    // selfRecv.receiveOwnEvents = true;
    //
    // eventBus->subscribe("orders/**", "com.yourco.orders", nullptr, selfRecv);
    // 
    // 这样 orders 插件也能收到自己发布的 orders/created 事件
    // 使用场景：自审计、日志记录等
}

/**
 * 样例：查询订阅信息
 */
void example_query_subscriptions()
{
    // 在调试或监控场景中，可以查询 EventBus 的状态：
    //
    // // 检查某个 topic 有多少订阅者
    // int count = eventBus->subscriberCount("orders/created");
    // MPF_LOG_DEBUG("OrdersPlugin",
    //     QString("orders/created has %1 subscribers").arg(count));
    //
    // // 查看本插件的所有订阅
    // QStringList mySubs = eventBus->subscriptionsFor("com.yourco.orders");
    // for (const QString& sub : mySubs) {
    //     MPF_LOG_DEBUG("OrdersPlugin", QString("Active subscription: %1").arg(sub));
    // }
    //
    // // 查看所有活跃主题
    // QStringList topics = eventBus->activeTopics();
    // for (const QString& topic : topics) {
    //     auto stats = eventBus->topicStats(topic);
    //     MPF_LOG_DEBUG("OrdersPlugin",
    //         QString("Topic %1: %2 subs, %3 events")
    //             .arg(topic)
    //             .arg(stats.subscriberCount)
    //             .arg(stats.eventCount));
    // }
}
