/**
 * =============================================================================
 * Orders Service - 业务服务类实现
 * =============================================================================
 * 
 * 这是业务服务类的实现模板，展示了：
 * - 数据结构与 QVariantMap 的相互转换
 * - CRUD 操作的标准实现模式
 * - 信号发射的时机
 * - MPF HTTP 客户端的使用方法
 * 
 * 【创建新服务时需要修改的地方】
 * 1. 数据结构的 toVariantMap/fromVariantMap 方法
 * 2. CRUD 方法的具体实现
 * 3. 业务逻辑方法
 * =============================================================================
 */

#include "orders_service.h"

// -----------------------------------------------------------------------------
// 【MPF HTTP 客户端】
// 包含 MPF 提供的 HTTP 客户端库头文件
// 提供了简洁的 HTTP 请求 API
// -----------------------------------------------------------------------------
#include <mpf/http/http_client.h>

#include <QUuid>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <algorithm>

namespace orders {

// =============================================================================
// 数据结构方法实现
// =============================================================================

/**
 * @brief 将 Order 结构体转换为 QVariantMap
 * 
 * 【设计说明】
 * QVariantMap 是 Qt 的通用键值对容器，可以直接传递给 QML
 * QML 中可以像访问 JavaScript 对象一样访问属性：
 * @code{.qml}
 * var order = OrdersService.getOrder(id)
 * console.log(order.customerName)  // 直接访问属性
 * console.log(order.total)         // 访问计算属性
 * @endcode
 * 
 * 【修改点1】根据你的数据结构修改字段映射
 */
QVariantMap Order::toVariantMap() const
{
    return {
        {"id", id},
        {"customerName", customerName},
        {"productName", productName},
        {"quantity", quantity},
        {"price", price},
        {"status", status},
        {"createdAt", createdAt},
        {"updatedAt", updatedAt},
        {"total", quantity * price}  // 计算属性，方便 QML 直接使用
    };
}

/**
 * @brief 从 QVariantMap 创建 Order 结构体
 * 
 * 【设计说明】
 * 处理从 QML 传入的数据，使用 value() 方法可以提供默认值
 * 这样即使 QML 没有传入某个字段，也不会出错
 * 
 * 【修改点2】根据你的数据结构修改字段映射
 */
Order Order::fromVariantMap(const QVariantMap& map)
{
    Order order;
    order.id = map.value("id").toString();
    order.customerName = map.value("customerName").toString();
    order.productName = map.value("productName").toString();
    order.quantity = map.value("quantity").toInt();
    order.price = map.value("price").toDouble();
    order.status = map.value("status", "pending").toString();  // 默认值
    order.createdAt = map.value("createdAt").toDateTime();
    order.updatedAt = map.value("updatedAt").toDateTime();
    return order;
}

// =============================================================================
// 服务类构造/析构
// =============================================================================

OrdersService::OrdersService(QObject* parent)
    : QObject(parent)
    // -------------------------------------------------------------------------
    // 【HTTP 客户端初始化】
    // 创建 MPF HTTP 客户端实例
    // 传入 this 作为 parent，确保生命周期管理
    // -------------------------------------------------------------------------
    , m_httpClient(std::make_unique<mpf::http::HttpClient>(this))
{
}

OrdersService::~OrdersService() = default;

// =============================================================================
// CRUD 操作实现
// =============================================================================

/**
 * @brief 获取所有数据
 * 
 * 【实现模式】
 * 遍历内部数据列表，转换为 QVariantList 返回给 QML
 */
QVariantList OrdersService::getAllOrders() const
{
    QVariantList result;
    for (const Order& order : m_orders) {
        result.append(order.toVariantMap());
    }
    return result;
}

/**
 * @brief 根据 ID 获取单条数据
 * 
 * 【实现模式】
 * 使用 std::find_if 查找，比循环更简洁
 * 找不到时返回空 QVariantMap，QML 中可以用 Object.keys(result).length === 0 判断
 */
QVariantMap OrdersService::getOrder(const QString& id) const
{
    auto it = std::find_if(m_orders.begin(), m_orders.end(),
        [&id](const Order& o) { return o.id == id; });
    
    if (it != m_orders.end()) {
        return it->toVariantMap();
    }
    return {};  // 返回空 map 表示未找到
}

/**
 * @brief 创建新数据
 * 
 * 【实现模式】
 * 1. 从 QVariantMap 构造对象
 * 2. 生成 ID 和时间戳
 * 3. 设置默认值
 * 4. 添加到列表
 * 5. 发射信号通知变化
 * 
 * 【信号发射】
 * - orderCreated: 特定事件信号，携带新 ID
 * - ordersChanged: 通用变化信号，用于刷新 UI
 */
QString OrdersService::createOrder(const QVariantMap& data)
{
    Order order = Order::fromVariantMap(data);
    order.id = generateId();                        // 生成唯一 ID
    order.createdAt = QDateTime::currentDateTime(); // 设置创建时间
    order.updatedAt = order.createdAt;              // 更新时间同创建时间
    
    // 默认状态
    if (order.status.isEmpty()) {
        order.status = "pending";
    }
    
    m_orders.append(order);
    
    // 发射信号通知 QML
    emit orderCreated(order.id);
    emit ordersChanged();
    
    return order.id;
}

/**
 * @brief 更新数据
 * 
 * 【实现模式】
 * 部分更新：只修改 data 中包含的字段
 * 使用 contains() 检查是否需要更新某个字段
 */
bool OrdersService::updateOrder(const QString& id, const QVariantMap& data)
{
    auto it = std::find_if(m_orders.begin(), m_orders.end(),
        [&id](const Order& o) { return o.id == id; });
    
    if (it == m_orders.end()) {
        return false;  // 未找到
    }
    
    // 部分更新：只更新传入的字段
    if (data.contains("customerName")) it->customerName = data["customerName"].toString();
    if (data.contains("productName")) it->productName = data["productName"].toString();
    if (data.contains("quantity")) it->quantity = data["quantity"].toInt();
    if (data.contains("price")) it->price = data["price"].toDouble();
    if (data.contains("status")) it->status = data["status"].toString();
    
    it->updatedAt = QDateTime::currentDateTime();  // 更新时间戳
    
    emit orderUpdated(id);
    emit ordersChanged();
    
    return true;
}

/**
 * @brief 删除数据
 * 
 * 【实现模式】
 * 使用 erase() 从容器中移除元素
 */
bool OrdersService::deleteOrder(const QString& id)
{
    auto it = std::find_if(m_orders.begin(), m_orders.end(),
        [&id](const Order& o) { return o.id == id; });
    
    if (it == m_orders.end()) {
        return false;
    }
    
    m_orders.erase(it);
    
    emit orderDeleted(id);
    emit ordersChanged();
    
    return true;
}

// =============================================================================
// 业务操作实现
// =============================================================================

/**
 * @brief 便捷方法：更新状态
 * 
 * 【设计说明】
 * 这是一个便捷方法，内部调用 updateOrder
 * 对于常用操作，提供简化的 API
 */
bool OrdersService::updateStatus(const QString& id, const QString& status)
{
    return updateOrder(id, {{"status", status}});
}

/**
 * @brief 按条件筛选数据
 */
QVariantList OrdersService::getOrdersByStatus(const QString& status) const
{
    QVariantList result;
    for (const Order& order : m_orders) {
        if (order.status == status) {
            result.append(order.toVariantMap());
        }
    }
    return result;
}

/**
 * @brief 获取数据总数
 */
int OrdersService::getOrderCount() const
{
    return m_orders.size();
}

/**
 * @brief 计算统计数据（总收入）
 */
double OrdersService::getTotalRevenue() const
{
    double total = 0;
    for (const Order& order : m_orders) {
        total += order.quantity * order.price;
    }
    return total;
}

/**
 * @brief 生成唯一 ID
 * 
 * 使用 Qt 的 UUID 生成器，取前 8 位作为简短 ID
 */
QString OrdersService::generateId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}

// =============================================================================
// HTTP 网络操作
// 【MPF HTTP 客户端使用示例】
// =============================================================================

/**
 * @brief 从服务器获取数据
 * 
 * 【HTTP 客户端使用完整示例】
 * 
 * 这个方法展示了 MPF HTTP 客户端的完整使用流程：
 * 
 * 1. 创建请求选项
 *    - timeoutMs: 请求超时时间（毫秒）
 *    - headers: 自定义请求头（如 Authorization）
 * 
 * 2. 发送请求
 *    - get(): GET 请求
 *    - post(): POST 请求
 *    - postJson(): POST JSON 数据
 * 
 * 3. 处理响应
 *    - 使用 Qt 信号槽机制异步处理
 *    - reply->error() 检查错误
 *    - reply->readAll() 获取响应数据
 *    - 记得调用 reply->deleteLater() 释放资源
 * 
 * 【异步处理模式】
 * HTTP 请求是异步的，结果通过信号通知：
 * @code{.qml}
 * Connections {
 *     target: OrdersService
 *     function onFetchCompleted(success, message) {
 *         if (success) {
 *             listView.model = OrdersService.getAllOrders()
 *         }
 *     }
 * }
 * @endcode
 */
void OrdersService::fetchOrdersFromServer(const QString& apiUrl)
{
    // -------------------------------------------------------------------------
    // 步骤1: 配置请求选项
    // -------------------------------------------------------------------------
    mpf::http::HttpClient::RequestOptions options;
    options.timeoutMs = 10000;  // 10 秒超时
    
    // 如果需要认证，可以添加 headers:
    // options.headers["Authorization"] = "Bearer " + token;
    
    // -------------------------------------------------------------------------
    // 步骤2: 发送 GET 请求
    // -------------------------------------------------------------------------
    QNetworkReply* reply = m_httpClient->get(QUrl(apiUrl), options);
    
    // -------------------------------------------------------------------------
    // 步骤3: 异步处理响应
    // 使用 lambda 连接 finished 信号
    // -------------------------------------------------------------------------
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        // 重要：响应处理完后释放 reply 对象
        reply->deleteLater();
        
        // 检查网络错误
        if (reply->error() != QNetworkReply::NoError) {
            emit fetchCompleted(false, reply->errorString());
            return;
        }
        
        // 读取响应数据
        QByteArray data = reply->readAll();
        
        // 解析 JSON
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        // 验证响应格式（期望是数组）
        if (!doc.isArray()) {
            emit fetchCompleted(false, QStringLiteral("Invalid JSON response"));
            return;
        }
        
        // 清空现有数据并加载新数据
        m_orders.clear();
        QJsonArray array = doc.array();
        for (const QJsonValue& value : array) {
            if (value.isObject()) {
                QVariantMap map = value.toObject().toVariantMap();
                m_orders.append(Order::fromVariantMap(map));
            }
        }
        
        // 通知数据已更新
        emit ordersChanged();
        emit fetchCompleted(true, QStringLiteral("Fetched %1 orders").arg(m_orders.size()));
    });
}

} // namespace orders
