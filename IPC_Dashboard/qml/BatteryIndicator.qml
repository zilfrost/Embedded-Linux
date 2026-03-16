// BatteryIndicator.qml – horizontal battery bar with percentage label
import QtQuick 2.15

Item {
    id: root

    property int  level     : 100   // 0–100 %
    property bool charging  : false
    property real barWidth  : 200
    property real barHeight : 28

    width:  barWidth + 60
    height: barHeight + 24

    /* ---- label ---- */
    Text {
        id: title
        text:  "BATTERY"
        color: "#888888"
        font { pixelSize: 11; letterSpacing: 2; family: "Roboto" }
        anchors { top: parent.top; horizontalCenter: parent.horizontalCenter }
    }

    /* ---- battery shell ---- */
    Row {
        anchors { top: title.bottom; topMargin: 4; horizontalCenter: parent.horizontalCenter }
        spacing: 2

        Rectangle {
            width:  root.barWidth
            height: root.barHeight
            radius: 5
            color:  "transparent"
            border { color: "#555555"; width: 2 }

            /* fill bar */
            Rectangle {
                anchors { left: parent.left; top: parent.top; bottom: parent.bottom; margins: 3 }
                width:  Math.max(0, (root.barWidth - 6) * (root.level / 100))
                radius: 3
                color:  root.charging  ? "#29b6f6"
                      : root.level > 50 ? "#00e676"
                      : root.level > 20 ? "#ffab00"
                      :                   "#ff1744"

                Behavior on width { NumberAnimation { duration: 400; easing.type: Easing.OutCubic } }
            }

            /* percentage text */
            Text {
                anchors.centerIn: parent
                text:  root.level + "%"
                color: "#ffffff"
                font { pixelSize: 12; bold: true; family: "Roboto" }
            }
        }

        /* battery tip nub */
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width:  6
            height: root.barHeight * 0.5
            radius: 2
            color:  "#555555"
        }
    }

    /* ---- charging bolt icon ---- */
    Text {
        visible: root.charging
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }
        text:  "⚡ CHARGING"
        color: "#29b6f6"
        font { pixelSize: 11; bold: true; family: "Roboto" }
    }
}
