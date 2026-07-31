#ifndef TAG_HF_H
#define TAG_HF_H

#include "qstring.h"
#include <QMetaType>
#include <vector>
#include "../c++_lib/inc/rfidlib.h"
// 高频(HF)标签类，保存一张RFID标签的盘点信息
class CTag_HF
{
public:
    QString m_uid;      // 标签唯一标识号(UID)，十六进制字符串
    UINT32  m_counter;  // 该标签被读到的次数(盘点计数)
    UINT32  m_type;     // 标签类型ID(如 NXP ICODE SLI、ST M24LR64 等)
    UINT32  m_antNo;    // 读到该标签的天线编号
    UINT32  m_AIP;      // 空中接口协议类型(如 ISO15693、ISO14443A)
    USHORT  m_rssi;     // 信号强度(RSSI)值

public:
    // 赋值运算符重载：把另一个标签的信息复制到当前对象
    CTag_HF &operator=(const CTag_HF &src)
    {
        m_uid = src.m_uid;        // 复制UID
        m_counter= src.m_counter;// 复制读到次数
        m_type = src.m_type;      // 复制标签类型
        m_antNo = src.m_antNo;   // 复制天线编号
        m_AIP = src.m_AIP;        // 复制协议类型
        m_rssi = src.m_rssi;      // 复制信号强度

        return *this;             // 返回自身引用，支持连续赋值 a=b=c
    }
    // 判断两个标签是否同一张(只比较UID)
    bool equal(const CTag_HF &src)
    {
        if(m_uid == src.m_uid)    // UID相同视为同一张标签
            return true;
        return false;
    }
};

Q_DECLARE_METATYPE(CTag_HF)
Q_DECLARE_METATYPE(std::vector<CTag_HF>)

#endif // TAG_HF_H
