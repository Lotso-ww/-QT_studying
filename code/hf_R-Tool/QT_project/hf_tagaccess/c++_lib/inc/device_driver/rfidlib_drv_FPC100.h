#ifndef __RFIDLIB_FPC100_H__
#define __RFIDLIB_FPC100_H__


#ifdef __cplusplus
extern "C" {
#endif

	err_t RFIDLIB_API Doc_SerialOpen(LPCTSTR comname, DWORD baud, LPCTSTR frame, RFID_READER_HANDLE *o_hr /* out parameter */);
	err_t RFIDLIB_API Doc_TCPOpen(LPCTSTR readerIPAddr, WORD remotePort, LPCTSTR localIPToBind, RFID_READER_HANDLE *o_hr /* out parameter */);
	err_t RFIDLIB_API Doc_Close(RFID_READER_HANDLE hr);

#ifdef __cplusplus
}
#endif

#endif
