#ifndef __RFIDLIB_AIP_ISO14443A_H__
#define __RFIDLIB_AIP_ISO14443A_H__


#include "rfidlib.h"


#ifdef __cplusplus
extern "C" {
#endif



DWORD RFIDLIB_API ISO14443A_GetLibVersion(LPTSTR buf ,DWORD nSize /* in character */)  ;
err_t RFIDLIB_API ISO14443A_ParseTagDataReport(RFID_DN_HANDLE hTagReport,
										  DWORD *aip_id,
										  DWORD *tag_id,
										  DWORD * ant_id,
										  BYTE uid[],
										  BYTE *uidlen)   ;
RFID_DN_HANDLE RFIDLIB_API ISO14443A_CreateInvenParam(RFID_DN_HANDLE hInvenParamSpecList,
															BYTE AntennaID/* By default set to 0,apply to all antenna */
															)   ;


err_t RFIDLIB_API ISO14443A_Connect(RFID_READER_HANDLE hr, BYTE tagType, BYTE uid[], BYTE uidLen, RFID_TAG_HANDLE *ht);



/*
*  ultralight locking block type
*/
enum{
	MFUL_LOCKING_BLK_TYPE_OTP,
	MFUL_LOCKING_BLK_TYPE_PAGE_4_9,
	MFUL_LOCKING_BLK_TYPE_PAGE_10_15,
	MFUL_LOCKING_BLK_TYPE_PAGE_16_27,
	MFUL_LOCKING_BLK_TYPE_PAGE_28_39,
	MFUL_LOCKING_BLK_TYPE_PAGE_COUNTER,
	MFUL_LOCKING_BLK_TYPE_PAGE_AUTH0,
	MFUL_LOCKING_BLK_TYPE_PAGE_AUTH1,
	MFUL_LOCKING_BLK_TYPE_PAGE_KEY ,

};

/*
* lock page type
*/
enum{
	MFUL_LOCKING_PAGE_TYPE_PAGE_OTP,
	MFUL_LOCKING_PAGE_TYPE_PAGE_4,
	MFUL_LOCKING_PAGE_TYPE_PAGE_5 ,
	MFUL_LOCKING_PAGE_TYPE_PAGE_6 ,
	MFUL_LOCKING_PAGE_TYPE_PAGE_7,
	MFUL_LOCKING_PAGE_TYPE_PAGE_8,
	MFUL_LOCKING_PAGE_TYPE_PAGE_9,
	MFUL_LOCKING_PAGE_TYPE_PAGE_10,
	MFUL_LOCKING_PAGE_TYPE_PAGE_11,
	MFUL_LOCKING_PAGE_TYPE_PAGE_12,
	MFUL_LOCKING_PAGE_TYPE_PAGE_13,
	MFUL_LOCKING_PAGE_TYPE_PAGE_14,
	MFUL_LOCKING_PAGE_TYPE_PAGE_15,
	MFUL_LOCKING_PAGE_TYPE_PAGE_16_19,
	MFUL_LOCKING_PAGE_TYPE_PAGE_20_23,
	MFUL_LOCKING_PAGE_TYPE_PAGE_24_27,
	MFUL_LOCKING_PAGE_TYPE_PAGE_28_31,
	MFUL_LOCKING_PAGE_TYPE_PAGE_32_35,
	MFUL_LOCKING_PAGE_TYPE_PAGE_36_39,
	MFUL_LOCKING_PAGE_TYPE_PAGE_41,
	MFUL_LOCKING_PAGE_TYPE_PAGE_42,
	MFUL_LOCKING_PAGE_TYPE_PAGE_43,
	MFUL_LOCKING_PAGE_TYPE_PAGE_KEY
};

DWORD RFIDLIB_API ULTRALIGHT_GetLibVersion(LPTSTR buf ,DWORD nSize /* in character */)  ;
err_t RFIDLIB_API ULTRALIGHT_Connect(RFID_READER_HANDLE hr,BYTE uid[] /* 7 bytes */,RFID_TAG_HANDLE *ht) ;
err_t RFIDLIB_API ULTRALIGHT_LockPage(RFID_READER_HANDLE hr ,
									  RFID_TAG_HANDLE ht,
									  DWORD pageType)  ; 
err_t RFIDLIB_API ULTRALIGHT_LockBlock(RFID_READER_HANDLE hr ,
									   RFID_TAG_HANDLE ht,
									   DWORD blockType) ;

err_t RFIDLIB_API ULTRALIGHT_ReadMultiplePages(RFID_READER_HANDLE hr ,
											   RFID_TAG_HANDLE ht,
											   DWORD pageStart,
											   DWORD pageNum ,
											   BYTE databuf[] ,
											   DWORD *nSize ) ;
err_t RFIDLIB_API ULTRALIGHT_WriteMultiplePages(RFID_READER_HANDLE hr ,
												RFID_TAG_HANDLE ht, 
												DWORD  pageStart,
												DWORD pageNum ,
												BYTE databuf[] ,
												DWORD bytesToWrite) ;
err_t RFIDLIB_API ULTRALIGHT_Authenticate(RFID_READER_HANDLE hr ,
										  RFID_TAG_HANDLE ht, 
										  BYTE key[] /* 16bytes */) ;
err_t RFIDLIB_API ULTRALIGHT_UpdatePassword(RFID_READER_HANDLE hr ,
											RFID_TAG_HANDLE ht, 
											BYTE key[]) ;
/*
* auth0:0x03-0x30
* auth1:0:需要认证,1:不需要认证
*/
err_t RFIDLIB_API ULTRALIGHT_UpdateAuthConfig(RFID_READER_HANDLE hr ,
											  RFID_TAG_HANDLE ht,
											  BYTE auth0,
											  BYTE auth1) ;


err_t RFIDLIB_API DESFIRE_SelectApplication(RFID_READER_HANDLE hr ,RFID_TAG_HANDLE ht, DWORD appID ) ;
err_t RFIDLIB_API DESFIRE_Authenticate(RFID_READER_HANDLE hr ,RFID_TAG_HANDLE ht, BYTE auth_type,
									BYTE keyNo, BYTE keyVer, BYTE keyLen,BYTE key[] ) ;
err_t RFIDLIB_API DESFIRE_ChangeKeySettings(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht, BYTE mask);

err_t RFIDLIB_API DESFIRE_GetKeySettings(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht,DWORD* keySetting, DWORD *keyCnt, DWORD *keyType);

err_t RFIDLIB_API DESFIRE_ChangeKey(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht,BYTE opt, BYTE mKeyNo,BYTE mOldKeyType,BYTE mOldKeyVer,BYTE mOldKeyLen, BYTE mOldKey[],
									BYTE mNewKeyType,BYTE mNewKeyVer,BYTE mNewKeyLen, BYTE mNewKey[]);
err_t RFIDLIB_API DESFIRE_GetKeyVersion(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht, BYTE keyNo, DWORD *kVer);
err_t RFIDLIB_API DESFIRE_GetCardUID(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD* uidLen, BYTE uid[]);
//TODO
err_t RFIDLIB_API DESFIRE_DeleteApplication(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD aid);
err_t RFIDLIB_API DESFIRE_GetAppIds(RFID_DN_HANDLE hr, RFID_TAG_HANDLE ht, DWORD* cnt, DWORD ids[]);
err_t RFIDLIB_API DESFIRE_CreateApplication(RFID_DN_HANDLE hr, RFID_DN_HANDLE ht,DWORD aid, BYTE keySet1, BYTE keySet2, DWORD isoID, BYTE nSize,BYTE df_name[]);
err_t RFIDLIB_API DESFIRE_FormatPICC(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht);
err_t RFIDLIB_API DESFIRE_GetVersion(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht,BYTE info[], BYTE uid[], BYTE bat[]);
err_t RFIDLIB_API DESFIRE_FreeMem(RFID_DN_HANDLE hr, RFID_TAG_HANDLE ht, DWORD* mem_size);
//err_t RFIDLIB_API DESFIRE_GetAppIds(RFID_DN_HANDLE hr, RFID_TAG_HANDLE ht, BYTE* file_cnt, DWORD File_ids[]);
err_t RFIDLIB_API DESFIRE_GetFileSettings(RFID_DN_HANDLE hr,RFID_TAG_HANDLE ht, BYTE fileId, BYTE* fileType,BYTE* comSet,DWORD* accessRight, BYTE cfg[]);
err_t RFIDLIB_API DESFIRE_ChangeFileSettings(RFID_DN_HANDLE hr,RFID_TAG_HANDLE ht, BYTE fileId, BYTE old_ComSet,BYTE new_comSet,DWORD accessRight);
err_t RFIDLIB_API DESFIRE_DeleteFile(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD fileId);
err_t RFIDLIB_API DESFIRE_CreateStdDataFile(RFID_READER_HANDLE hr,RFID_DN_HANDLE ht,BYTE opt,BYTE fileId,BYTE iso1876_fileId,BYTE comSett,DWORD accessRight, DWORD fileSize );
err_t RFIDLIB_API DESFIRE_CreateBackupDataFile(RFID_READER_HANDLE hr,RFID_DN_HANDLE ht,BYTE opt,BYTE fileId,BYTE iso1876_fileId,BYTE comSett,DWORD accessRight, DWORD fileSize );
err_t RFIDLIB_API DESFIRE_CreateValueFile(RFID_READER_HANDLE hr,RFID_DN_HANDLE ht,BYTE fileId,BYTE comSett,DWORD accessRight, DWORD lowerLimit,DWORD upperLimit,DWORD value, BYTE limitCreditEnable);
err_t RFIDLIB_API DESFIRE_CreateLinearRecordFile(RFID_READER_HANDLE hr,RFID_DN_HANDLE ht,BYTE opt,BYTE fileId,BYTE iso1876_fileId,BYTE comSett,DWORD accessRight, DWORD recordSize,DWORD maxRecordSize );
err_t RFIDLIB_API DESFIRE_CreateCyclicRecordFile(RFID_READER_HANDLE hr,RFID_DN_HANDLE ht,BYTE opt,BYTE fileId,BYTE iso1876_fileId,BYTE comSett,DWORD accessRight, DWORD recordSize,DWORD maxRecordSize );
err_t RFIDLIB_API DESFIRE_AbortTransaction(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht);
err_t RFIDLIB_API DESFIRE_CommitTransaction(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht);
err_t RFIDLIB_API DESFIRE_ClearRecordFile(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht,BYTE fileId);
err_t RFIDLIB_API DESFIRE_ReadRecord(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht,BYTE fileId,BYTE comSett,DWORD recordOffset, DWORD recordCnt,BYTE* dlen, BYTE data[]);
err_t RFIDLIB_API DESFIRE_WriteRecord(RFID_READER_HANDLE hr, RFID_TAG_HANDLE ht,BYTE fileId,BYTE comSett,DWORD recordOffset, DWORD len, BYTE data[]);
err_t RFIDLIB_API DESFIRE_LimitedCredit(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD fileId,BYTE comSett,DWORD value);
err_t RFIDLIB_API DESFIRE_Debit(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD fileId,BYTE comSett,DWORD value);
err_t RFIDLIB_API DESFIRE_Credit(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD fileId,BYTE comSett,DWORD value);
err_t RFIDLIB_API DESFIRE_GetValue(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD fileId,BYTE comSett,BYTE isFree,DWORD* value);
err_t RFIDLIB_API DESFIRE_WriteData(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD fileId,BYTE comSett,DWORD offset, DWORD length, BYTE data[]);
err_t RFIDLIB_API DESFIRE_ReadData(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD fileId,BYTE comSett,DWORD offset, DWORD length,DWORD* dataSize, BYTE data[]);
err_t RFIDLIB_API DESFIRE_SetConfiguration(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,BYTE opt,int nSize,BYTE data[]);
err_t RFIDLIB_API DESFIRE_GetFileIDs(RFID_READER_HANDLE hr,RFID_TAG_HANDLE ht,DWORD* cnt, BYTE ids[]);


err_t RFIDLIB_API MFCL_Connect(RFID_READER_HANDLE hr,BYTE tagType,BYTE uid[] /* 4 bytes */,RFID_TAG_HANDLE *ht) ;
err_t RFIDLIB_API MFCL_ReadBlock(RFID_READER_HANDLE hr ,
								 RFID_TAG_HANDLE ht,
								 BYTE  blkAddr ,
								 BYTE  blkData[],
								 DWORD nSize 
								 ) ;
err_t RFIDLIB_API MFCL_ReadMultipleBlocks(RFID_READER_HANDLE hr ,
										  RFID_TAG_HANDLE ht,
										  BYTE  blkAddr ,
										  BYTE blkNum,
										  BOOLEAN autoAuth,
										  BYTE keyType,
										  BYTE key[],
										  BYTE  blkData[],
										  DWORD nSize 
										  ) ;
err_t RFIDLIB_API MFCL_WriteMultipleBlocks(RFID_READER_HANDLE hr,
										   RFID_TAG_HANDLE ht,
										   BYTE blkAddr ,
										   BYTE blkNum ,
										   BOOLEAN autoAuth,
										   BYTE keyType,
										   BYTE key[],
										   BYTE blkData[],
										   DWORD nSize ) ;
err_t RFIDLIB_API MFCL_WriteBlock(RFID_READER_HANDLE hr,
								  RFID_TAG_HANDLE ht,
								  BYTE blkAddr ,
								  BYTE blkData[] /* 16 bytes */) ;
err_t RFIDLIB_API MFCL_Authenticate(RFID_READER_HANDLE hr,
									RFID_TAG_HANDLE ht,
									BYTE blkAddr ,
									BYTE keyType,
									BYTE key[]) ;
err_t RFIDLIB_API MFCL_Increment(RFID_READER_HANDLE hr,
								 RFID_TAG_HANDLE ht,
								 BYTE blkAddr ,
								 DWORD val) ;
err_t RFIDLIB_API MFCL_Decrement(RFID_READER_HANDLE hr,
								 RFID_TAG_HANDLE ht,
								 BYTE blkAddr ,
								 DWORD val) ;

err_t RFIDLIB_API MFCL_Restore(RFID_READER_HANDLE hr,
							   RFID_TAG_HANDLE ht,
							   BYTE blkAddr ) ;

err_t RFIDLIB_API MFCL_Transfer(RFID_READER_HANDLE hr,
								RFID_TAG_HANDLE ht,
								BYTE blkAddr) ;

err_t RFIDLIB_API MFCL_FormatValueBlock(RFID_READER_HANDLE hr,
										RFID_TAG_HANDLE ht,
										BYTE blkAddr,
										DWORD initValue) ;
err_t RFIDLIB_API MFCL_CreateAccessCondition(BYTE blk0AccType /*in */,
											 BYTE blk1AccType /*in */,
											 BYTE blk2AccType /*in */,
											 BYTE trailerAccType /*in */,
											 BYTE formattedAccCondi[]/*out */ )  ;
BYTE RFIDLIB_API MFCL_Sector2Block(BYTE sector) ; 
BYTE RFIDLIB_API MFCL_Block2Sector(BYTE block) ;
BOOLEAN RFIDLIB_API MFCL_IsTailerBlock(BYTE blkAddr) ;
err_t RFIDLIB_API MFCL_ParseAccessCondi(BYTE formattedAccCondi[] ,BYTE *blk0AccType,BYTE *blk1AccType,BYTE *blk2AccType,BYTE *trailerAccType)  ;


/* 
* NTAG21x functions
*/
err_t RFIDLIB_API NTAG21x_Connect(RFID_READER_HANDLE hr,BYTE uid[] /* 7 bytes */,RFID_TAG_HANDLE *ht) ;
err_t RFIDLIB_API NTAG21x_ReadMultiplePages(RFID_READER_HANDLE hr ,
											RFID_TAG_HANDLE ht,
											DWORD pageStart,
											DWORD pageNum ,
											BYTE databuf[] ,
											DWORD *nSize ) ;
err_t RFIDLIB_API NTAG21x_WriteMultiplePages(RFID_READER_HANDLE hr ,
											 RFID_TAG_HANDLE ht, 
											 DWORD pageStart,
											 DWORD pageNum ,
											 BYTE databuf[] ,
											 DWORD bytesToWrite) ;
err_t RFIDLIB_API NTAG21x_Authenticate(RFID_READER_HANDLE hr ,
									   RFID_TAG_HANDLE ht,
									   DWORD pwd /* in */,
									   WORD *pack /*out */) ;
err_t RFIDLIB_API NTAG21x_GetVersion(RFID_READER_HANDLE hr ,
									 RFID_TAG_HANDLE ht,
									 BYTE ver[] /* out */) ;
err_t RFIDLIB_API NTAG21x_ReadCnt(RFID_READER_HANDLE hr ,
								  RFID_TAG_HANDLE ht,
								  DWORD *counter /* out */) ;
err_t RFIDLIB_API NTAG21x_ReadSig(RFID_READER_HANDLE hr ,
								  RFID_TAG_HANDLE ht,
								  BYTE sig[] /* out */) ;




err_t RFIDLIB_API ISO14443Ap4_Transceive(RFID_READER_HANDLE hr,
	RFID_TAG_HANDLE ht,
	BYTE sndData[],
	DWORD sndLen,
	BYTE rcvBuff[],
	DWORD *nSize /* IN:buffer size ,OUT:bytes written */);

/*
* 该API属于 有 连接透传
*/
err_t RFIDLIB_API ISO14443Ap3_Transceive(RFID_READER_HANDLE hr,
	RFID_TAG_HANDLE ht,

	 BYTE IsTxCRC
	, BYTE IsRxCRC
	,BYTE *reqData
	, DWORD reqLen
	, DWORD nBytesTagWillRet
	, BYTE *pRxBuffer
	, DWORD *nSize
	, DWORD time
	);
/*
* 该API属于 无 连接透传
*/
err_t RFIDLIB_API ISO14443Ap3_Transceive_v2(RFID_READER_HANDLE hr,
	WORD flag
	, BYTE * reqData
	, DWORD reqLen
	, DWORD nBytesTagWillRet
	, BYTE * pRxBuffer
	, DWORD * nSize
	, DWORD time);

#ifdef __cplusplus
}
#endif

#endif