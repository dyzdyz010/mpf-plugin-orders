import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MPF.Components 1.0

MPFCard {
    id: root
    
    property string orderId: ""
    property string customerName: ""
    property string productName: ""
    property int quantity: 0
    property double price: 0
    property double total: 0
    property string status: ""
    property date createdAt
    
    signal statusChangeRequested(string newStatus)
    signal deleteRequested()
    
    implicitHeight: 120
    cardColor: Theme ? Theme.surfaceColor : "#F5F5F5"
    borderWidth: 0
    clickable: true
    hoverable: true
    contentPadding: Theme ? Theme.spacingMedium : 16
    
    RowLayout {
        anchors.fill: parent
        spacing: Theme ? Theme.spacingMedium : 16
        
        // Main info
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            
            RowLayout {
                spacing: Theme ? Theme.spacingSmall : 8
                
                Label {
                    text: root.productName
                    font.pixelSize: 16
                    font.bold: true
                    color: Theme ? Theme.textColor : "#212121"
                }
                
                // Status badge using StatusBadge component
                StatusBadge {
                    status: root.status
                    size: "small"
                }
            }
            
            Label {
                text: qsTr("Customer: %1").arg(root.customerName)
                font.pixelSize: 14
                color: Theme ? Theme.textSecondaryColor : "#757575"
            }
            
            Label {
                text: qsTr("Qty: %1 × $%2").arg(root.quantity).arg(root.price.toFixed(2))
                font.pixelSize: 13
                color: Theme ? Theme.textSecondaryColor : "#757575"
            }
            
            Label {
                text: Qt.formatDateTime(root.createdAt, "yyyy-MM-dd hh:mm")
                font.pixelSize: 12
                color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.7) : "#9E9E9E"
            }
        }
        
        // Total and actions
        ColumnLayout {
            spacing: Theme ? Theme.spacingSmall : 8
            Layout.alignment: Qt.AlignRight
            
            Label {
                text: "$" + root.total.toFixed(2)
                font.pixelSize: 20
                font.bold: true
                color: Theme ? Theme.primaryColor : "#2196F3"
                Layout.alignment: Qt.AlignRight
            }
            
            RowLayout {
                spacing: 4
                
                // Status change menu
                ComboBox {
                    id: statusCombo
                    model: ["pending", "processing", "shipped", "delivered", "cancelled"]
                    currentIndex: model.indexOf(root.status)
                    
                    implicitWidth: 100
                    
                    onActivated: function(index) {
                        if (model[index] !== root.status) {
                            root.statusChangeRequested(model[index])
                        }
                    }
                }
                
                MPFButton {
                    text: "🗑️"
                    type: "danger"
                    size: "small"
                    iconOnly: true
                    onClicked: root.deleteRequested()
                }
            }
        }
    }
}
