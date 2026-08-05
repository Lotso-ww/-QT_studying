#ifndef TAGPAYLOADCODEC_H
#define TAGPAYLOADCODEC_H

#include "rfidtagtypes.h"

class TagPayloadCodec
{
public:
    static const quint8 FormatVersion = 0x01;
    // Byte 1 stores the full payload size, including the version and length
    // fields themselves. The two leading fields occupy HeaderFieldLength bytes.
    static const int HeaderFieldLength = 2;
    static const int HeaderLength = 16;
    static const int NameLength = 8;
    static const int MaxMedicalRecordLength = 64;

    static bool encode(const TagPayload &payload, QByteArray *encoded, QString *error = nullptr);
    static bool decode(const QByteArray &raw, TagPayload *payload, QString *error = nullptr);
    static bool encodeBcdTime(const QDateTime &dateTime, QByteArray *encoded, QString *error = nullptr);
    static bool decodeBcdTime(const QByteArray &encoded, QDateTime *dateTime, QString *error = nullptr);
    static bool padForBlocks(const QByteArray &payload, int blockSize, QByteArray *padded, QString *error = nullptr);
    static QString toHex(const QByteArray &data);

private:
    static bool encodeName(const QString &name, QByteArray *encoded, QString *error);
    static bool decodeName(const QByteArray &encoded, QString *name, QString *error);
    static bool validateMedicalRecord(const QByteArray &record, QString *error);
    static void setError(QString *error, const QString &message);
};

#endif // TAGPAYLOADCODEC_H
