#ifndef CWAQRCODEGENERATOR_H
#define CWAQRCODEGENERATOR_H

#include <QObject>
#include <QtDebug>
#include <QRandomGenerator>
#include <QFile>
#include <QSettings>
#include <qquickimageprovider.h>
#include <QtQml/QQmlContext>
#include <QTimer>
#include <QDate>
#include <QDateTime>
#include <QRegularExpression>

#include "QZXing.h"
#include "trace_location.pb.h"

using ::internal::pt::TraceLocationType;
using ::internal::pt::TraceLocation;
using ::internal::pt::CrowdNotifierData;
using ::internal::pt::CWALocationData;
using ::internal::pt::QRCodePayload;

class CwaQrCodeGenerator : public QObject, public QQuickImageProvider
{
    Q_OBJECT
    Q_PROPERTY(QVariant currentQrCode READ currentQrCode NOTIFY currentQrCodeChanged)
    Q_PROPERTY(QString remaningTimeUntilRegeneration READ remaningTimeUntilRegeneration NOTIFY remaningTimeUntilRegenerationChanged)
    Q_PROPERTY(QVariantMap guiConfiguration READ guiConfiguration NOTIFY guiConfigurationChanged)

public:
    explicit CwaQrCodeGenerator(QString configFilePath, QObject *parent = nullptr);

    enum State {
        Initializing,
        Ready
    };

    static void registerQMLImageProvider(QString configFilePath, QQmlEngine& engine);

    Q_INVOKABLE QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
    Q_INVOKABLE QVariantMap currentQrCode();
    Q_INVOKABLE QString remaningTimeUntilRegeneration();
    Q_INVOKABLE QVariantMap guiConfiguration();

private:
    typedef struct CwaLocationConfiguration {
        TraceLocationType type;
        QString description;
        QString address;
        uint32_t defaultCheckinLengthInMinutes;
        uint64_t startTimestamp;
        uint64_t endTimestamp;
        bool isValid;
    } cwaQrCodeConfiguration_t;

    typedef struct CwaRegenerationConfiguration {
        int hour;
        int minute;
    } cwaRegenerationConfiguration_t;

    typedef struct CwaGuiConfiguration {
        int swapAfterMinutes;
        int swapAnimationDuration;
    } cwaGuiConfiguration_t;

    const QList<TraceLocationType> _temporaryLocationTypes = {
        TraceLocationType::LOCATION_TYPE_PERMANENT_OTHER,
        TraceLocationType::LOCATION_TYPE_TEMPORARY_CULTURAL_EVENT,
        TraceLocationType::LOCATION_TYPE_TEMPORARY_CLUB_ACTIVITY,
        TraceLocationType::LOCATION_TYPE_TEMPORARY_PRIVATE_EVENT,
        TraceLocationType::LOCATION_TYPE_TEMPORARY_WORSHIP_SERVICE
    };

    CwaLocationConfiguration _locationConfiguration;
    CwaRegenerationConfiguration _regenerationConfiguration;
    CwaGuiConfiguration _guiConfiguration;
    QString _currentQrCodePayload;
    bool _qrCodeIndex;
    State _state;
    QTimer* _regenerationTimer;
    QTimer* _remaningRegenerationTimeRefreshTimer;

    std::string _generateRandomSeed(uint legth);
    void _readConfigFile(QString path);
    bool _generateQrCodePayload();
    void _startRegenerationTimer();
    QString _resolvePlayceholders(QString sourceString);

signals:
    void currentQrCodeChanged();
    void remaningTimeUntilRegenerationChanged();
    void guiConfigurationChanged();

};

#endif // CWAQRCODEGENERATOR_H
