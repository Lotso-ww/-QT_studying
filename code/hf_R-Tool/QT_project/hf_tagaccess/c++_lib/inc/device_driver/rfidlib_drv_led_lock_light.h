#ifndef __RFIDLIB_DRV_LED_LOCK_LIGHT_H__
#define __RFIDLIB_DRV_LED_LOCK_LIGHT_H__


#include "rfidlib.h"
#include "LedScreen.h"
#include "Lock.h"
#include "Light.h"

err_t LED_SerOpen(LPCTSTR comname, DWORD baud, LPCTSTR frame, BYTE busAddr,
								  RFID_READER_HANDLE *o_hr /* out parameter */);
err_t LOCK_SerOpen(LPCTSTR comname, DWORD baud, LPCTSTR frame, BYTE busAddr,
                                  RFID_READER_HANDLE *o_hr /* out parameter */);
err_t LIGHT_SerOpen(LPCTSTR comname, DWORD baud, LPCTSTR frame, BYTE busAddr,
                                  RFID_READER_HANDLE *o_hr /* out parameter */);
err_t LED_LOCK_LIGHT_CB_Open(LPCTSTR openStr, RFID_READER_HANDLE *hVR);

#endif
