// SpeedGauge.qml – circular speedometer for the EV dashboard
import QtQuick 2.15
import QtQuick.Shapes 1.15

Item {
    id: root

    property int  value    : 0          // current speed (km/h)
    property int  maxValue : 260        // full-scale speed
    property real size     : 300        // diameter of the gauge

    width:  size
    height: size

    /* ---- arc geometry helpers ---- */
    readonly property real cx        : size / 2
    readonly property real cy        : size / 2
    readonly property real outerR    : size / 2 - 4
    readonly property real innerR    : outerR - 18
    readonly property real startAngle: 225   // degrees (7 o'clock)
    readonly property real sweepTotal: 270   // total arc sweep
    readonly property real valueAngle: startAngle + sweepTotal * (value / maxValue)

    /* ---- track arc (dark grey) ---- */
    Shape {
        anchors.fill: parent
        ShapePath {
            strokeColor: "#2a2a2a"
            strokeWidth: 18
            fillColor:   "transparent"
            capStyle:    ShapePath.RoundCap
            PathAngleArc {
                centerX:      root.cx
                centerY:      root.cy
                radiusX:      (root.outerR + root.innerR) / 2
                radiusY:      (root.outerR + root.innerR) / 2
                startAngle:   root.startAngle - 90
                sweepAngle:   root.sweepTotal
            }
        }
    }

    /* ---- value arc (green → amber → red based on speed) ---- */
    Shape {
        anchors.fill: parent
        ShapePath {
            strokeColor: value < 100 ? "#00e676"
                       : value < 180 ? "#ffab00"
                       :               "#ff1744"
            strokeWidth: 18
            fillColor:   "transparent"
            capStyle:    ShapePath.RoundCap
            PathAngleArc {
                centerX:    root.cx
                centerY:    root.cy
                radiusX:    (root.outerR + root.innerR) / 2
                radiusY:    (root.outerR + root.innerR) / 2
                startAngle: root.startAngle - 90
                sweepAngle: root.sweepTotal * (root.value / root.maxValue)
            }
        }
    }

    /* ---- tick marks ---- */
    Repeater {
        model: 14  // 0, 20, 40 … 260 km/h
        delegate: Rectangle {
            property real tickAngle: (root.startAngle + index * (root.sweepTotal / 13)) * Math.PI / 180
            property bool isMajor  : (index % 1 === 0)
            width:  isMajor ? 2 : 1
            height: isMajor ? 12 : 7
            color:  "#888888"
            x: root.cx + Math.cos(tickAngle) * (root.outerR - 2) - width / 2
            y: root.cy + Math.sin(tickAngle) * (root.outerR - 2) - height / 2
            transform: Rotation {
                origin.x: width / 2
                origin.y: height / 2
                angle:    root.startAngle + index * (root.sweepTotal / 13) + 90
            }
        }
    }

    /* ---- speed label ---- */
    Column {
        anchors.centerIn: parent
        spacing: 2

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  root.value
            font { pixelSize: root.size * 0.22; bold: true; family: "Roboto" }
            color: "#ffffff"
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text:  "km/h"
            font { pixelSize: root.size * 0.07; family: "Roboto" }
            color: "#aaaaaa"
        }
    }

    /* ---- outer ring ---- */
    Rectangle {
        anchors.centerIn: parent
        width:  root.size
        height: root.size
        radius: root.size / 2
        color:  "transparent"
        border { color: "#333333"; width: 2 }
    }
}
