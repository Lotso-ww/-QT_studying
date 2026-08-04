#include "rfiddeviceadapter.h"

#include "CAEDevice_HF.h"

CaeRfidDeviceAdapter::CaeRfidDeviceAdapter(void *reader, void *connectedTag)
    : m_reader(reader)
    , m_tagHandle(connectedTag)
{
}

CaeRfidDeviceAdapter::~CaeRfidDeviceAdapter()
{
    disconnectTag();
}

RfidDeviceResult CaeRfidDeviceAdapter::setAccessAntenna(quint32 antenna)
{
    // MainWindow has already selected the antenna before creating an external tag
    // handle. Re-selecting it after ISO15693_Connect invalidates that session on
    // some RD5200 drivers, so preserve the active session in this case.
    if (m_tagHandle)
        return RfidDeviceResult {true, 0, QString()};
    return CAEDevice_HF::setAccessAntenna(m_reader, antenna);
}

RfidDeviceResult CaeRfidDeviceAdapter::connectIso15693(quint32 tagType, const QString &uid)
{
    if (m_tagHandle)
        return RfidDeviceResult {true, 0, QString()};
    disconnectTag();
    RFID_TAG_HANDLE tagHandle = nullptr;
    const RfidDeviceResult result = CAEDevice_HF::connectIso15693(m_reader, tagType, uid, &tagHandle);
    if (result.success)
    {
        m_tagHandle = tagHandle;
        m_ownsTagHandle = true;
    }
    return result;
}

RfidDeviceResult CaeRfidDeviceAdapter::disconnectTag()
{
    if (!m_ownsTagHandle)
        return RfidDeviceResult {true, 0, QString()};
    RFID_TAG_HANDLE tagHandle = m_tagHandle;
    const RfidDeviceResult result = CAEDevice_HF::disconnectTag(m_reader, &tagHandle);
    if (result.success)
    {
        m_tagHandle = tagHandle;
        m_ownsTagHandle = false;
    }
    return result;
}

RfidDeviceResult CaeRfidDeviceAdapter::getSystemInfo(TagSystemInfo *systemInfo)
{
    return CAEDevice_HF::getSystemInfo(m_reader, m_tagHandle, systemInfo);
}

RfidDeviceResult CaeRfidDeviceAdapter::readBlocks(int basicIndex, int blockCount, int blockSize, QByteArray *data)
{
    return CAEDevice_HF::readBlocks(m_reader, m_tagHandle, basicIndex, blockCount, blockSize, data);
}

RfidDeviceResult CaeRfidDeviceAdapter::writeBlocks(int basicIndex, int blockCount, int blockSize,
                                                    const QByteArray &data)
{
    return CAEDevice_HF::writeBlocks(m_reader, m_tagHandle, basicIndex, blockCount, blockSize, data);
}
