import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MPF.Components 1.0

MPFDialog {
    id: root
    
    property var orderData: ({})
    
    title: qsTr("Create New Order")
    acceptText: qsTr("Create")
    rejectText: qsTr("Cancel")
    
    width: Math.min(400, parent ? parent.width - 40 : 400)
    
    onOpened: {
        customerField.clear()
        productField.clear()
        quantityField.text = "1"
        priceField.text = ""
        customerField.focus()
    }
    
    onAccepted: {
        orderData = {
            customerName: customerField.text,
            productName: productField.text,
            quantity: parseInt(quantityField.text) || 1,
            price: parseFloat(priceField.text) || 0,
            status: "pending"
        }
    }
    
    content: ColumnLayout {
        width: parent.width
        spacing: Theme ? Theme.spacingMedium : 16
        
        // Customer name
        MPFTextField {
            id: customerField
            Layout.fillWidth: true
            label: qsTr("Customer Name")
            placeholder: qsTr("Enter customer name")
            required: true
        }
        
        // Product name
        MPFTextField {
            id: productField
            Layout.fillWidth: true
            label: qsTr("Product Name")
            placeholder: qsTr("Enter product name")
            required: true
        }
        
        // Quantity and Price row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme ? Theme.spacingMedium : 16
            
            MPFTextField {
                id: quantityField
                Layout.fillWidth: true
                label: qsTr("Quantity")
                placeholder: "1"
                text: "1"
                validator: IntValidator { bottom: 1 }
                inputMethodHints: Qt.ImhDigitsOnly
            }
            
            MPFTextField {
                id: priceField
                Layout.fillWidth: true
                label: qsTr("Unit Price")
                prefix: "$"
                placeholder: "0.00"
                validator: DoubleValidator { bottom: 0; decimals: 2 }
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
        }
        
        // Total preview
        MPFCard {
            Layout.fillWidth: true
            implicitHeight: 48
            cardColor: Theme ? Qt.alpha(Theme.primaryColor, 0.1) : "#1A2196F3"
            borderWidth: 0
            contentPadding: Theme ? Theme.spacingSmall : 8
            
            RowLayout {
                anchors.fill: parent
                
                Label {
                    text: qsTr("Total:")
                    font.pixelSize: 14
                    color: Theme ? Theme.textSecondaryColor : "#757575"
                }
                
                Item { Layout.fillWidth: true }
                
                Label {
                    text: {
                        var qty = parseInt(quantityField.text) || 0
                        var price = parseFloat(priceField.text) || 0
                        return "$" + (qty * price).toFixed(2)
                    }
                    font.pixelSize: 18
                    font.bold: true
                    color: Theme ? Theme.primaryColor : "#2196F3"
                }
            }
        }
    }
}
