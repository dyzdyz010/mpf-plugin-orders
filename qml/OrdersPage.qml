/**
 * =============================================================================
 * Orders Page - 主页面 QML
 * =============================================================================
 *
 * 这是一个完整的列表页面模板，展示了：
 * - 使用 MPF UI 组件库 (MPF.Components)
 * - 使用插件服务 (OrdersService)
 * - 使用数据模型 (OrderModel)
 * - 主题适配 (Theme 对象)
 * - 导航系统 (Navigation)
 * - 国际化 (qsTr)
 *
 * 【创建新页面时需要修改的地方】
 * 1. import 语句中的模块 URI
 * 2. 模型和服务名称
 * 3. 列表项组件和属性绑定
 * 4. 对话框和操作按钮
 *
 * 【MPF UI 组件使用说明】
 * - MPFCard: 卡片容器组件
 * - MPFButton: 统一风格按钮
 * - MPFDialog: 对话框组件
 * - MPFTextField: 输入框组件
 * - StatusBadge: 状态徽章组件
 * =============================================================================
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// -----------------------------------------------------------------------------
// 【模块导入】
// OrdersService 和 OrderModel 已在 C++ 中注册到 QML 引擎
// MPF.Components: MPF UI 组件库，提供 MPFCard、MPFButton 等组件
// -----------------------------------------------------------------------------
// import YourCo.Orders 1.0  // 不需要，类型已在 C++ 中注册
import MPF.Components 1.0

Page {
    id: root

    // -------------------------------------------------------------------------
    // 【国际化】
    // 使用 qsTr() 包装所有用户可见的文本
    // 这样可以通过 Qt 的翻译系统实现多语言支持
    // -------------------------------------------------------------------------
    title: qsTr("Orders")

    // -------------------------------------------------------------------------
    // 【主题适配】
    // Theme 是宿主应用提供的全局对象，包含颜色、间距等主题值
    // 使用三元运算符提供回退值，防止 Theme 未定义时出错
    // -------------------------------------------------------------------------
    background: Rectangle {
        color: Theme ? Theme.backgroundColor : "#FFFFFF"
    }

    // =========================================================================
    // 数据模型
    // =========================================================================

    // -------------------------------------------------------------------------
    // 【数据模型使用】
    // OrderModel 是在 C++ 中定义的 QAbstractListModel 子类
    // 通过 qmlRegisterType 注册到 QML
    //
    // service 属性绑定到 OrdersService 单例，模型会自动监听数据变化
    //
    // 【修改点2】改为你的模型类名和服务名
    // -------------------------------------------------------------------------
    OrderModel {
        id: orderModel
        service: OrdersService
    }

    // =========================================================================
    // 页面头部 - 工具栏
    // =========================================================================

    header: ToolBar {
        background: Rectangle {
            color: Theme ? Theme.surfaceColor : "#F5F5F5"
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: Theme ? Theme.spacingSmall : 8
            spacing: Theme ? Theme.spacingSmall : 8

            // 标题和计数
            Label {
                text: qsTr("Orders (%1)").arg(orderModel.count)
                font.pixelSize: 18
                font.bold: true
                color: Theme ? Theme.textColor : "#212121"
                Layout.fillWidth: true
            }

            // -----------------------------------------------------------------
            // 【筛选器】
            // ComboBox 用于状态筛选
            // 修改模型的 filterStatus 属性会自动触发筛选
            // -----------------------------------------------------------------
            ComboBox {
                id: statusFilter
                model: ["All", "pending", "processing", "shipped", "delivered", "cancelled"]
                onCurrentTextChanged: {
                    orderModel.filterStatus = currentText === "All" ? "" : currentText
                }
            }

            // -----------------------------------------------------------------
            // 【MPF 按钮组件】
            // MPFButton 是 MPF UI 组件库提供的统一风格按钮
            //
            // 属性说明：
            // - text: 按钮文本
            // - type: 按钮类型 (primary, secondary, success, warning, danger, ghost)
            // - size: 按钮尺寸 (small, medium, large)
            // - loading: 加载状态
            // - iconOnly: 只显示图标
            // -----------------------------------------------------------------
            MPFButton {
                text: "+"
                type: "primary"    // 主要按钮样式
                size: "small"      // 小尺寸
                onClicked: createDialog.open()
            }
        }
    }

    // =========================================================================
    // 统计卡片区域
    // =========================================================================

    RowLayout {
        id: statsRow
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme ? Theme.spacingMedium : 16
        spacing: Theme ? Theme.spacingMedium : 16

        // ---------------------------------------------------------------------
        // 【服务方法调用】
        // 直接调用 OrdersService 的 Q_INVOKABLE 方法获取数据
        // ---------------------------------------------------------------------
        StatCard {
            label: qsTr("Total Orders")
            value: OrdersService.getOrderCount()
            Layout.fillWidth: true
        }

        StatCard {
            label: qsTr("Revenue")
            value: "$" + OrdersService.getTotalRevenue().toFixed(2)
            Layout.fillWidth: true
        }
    }

    // =========================================================================
    // 数据列表
    // =========================================================================

    ListView {
        id: ordersList
        anchors.top: statsRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Theme ? Theme.spacingMedium : 16
        anchors.topMargin: Theme ? Theme.spacingSmall : 8

        spacing: Theme ? Theme.spacingSmall : 8
        clip: true

        // 绑定数据模型
        model: orderModel

        // ---------------------------------------------------------------------
        // 【列表项代理】
        // delegate 定义每个列表项的渲染方式
        // OrderCard 是单独定义的组件
        //
        // model.xxx 可以访问模型中定义的 role 数据
        //
        // 【修改点3】改为你的列表项组件和属性绑定
        // ---------------------------------------------------------------------
        delegate: OrderCard {
            width: ordersList.width

            // 属性绑定到模型数据
            orderId: model.id
            customerName: model.customerName
            productName: model.productName
            quantity: model.quantity
            price: model.price
            total: model.total
            status: model.status
            createdAt: model.createdAt

            // -----------------------------------------------------------------
            // 【详情弹窗】
            // 点击订单卡片打开详情弹窗（Popup），不使用页面导航
            // 这避免了跨 DLL 动态加载 QML 组件的问题
            // -----------------------------------------------------------------
            onClicked: {
                detailPopup.orderId = model.id
                detailPopup.open()
            }

            // 状态变更处理
            onStatusChangeRequested: function(newStatus) {
                OrdersService.updateStatus(model.id, newStatus)
            }

            // 删除处理
            onDeleteRequested: {
                deleteDialog.orderId = model.id
                deleteDialog.open()
            }
        }

        // 空状态提示
        Label {
            anchors.centerIn: parent
            visible: orderModel.count === 0
            text: qsTr("No orders found")
            color: Theme ? Theme.textSecondaryColor : "#757575"
            font.pixelSize: 16
        }
    }

    // =========================================================================
    // 对话框
    // =========================================================================

    // -------------------------------------------------------------------------
    // 【创建对话框】
    // CreateOrderDialog 是单独定义的组件
    // onAccepted 信号处理创建逻辑
    // -------------------------------------------------------------------------
    CreateOrderDialog {
        id: createDialog
        onAccepted: {
            OrdersService.createOrder(orderData)
        }
    }

    // -------------------------------------------------------------------------
    // 【订单详情弹窗】
    // 使用 Popup 显示订单详情，避免页面导航
    // -------------------------------------------------------------------------
    Popup {
        id: detailPopup
        
        property string orderId: ""
        property var orderData: ({})
        
        modal: true
        anchors.centerIn: parent
        width: Math.min(500, root.width - 48)
        height: Math.min(600, root.height - 48)
        padding: 24
        
        background: Rectangle {
            color: Theme ? Theme.surfaceColor : "#FFFFFF"
            radius: Theme ? Theme.radiusMedium : 12
        }
        
        onOpened: {
            orderData = OrdersService.getOrder(orderId)
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
            
            // Header
            RowLayout {
                Layout.fillWidth: true
                
                Label {
                    text: qsTr("Order #%1").arg(detailPopup.orderId)
                    font.pixelSize: 20
                    font.bold: true
                    color: Theme ? Theme.textColor : "#212121"
                    Layout.fillWidth: true
                }
                
                Button {
                    text: "✕"
                    flat: true
                    onClicked: detailPopup.close()
                }
            }
            
            // Status
            RowLayout {
                Label { text: qsTr("Status:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { 
                    text: detailPopup.orderData.status || ""
                    font.bold: true
                    color: Theme ? Theme.primaryColor : "#2196F3"
                }
            }
            
            // Customer
            RowLayout {
                Label { text: qsTr("Customer:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { text: detailPopup.orderData.customerName || ""; color: Theme ? Theme.textColor : "#212121" }
            }
            
            // Product
            RowLayout {
                Label { text: qsTr("Product:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { text: detailPopup.orderData.productName || ""; color: Theme ? Theme.textColor : "#212121" }
            }
            
            // Quantity & Price
            RowLayout {
                Label { text: qsTr("Quantity:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { text: String(detailPopup.orderData.quantity || 0); color: Theme ? Theme.textColor : "#212121" }
                Item { width: 24 }
                Label { text: qsTr("Price:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { text: "$" + (detailPopup.orderData.price || 0).toFixed(2); color: Theme ? Theme.textColor : "#212121" }
            }
            
            // Total
            RowLayout {
                Label { text: qsTr("Total:"); font.bold: true; color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { 
                    text: "$" + (detailPopup.orderData.total || 0).toFixed(2)
                    font.pixelSize: 18
                    font.bold: true
                    color: Theme ? Theme.primaryColor : "#2196F3"
                }
            }
            
            Item { Layout.fillHeight: true }
            
            // Actions
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                
                ComboBox {
                    id: statusCombo
                    model: ["pending", "processing", "shipped", "delivered", "cancelled"]
                    currentIndex: model.indexOf(detailPopup.orderData.status)
                    
                    onActivated: function(index) {
                        if (model[index] !== detailPopup.orderData.status) {
                            OrdersService.updateStatus(detailPopup.orderId, model[index])
                            detailPopup.orderData = OrdersService.getOrder(detailPopup.orderId)
                        }
                    }
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: qsTr("Delete")
                    onClicked: {
                        deleteDialog.orderId = detailPopup.orderId
                        deleteDialog.open()
                        detailPopup.close()
                    }
                }
                
                Button {
                    text: qsTr("Close")
                    onClicked: detailPopup.close()
                }
            }
        }
    }
    
    // -------------------------------------------------------------------------
    // 【删除确认对话框】
    // -------------------------------------------------------------------------
    Dialog {
        id: deleteDialog

        property string orderId: ""

        title: qsTr("Delete Order")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No

        anchors.centerIn: parent

        Label {
            text: qsTr("Are you sure you want to delete this order?")
        }

        onAccepted: {
            OrdersService.deleteOrder(orderId)
        }
    }

    // =========================================================================
    // 内联组件定义
    // =========================================================================

    // -------------------------------------------------------------------------
    // 【QML 内联组件】
    // component 关键字定义可复用的内联组件
    // 这里定义了一个统计卡片组件，继承自 MPFCard
    //
    // 【MPFCard 使用】
    // MPFCard 是 MPF UI 组件库提供的卡片容器
    //
    // 属性说明：
    // - title/subtitle: 卡片标题（可选）
    // - cardColor: 背景颜色
    // - borderWidth: 边框宽度
    // - cardRadius: 圆角半径
    // - hoverable/clickable: 交互效果
    // - elevated: 阴影效果
    // -------------------------------------------------------------------------
    component StatCard: MPFCard {
        property string label: ""
        property string value: ""

        implicitHeight: 80
        cardColor: Theme ? Theme.surfaceColor : "#F5F5F5"
        borderWidth: 0

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 4

            Label {
                text: parent.parent.label
                font.pixelSize: 12
                color: Theme ? Theme.textSecondaryColor : "#757575"
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                text: parent.parent.value
                font.pixelSize: 24
                font.bold: true
                color: Theme ? Theme.primaryColor : "#2196F3"
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
