#ifndef __RFIDLIB_DRV_QBCHANNEL_H__
#define __RFIDLIB_DRV_QBCHANNEL_H__


err_t RFIDLIB_API QBChannel_HidOpen(BYTE AddrMode ,LPCTSTR DeviceIdentifyStr,RFID_READER_HANDLE *o_hr /* out parameter */) ;
err_t RFIDLIB_API QBChannel_EthOpen(LPCTSTR ipAddr,WORD port ,RFID_READER_HANDLE *o_hr) ;
err_t RFIDLIB_API QBChannel_SerOpen(LPCTSTR comname,DWORD baud,LPCTSTR frame,
								RFID_READER_HANDLE *o_hr /* out parameter */)  ;









#endif


