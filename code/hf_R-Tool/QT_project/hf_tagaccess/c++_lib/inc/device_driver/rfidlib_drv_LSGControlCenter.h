#ifndef __RFIDLIB_LSGCONTROLCENTER_H__
#define __RFIDLIB_LSGCONTROLCENTER_H__

#include "rfidlib.h"

/*
*  Scan data option 
*/


//LSG控制中心
err_t LSGControlCenter_USBOpen(BYTE AddrMode, LPCTSTR DeviceIdentifyStr, 
								RFID_READER_HANDLE *o_hr /* out parameter */);
err_t LSGControlCenter_SerOpen(LPCTSTR comname, DWORD baud, LPCTSTR frame, BYTE busAddr, 
								  RFID_READER_HANDLE *o_hr /* out parameter */);

err_t LSGControlCenter_TCPOpen(LPCTSTR readerIPAddr, WORD remotePort, LPCTSTR localIPToBind, RFID_READER_HANDLE *o_hr);

/**
 * @brief LSGControlCenter_CB_Open 解析打开设备
 * @param openStr 连接字符串
 * @param hVR     设备句柄
 * @return 0=成功
 */
err_t LSGControlCenter_CB_Open(LPCTSTR openStr, RFID_READER_HANDLE *hVR);





#endif
