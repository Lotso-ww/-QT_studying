#include "gfunction.h"
#include <cstring>
#include <cstdlib>


wchar_t * ANSIToUnicode( const char* str )
{
    int textlen=strlen(str)*2+1;
    wchar_t * result;
    result=(wchar_t *)malloc(textlen * sizeof(wchar_t));
    memset(result, 0, textlen*sizeof(wchar_t));
    mbstowcs(result, str, textlen);
    return result;
}

char * UnicodeToANSI( const wchar_t* wcstr )
{
    char* result;
    int textlen=wcslen(wcstr)*2+1;
    result=(char *)malloc(textlen*sizeof(char));
    memset( result, 0, sizeof(char) * textlen);
    wcstombs(result, wcstr, textlen);
    return result;
}

void BytesToHexStr(unsigned char *bBuffer,int bLen,char *strBuf)
{
     unsigned char l4b,r4b;
     char l4c,r4c;
     int i;
     for(i=0;i<bLen;i++)
     {
          r4b=bBuffer[i] & 0x0f;
          if(r4b<=9)
              r4c=0x30+r4b;
          else
              r4c=0x41+(r4b-0x0a);

          l4b=(bBuffer[i] & 0xf0) >> 4;
          if(l4b<=9)
              l4c=0x30+l4b;
          else
              l4c=0x41+(l4b-0x0a);

          strBuf[i*2]=l4c;
          strBuf[i*2+1]=r4c;

     }
}
bool HexStrToBytes(const char *strBuf,unsigned char byBuf[],int &byLen)
{
     int                      slen=0;
     int                      byteCount;
     int                      index=0;
     unsigned char            btmp;
     int                      val;
     int                      itmp =0;

     slen=(int)strlen(strBuf);
     if(slen<2)              return false;
     //不是2的倍数
     if((slen % 2)!=0)       return false;

     byteCount=slen / 2;

     if(byLen > 0 && byteCount > byLen)
         byteCount = byLen;

     itmp=byteCount;
     //全部转大写
     //CharToUpper(strBuf);
     while(byteCount)
     {
          btmp=0;
          val=CharToInt(strBuf[index]);
          if(val==-1)  return false;
          btmp=(val << 4) & 0xf0;

          val=CharToInt(strBuf[index+1]);
          if(val==-1)  return false;

          btmp = btmp | (val & 0x0f);

          byBuf[itmp-byteCount] =btmp;

          index=index+2;

          byteCount--;
     }

     byLen=itmp;

     return true;

}

int CharToInt(char val)
{
     int ret=-1;
     if(val>='0' && val <='9')
     {
         ret=val-0x30;
     }
     if(val>='A'  && val<='F')
     {
         ret=val-'A'+ 0x0a;
     }
     if(val>='a' && val<='f')
     {
         ret=val-'a'+0x0a;
     }

     return ret;

}
