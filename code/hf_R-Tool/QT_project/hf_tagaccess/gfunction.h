#ifndef GFUNCTION_H
#define GFUNCTION_H


// 把 Unicode(宽字符)字符串转换为 ANSI(多字节)字符串，返回新分配的 char* (调用者负责释放)
char * UnicodeToANSI( const wchar_t* str );

// 把 ANSI(多字节 char)字符串转换为 Unicode(宽字符)字符串，返回新分配的 wchar_t* (调用者负责释放)
wchar_t * ANSIToUnicode( const char* str );

// 把字节数组(bBuffer, 长度 bLen)转换为十六进制字符串(写入 strBuf)
// 例如: 字节 0xAB -> "AB"
void BytesToHexStr(unsigned char *bBuffer,int bLen,char *strBuf);

// 把十六进制字符串(strBuf)转换回字节数组(写入 byBuf)
// byLen 输入时为缓冲区最大容量，输出时为实际写入的字节数
// 返回值: 转换成功返回 true，失败(字符串不合法)返回 false
bool HexStrToBytes(const char *strBuf,unsigned char byBuf[],int &byLen);

// 把单个十六进制字符('0'~'9','A'~'F','a'~'f')转换为对应的整数(0~15)
// 如果字符不是合法的十六进制字符，返回 -1
int CharToInt(char val);

#endif // GFUNCTION_H