/**
 * =============================================================================
 * Orders Service - 业务服务类头文件
 * =============================================================================
 * 
 * 这是一个业务服务类的模板，展示了如何：
 * - 定义数据结构（Order struct）
 * - 创建可被 QML 调用的服务类
 * - 使用 MPF HTTP 客户端库进行网络请求
 * - 定义信号用于数据变化通知
 * 
 * 【创建新服务时需要修改的地方】
 * 1. 数据结构定义（struct Order -> struct YourData）
 * 2. 服务类名（OrdersService -> YourService）
 * 3. CRUD 方法名和参数
 * 4. 信号定义
 * 
 * 【设计模式】
 * 这个服务类使用了以下设计模式：
 * - 单例模式：通过 qmlRegisterSingletonInstance 在 QML 中作为单例使用
 * - 观察者模式：通过 Qt 信号通知数据变化
 * - DTO 模式：Order 结构体作为数据传输对象
 * =============================================================================
 */

#pragma once

#include <QObject>
#include <QList>
#include <QVariantMap>
#include <QDateTime>
#include <memory>

// MPF HTTP 客户端前向声明
// 用于网络请求功能（可选功能）
namespace mpf::http {
class HttpClient;
}

// 【修改点1】命名空间
namespace orders {

// =============================================================================
// 数据结构定义
// =============================================================================

/**
 * @brief 订单数据结构
 * 
 * 【修改点2】根据你的业务需求定义数据字段
 * 
 * 设计建议：
 * - 使用 QString 而不是 std::string（Qt 生态兼容性）
 * - 使用 QDateTime 处理时间
 * - 提供 toVariantMap/fromVariantMap 用于 QML 交互
 */
struct Order {
    QString id;              // 唯一标识符
    QString customerName;    // 客户名称
    QString productName;     // 产品名称
    int quantity;            // 数量
    double price;            // 单价
    QString status;          // 状态: pending, processing, shipped, delivered, cancelled
    QDateTime createdAt;     // 创建时间
    QDateTime updatedAt;     // 更新时间
    
    /**
     * @brief 转换为 QVariantMap
     * 
     * 用于将 C++ 结构体传递给 QML
     * QML 中可以直接访问属性：order.customerName, order.price 等
     */
    QVariantMap toVariantMap() const;
    
    /**
     * @brief 从 QVariantMap 创建
     * 
     * 用于从 QML 传入的数据创建 C++ 对象
     */
    static Order fromVariantMap(const QVariantMap& map);
};

// =============================================================================
// 服务类定义
// =============================================================================

/**
 * @brief 订单业务服务类
 * 
 * 【功能说明】
 * 这个类提供订单管理的核心业务逻辑，包括：
 * - CRUD 操作（创建、读取、更新、删除）
 * - 业务操作（状态更新、统计查询）
 * - 网络操作（从服务器获取数据）
 * 
 * 【QML 集成】
 * 此类通过 qmlRegisterSingletonInstance 注册为 QML 单例
 * 所有 Q_INVOKABLE 方法都可以在 QML 中直接调用：
 * 
 * @code{.qml}
 * import YourCo.Orders 1.0
 * 
 * Button {
 *     onClicked: {
 *         var orders = OrdersService.getAllOrders()
 *         var newId = OrdersService.createOrder({
 *             customerName: "Test",
 *             productName: "Widget",
 *             quantity: 1,
 *             price: 99.99
 *         })
 *     }
 * }
 * @endcode
 * 
 * 【HTTP 客户端使用】
 * 此服务集成了 MPF HTTP 客户端库（mpf-http-client），展示了：
 * - 如何创建 HttpClient 实例
 * - 如何发送 GET 请求并处理响应
 * - 如何解析 JSON 响应数据
 * 
 * 【修改点3】类名和方法
 */
class OrdersService : public QObject
{
    Q_OBJECT

public:
    explicit OrdersService(QObject* parent = nullptr);
    ~OrdersService() override;

    // =========================================================================
    // CRUD 操作
    // Q_INVOKABLE 宏使方法可从 QML 调用
    // =========================================================================
    
    /**
     * @brief 获取所有订单
     * @return QVariantList 订单列表（每个元素是 QVariantMap）
     * 
     * QML 使用示例：
     * @code{.qml}
     * var orders = OrdersService.getAllOrders()
     * for (var i = 0; i < orders.length; i++) {
     *     console.log(orders[i].customerName)
     * }
     * @endcode
     */
    Q_INVOKABLE QVariantList getAllOrders() const;
    
    /**
     * @brief 根据 ID 获取单个订单
     * @param id 订单 ID
     * @return QVariantMap 订单数据，如果不存在返回空 map
     */
    Q_INVOKABLE QVariantMap getOrder(const QString& id) const;
    
    /**
     * @brief 创建新订单
     * @param data 订单数据（从 QML 传入的 JavaScript 对象）
     * @return QString 新创建订单的 ID
     * 
     * QML 使用示例：
     * @code{.qml}
     * var newId = OrdersService.createOrder({
     *     customerName: customerField.text,
     *     productName: productField.text,
     *     quantity: parseInt(quantityField.text),
     *     price: parseFloat(priceField.text)
     * })
     * @endcode
     */
    Q_INVOKABLE QString createOrder(const QVariantMap& data);
    
    /**
     * @brief 更新订单
     * @param id 订单 ID
     * @param data 要更新的字段（只需包含要修改的字段）
     * @return bool 是否更新成功
     */
    Q_INVOKABLE bool updateOrder(const QString& id, const QVariantMap& data);
    
    /**
     * @brief 删除订单
     * @param id 订单 ID
     * @return bool 是否删除成功
     */
    Q_INVOKABLE bool deleteOrder(const QString& id);
    
    // =========================================================================
    // 业务操作
    // =========================================================================
    
    /**
     * @brief 更新订单状态
     * @param id 订单 ID
     * @param status 新状态
     * @return bool 是否更新成功
     * 
     * 这是一个便捷方法，内部调用 updateOrder
     */
    Q_INVOKABLE bool updateStatus(const QString& id, const QString& status);
    
    /**
     * @brief 按状态筛选订单
     * @param status 状态值
     * @return QVariantList 符合条件的订单列表
     */
    Q_INVOKABLE QVariantList getOrdersByStatus(const QString& status) const;
    
    /**
     * @brief 获取订单总数
     * @return int 订单数量
     */
    Q_INVOKABLE int getOrderCount() const;
    
    /**
     * @brief 获取总收入
     * @return double 所有订单的总金额
     */
    Q_INVOKABLE double getTotalRevenue() const;
    
    // =========================================================================
    // HTTP 网络操作
    // 【MPF HTTP 客户端使用示例】
    // =========================================================================
    
    /**
     * @brief 从服务器获取订单数据
     * @param apiUrl API 地址
     * 
     * 【HTTP 客户端使用示例】
     * 这个方法展示了如何使用 MPF HTTP 客户端库：
     * 1. 创建请求选项（设置超时等）
     * 2. 发送 GET 请求
     * 3. 处理异步响应
     * 4. 解析 JSON 数据
     * 
     * QML 使用示例：
     * @code{.qml}
     * OrdersService.fetchOrdersFromServer("https://api.example.com/orders")
     * 
     * Connections {
     *     target: OrdersService
     *     function onFetchCompleted(success, message) {
     *         if (success) {
     *             console.log("Loaded:", message)
     *         } else {
     *             console.error("Error:", message)
     *         }
     *     }
     * }
     * @endcode
     */
    Q_INVOKABLE void fetchOrdersFromServer(const QString& apiUrl);

    // =========================================================================
    // 信号定义
    // 用于通知 QML 数据变化
    // =========================================================================

signals:
    /**
     * @brief 订单创建信号
     * @param id 新创建订单的 ID
     */
    void orderCreated(const QString& id);
    
    /**
     * @brief 订单更新信号
     * @param id 被更新订单的 ID
     */
    void orderUpdated(const QString& id);
    
    /**
     * @brief 订单删除信号
     * @param id 被删除订单的 ID
     */
    void orderDeleted(const QString& id);
    
    /**
     * @brief 订单列表变化信号
     * 
     * 任何增删改操作后都会发出此信号
     * 用于刷新 UI 列表
     * 
     * QML 使用示例：
     * @code{.qml}
     * Connections {
     *     target: OrdersService
     *     function onOrdersChanged() {
     *         // 刷新列表
     *         orderModel.refresh()
     *     }
     * }
     * @endcode
     */
    void ordersChanged();
    
    /**
     * @brief 网络请求完成信号
     * @param success 是否成功
     * @param message 结果消息
     */
    void fetchCompleted(bool success, const QString& message);

private:
    /**
     * @brief 生成唯一 ID
     * @return QString 8 位 UUID
     */
    QString generateId() const;
    
    QList<Order> m_orders;                               // 订单数据存储
    std::unique_ptr<mpf::http::HttpClient> m_httpClient; // HTTP 客户端实例
};

} // namespace orders
