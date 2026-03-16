// main.qml – EV Instrument Panel Cluster root window
// Target: Raspberry Pi 4 with 7-inch HDMI display (1024 × 600)
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    id: root

    visible:      true
    width:        1024
    height:       600
    title:        "IPC Dashboard"
    color:        "#0d0d0d"

    /* Full-screen on Pi (no window decorations) */
    flags: Qt.Window | Qt.FramelessWindowHint

    /* ------------------------------------------------------------------ */
    /* Clock                                                                */
    /* ------------------------------------------------------------------ */
    Timer {
        interval: 1000
        running:  true
        repeat:   true
        triggeredOnStart: true
        onTriggered: clockLabel.text = Qt.formatTime(new Date(), "hh:mm")
    }

    /* ================================================================== */
    /* TOP BAR                                                              */
    /* ================================================================== */
    Rectangle {
        id: topBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 40
        color:  "#161616"

        Row {
            anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }
            spacing: 8

            /* Left turn signal */
            TurnSignal {
                id: leftArrow
                active:   vehicleData.leftSignal
                mirrored: false
                width: 36; height: 28
            }

            /* Right turn signal */
            TurnSignal {
                id: rightArrow
                active:   vehicleData.rightSignal
                mirrored: true
                width: 36; height: 28
            }
        }

        /* Clock */
        Text {
            id: clockLabel
            anchors.centerIn: parent
            color: "#cccccc"
            font { pixelSize: 16; family: "Roboto" }
        }

        /* Range */
        Row {
            anchors { right: parent.right; rightMargin: 16; verticalCenter: parent.verticalCenter }
            spacing: 4
            Text {
                text:  "🔋 Range:"
                color: "#888888"
                font { pixelSize: 13; family: "Roboto" }
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text:  vehicleData.range + " km"
                color: "#ffffff"
                font { pixelSize: 13; bold: true; family: "Roboto" }
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    /* ================================================================== */
    /* MAIN CONTENT AREA                                                    */
    /* ================================================================== */
    Item {
        anchors {
            top:    topBar.bottom
            bottom: bottomBar.top
            left:   parent.left
            right:  parent.right
        }

        /* ---------- LEFT PANEL: RPM & motor temp ---------- */
        Column {
            anchors { left: parent.left; leftMargin: 24; verticalCenter: parent.verticalCenter }
            spacing: 20

            /* RPM gauge (vertical bar style) */
            Column {
                spacing: 6
                Text {
                    text:  "RPM"
                    color: "#888888"
                    font { pixelSize: 11; letterSpacing: 2; family: "Roboto" }
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Rectangle {
                    width:  28
                    height: 180
                    radius: 4
                    color:  "#1e1e1e"
                    border { color: "#333333"; width: 1 }

                    Rectangle {
                        anchors { bottom: parent.bottom; left: parent.left; right: parent.right; margins: 3 }
                        height: Math.max(0, (180 - 6) * (vehicleData.rpm / 10000))
                        radius: 3
                        color:  vehicleData.rpm < 6000 ? "#00e676"
                              : vehicleData.rpm < 8500 ? "#ffab00"
                              :                           "#ff1744"
                        Behavior on height { NumberAnimation { duration: 150 } }
                    }
                }
                Text {
                    text:  (vehicleData.rpm / 1000).toFixed(1) + "k"
                    color: "#ffffff"
                    font { pixelSize: 13; bold: true; family: "Roboto" }
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }

            /* Motor temperature */
            Column {
                spacing: 6
                Text {
                    text:  "MOTOR"
                    color: "#888888"
                    font { pixelSize: 11; letterSpacing: 2; family: "Roboto" }
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Rectangle {
                    width:  28
                    height: 120
                    radius: 4
                    color:  "#1e1e1e"
                    border { color: "#333333"; width: 1 }

                    Rectangle {
                        anchors { bottom: parent.bottom; left: parent.left; right: parent.right; margins: 3 }
                        height: Math.max(0, (120 - 6) * ((vehicleData.motorTemp - 20) / 80))
                        radius: 3
                        color:  vehicleData.motorTemp < 60 ? "#29b6f6"
                              : vehicleData.motorTemp < 80 ? "#ffab00"
                              :                               "#ff1744"
                        Behavior on height { NumberAnimation { duration: 300 } }
                    }
                }
                Text {
                    text:  vehicleData.motorTemp.toFixed(0) + "°C"
                    color: "#ffffff"
                    font { pixelSize: 12; bold: true; family: "Roboto" }
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        /* ---------- CENTER: Speed gauge ---------- */
        SpeedGauge {
            anchors.centerIn: parent
            size:  320
            value: vehicleData.speed

            Behavior on value { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }
        }

        /* ---------- RIGHT PANEL: Battery & gear ---------- */
        Column {
            anchors { right: parent.right; rightMargin: 24; verticalCenter: parent.verticalCenter }
            spacing: 28

            BatteryIndicator {
                level:    vehicleData.batteryLevel
                charging: vehicleData.isCharging
                barWidth: 180
            }

            /* Gear indicator */
            Column {
                spacing: 6
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text:  "GEAR"
                    color: "#888888"
                    font { pixelSize: 11; letterSpacing: 2; family: "Roboto" }
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                GearIndicator {
                    anchors.horizontalCenter: parent.horizontalCenter
                    currentGear: vehicleData.gear
                }
            }
        }
    }

    /* ================================================================== */
    /* BOTTOM STATUS BAR                                                    */
    /* ================================================================== */
    Rectangle {
        id: bottomBar
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
        height: 36
        color:  "#161616"

        Row {
            anchors.centerIn: parent
            spacing: 48

            /* Odometer / trip placeholder */
            Row {
                spacing: 4
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    text:  "ODO"
                    color: "#666666"
                    font.pixelSize: 11
                    font.family:    "Roboto"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    text:  "00000 km"
                    color: "#aaaaaa"
                    font.pixelSize: 13
                    font.bold:      true
                    font.family:    "Roboto"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            /* Brake / regen placeholder */
            Row {
                spacing: 4
                anchors.verticalCenter: parent.verticalCenter
                Rectangle {
                    width:  10
                    height: 10
                    radius: 5
                    color:  vehicleData.speed === 0 ? "#ff1744" : "#333333"
                    anchors.verticalCenter: parent.verticalCenter
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
                Text {
                    text:  "BRAKE"
                    color: "#666666"
                    font.pixelSize: 11
                    font.family:    "Roboto"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            /* Power mode indicator */
            Row {
                spacing: 4
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    text:  "MODE"
                    color: "#666666"
                    font.pixelSize: 11
                    font.family:    "Roboto"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    text:  "ECO"
                    color: "#00e676"
                    font.pixelSize: 12
                    font.bold:      true
                    font.family:    "Roboto"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
