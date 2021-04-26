QT += quick qml quickcontrols2 widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Don't define when in release mode
CONFIG(debug, debug | release) {
    # Testing enviroment
    TEST_CONFIG="\"$$PWD/cwa-qr.conf.example\""
    DEFINES += TEST_CONFIG=\"\\\"$${TEST_CONFIG}\\\"\"
}

INCLUDEPATH += headers

SOURCES += \
        sources/cwaqrcodegenerator.cpp \
        sources/trace_location.pb.cc \
        sources/main.cpp

HEADERS += \
    headers/cwaqrcodegenerator.h \
    headers/trace_location.pb.h

RESOURCES += resources/qml/qml.qrc \
    resources/shared/shared.qrc

LIBS += -lprotobuf

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
TARGET = cwa-qr-code-generator
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += qzxing_multimedia
include(QZXing/QZXing.pri)

DISTFILES += \
    README.md \
    cwa-qr.conf.example
