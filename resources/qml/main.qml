import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0

Window {
    visible: true
    width: screenWidth //354 //(height / 9) * 16 //354
    height: screenHeight //501
    title: qsTr("CWA-Qr-code-generator")

    Page {
        id: app

        property bool swappedLayout: false

        anchors.fill: parent

        FontLoader {
            id: defaultFont
            source: "qrc:/Roboto-Regular.ttf"
        }

        Timer {
            id: layoutSwapTimer
            interval: CwaQrCodeGenerator.guiConfiguration.swapAfterMinutes * 60 * 1000
            repeat: true
            running: true
            onTriggered: app.swappedLayout = !app.swappedLayout
        }

        Item {
            anchors.fill: parent

            ColumnLayout {
                x: app.landscape() ? app.swappedLayout ? 0 : parent.width * 0.5 : 0
                y: app.landscape() ? 0 : app.swappedLayout ? 0 : parent.height * 0.6

                height: app.landscape() ? parent.height : parent.height * 0.4
                width: app.landscape() ? parent.width * 0.5 : parent.width

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

                Behavior on x {
                    NumberAnimation {
                        duration: CwaQrCodeGenerator.guiConfiguration.swapAnimationDuration
                        easing.type: Easing.InOutQuart
                    }
                }

                Behavior on y {
                    NumberAnimation {
                        duration: CwaQrCodeGenerator.guiConfiguration.swapAnimationDuration
                        easing.type: Easing.InOutQuart
                    }
                }
            }

            ColumnLayout {
                x: app.landscape() ? app.swappedLayout ? parent.width * 0.5 : 0 : 0
                y: app.landscape() ? 0 : app.swappedLayout ? parent.height * 0.4 : 0

                height: app.landscape() ? parent.height : parent.height * 0.6
                width: app.landscape() ? parent.width * 0.5 : parent.width

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

                                source: CwaQrCodeGenerator.currentQrCode.imageUrl

                                sourceSize.width: width
                                sourceSize.height: width
                            }
                        }

                        Label {
                            height: qrCodeOutline.margins * 0.2
                            width: qrCodeOutline.width

                            elide: Text.ElideRight

                            font.pixelSize: height
                            font.family: defaultFont.name

                            text: CwaQrCodeGenerator.currentQrCode.description
                        }

                        Label {
                            id: qrCodeAddressLabel
                            height: qrCodeOutline.margins * 0.12
                            width: qrCodeOutline.width

                            elide: Text.ElideRight

                            font.pixelSize: height
                            font.family: defaultFont.name

                            text: CwaQrCodeGenerator.currentQrCode.address
                        }
                    }
                }

                Behavior on x {
                    NumberAnimation {
                        duration: CwaQrCodeGenerator.guiConfiguration.swapAnimationDuration
                        easing.type: Easing.InOutQuart
                    }
                }

                Behavior on y {
                    NumberAnimation {
                        duration: CwaQrCodeGenerator.guiConfiguration.swapAnimationDuration
                        easing.type: Easing.InOutQuart
                    }
                }

            }
        }

        Label {
            anchors {
                bottom: parent.bottom
                bottomMargin: height * 0.5
                horizontalCenter: parent.horizontalCenter
            }

            height: qrCodeAddressLabel.height * 0.7

            font.pixelSize: height
            font.family: defaultFont.name

            text: "Neuer QR-Code in " + CwaQrCodeGenerator.remaningTimeUntilRegeneration

        }

        function landscape() {
            return app.height < app.width
        }

    }
}
