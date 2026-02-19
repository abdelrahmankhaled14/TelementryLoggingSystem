import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    visible: true
    width: 900
    height: 600
    title: qsTr("Telemetry Dashboard")

    color: "#121212"

    // History buffer for CPU samples
    property var cpuSamples: []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // ===== Header =====
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                text: "Telemetry Dashboard"
                font.pixelSize: 24
                color: "white"
                Layout.alignment: Qt.AlignVCenter
            }

            Rectangle {
                Layout.fillWidth: true
                color: "transparent"
            }

            Rectangle {
                radius: 8
                color: "#1f1f1f"
                border.color: "#333"
                implicitWidth: 200
                implicitHeight: 40

                Row {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 6

                    Label {
                        text: "CPU:"
                        color: "#bbbbbb"
                        font.pixelSize: 14
                    }

                    Label {
                        text: backend.cpuUsage + "%"
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                    }
                }
            }
        }

        // ===== Main content row =====
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16

            // ===== Left: Gauge card =====
            Rectangle {
                Layout.preferredWidth: 280
                Layout.fillHeight: true
                radius: 16
                color: "#1e1e1e"
                border.color: "#333333"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16

                    Label {
                        text: "CPU Load"
                        color: "white"
                        font.pixelSize: 18
                    }

                    Rectangle {
                        id: gaugeContainer
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width * 0.8
                        height: width
                        color: "transparent"

                        Dial {
                            id: cpuDial
                            anchors.fill: parent

                            from: 0
                            to: 100
                            value: backend.cpuUsage

                            Behavior on value {
                                NumberAnimation {
                                    duration: 200
                                    easing.type: Easing.InOutQuad
                                }
                            }
                        }

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                text: backend.cpuUsage + "%"
                                color: "white"
                                font.pixelSize: 28
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }

                            Label {
                                text: {
                                    if (backend.cpuUsage >= 90)      "CRITICAL"
                                    else if (backend.cpuUsage >= 70) "High"
                                    else if (backend.cpuUsage >= 40) "Normal"
                                    else                             "Low"
                                }
                                color: "#bbbbbb"
                                font.pixelSize: 14
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }

                    Row {
                        spacing: 8
                        anchors.horizontalCenter: parent.horizontalCenter

                        Rectangle { width: 14; height: 14; radius: 3; color: "#4caf50" }
                        Label { text: "OK"; color: "#bbbbbb"; font.pixelSize: 12 }

                        Rectangle { width: 14; height: 14; radius: 3; color: "#ff9800" }
                        Label { text: "High"; color: "#bbbbbb"; font.pixelSize: 12 }

                        Rectangle { width: 14; height: 14; radius: 3; color: "#f44336" }
                        Label { text: "Critical"; color: "#bbbbbb"; font.pixelSize: 12 }
                    }
                }
            }

            // ===== Right: History card =====
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 16
                color: "#1e1e1e"
                border.color: "#333333"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8

                    Label {
                        text: "CPU Usage History"
                        color: "white"
                        font.pixelSize: 18
                        Layout.fillWidth: true
                    }

                    Canvas {
                        id: cpuChart
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        antialiasing: true

                        onPaint: {
                            var ctx = getContext("2d");
                            ctx.reset();

                            ctx.fillStyle = "#1e1e1e";
                            ctx.fillRect(0, 0, width, height);

                            // draw horizontal grid
                            ctx.strokeStyle = "#333333";
                            ctx.lineWidth = 1;
                            var steps = 5;
                            for (var i = 0; i <= steps; ++i) {
                                var y = i * height / steps;
                                ctx.beginPath();
                                ctx.moveTo(0, y);
                                ctx.lineTo(width, y);
                                ctx.stroke();
                            }

                            var arr = root.cpuSamples;
                            if (!arr || arr.length < 2)
                                return;

                            var maxSamples = arr.length;
                            var dx = width / (maxSamples - 1);

                            ctx.beginPath();
                            ctx.lineWidth = 2;

                            for (var j = 0; j < maxSamples; ++j) {
                                var v = arr[j]; // 0..100
                                var px = j * dx;
                                var py = height - (v / 100.0) * height;

                                if (j === 0)
                                    ctx.moveTo(px, py);
                                else
                                    ctx.lineTo(px, py);
                            }

                            var last = arr[arr.length - 1];
                            if (last >= 90)
                                ctx.strokeStyle = "#f44336";
                            else if (last >= 70)
                                ctx.strokeStyle = "#ff9800";
                            else
                                ctx.strokeStyle = "#4caf50";

                            ctx.stroke();
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        Label {
                            text: "Samples: " + root.cpuSamples.length
                            color: "#bbbbbb"
                            font.pixelSize: 12
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            color: "transparent"
                        }

                        Label {
                            text: "Latest: " + backend.cpuUsage + "%"
                            color: "white"
                            font.pixelSize: 14
                        }
                    }
                }

                Connections {
                    target: backend

                    function onCpuSample(value) {
                        var arr = root.cpuSamples.slice();
                        arr.push(value);

                        var maxLen = 80;
                        if (arr.length > maxLen)
                            arr = arr.slice(arr.length - maxLen);

                        root.cpuSamples = arr;
                        cpuChart.requestPaint();
                    }
                }
            }
        }
    }
}
