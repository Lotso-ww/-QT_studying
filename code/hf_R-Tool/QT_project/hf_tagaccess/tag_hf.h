#ifndef TAG_HF_H
#define TAG_HF_H

#include "qstring.h"
#include "../c++_lib/inc/rfidlib.h"
//HF Tag
class CTag_HF
{
public:
    QString m_uid;
    UINT32 m_counter;
    UINT32 m_type;
    UINT32 m_antNo;
    UINT32 m_AIP;
    USHORT m_rssi;

public:
    CTag_HF &operator=(const CTag_HF &src)
    {
        m_uid = src.m_uid;//epc
        m_counter= src.m_counter;
        m_type = src.m_type;
        m_antNo = src.m_antNo;
        m_AIP = src.m_AIP;
        m_rssi = src.m_rssi;

        return *this;
    }
    bool equal(const CTag_HF &src)
    {
        if(m_uid == src.m_uid)
            return true;
        return false;
    }
};

#endif // TAG_HF_H
