#include "cwaqrcodegenerator.h"

CwaQrCodeGenerator::CwaQrCodeGenerator(QString configFilePath, QObject *parent) : QObject(parent), QQuickImageProvider(QQuickImageProvider::Image)
{
    this->_state = Initializing;
    this->_locationConfiguration.isValid = false;
    this->_currentQrCodePayload = "";
    this->_qrCodeIndex = false;

    this->_regenerationTimer = new QTimer(this);
    this->_regenerationTimer->setSingleShot(true);
    connect(this->_regenerationTimer, &QTimer::timeout, [=] {
        this->_generateQrCodePayload();
        this->_startRegenerationTimer();
    });

    this->_readConfigFile(configFilePath);
    this->_generateQrCodePayload();
    this->_startRegenerationTimer();

    this->_state = Ready;

    this->_remaningRegenerationTimeRefreshTimer = new QTimer(this);
    this->_remaningRegenerationTimeRefreshTimer->setSingleShot(false);
    this->_remaningRegenerationTimeRefreshTimer->setInterval(1000);
    connect(this->_remaningRegenerationTimeRefreshTimer, &QTimer::timeout, [=] {
        emit this->remaningTimeUntilRegenerationChanged();
    });
    this->_remaningRegenerationTimeRefreshTimer->start();

}

void CwaQrCodeGenerator::registerQMLImageProvider(QString configFilePath, QQmlEngine& engine)
{
    CwaQrCodeGenerator* tmpGenerator = new CwaQrCodeGenerator(configFilePath);
    engine.rootContext()->setContextProperty("CwaQrCodeGenerator", tmpGenerator);
    engine.addImageProvider(QLatin1String("cwaqrcode"), tmpGenerator);
}

QVariantMap CwaQrCodeGenerator::currentQrCode() {
    if (!this->_locationConfiguration.isValid) {
        return {
            {"description", "No valid configuration!"},
            {"address", "Plase check your configuration file!"},
            {"imageUrl", "qrc:/CWA_icon.png"},
            {"isValid", this->_locationConfiguration.isValid}
        };
    }
    else {
        return {
            {"imageUrl", "image://cwaqrcode/" + QString::number(this->_qrCodeIndex)},
            {"type", this->_locationConfiguration.type},
            {"description", this->_resolvePlayceholders(this->_locationConfiguration.description)},
            {"address", this->_resolvePlayceholders(this->_locationConfiguration.address)},
            {"defaultCheckinLengthInMinutes", this->_locationConfiguration.defaultCheckinLengthInMinutes},
            {"startTimestamp", double(this->_locationConfiguration.startTimestamp)},
            {"endTimestamp", double(this->_locationConfiguration.endTimestamp)},
            {"isValid", this->_locationConfiguration.isValid}
        };
    }
}

void CwaQrCodeGenerator::_readConfigFile(QString path) {
    QSettings settingsReader(path, QSettings::IniFormat);
    if(settingsReader.status() != QSettings::NoError) {
        this->_locationConfiguration.isValid = false;
        return;
    }

    this->_locationConfiguration.type = TraceLocationType(settingsReader.value("location/type", 0).toInt());
    this->_locationConfiguration.description = settingsReader.value("location/description", "").toString();
    this->_locationConfiguration.address = settingsReader.value("location/address", "").toString();
    this->_locationConfiguration.defaultCheckinLengthInMinutes = settingsReader.value("location/defaultCheckinLengthInMinutes", 0).toUInt();
    this->_locationConfiguration.startTimestamp = settingsReader.value("location/startTimestamp", 0).toUInt();
    this->_locationConfiguration.endTimestamp = settingsReader.value("location/endTimestamp", 0).toUInt();

    if (
        this->_locationConfiguration.description.isEmpty() ||
        this->_locationConfiguration.address.isEmpty()
    )
        this->_locationConfiguration.isValid  = false;
    else if(
        this->_temporaryLocationTypes.contains(this->_locationConfiguration.type) &&
        (
            this->_locationConfiguration.startTimestamp == 0 ||
            this->_locationConfiguration.endTimestamp == 0 ||
            this->_locationConfiguration.endTimestamp < this->_locationConfiguration.startTimestamp
        )
    )
        this->_locationConfiguration.isValid = false;
    else
        this->_locationConfiguration.isValid = true;

    this->_regenerationConfiguration.hour = settingsReader.value("regeneration/hour", 4).toInt();
    this->_regenerationConfiguration.minute = settingsReader.value("regeneration/minute", 0).toInt();

    return;
}

std::string CwaQrCodeGenerator::_generateRandomSeed(uint legth) {
    std::string buf;
    std::random_device engine;

    for(uint i = 0; i < legth; i++) {
        buf += engine();
    }

    return buf;
}

QString CwaQrCodeGenerator::_resolvePlayceholders(QString sourceString) {
    QString resolvedString = sourceString;

    QRegularExpression reg("\\{\\{[^\\%]+\%[^\\}]+\\}\\}", QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator match = reg.globalMatch(sourceString);

    if(match.isValid()) {
        while (match.hasNext()) {
            QString stringMatch = match.next().captured();

            QString cleanedStringMatch = stringMatch;
            cleanedStringMatch = cleanedStringMatch.replace("{{", "").replace("}}", "");

            QString macroType = cleanedStringMatch.section("%", 0, 0);
            QString macroOptions = cleanedStringMatch.section("%", 1);

            QString resolvedMacro;
            if(macroType == "date") {
                resolvedMacro = QDate::currentDate().toString(macroOptions);
            }
            else if(macroType == "time") {
                resolvedMacro = QDateTime::currentDateTime().toString(macroOptions);
            }
            else {
                resolvedMacro = "{{INVALID}}";
            }

            resolvedString = resolvedString.replace(
                                 stringMatch,
                                 resolvedMacro
                             );
        }
    }

    return resolvedString;
}

bool CwaQrCodeGenerator::_generateQrCodePayload() {
    if (!this->_locationConfiguration.isValid)
        return false;

    TraceLocation locationData;
    locationData.set_version(1);
    locationData.mutable_description()->assign(
        this->_resolvePlayceholders(this->_locationConfiguration.description).toStdString()
    );
    locationData.mutable_address()->assign(
        this->_resolvePlayceholders(this->_locationConfiguration.address).toStdString()
    );
    if(this->_locationConfiguration.startTimestamp > 0)
        locationData.set_starttimestamp(this->_locationConfiguration.startTimestamp);
    if(this->_locationConfiguration.endTimestamp > 0)
        locationData.set_endtimestamp(this->_locationConfiguration.endTimestamp);

    CrowdNotifierData crowdNotifierData;
    crowdNotifierData.set_version(1);
    QByteArray publicKey = QByteArray::fromBase64("gwLMzE153tQwAOf2MZoUXXfzWTdlSpfS99iZffmcmxOG9njSK4RTimFOFwDh6t0Tyw8XR01ugDYjtuKwjjuK49Oh83FWct6XpefPi9Skjxvvz53i9gaMmUEc96pbtoaA");
    crowdNotifierData.mutable_publickey()->assign(publicKey);
    crowdNotifierData.mutable_cryptographicseed()->assign(this->_generateRandomSeed(16));

    CWALocationData vendorData;
    vendorData.set_version(1);
    vendorData.set_type(this->_locationConfiguration.type);
    vendorData.set_defaultcheckinlengthinminutes(this->_locationConfiguration.defaultCheckinLengthInMinutes);
    std::string serializedVendorData;
    vendorData.SerializeToString(&serializedVendorData);

    QRCodePayload payload;
    payload.set_version(1);
    payload.mutable_locationdata()->CopyFrom(locationData);
    payload.mutable_crowdnotifierdata()->CopyFrom(crowdNotifierData);
    payload.mutable_vendordata()->assign(serializedVendorData);

    QByteArray payloadBytes = QByteArray::fromStdString(payload.SerializeAsString());
    this->_currentQrCodePayload = payloadBytes.toBase64(QByteArray::Base64UrlEncoding);

    google::protobuf::ShutdownProtobufLibrary();

    // force reload of frontend
    this->_qrCodeIndex =! this->_qrCodeIndex;
    emit this->currentQrCodeChanged();

    return true;
}

void CwaQrCodeGenerator::_startRegenerationTimer() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime timeOfRegeneration = QDateTime::currentDateTime();
    timeOfRegeneration.setTime(QTime(this->_regenerationConfiguration.hour, this->_regenerationConfiguration.minute));

    if(now.msecsTo(timeOfRegeneration) < 0) {
        timeOfRegeneration = timeOfRegeneration.addDays(1);
    }

    qDebug() << "Generating new QR-Code in " << now.msecsTo(timeOfRegeneration);
    this->_regenerationTimer->setInterval(now.msecsTo(timeOfRegeneration));
    this->_regenerationTimer->start();
}

QString CwaQrCodeGenerator::remaningTimeUntilRegeneration() {
    int remaningSeconds = this->_regenerationTimer->remainingTime() / 1000;
    int remaningMinutes = remaningSeconds / 60;
    int remaningHours = remaningMinutes / 60;

    if(remaningHours > 0) {
        return QString::number(remaningHours) + " Stunden.";
    }
    else if(remaningMinutes > 0) {
        return QString::number(remaningMinutes) + " Minuten.";
    }
    else {
        return QString::number(remaningSeconds) + " Sekunden.";
    }
}

QImage CwaQrCodeGenerator::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    Q_UNUSED(id)
    QImage result;

    if(this->_currentQrCodePayload.isEmpty()) {
        result = QImage(":/CWA_icon.png");
    }
    else {
        QString qrCodeContent = "https://e.coronawarn.app/?v=1#" + this->_currentQrCodePayload;
        QZXingEncoderConfig encoderConfig(QZXing::EncoderFormat_QR_CODE, requestedSize, QZXing::EncodeErrorCorrectionLevel_M, false, false);
        result = QZXing::encodeData(qrCodeContent, encoderConfig);
    }

    if(result.isNull()) {
        // in case of errors, return an empty image
        result = QPixmap(1,1).toImage();
    }

    *size = result.size();
    return result;
}
