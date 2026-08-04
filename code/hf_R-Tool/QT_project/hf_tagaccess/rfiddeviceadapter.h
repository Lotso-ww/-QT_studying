#ifndef RFIDDEVICEADAPTER_H
#define RFIDDEVICEADAPTER_H

#include "rfidtagtypes.h"

class RfidDeviceAdapter
{
public:
    virtual ~RfidDeviceAdapter() = default;

    virtual RfidDeviceResult setAccessAntenna(quint32 antenna) = 0;
    virtual RfidDeviceResult connectIso15693(quint32 tagType, const QString &uid) = 0;
    virtual RfidDeviceResult disconnectTag() = 0;
    virtual RfidDeviceResult getSystemInfo(TagSystemInfo *systemInfo) = 0;
    virtual RfidDeviceResult readBlocks(int basicIndex, int blockCount, int blockSize, QByteArray *data) = 0;
    virtual RfidDeviceResult writeBlocks(int basicIndex, int blockCount, int blockSize,
                                         const QByteArray &data) = 0;
};

class CaeRfidDeviceAdapter : public RfidDeviceAdapter
{
public:
    explicit CaeRfidDeviceAdapter(void *reader, void *connectedTag = nullptr);
    ~CaeRfidDeviceAdapter() override;

    RfidDeviceResult setAccessAntenna(quint32 antenna) override;
    RfidDeviceResult connectIso15693(quint32 tagType, const QString &uid) override;
    RfidDeviceResult disconnectTag() override;
    RfidDeviceResult getSystemInfo(TagSystemInfo *systemInfo) override;
    RfidDeviceResult readBlocks(int basicIndex, int blockCount, int blockSize, QByteArray *data) override;
    RfidDeviceResult writeBlocks(int basicIndex, int blockCount, int blockSize,
                                 const QByteArray &data) override;

private:
    void *m_reader = nullptr;
    void *m_tagHandle = nullptr;
    bool m_ownsTagHandle = false;
};

#endif // RFIDDEVICEADAPTER_H
