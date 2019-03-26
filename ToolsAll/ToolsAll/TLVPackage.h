#pragma once
#if 0
class TLVPackage
{
public:
	TLVPackage();
	~TLVPackage();
};
#endif
// TLV结构体
struct TLVEntity {
	unsigned char* Tag;         //标记
	unsigned char* Length;      //数据长度
	unsigned char* Value;       //数据
	unsigned int TagSize;       //标记占用字节数
	unsigned int LengthSize;    //数据长度占用字节数
	unsigned int ValueLength;
	TLVEntity* Sub_TLVEntity;   //子嵌套TLV实体
};
// TLV打包类
class TLVPackage
{
public:
	TLVPackage();
	virtual ~TLVPackage();
	//构造TLV实体
	static void Construct(unsigned char* buffer, unsigned int bufferLength, TLVEntity* tlvEntity, unsigned int& entityLength, unsigned int status = 0);
	//解析TLV字节数组
	static void Parse(TLVEntity* tlvEntity, unsigned int entityLength, unsigned char* buffer, unsigned int& bufferLength);
	static int Hex2Ascii(char* hex, char* ascii, int hexLeng);
	static int Ascii2Hex(char* ascii, char* hex);
	static int CONV_BcdAsc(UCHAR *pucDest, UCHAR *pucSrc, int uiSrcLen);
	static int CONV_AscBcd(UCHAR *pucDest, UCHAR *pucSrc, int uiSrcLen);
};
