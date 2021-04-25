#include "cwaqrcodegenerator.h"

CwaQrCodeGenerator::CwaQrCodeGenerator(QString configFilePath, QObject *parent) : QObject(parent), QQuickImageProvider(QQuickImageProvider::Image)
{
    this->_configuration.isValid = false;
    this->_currentQrCodePayload = "";

    this->_readConfigFile(configFilePath);
    this->_generateQrCodePayload();
}

void CwaQrCodeGenerator::registerQMLImageProvider(QString configFilePath, QQmlEngine& engine)
{
    CwaQrCodeGenerator* tmpGenerator = new CwaQrCodeGenerator(configFilePath);
    engine.rootContext()->setContextProperty("CwaQrCodeGenerator", tmpGenerator);
    engine.addImageProvider(QLatin1String("CwaQrCode"), tmpGenerator);
}

QVariantMap CwaQrCodeGenerator::getConfig() {
    return {
        {"type", this->_configuration.type},
        {"description", this->_configuration.description},
        {"address", this->_configuration.address},
        {"defaultCheckinLengthInMinutes", this->_configuration.defaultCheckinLengthInMinutes},
        {"startTimestamp", double(this->_configuration.startTimestamp)},
        {"endTimestamp", double(this->_configuration.endTimestamp)},
        {"isValid", this->_configuration.isValid}
    };
}

bool CwaQrCodeGenerator::_readConfigFile(QString path) {
    QSettings settingsReader(path, QSettings::IniFormat);
    if(settingsReader.status() != QSettings::NoError) {
        this->_configuration.isValid = false;
        return false;
    }

    this->_configuration.type = TraceLocationType(settingsReader.value("location/type", 0).toInt());
    this->_configuration.description = settingsReader.value("location/description", "<empty>").toString();
    this->_configuration.address = settingsReader.value("location/address", "<empty>").toString();
    this->_configuration.defaultCheckinLengthInMinutes = settingsReader.value("location/defaultCheckinLengthInMinutes", 0).toUInt();
    this->_configuration.startTimestamp = settingsReader.value("location/startTimestamp", 0).toUInt();
    this->_configuration.endTimestamp = settingsReader.value("location/endTimestamp", 0).toUInt();

    if(
        this->_temporaryLocationTypes.contains(this->_configuration.type) &&
        (
            this->_configuration.startTimestamp == 0 ||
            this->_configuration.endTimestamp == 0 ||
            this->_configuration.endTimestamp < this->_configuration.startTimestamp
        )
    ) {
        this->_configuration.isValid = false;
        return false;
    }

    this->_configuration.isValid = true;

    return true;
}

std::string CwaQrCodeGenerator::_generateRandomSeed(uint legth) {
    std::string buf;
    std::random_device engine;
    // TODO: refactor
    for(uint i = 0; i < legth; i++) {
        std::string bufBuf;
        bufBuf = engine();
        buf.append(bufBuf);
    }

    return buf;
}

bool CwaQrCodeGenerator::_generateQrCodePayload() {
    if (!this->_configuration.isValid)
        return false;

    TraceLocation locationData;
    locationData.set_version(1);
    locationData.mutable_description()->assign(this->_configuration.description.toStdString());
    locationData.mutable_address()->assign(this->_configuration.address.toStdString());
    if(this->_configuration.startTimestamp > 0)
        locationData.set_starttimestamp(this->_configuration.startTimestamp);
    if(this->_configuration.endTimestamp > 0)
        locationData.set_endtimestamp(this->_configuration.endTimestamp);

    CrowdNotifierData crowdNotifierData;
    crowdNotifierData.set_version(1);
    QByteArray publicKey = QByteArray::fromBase64("gwLMzE153tQwAOf2MZoUXXfzWTdlSpfS99iZffmcmxOG9njSK4RTimFOFwDh6t0Tyw8XR01ugDYjtuKwjjuK49Oh83FWct6XpefPi9Skjxvvz53i9gaMmUEc96pbtoaA");
    crowdNotifierData.mutable_publickey()->assign(publicKey);
    crowdNotifierData.mutable_cryptographicseed()->assign(this->_generateRandomSeed(16));

    CWALocationData vendorData;
    vendorData.set_version(1);
    vendorData.set_type(TraceLocationType::LOCATION_TYPE_PERMANENT_EDUCATIONAL_INSTITUTION);
    vendorData.set_defaultcheckinlengthinminutes(this->_configuration.defaultCheckinLengthInMinutes);
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

    return true;
}

QImage CwaQrCodeGenerator::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    Q_UNUSED(id)
    if(this->_currentQrCodePayload.isEmpty())
        return QImage();

    QString qrCodeContent = "https://e.coronawarn.app/?v=1#" + this->_currentQrCodePayload;
    QZXingEncoderConfig encoderConfig(QZXing::EncoderFormat_QR_CODE, requestedSize, QZXing::EncodeErrorCorrectionLevel_M, false, false);
    QImage result = QZXing::encodeData(qrCodeContent, encoderConfig);
    *size = result.size();

    return result;
}
