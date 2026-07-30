#ifndef __RFIDLIB_ANM200_H__
#define __RFIDLIB_ANM200_H__

#include "rfidlib.h"


/*
* Reader system mode
*/
enum
{
	ANM200_SYSMODE_HOST=0x00, 
	ANM200_SYSMODE_SCAN
};

/*
*  Scan data option 
*/


err_t ANM200_Open(LPCTSTR comname, DWORD baud, LPCTSTR frame, RFID_READER_HANDLE *o_hr /* out parameter */);
err_t ANM200_Close(RFID_READER_HANDLE hr);
err_t ANM200_CB_Open(LPCTSTR openStr, RFID_READER_HANDLE *hVR);











#endif
