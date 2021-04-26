import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0

Window {
    visible: true
    width: 354 //(height / 9) * 16 //354
    height: 501
    title: qsTr("CWA-Qr-code-generator")

    Page {
        id: app

        anchors.fill: parent

        GridLayout {
            anchors.fill: parent

            rows: app.landscape() ? 1:2
            columns: app.landscape() ? 2:1

            ColumnLayout {
                Layout.preferredHeight: app.landscape() ? parent.height : parent.height * 0.6
                Layout.preferredWidth: app.landscape() ? parent.width * 0.5 : parent.width

                Item {
                    id: qrCodeContainer
                    Layout.preferredWidth: app.landscape() ? parent.width : parent.width
                    Layout.preferredHeight: app.landscape() ? parent.height : parent.height

                    Column {
                        anchors.centerIn: parent
                        spacing: qrCodeOutline.margins * 0.1

                        Image {
                            id: qrCodeOutline

                            property int margins: app.landscape() ? qrCodeContainer.width * 0.2:qrCodeContainer.height * 0.2

                            width: app.landscape() ? height : Math.min(qrCodeContainer.width, qrCodeContainer.height) - margins
                            height: app.landscape() ? Math.min(qrCodeContainer.height, qrCodeContainer.width) - margins : width

                            fillMode: Image.PreserveAspectFit

                            antialiasing: true
                            mipmap: true

                            source: "qrc:/pt-poster-qr-outline.png"

                            Image {
                                anchors.fill: parent
                                anchors.margins: parent.height * 0.05

                                fillMode: Image.PreserveAspectFit

                                source: "image://CwaQrCode"

                                sourceSize.width: width
                                sourceSize.height: width
                            }

                        }

                        Label {
                            height: qrCodeOutline.margins * 0.2
                            width: qrCodeOutline.width

                            elide: Text.ElideRight

                            font.pixelSize: height
                            text: CwaQrCodeGenerator.getConfig().description
                        }

                        Label {
                            height: qrCodeOutline.margins * 0.15
                            width: qrCodeOutline.width

                            elide: Text.ElideRight

                            font.pixelSize: height
                            text: CwaQrCodeGenerator.getConfig().address
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.preferredHeight: app.landscape() ? parent.height : parent.height * 0.4
                Layout.preferredWidth: app.landscape() ? parent.width * 0.5 : parent.width

                Item {
                    Layout.preferredWidth: app.landscape() ? parent.width : parent.width
                    Layout.maximumWidth: app.landscape() ? -1 : qrCodeOutline.width * 1.2
                    Layout.preferredHeight: app.landscape() ? parent.height : parent.height
                    Layout.alignment: Layout.Center

                    Image {
                        anchors.fill: parent
                        anchors.margins: app.landscape() ? parent.width * 0.1 : parent.height * 0.1

                        fillMode: Image.PreserveAspectFit

                        antialiasing: true
                        mipmap: true

                        source: "qrc:/pt-poster-qr-disclaimer.png"
                    }
                }
            }
        }

        function landscape() {
            return app.height < app.width
        }

    }
}
