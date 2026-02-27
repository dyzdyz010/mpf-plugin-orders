/**
 * =============================================================================
 * 样例 05: QML 层的 EventBus 使用
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * 在 QML 中如何使用 EventBus 发布和接收事件。
 *
 * 【运行时依赖】
 * EventBus 是 Host 通过 setContextProperty("EventBus", ...) 注入的全局对象。
 * QML 代码不需要 import 任何模块就可以直接使用。
 *
 * 【QML vs C++ 的 EventBus 使用】
 * - QML 层：通过 Connections + onEventPublished 接收所有事件，手动过滤
 * - C++ 层：通过 connect(SIGNAL(eventPublished(...))) 接收，手动过滤
 * - 两者等价，选择取决于业务逻辑放在哪一层
 * =============================================================================
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MPF.Components 1.0

Page {
    id: root
    title: "EventBus QML 使用示例"

    background: Rectangle {
        color: Theme ? Theme.backgroundColor : "#FAFAFA"
    }

    // =========================================================================
    // 【核心】EventBus 事件监听
    // =========================================================================
    //
    // Connections 组件连接到全局 EventBus 对象
    // onEventPublished 会在每个事件发布时被调用
    // 【重要】所有事件都会触发这个回调，需要手动过滤 topic

    Connections {
        target: EventBus  // 全局 EventBus 对象（Host 注入）

        function onEventPublished(topic, data, senderId) {
            // =================================================================
            // 手动过滤感兴趣的 topic
            // =================================================================

            if (topic === "rules/check/completed") {
                // 规则检查完成事件
                var orderId = data.orderId
                var passed = data.passed

                if (passed) {
                    statusText.text = "订单 " + orderId + " 规则检查通过 ✓"
                    statusText.color = Theme ? Theme.successColor : "green"
                } else {
                    statusText.text = "订单 " + orderId + " 规则检查失败: " + data.reason
                    statusText.color = Theme ? Theme.errorColor : "red"
                }
            }
            else if (topic === "system/theme/changed") {
                // 主题变更事件
                console.log("主题已切换到:", data.themeName)
            }

            // =================================================================
            // 事件日志（调试用）
            // =================================================================
            eventLog.append({
                time: new Date().toLocaleTimeString(),
                topic: topic,
                sender: senderId,
                data: JSON.stringify(data)
            })
        }
    }

    // =========================================================================
    // 组件初始化时注册订阅
    // =========================================================================
    Component.onCompleted: {
        // subscribeSimple 是 subscribe 的简化版本（无需 SubscriptionOptions）
        EventBus.subscribeSimple("rules/**", "com.yourco.orders.qml")
        EventBus.subscribeSimple("system/**", "com.yourco.orders.qml")
        console.log("QML: Subscribed to rules/** and system/**")
    }

    // 组件销毁时取消订阅
    Component.onDestruction: {
        EventBus.unsubscribeAll("com.yourco.orders.qml")
        console.log("QML: Unsubscribed all")
    }

    // =========================================================================
    // UI 布局
    // =========================================================================
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme ? Theme.spacingMedium : 16
        spacing: Theme ? Theme.spacingMedium : 16

        // 标题
        Label {
            text: "EventBus 事件发布与监听示例"
            font.pixelSize: 20
            font.bold: true
            color: Theme ? Theme.textColor : "#212121"
        }

        // 状态显示
        Label {
            id: statusText
            text: "等待事件..."
            font.pixelSize: 14
            color: Theme ? Theme.textSecondaryColor : "#757575"
        }

        // -----------------------------------------------------------------
        // 【发布事件】
        // 在 QML 中发布事件的示例
        // -----------------------------------------------------------------
        MPFCard {
            title: "发布事件"
            Layout.fillWidth: true

            ColumnLayout {
                spacing: Theme ? Theme.spacingSmall : 8

                MPFButton {
                    text: "发布 orders/created 事件"
                    type: "primary"
                    onClicked: {
                        // publish() 发布事件
                        // 参数: topic, data (JS object), senderId
                        var notified = EventBus.publish(
                            "orders/created",
                            {
                                "orderId": "ORD-" + Date.now(),
                                "customerName": "QML 测试客户",
                                "totalAmount": 199.99
                            },
                            "com.yourco.orders"
                        )
                        console.log("通知了", notified, "个订阅者")
                    }
                }

                MPFButton {
                    text: "发布 orders/status_changed 事件"
                    type: "success"
                    onClicked: {
                        EventBus.publish(
                            "orders/status_changed",
                            {
                                "orderId": "ORD-001",
                                "oldStatus": "pending",
                                "newStatus": "processing"
                            },
                            "com.yourco.orders"
                        )
                    }
                }
            }
        }

        // -----------------------------------------------------------------
        // 【事件日志】
        // 显示所有接收到的事件
        // -----------------------------------------------------------------
        MPFCard {
            title: "事件日志"
            subtitle: "实时显示接收到的 EventBus 事件"
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: eventLogView
                anchors.fill: parent
                clip: true
                spacing: 4

                model: ListModel {
                    id: eventLog
                }

                delegate: Rectangle {
                    width: eventLogView.width
                    height: delegateLayout.implicitHeight + 12
                    radius: Theme ? Theme.radiusSmall : 4
                    color: Theme ? Qt.alpha(Theme.primaryColor, 0.05) : "#F5F5F5"

                    ColumnLayout {
                        id: delegateLayout
                        anchors {
                            left: parent.left; right: parent.right
                            verticalCenter: parent.verticalCenter
                            margins: 8
                        }
                        spacing: 2

                        RowLayout {
                            Label {
                                text: model.time
                                font.pixelSize: 11
                                color: Theme ? Theme.textSecondaryColor : "#999"
                            }
                            Label {
                                text: model.topic
                                font.pixelSize: 12
                                font.bold: true
                                color: Theme ? Theme.primaryColor : "#2196F3"
                            }
                            Label {
                                text: "← " + model.sender
                                font.pixelSize: 11
                                color: Theme ? Theme.textSecondaryColor : "#999"
                            }
                        }

                        Label {
                            text: model.data
                            font.pixelSize: 11
                            color: Theme ? Theme.textColor : "#333"
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }

        // -----------------------------------------------------------------
        // 【查询 EventBus 状态】
        // -----------------------------------------------------------------
        RowLayout {
            spacing: Theme ? Theme.spacingSmall : 8

            MPFButton {
                text: "查询订阅者数"
                type: "ghost"
                size: "small"
                onClicked: {
                    var count = EventBus.subscriberCount("orders/created")
                    statusText.text = "orders/created 订阅者: " + count
                    statusText.color = Theme ? Theme.textColor : "#212121"
                }
            }

            MPFButton {
                text: "查询活跃主题"
                type: "ghost"
                size: "small"
                onClicked: {
                    var topics = EventBus.topics  // Q_PROPERTY
                    statusText.text = "活跃主题: " + topics.join(", ")
                    statusText.color = Theme ? Theme.textColor : "#212121"
                }
            }

            MPFButton {
                text: "总订阅数"
                type: "ghost"
                size: "small"
                onClicked: {
                    statusText.text = "总订阅数: " + EventBus.totalSubscribers
                    statusText.color = Theme ? Theme.textColor : "#212121"
                }
            }
        }
    }
}
