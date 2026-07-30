#ifndef GFUNCTION_H
#define GFUNCTION_H


char * UnicodeToANSI( const wchar_t* str );
wchar_t * ANSIToUnicode( const char* str );
void BytesToHexStr(unsigned char *bBuffer,int bLen,char *strBuf);
bool HexStrToBytes(const char *strBuf,unsigned char *byBuf,int &byLen);
int CharToInt(char val);
#endif // GFUNCTION_H
