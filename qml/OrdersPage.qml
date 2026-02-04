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
// YourCo.Orders: 插件的 QML 模块，提供 OrdersService 和 OrderModel
// MPF.Components: MPF UI 组件库，提供 MPFCard、MPFButton 等组件
//
// 【修改点1】将 YourCo.Orders 改为你的模块 URI
// -----------------------------------------------------------------------------
import YourCo.Orders 1.0
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
            // 【导航】
            // Navigation 是宿主应用提供的全局导航服务
            // push(route, params) 导航到指定路由，并传递参数
            //
            // 路由名称在插件 start() 中通过 INavigation::registerRoute 注册
            // -----------------------------------------------------------------
            onClicked: {
                Navigation.push("orders/detail", {orderId: model.id})
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
    // 【删除确认对话框】
    // 使用 Qt Quick Controls 的 Dialog 组件
    // 【修改点4】可以改为使用 MPFDialog 获得统一风格
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
