#include "stdafx.h"
#include "TLVPackage.h"

#if 0
TLVPackage::TLVPackage()
{
}


TLVPackage::~TLVPackage()
{
}
#endif

// TLV�ṹ��
struct TLVEntity {
	unsigned char* Tag;         //���
	unsigned char* Length;      //���ݳ���
	unsigned char* Value;       //����
	unsigned int TagSize;       //���ռ���ֽ���
	unsigned int LengthSize;    //���ݳ���ռ���ֽ���
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
};