/**
 * =============================================================================
 * 样例 04: 使用 mpf-http-client 发起网络请求
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * 订单插件如何使用 mpf-http-client 库发起 HTTP 请求。
 *
 * 【编译时依赖】
 * mpf-http-client 是一个独立的 C++ 库，需要在 CMakeLists.txt 中链接：
 *   target_link_libraries(orders-plugin PRIVATE MPF::mpf-http-client)
 *
 * 【mpf-http-client 提供了什么】
 * - HttpClient 类，封装了 QNetworkAccessManager
 * - 支持 GET、POST、POST JSON 请求
 * - 支持超时控制和自定义请求头
 * - 返回 QNetworkReply*，使用标准 Qt 异步模式
 * =============================================================================
 */

#include <mpf/http/http_client.h>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>

/**
 * 样例：GET 请求 —— 从 API 获取数据
 */
void example_get_request()
{
    // =========================================================================
    // 创建 HttpClient 实例
    // =========================================================================
    // 通常在服务构造函数中创建，整个服务生命周期使用同一个实例
    //
    // OrdersService::OrdersService(QObject* parent)
    //     : QObject(parent)
    //     , m_httpClient(std::make_unique<mpf::http::HttpClient>(this))
    // {}

    // =========================================================================
    // 发送 GET 请求
    // =========================================================================
    //
    // // 配置请求选项
    // mpf::http::HttpClient::RequestOptions options;
    // options.timeoutMs = 10000;  // 10 秒超时
    // options.headers["Accept"] = "application/json";
    //
    // // 如果需要认证
    // options.headers["Authorization"] = "Bearer " + m_token;
    //
    // // 发送请求（异步）
    // QNetworkReply* reply = m_httpClient->get(
    //     QUrl("https://api.example.com/orders"),
    //     options
    // );
    //
    // // 处理响应
    // connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    //     reply->deleteLater();  // 【重要】释放 reply 对象
    //
    //     if (reply->error() != QNetworkReply::NoError) {
    //         MPF_LOG_ERROR("OrdersService",
    //             QString("GET failed: %1").arg(reply->errorString()));
    //         emit fetchCompleted(false, reply->errorString());
    //         return;
    //     }
    //
    //     QByteArray data = reply->readAll();
    //     QJsonDocument doc = QJsonDocument::fromJson(data);
    //
    //     if (doc.isArray()) {
    //         QJsonArray array = doc.array();
    //         for (const QJsonValue& val : array) {
    //             QVariantMap item = val.toObject().toVariantMap();
    //             // 处理每条数据...
    //         }
    //     }
    //
    //     emit fetchCompleted(true, "Success");
    // });
}

/**
 * 样例：POST 请求 —— 提交订单到服务器
 */
void example_post_request()
{
    // =========================================================================
    // POST JSON 数据
    // =========================================================================
    // postJson() 自动设置 Content-Type: application/json
    //
    // QJsonObject orderData;
    // orderData["customerName"] = "张三";
    // orderData["productName"] = "Widget Pro";
    // orderData["quantity"] = 2;
    // orderData["price"] = 99.99;
    //
    // mpf::http::HttpClient::RequestOptions options;
    // options.timeoutMs = 15000;
    // options.headers["Authorization"] = "Bearer " + m_token;
    //
    // QNetworkReply* reply = m_httpClient->postJson(
    //     QUrl("https://api.example.com/orders"),
    //     orderData,
    //     options
    // );
    //
    // connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    //     reply->deleteLater();
    //
    //     if (reply->error() != QNetworkReply::NoError) {
    //         emit submitCompleted(false, reply->errorString());
    //         return;
    //     }
    //
    //     // 读取服务器返回的订单（含服务器生成的 ID）
    //     QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    //     QJsonObject result = doc.object();
    //     QString serverId = result["id"].toString();
    //
    //     emit submitCompleted(true, serverId);
    // });
}

/**
 * 样例：POST 原始数据
 */
void example_post_raw()
{
    // 如果需要发送非 JSON 格式的数据：
    //
    // QByteArray xmlData = "<order><customer>张三</customer></order>";
    //
    // QNetworkReply* reply = m_httpClient->post(
    //     QUrl("https://api.example.com/orders/xml"),
    //     xmlData,
    //     "application/xml",     // Content-Type
    //     options
    // );
}

/**
 * 样例：在 QML 中触发 HTTP 请求
 *
 * QML 通过 Q_INVOKABLE 方法调用 C++ 的 HTTP 逻辑，
 * 通过信号接收结果。
 */
void example_qml_integration()
{
    // QML 代码：
    //
    // Button {
    //     text: "刷新数据"
    //     onClicked: OrdersService.fetchOrdersFromServer(
    //         "https://api.example.com/orders"
    //     )
    // }
    //
    // Connections {
    //     target: OrdersService
    //     function onFetchCompleted(success, message) {
    //         if (success) {
    //             statusText.text = "数据已刷新"
    //             // 模型会自动更新（因为 ordersChanged 信号）
    //         } else {
    //             statusText.text = "刷新失败: " + message
    //         }
    //     }
    // }
}
