// TurnSignal.qml – animated left or right turn-signal arrow
import QtQuick 2.15

Item {
    id: root

    property bool active   : false
    property bool mirrored : false   // true → right arrow

    width:  44
    height: 36

    /* Blink timer (matches standard ~80 bpm flasher cadence) */
    Timer {
        running:  root.active
        repeat:   true
        interval: 600
        onTriggered: blinkState = !blinkState
    }

    property bool blinkState: false
    onActiveChanged: if (!active) blinkState = false

    /* Arrow drawn from three rectangles forming a chevron "◄" / "►" */
    Canvas {
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.fillStyle = (root.active && root.blinkState) ? "#00e676" : "#2a2a2a"

            var w = width, h = height
            ctx.beginPath()
            if (!root.mirrored) {
                /* left arrow ◄ */
                ctx.moveTo(w * 0.55, 0)
                ctx.lineTo(w * 0.55, h * 0.30)
                ctx.lineTo(w,        h * 0.30)
                ctx.lineTo(w,        h * 0.70)
                ctx.lineTo(w * 0.55, h * 0.70)
                ctx.lineTo(w * 0.55, h)
                ctx.lineTo(0,        h * 0.50)
            } else {
                /* right arrow ► */
                ctx.moveTo(w * 0.45, 0)
                ctx.lineTo(w * 0.45, h * 0.30)
                ctx.lineTo(0,        h * 0.30)
                ctx.lineTo(0,        h * 0.70)
                ctx.lineTo(w * 0.45, h * 0.70)
                ctx.lineTo(w * 0.45, h)
                ctx.lineTo(w,        h * 0.50)
            }
            ctx.closePath()
            ctx.fill()
        }
        /* Redraw whenever blink state or active flag changes */
        Connections {
            target: root
            function onBlinkStateChanged() { canvas.requestPaint() }
            function onActiveChanged()     { canvas.requestPaint() }
        }
        id: canvas
    }
}
