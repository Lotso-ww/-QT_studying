#include "gfunction.h"
#include <cstring>
#include <cstdlib>


// 把 ANSI(多字节 char)字符串转换为 Unicode(宽字符 wchar_t)字符串
// str: 输入的 ANSI 字符串
// 返回: 新分配内存的 wchar_t 字符串，调用者需用 free() 释放
wchar_t * ANSIToUnicode( const char* str )
{
    int textlen=strlen(str)*2+1;                // 计算需要的宽字符长度(预留余量)
    wchar_t * result;
    result=(wchar_t *)malloc(textlen * sizeof(wchar_t));  // 分配内存
    memset(result, 0, textlen*sizeof(wchar_t));           // 清零
    mbstowcs(result, str, textlen);               // 多字节转宽字符
    return result;
}

// 把 Unicode(宽字符)字符串转换为 ANSI(多字节 char)字符串
// wcstr: 输入的宽字符字符串
// 返回: 新分配内存的 char 字符串，调用者需用 free() 释放
char * UnicodeToANSI( const wchar_t* wcstr )
{
    char* result;
    int textlen=wcslen(wcstr)*2+1;               // 计算需要的字节数(每个字符最多2字节+1余量)
    result=(char *)malloc(textlen*sizeof(char)); // 分配内存
    memset( result, 0, sizeof(char) * textlen);  // 清零
    wcstombs(result, wcstr, textlen);            // 宽字符转多字节
    return result;
}

// 把字节数组转换为十六进制字符串
// bBuffer: 输入的字节数组，bLen: 字节个数，strBuf: 输出的字符串缓冲区(长度至少为 bLen*2)
// 原理: 一个字节(8位)拆成高4位和低4位，每4位转成一个十六进制字符
void BytesToHexStr(unsigned char *bBuffer,int bLen,char *strBuf)
{
     unsigned char l4b,r4b;     // l4b=高4位, r4b=低4位
     char l4c,r4c;              // l4c=高4位对应字符, r4c=低4位对应字符
     int i;
     for(i=0;i<bLen;i++)
     {
          // 取低4位
          r4b=bBuffer[i] & 0x0f;
          if(r4b<=9)
              r4c=0x30+r4b;        // 0~9 转成 '0'~'9' (0x30='0')
          else
              r4c=0x41+(r4b-0x0a); // 10~15 转成 'A'~'F' (0x41='A')

          // 取高4位
          l4b=(bBuffer[i] & 0xf0) >> 4;
          if(l4b<=9)
              l4c=0x30+l4b;        // 0~9 转成 '0'~'9'
          else
              l4c=0x41+(l4b-0x0a); // 10~15 转成 'A'~'F'

          strBuf[i*2]=l4c;         // 高4位字符放前面
          strBuf[i*2+1]=r4c;       // 低4位字符放后面
     }
}

// 把十六进制字符串转换回字节数组
// strBuf: 输入的十六进制字符串，byBuf: 输出的字节缓冲区，byLen: 输入为最大容量，输出为实际字节数
// 返回: 成功 true，失败 false
bool HexStrToBytes(const char *strBuf,unsigned char byBuf[],int &byLen)
{
     int                      slen=0;       // 输入字符串长度
     int                      byteCount;    // 需要转换的字节个数
     int                      index=0;      // 当前处理的字符索引
     unsigned char            btmp;         // 临时存放拼接后的字节
     int                      val;          // 单个十六进制字符转换后的数值
     int                      itmp =0;      // 保存初始的 byteCount

     slen=(int)strlen(strBuf);
     if(slen<2)              return false;  // 至少要有2个字符(1个字节)
     // 字符串长度不是2的倍数则不合法(每2个字符表示1个字节)
     if((slen % 2)!=0)       return false;

     byteCount=slen / 2;                      // 总共要转换的字节数

     // 如果传入的缓冲区容量(byLen)有限，则最多只转换 byLen 个字节
     if(byLen > 0 && byteCount > byLen)
         byteCount = byLen;

     itmp=byteCount;
     // 逐字节处理：每2个十六进制字符拼成1个字节
     while(byteCount)
     {
          btmp=0;
          // 把第1个十六进制字符转成数值，作为字节的高4位
          val=CharToInt(strBuf[index]);
          if(val==-1)  return false;           // 非法字符
          btmp=(val << 4) & 0xf0;

          // 把第2个十六进制字符转成数值，作为字节的低4位
          val=CharToInt(strBuf[index+1]);
          if(val==-1)  return false;

          btmp = btmp | (val & 0x0f);          // 高4位和低4位合并成一个字节

          byBuf[itmp-byteCount] =btmp;        // 写入输出缓冲区

          index=index+2;                       // 处理下一对字符

          byteCount--;
     }

     byLen=itmp;                               // 返回实际转换的字节数

     return true;

}

// 把单个十六进制字符转换为对应的整数(0~15)
// 合法字符: '0'~'9'(0~9), 'A'~'F'(10~15), 'a'~'f'(10~15)
// 非法字符返回 -1
int CharToInt(char val)
{
     int ret=-1;
     if(val>='0' && val <='9')
     {
         ret=val-0x30;           // '0'~'9' 转成 0~9
     }
     if(val>='A'  && val<='F')
     {
         ret=val-'A'+ 0x0a;      // 'A'~'F' 转成 10~15
     }
     if(val>='a' && val<='f')
     {
         ret=val-'a'+0x0a;       // 'a'~'f' 转成 10~15
     }

     return ret;

}