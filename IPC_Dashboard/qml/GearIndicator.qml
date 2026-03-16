// GearIndicator.qml – P / R / N / D gear selector display
import QtQuick 2.15

Row {
    id: root

    property string currentGear: "P"

    spacing: 8

    Repeater {
        model: ["P", "R", "N", "D"]

        delegate: Rectangle {
            property bool active: modelData === root.currentGear

            width:  38
            height: 38
            radius: 4
            color:  active ? gearColor(modelData) : "#1e1e1e"
            border { color: active ? gearColor(modelData) : "#333333"; width: 1 }

            function gearColor(g) {
                switch (g) {
                case "R": return "#ff5252"   // red – reverse
                case "N": return "#ffab00"   // amber – neutral
                case "D": return "#00e676"   // green – drive
                default:  return "#42a5f5"   // blue – park
                }
            }

            Text {
                anchors.centerIn: parent
                text:  modelData
                color: parent.active ? "#121212" : "#666666"
                font { pixelSize: 18; bold: true; family: "Roboto" }
            }

            Behavior on color { ColorAnimation { duration: 200 } }
        }
    }
}
