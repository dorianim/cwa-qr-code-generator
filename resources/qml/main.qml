import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0

Window {
    visible: true
    width: (height / 9) * 16
    height: 480
    title: qsTr("CWA-Qr-code-generator")

    Page {
        id: app

        anchors.fill: parent

        RowLayout {
            anchors.fill: parent

            ColumnLayout {
                Layout.preferredHeight: parent.height
                Layout.preferredWidth: parent.width * 0.5

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: width

                    Rectangle {
                        anchors.centerIn: parent

                        width: parent.width * 0.9
                        height: width

                        radius: height * 0.05

                        // from: b7dff7
                        // to: bb1504

                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "#b7dff7" }
                            GradientStop { position: 1.5; color: "#bb1504" }
                        }

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: parent.height * 0.025
                            radius: width * 0.05 * 0.5
                            color: "#ffffff"
                            Image {
                                anchors.fill: parent
                                anchors.margins: parent.height * 0.05

                                fillMode: Image.PreserveAspectFit

                                source: "image://CwaQrCode"

                                sourceSize.width: width
                                sourceSize.height: height
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.preferredHeight: parent.height
                Layout.preferredWidth: parent.width * 0.5

                Item {
                    Layout.preferredHeight: parent.height * 0.05
                    Layout.fillWidth: true
                }
/*
                Label {
                    Layout.preferredHeight: parent.height * 0.2
                    Layout.fillWidth: true
                    height: Layout.preferredHeight

                    color: "#00719b"

                    font.bold: true
                    font.pixelSize: height * 0.4

                    horizontalAlignment: Text.AlignHCenter
                    text: CwaQrCodeGenerator.getConfig().description
                }

                Label {
                    Layout.preferredHeight: parent.height * 0.1
                    Layout.fillWidth: true
                    height: Layout.preferredHeight

                    color: "#00719b"

                    font.bold: false
                    font.pixelSize: height * 0.4

                    horizontalAlignment: Text.AlignHCenter
                    text: CwaQrCodeGenerator.getConfig().address
                }

                Item {
                    Layout.preferredHeight: parent.height * 0.05
                    Layout.fillWidth: true
                }*/

                Label {
                    Layout.preferredHeight: parent.height * 0.2
                    Layout.fillWidth: true
                    height: Layout.preferredHeight

                    color: "#00719b"

                    font.bold: false
                    font.pixelSize: height * 0.4

                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("CHECK IN.\nSTOP THE VIRUS.")
                }

                Image {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height * 0.4
                    Layout.alignment: Layout.Center
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/CWA_title.png"
                }
            }
        }
    }
}
