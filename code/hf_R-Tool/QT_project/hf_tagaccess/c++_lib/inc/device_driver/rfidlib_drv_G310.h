#ifndef __RFIDLIB_DRV_G310_H__
#define __RFIDLIB_DRV_G310_H__
#include "rfidlib.h"


err_t G310_SerOpen(LPCTSTR comname, DWORD baud, LPCTSTR frame, BYTE busAddr,
                                  RFID_READER_HANDLE *o_hr /* out parameter */);

err_t G310_SerOpen(LPCTSTR comname, DWORD baud, LPCTSTR frame, BYTE busAddr,
                                  RFID_READER_HANDLE *o_hr /* out parameter */);

err_t G310_TCPOpen(LPCTSTR readerIPAddr, WORD remotePort, LPCTSTR localIPToBind, RFID_READER_HANDLE *o_hr);

/**
 * @brief G310_CB_Open 解析打开设备
 * @param openStr 连接字符串
 * @param hVR     设备句柄
 * @return 0=成功
 */
err_t G310_CB_Open(LPCTSTR openStr, RFID_READER_HANDLE *hVR);


#endif
