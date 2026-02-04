import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YourCo.Orders 1.0
import MPF.Components 1.0

Page {
    id: root
    
    property string orderId: ""
    property var orderData: ({})
    
    title: qsTr("Order Details")
    
    background: Rectangle {
        color: Theme ? Theme.backgroundColor : "#FFFFFF"
    }
    
    Component.onCompleted: {
        loadOrder()
    }
    
    function loadOrder() {
        orderData = OrdersService.getOrder(orderId)
    }
    
    Connections {
        target: OrdersService
        function onOrderUpdated(id) {
            if (id === root.orderId) {
                loadOrder()
            }
        }
    }
    
    ScrollView {
        anchors.fill: parent
        anchors.margins: Theme ? Theme.spacingMedium : 16
        
        ColumnLayout {
            width: root.width - 2 * (Theme ? Theme.spacingMedium : 16)
            spacing: Theme ? Theme.spacingMedium : 16
            
            // Header card using MPFCard
            MPFCard {
                Layout.fillWidth: true
                cardColor: Theme ? Theme.surfaceColor : "#F5F5F5"
                borderWidth: 0
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme ? Theme.spacingSmall : 8
                    
                    RowLayout {
                        Label {
                            text: qsTr("Order #%1").arg(root.orderId)
                            font.pixelSize: 20
                            font.bold: true
                            color: Theme ? Theme.textColor : "#212121"
                            Layout.fillWidth: true
                        }
                        
                        // Status badge using StatusBadge component
                        StatusBadge {
                            status: orderData.status || ""
                            size: "medium"
                        }
                    }
                    
                    Label {
                        text: qsTr("Created: %1").arg(Qt.formatDateTime(orderData.createdAt, "yyyy-MM-dd hh:mm:ss"))
                        font.pixelSize: 13
                        color: Theme ? Theme.textSecondaryColor : "#757575"
                    }
                }
            }
            
            // Customer info using MPFCard
            MPFCard {
                title: qsTr("Customer Information")
                Layout.fillWidth: true
                cardColor: Theme ? Theme.surfaceColor : "#F5F5F5"
                borderWidth: 0
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme ? Theme.spacingSmall : 8
                    
                    Label {
                        text: orderData.customerName || ""
                        font.pixelSize: 16
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }
                }
            }
            
            // Product info using MPFCard
            MPFCard {
                title: qsTr("Product Details")
                Layout.fillWidth: true
                cardColor: Theme ? Theme.surfaceColor : "#F5F5F5"
                borderWidth: 0
                
                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    columnSpacing: Theme ? Theme.spacingMedium : 16
                    rowSpacing: Theme ? Theme.spacingSmall : 8
                    
                    Label { text: qsTr("Product:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                    Label { text: orderData.productName || ""; font.bold: true; color: Theme ? Theme.textColor : "#212121" }
                    
                    Label { text: qsTr("Quantity:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                    Label { text: String(orderData.quantity || 0); color: Theme ? Theme.textColor : "#212121" }
                    
                    Label { text: qsTr("Unit Price:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                    Label { text: "$" + (orderData.price || 0).toFixed(2); color: Theme ? Theme.textColor : "#212121" }
                    
                    Label { text: qsTr("Total:"); color: Theme ? Theme.textSecondaryColor : "#757575"; font.bold: true }
                    Label { 
                        text: "$" + (orderData.total || 0).toFixed(2)
                        font.pixelSize: 18
                        font.bold: true
                        color: Theme ? Theme.primaryColor : "#2196F3"
                    }
                }
            }
            
            // Actions
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme ? Theme.spacingSmall : 8
                
                MPFButton {
                    text: qsTr("Back")
                    type: "ghost"
                    onClicked: Navigation.pop()
                }
                
                Item { Layout.fillWidth: true }
                
                ComboBox {
                    id: statusCombo
                    model: ["pending", "processing", "shipped", "delivered", "cancelled"]
                    currentIndex: model.indexOf(orderData.status)
                    
                    onActivated: function(index) {
                        if (model[index] !== orderData.status) {
                            OrdersService.updateStatus(root.orderId, model[index])
                        }
                    }
                }
                
                MPFButton {
                    text: qsTr("Delete")
                    type: "danger"
                    onClicked: deleteDialog.open()
                }
            }
        }
    }
    
    // Delete confirmation using MPFDialog
    MPFDialog {
        id: deleteDialog
        title: qsTr("Delete Order")
        type: "danger"
        acceptText: qsTr("Delete")
        rejectText: qsTr("Cancel")
        
        content: Label {
            text: qsTr("Are you sure you want to delete this order?")
            wrapMode: Text.WordWrap
        }
        
        onAccepted: {
            OrdersService.deleteOrder(root.orderId)
            Navigation.pop()
        }
    }
}
