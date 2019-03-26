#pragma once
#if 0
class TLVPackage
{
public:
	TLVPackage();
	~TLVPackage();
};
#endif
// TLV�ṹ��
struct TLVEntity {
	unsigned char* Tag;         //���
	unsigned char* Length;      //���ݳ���
	unsigned char* Value;       //����
	unsigned int TagSize;       //���ռ���ֽ���
	unsigned int LengthSize;    //���ݳ���ռ���ֽ���
	unsigned int ValueLength;
	TLVEntity* Sub_TLVEntity;   //��Ƕ��TLVʵ��
};
// TLV�����
class TLVPackage
{
public:
	TLVPackage();
	virtual ~TLVPackage();
	//����TLVʵ��
	static void Construct(unsigned char* buffer, unsigned int bufferLength, TLVEntity* tlvEntity, unsigned int& entityLength, unsigned int status = 0);
	//����TLV�ֽ�����
	static void Parse(TLVEntity* tlvEntity, unsigned int entityLength, unsigned char* buffer, unsigned int& bufferLength);
	static int Hex2Ascii(char* hex, char* ascii, int hexLeng);
	static int Ascii2Hex(char* ascii, char* hex);
	static int CONV_BcdAsc(UCHAR *pucDest, UCHAR *pucSrc, int uiSrcLen);
	static int CONV_AscBcd(UCHAR *pucDest, UCHAR *pucSrc, int uiSrcLen);
};
