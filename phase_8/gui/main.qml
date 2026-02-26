import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15

Window {
    id: root
    visible: true
    width: 1000
    height: 600
    title: qsTr("Telemetry Dashboard")

    color: "#05070a"   // dark cockpit background

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // ==== Top row: three car-style gauges ====
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 260
            spacing: 24

            // ---------- CPU Gauge (like tachometer) ----------
            Item {
                id: cpuGauge
                Layout.fillWidth: true
                Layout.preferredHeight: 260

                // value from backend
                property real value: logParser.cpu
                // smooth animated value
                property real displayedValue: 0

                // range mapping
                property real minValue: 0
                property real maxValue: 100
                // angle sweep like car dial (-210° to +30°)
                property real minAngleDeg: -210
                property real maxAngleDeg: 30

                Behavior on displayedValue {
                    NumberAnimation {
                        duration: 250
                        easing.type: Easing.InOutQuad
                    }
                }

                onValueChanged: {
                    displayedValue = value
                }

                // Dial background and arc
                Canvas {
                    id: cpuCanvas
                    anchors.fill: parent
                    antialiasing: true

                    onPaint: {
                        var ctx = getContext("2d");
                        var w = width;
                        var h = height;
                        ctx.reset();

                        // background
                        var grad = ctx.createRadialGradient(w/2, h/2, 10, w/2, h/2, Math.max(w,h)/2);
                        grad.addColorStop(0, "#11141b");
                        grad.addColorStop(1, "#05070a");
                        ctx.fillStyle = grad;
                        ctx.fillRect(0, 0, w, h);

                        var cx = w / 2;
                        var cy = h * 0.6;              // slightly lower to feel like a real dial
                        var radius = Math.min(w, h) * 0.4;

                        var minAngle = cpuGauge.minAngleDeg * Math.PI / 180.0;
                        var maxAngle = cpuGauge.maxAngleDeg * Math.PI / 180.0;

                        // ticks
                        ctx.save();
                        ctx.translate(cx, cy);
                        var totalTicks = 10;
                        ctx.strokeStyle = "#505764";
                        ctx.lineWidth = 2;
                        for (var i = 0; i <= totalTicks; ++i) {
                            var t = i / totalTicks;
                            var a = minAngle + t * (maxAngle - minAngle);
                            var x1 = Math.cos(a) * (radius * 0.78);
                            var y1 = Math.sin(a) * (radius * 0.78);
                            var x2 = Math.cos(a) * (radius * 0.9);
                            var y2 = Math.sin(a) * (radius * 0.9);
                            ctx.beginPath();
                            ctx.moveTo(x1, y1);
                            ctx.lineTo(x2, y2);
                            ctx.stroke();
                        }
                        ctx.restore();

                        // base arc
                        ctx.strokeStyle = "#282d37";
                        ctx.lineWidth = 14;
                        ctx.beginPath();
                        ctx.arc(cx, cy, radius, minAngle, maxAngle, false);
                        ctx.stroke();

                        // value arc
                        var frac = (cpuGauge.displayedValue - cpuGauge.minValue) /
                                   (cpuGauge.maxValue - cpuGauge.minValue);
                        if (frac < 0) frac = 0;
                        if (frac > 1) frac = 1;
                        var angle = minAngle + frac * (maxAngle - minAngle);

                        // color ramp: green → yellow → red
                        var color = "#26c65b";
                        if (frac > 0.7 && frac <= 0.9)
                            color = "#ffb300";
                        else if (frac > 0.9)
                            color = "#ff5252";

                        ctx.strokeStyle = color;
                        ctx.lineWidth = 14;
                        ctx.beginPath();
                        ctx.arc(cx, cy, radius, minAngle, angle, false);
                        ctx.stroke();

                        // labels
                        ctx.fillStyle = "#dfe5f0";
                        ctx.font = "bold 16px sans-serif";
                        ctx.textAlign = "center";
                        ctx.textBaseline = "middle";
                        ctx.fillText("CPU", cx, cy - radius * 0.45);

                        ctx.font = "24px sans-serif";
                        ctx.fillText(cpuGauge.displayedValue.toFixed(0) + " %", cx, cy + radius * 0.1);

                        ctx.font = "12px sans-serif";
                        ctx.fillStyle = "#9098a6";
                        ctx.fillText("LOAD", cx, cy + radius * 0.3);
                    }
                }

                // Needle on top of canvas
                Item {
                    id: cpuNeedleContainer
                    anchors.fill: parent

                    // rotation center around dial center
                    transform: Rotation {
                        id: cpuNeedleRotation
                        origin.x: width / 2
                        origin.y: height * 0.6
                        angle: {
                            var frac = (cpuGauge.displayedValue - cpuGauge.minValue) /
                                       (cpuGauge.maxValue - cpuGauge.minValue);
                            if (frac < 0) frac = 0;
                            if (frac > 1) frac = 1;
                            return cpuGauge.minAngleDeg +
                                   frac * (cpuGauge.maxAngleDeg - cpuGauge.minAngleDeg);
                        }
                    }

                    Rectangle {
                        id: cpuNeedle
                        width: 4
                        height: Math.min(parent.width, parent.height) * 0.38
                        radius: 2
                        color: "#ff5252"
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: parent.height * 0.6 - height   // base at center

                        antialiasing: true
                    }

                    // center cap
                    Rectangle {
                        width: 14
                        height: 14
                        radius: 7
                        color: "#11141b"
                        border.color: "#ff5252"
                        border.width: 2
                        anchors.centerIn: parent
                        anchors.verticalCenterOffset: parent.height * 0.1  * 0  // keep center aligned
                        y: parent.height * 0.6 - height/2
                    }
                }

                // trigger repaints on changes
                onDisplayedValueChanged: cpuCanvas.requestPaint()
                onWidthChanged: cpuCanvas.requestPaint()
                onHeightChanged: cpuCanvas.requestPaint()

                // warning light (like check engine)
                Rectangle {
                    width: 18
                    height: 10
                    radius: 4
                    color: logParser.cpu > 85 ? "#ff5252" : "#262b34"
                    border.color: "#555b66"
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 16

                    Text {
                        anchors.centerIn: parent
                        text: "CPU"
                        font.pixelSize: 9
                        color: logParser.cpu > 85 ? "#ffffff" : "#777f8b"
                    }
                }
            }

            // ---------- RAM Gauge ----------
            Item {
                id: ramGauge
                Layout.fillWidth: true
                Layout.preferredHeight: 260

                property real value: logParser.ram
                property real displayedValue: 0
                property real minValue: 0
                property real maxValue: 100
                property real minAngleDeg: -210
                property real maxAngleDeg: 30

                Behavior on displayedValue {
                    NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
                }
                onValueChanged: displayedValue = value

                Canvas {
                    id: ramCanvas
                    anchors.fill: parent
                    antialiasing: true

                    onPaint: {
                        var ctx = getContext("2d");
                        var w = width;
                        var h = height;
                        ctx.reset();

                        var grad = ctx.createRadialGradient(w/2, h/2, 10, w/2, h/2, Math.max(w,h)/2);
                        grad.addColorStop(0, "#10141a");
                        grad.addColorStop(1, "#05070a");
                        ctx.fillStyle = grad;
                        ctx.fillRect(0, 0, w, h);

                        var cx = w / 2;
                        var cy = h * 0.6;
                        var radius = Math.min(w, h) * 0.4;

                        var minAngle = ramGauge.minAngleDeg * Math.PI / 180.0;
                        var maxAngle = ramGauge.maxAngleDeg * Math.PI / 180.0;

                        // ticks
                        ctx.save();
                        ctx.translate(cx, cy);
                        var totalTicks = 10;
                        ctx.strokeStyle = "#505764";
                        ctx.lineWidth = 2;
                        for (var i = 0; i <= totalTicks; ++i) {
                            var t = i / totalTicks;
                            var a = minAngle + t * (maxAngle - minAngle);
                            var x1 = Math.cos(a) * (radius * 0.78);
                            var y1 = Math.sin(a) * (radius * 0.78);
                            var x2 = Math.cos(a) * (radius * 0.9);
                            var y2 = Math.sin(a) * (radius * 0.9);
                            ctx.beginPath();
                            ctx.moveTo(x1, y1);
                            ctx.lineTo(x2, y2);
                            ctx.stroke();
                        }
                        ctx.restore();

                        // base arc
                        ctx.strokeStyle = "#282d37";
                        ctx.lineWidth = 14;
                        ctx.beginPath();
                        ctx.arc(cx, cy, radius, minAngle, maxAngle, false);
                        ctx.stroke();

                        // value arc
                        var frac = (ramGauge.displayedValue - ramGauge.minValue) /
                                   (ramGauge.maxValue - ramGauge.minValue);
                        if (frac < 0) frac = 0;
                        if (frac > 1) frac = 1;
                        var angle = minAngle + frac * (maxAngle - minAngle);

                        var color = "#26c65b";
                        if (frac > 0.7 && frac <= 0.9)
                            color = "#ffb300";
                        else if (frac > 0.9)
                            color = "#ff5252";

                        ctx.strokeStyle = "#4caf50";
                        ctx.lineWidth = 14;
                        ctx.beginPath();
                        ctx.arc(cx, cy, radius, minAngle, angle, false);
                        ctx.stroke();

                        ctx.fillStyle = "#dfe5f0";
                        ctx.font = "bold 16px sans-serif";
                        ctx.textAlign = "center";
                        ctx.textBaseline = "middle";
                        ctx.fillText("RAM", cx, cy - radius * 0.45);

                        ctx.font = "24px sans-serif";
                        ctx.fillText(ramGauge.displayedValue.toFixed(0) + " %", cx, cy + radius * 0.1);

                        ctx.font = "12px sans-serif";
                        ctx.fillStyle = "#9098a6";
                        ctx.fillText("USAGE", cx, cy + radius * 0.3);
                    }
                }

                Item {
                    id: ramNeedleContainer
                    anchors.fill: parent

                    transform: Rotation {
                        id: ramNeedleRotation
                        origin.x: width / 2
                        origin.y: height * 0.6
                        angle: {
                            var frac = (ramGauge.displayedValue - ramGauge.minValue) /
                                       (ramGauge.maxValue - ramGauge.minValue);
                            if (frac < 0) frac = 0;
                            if (frac > 1) frac = 1;
                            return ramGauge.minAngleDeg +
                                   frac * (ramGauge.maxAngleDeg - ramGauge.minAngleDeg);
                        }
                    }

                    Rectangle {
                        width: 4
                        height: Math.min(parent.width, parent.height) * 0.38
                        radius: 2
                        color: "#4caf50"
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: parent.height * 0.6 - height
                        antialiasing: true
                    }

                    Rectangle {
                        width: 14
                        height: 14
                        radius: 7
                        color: "#11141b"
                        border.color: "#4caf50"
                        border.width: 2
                        anchors.centerIn: parent
                        y: parent.height * 0.6 - height/2
                    }
                }

                onDisplayedValueChanged: ramCanvas.requestPaint()
                onWidthChanged: ramCanvas.requestPaint()
                onHeightChanged: ramCanvas.requestPaint()

                // "memory" warning
                Rectangle {
                    width: 22
                    height: 10
                    radius: 4
                    color: logParser.ram > 80 ? "#ffb300" : "#262b34"
                    border.color: "#555b66"
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 16

                    Text {
                        anchors.centerIn: parent
                        text: "RAM"
                        font.pixelSize: 9
                        color: logParser.ram > 80 ? "#111" : "#777f8b"
                    }
                }
            }

            // ---------- TEMP Gauge ----------
            Item {
                id: tempGauge
                Layout.fillWidth: true
                Layout.preferredHeight: 260

                property real value: logParser.temp
                property real displayedValue: 0
                property real minValue: 0
                property real maxValue: 100       // assume 0–100°C
                property real minAngleDeg: -210
                property real maxAngleDeg: 30

                Behavior on displayedValue {
                    NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
                }
                onValueChanged: displayedValue = value

                Canvas {
                    id: tempCanvas
                    anchors.fill: parent
                    antialiasing: true

                    onPaint: {
                        var ctx = getContext("2d");
                        var w = width;
                        var h = height;
                        ctx.reset();

                        var grad = ctx.createRadialGradient(w/2, h/2, 10, w/2, h/2, Math.max(w,h)/2);
                        grad.addColorStop(0, "#12151c");
                        grad.addColorStop(1, "#05070a");
                        ctx.fillStyle = grad;
                        ctx.fillRect(0, 0, w, h);

                        var cx = w / 2;
                        var cy = h * 0.6;
                        var radius = Math.min(w, h) * 0.4;

                        var minAngle = tempGauge.minAngleDeg * Math.PI / 180.0;
                        var maxAngle = tempGauge.maxAngleDeg * Math.PI / 180.0;

                        // ticks
                        ctx.save();
                        ctx.translate(cx, cy);
                        var totalTicks = 10;
                        ctx.strokeStyle = "#505764";
                        ctx.lineWidth = 2;
                        for (var i = 0; i <= totalTicks; ++i) {
                            var t = i / totalTicks;
                            var a = minAngle + t * (maxAngle - minAngle);
                            var x1 = Math.cos(a) * (radius * 0.78);
                            var y1 = Math.sin(a) * (radius * 0.78);
                            var x2 = Math.cos(a) * (radius * 0.9);
                            var y2 = Math.sin(a) * (radius * 0.9);
                            ctx.beginPath();
                            ctx.moveTo(x1, y1);
                            ctx.lineTo(x2, y2);
                            ctx.stroke();
                        }
                        ctx.restore();

                        // base arc
                        ctx.strokeStyle = "#282d37";
                        ctx.lineWidth = 14;
                        ctx.beginPath();
                        ctx.arc(cx, cy, radius, minAngle, maxAngle, false);
                        ctx.stroke();

                        // value arc
                        var frac = (tempGauge.displayedValue - tempGauge.minValue) /
                                   (tempGauge.maxValue - tempGauge.minValue);
                        if (frac < 0) frac = 0;
                        if (frac > 1) frac = 1;
                        var angle = minAngle + frac * (maxAngle - minAngle);

                        var arcColor = "#26c6da";
                        if (tempGauge.displayedValue > 70 && tempGauge.displayedValue <= 85)
                            arcColor = "#ffb300";
                        else if (tempGauge.displayedValue > 85)
                            arcColor = "#ff5252";

                        ctx.strokeStyle = arcColor;
                        ctx.lineWidth = 14;
                        ctx.beginPath();
                        ctx.arc(cx, cy, radius, minAngle, angle, false);
                        ctx.stroke();

                        ctx.fillStyle = "#dfe5f0";
                        ctx.font = "bold 16px sans-serif";
                        ctx.textAlign = "center";
                        ctx.textBaseline = "middle";
                        ctx.fillText("TEMP", cx, cy - radius * 0.45);

                        ctx.font = "24px sans-serif";
                        ctx.fillText(tempGauge.displayedValue.toFixed(0) + " °C", cx, cy + radius * 0.1);

                        ctx.font = "12px sans-serif";
                        ctx.fillStyle = "#9098a6";
                        ctx.fillText("ENGINE", cx, cy + radius * 0.3);
                    }
                }

                Item {
                    id: tempNeedleContainer
                    anchors.fill: parent

                    transform: Rotation {
                        id: tempNeedleRotation
                        origin.x: width / 2
                        origin.y: height * 0.6
                        angle: {
                            var frac = (tempGauge.displayedValue - tempGauge.minValue) /
                                       (tempGauge.maxValue - tempGauge.minValue);
                            if (frac < 0) frac = 0;
                            if (frac > 1) frac = 1;
                            return tempGauge.minAngleDeg +
                                   frac * (tempGauge.maxAngleDeg - tempGauge.minAngleDeg);
                        }
                    }

                    Rectangle {
                        width: 4
                        height: Math.min(parent.width, parent.height) * 0.38
                        radius: 2
                        color: "#42a5f5"
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: parent.height * 0.6 - height
                        antialiasing: true
                    }

                    Rectangle {
                        width: 14
                        height: 14
                        radius: 7
                        color: "#11141b"
                        border.color: "#42a5f5"
                        border.width: 2
                        anchors.centerIn: parent
                        y: parent.height * 0.6 - height/2
                    }
                }

                onDisplayedValueChanged: tempCanvas.requestPaint()
                onWidthChanged: tempCanvas.requestPaint()
                onHeightChanged: tempCanvas.requestPaint()

                // overheat light
                Rectangle {
                    width: 24
                    height: 10
                    radius: 4
                    color: logParser.temp > 80 ? "#ff5252" : "#262b34"
                    border.color: "#555b66"
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 16

                    Text {
                        anchors.centerIn: parent
                        text: "HOT"
                        font.pixelSize: 9
                        color: logParser.temp > 80 ? "#ffffff" : "#777f8b"
                    }
                }
            }
        }

        // ==== Bottom: scrolling history strip ====
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 8
            color: "#05070a"
            border.color: "#13171f"

            Canvas {
                id: historyCanvas
                anchors.fill: parent
                anchors.margins: 12
                antialiasing: true

                onPaint: {
                    var ctx = getContext("2d");
                    var w = width;
                    var h = height;
                    ctx.reset();

                    var grad = ctx.createLinearGradient(0, 0, 0, h);
                    grad.addColorStop(0, "#11141b");
                    grad.addColorStop(1, "#05070a");
                    ctx.fillStyle = grad;
                    ctx.fillRect(0, 0, w, h);

                    var cpu = logParser.cpuHistory();
                    var ram = logParser.ramHistory();
                    var temp = logParser.tempHistory();

                    var n = cpu.length;
                    if (ram.length < n) n = ram.length;
                    if (temp.length < n) n = temp.length;

                    if (n <= 1)
                        return;

                    var maxPoints = 150;
                    var startIndex = 0;
                    if (n > maxPoints)
                        startIndex = n - maxPoints;

                    // find max to scale Y
                    var maxVal = 100;
                    for (var i = startIndex; i < n; ++i) {
                        if (cpu[i] > maxVal) maxVal = cpu[i];
                        if (ram[i] > maxVal) maxVal = ram[i];
                        if (temp[i] > maxVal) maxVal = temp[i];
                    }

                    var pad = 30;
                    var plotW = w - 2 * pad;
                    var plotH = h - 2 * pad;

                    // grid lines
                    ctx.strokeStyle = "#242a36";
                    ctx.lineWidth = 1;
                    ctx.beginPath();
                    // horizontal
                    for (var gy = 0; gy <= 4; ++gy) {
                        var yy = pad + gy * plotH / 4;
                        ctx.moveTo(pad, yy);
                        ctx.lineTo(w - pad, yy);
                    }
                    // vertical
                    for (var gx = 0; gx <= 6; ++gx) {
                        var xx = pad + gx * plotW / 6;
                        ctx.moveTo(xx, pad);
                        ctx.lineTo(xx, h - pad);
                    }
                    ctx.stroke();

                    function xForIndex(idx) {
                        var pos = idx - startIndex;
                        var visibleCount = n - startIndex;
                        if (visibleCount <= 1) visibleCount = 2;
                        return pad + (plotW * pos) / (visibleCount - 1);
                    }

                    function yForValue(v) {
                        var ratio = v / maxVal;
                        return (h - pad) - ratio * plotH;
                    }

                    function drawSeries(data, color) {
                        ctx.strokeStyle = color;
                        ctx.lineWidth = 2;
                        ctx.beginPath();
                        ctx.moveTo(xForIndex(startIndex), yForValue(data[startIndex]));
                        for (var i = startIndex + 1; i < n; ++i) {
                            ctx.lineTo(xForIndex(i), yForValue(data[i]));
                        }
                        ctx.stroke();
                    }

                    drawSeries(cpu, "#ff5252"); // CPU line
                    drawSeries(ram, "#4caf50"); // RAM line
                    drawSeries(temp, "#42a5f5"); // TEMP line

                    // legend
                    ctx.font = "11px sans-serif";
                    ctx.textBaseline = "middle";
                    ctx.textAlign = "left";

                    var lx = pad;
                    var ly = pad - 10;

                    ctx.fillStyle = "#ff5252";
                    ctx.fillRect(lx, ly - 4, 14, 3);
                    ctx.fillStyle = "#dfe5f0";
                    ctx.fillText("CPU", lx + 20, ly);

                    ctx.fillStyle = "#4caf50";
                    ctx.fillRect(lx + 70, ly - 4, 14, 3);
                    ctx.fillStyle = "#dfe5f0";
                    ctx.fillText("RAM", lx + 90, ly);

                    ctx.fillStyle = "#42a5f5";
                    ctx.fillRect(lx + 140, ly - 4, 14, 3);
                    ctx.fillStyle = "#dfe5f0";
                    ctx.fillText("TEMP", lx + 160, ly);
                }
            }
        }
    }

    // repaint history when backend history updates
    Connections {
        target: logParser
        function onHistoryChanged() {
            historyCanvas.requestPaint();
        }
    }

    // smooth continuous redraw loop for gauges + history
    Timer {
        interval: 100  // 10 fps for nice motion without burning CPU
        running: true
        repeat: true
        onTriggered: {
            cpuCanvas.requestPaint();
            ramCanvas.requestPaint();
            tempCanvas.requestPaint();
            historyCanvas.requestPaint();
        }
    }
}