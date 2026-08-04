#include "tagpayloadcodec.h"

#include <QTextCodec>

namespace {
bool isBcdByte(uchar value)
{
    return (value >> 4) <= 9 && (value & 0x0f) <= 9;
}
}

void TagPayloadCodec::setError(QString *error, const QString &message)
{
    if (error)
        *error = message;
}

bool TagPayloadCodec::validateMedicalRecord(const QByteArray &record, QString *error)
{
    if (record.isEmpty() || record.size() > MaxMedicalRecordLength) {
        setError(error, QStringLiteral("Medical record number must contain 1 to 64 ASCII characters."));
        return false;
    }
    for (char ch : record) {
        const bool valid = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')
                || (ch >= '0' && ch <= '9') || ch == '_' || ch == '-';
        if (!valid) {
            setError(error, QStringLiteral("Medical record number contains an unsupported character."));
            return false;
        }
    }
    return true;
}

bool TagPayloadCodec::encodeBcdTime(const QDateTime &dateTime, QByteArray *encoded, QString *error)
{
    if (!encoded || !dateTime.isValid() || dateTime.date().year() < 2000 || dateTime.date().year() > 2099) {
        setError(error, QStringLiteral("Insemination time must be valid and between 2000 and 2099."));
        return false;
    }
    const QString digits = dateTime.toString(QStringLiteral("yyMMddHHmm"));
    QByteArray bcd;
    bcd.reserve(5);
    for (int i = 0; i < digits.size(); i += 2)
        bcd.append(char((digits.at(i).digitValue() << 4) | digits.at(i + 1).digitValue()));
    *encoded = bcd;
    return true;
}

bool TagPayloadCodec::decodeBcdTime(const QByteArray &encoded, QDateTime *dateTime, QString *error)
{
    if (!dateTime || encoded.size() != 5) {
        setError(error, QStringLiteral("BCD time must contain exactly five bytes."));
        return false;
    }
    QString digits;
    for (char ch : encoded) {
        const uchar value = static_cast<uchar>(ch);
        if (!isBcdByte(value)) {
            setError(error, QStringLiteral("BCD time contains an invalid digit."));
            return false;
        }
        digits += QString::number(value >> 4);
        digits += QString::number(value & 0x0f);
    }
    const QDate date(2000 + digits.mid(0, 2).toInt(), digits.mid(2, 2).toInt(), digits.mid(4, 2).toInt());
    const QTime time(digits.mid(6, 2).toInt(), digits.mid(8, 2).toInt());
    if (!date.isValid() || !time.isValid()) {
        setError(error, QStringLiteral("BCD time does not represent a valid date and time."));
        return false;
    }
    *dateTime = QDateTime(date, time);
    return true;
}

bool TagPayloadCodec::encodeName(const QString &name, QByteArray *encoded, QString *error)
{
    if (!encoded || name.isEmpty()) {
        setError(error, QStringLiteral("Female name must not be empty."));
        return false;
    }
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    if (!codec) {
        setError(error, QStringLiteral("GBK codec is unavailable."));
        return false;
    }
    const QByteArray gbk = codec->fromUnicode(name);
    if (gbk.contains('?') && !name.contains('?')) {
        setError(error, QStringLiteral("Female name cannot be encoded as GBK."));
        return false;
    }
    if (gbk.size() > NameLength) {
        setError(error, QStringLiteral("Female name exceeds the eight-byte GBK limit."));
        return false;
    }
    *encoded = QByteArray(NameLength - gbk.size(), ' ') + gbk;
    return true;
}

bool TagPayloadCodec::decodeName(const QByteArray &encoded, QString *name, QString *error)
{
    if (!name || encoded.size() != NameLength) {
        setError(error, QStringLiteral("GBK name must contain exactly eight bytes."));
        return false;
    }
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    if (!codec) {
        setError(error, QStringLiteral("GBK codec is unavailable."));
        return false;
    }
    int firstContentByte = 0;
    while (firstContentByte < encoded.size() && encoded.at(firstContentByte) == ' ')
        ++firstContentByte;
    if (firstContentByte == encoded.size()) {
        setError(error, QStringLiteral("Female name must not be empty."));
        return false;
    }
    const QByteArray trimmed = encoded.mid(firstContentByte);
    *name = codec->toUnicode(trimmed);
    if (name->contains(QChar::ReplacementCharacter)) {
        setError(error, QStringLiteral("GBK name contains invalid byte sequences."));
        return false;
    }
    return true;
}

bool TagPayloadCodec::encode(const TagPayload &payload, QByteArray *encoded, QString *error)
{
    if (!encoded) {
        setError(error, QStringLiteral("Output buffer is required."));
        return false;
    }
    QByteArray time;
    QByteArray name;
    if (!encodeBcdTime(payload.inseminationTime, &time, error)
            || !encodeName(payload.femaleName, &name, error)
            || !validateMedicalRecord(payload.medicalRecordNumber, error))
        return false;

    const int contentLength = 1 + time.size() + name.size() + payload.medicalRecordNumber.size();
    if (contentLength > 255) {
        setError(error, QStringLiteral("Payload content exceeds the protocol length limit."));
        return false;
    }
    QByteArray result;
    result.reserve(2 + contentLength);
    result.append(char(payload.formatVersion));
    result.append(char(contentLength));
    result.append(char(payload.dishNumber));
    result.append(time);
    result.append(name);
    result.append(payload.medicalRecordNumber);
    *encoded = result;
    return true;
}

bool TagPayloadCodec::decode(const QByteArray &raw, TagPayload *payload, QString *error)
{
    if (!payload || raw.size() < HeaderLength) {
        setError(error, QStringLiteral("Payload is truncated."));
        return false;
    }
    const quint8 version = static_cast<quint8>(raw.at(0));
    const int contentLength = static_cast<quint8>(raw.at(1));
    if (contentLength < HeaderLength - 2 || raw.size() < contentLength + 2) {
        setError(error, QStringLiteral("Payload content length is invalid or truncated."));
        return false;
    }
    TagPayload result;
    result.formatVersion = version;
    result.dishNumber = static_cast<quint8>(raw.at(2));
    if (!decodeBcdTime(raw.mid(3, 5), &result.inseminationTime, error)
            || !decodeName(raw.mid(8, NameLength), &result.femaleName, error))
        return false;
    result.medicalRecordNumber = raw.mid(HeaderLength, contentLength - (HeaderLength - 2));
    if (!validateMedicalRecord(result.medicalRecordNumber, error))
        return false;
    *payload = result;
    return true;
}

bool TagPayloadCodec::padForBlocks(const QByteArray &payload, int blockSize, QByteArray *padded, QString *error)
{
    if (!padded || blockSize <= 0) {
        setError(error, QStringLiteral("Block size must be greater than zero."));
        return false;
    }
    const int remainder = payload.size() % blockSize;
    *padded = payload;
    if (remainder)
        padded->append(QByteArray(blockSize - remainder, '\0'));
    return true;
}

QString TagPayloadCodec::toHex(const QByteArray &data)
{
    return QString::fromLatin1(data.toHex(' ').toUpper());
}
