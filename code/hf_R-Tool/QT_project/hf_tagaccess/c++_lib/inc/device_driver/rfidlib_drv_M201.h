#ifndef __RFIDLIB_ANM201_H__
#define __RFIDLIB_ANM201_H__


#include "rfidlib.h"


err_t ANM201_OpenCom(LPCTSTR comname, DWORD baud, LPCTSTR frame, RFID_READER_HANDLE *o_hr /* out parameter */);

err_t ANM201_OpenHid(BYTE AddrMode, LPCTSTR DeviceIdentifyStr, RFID_READER_HANDLE *o_hr);

err_t ANM201_Close(RFID_READER_HANDLE hr);

err_t ANM201_GetNoiseLevel(RFID_READER_HANDLE hr, BYTE noisebuf[], DWORD *nSize);

err_t ANM201_CB_Open(LPCTSTR openStr, RFID_READER_HANDLE *hVR);

















#endif

