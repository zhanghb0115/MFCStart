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

// ����TLV
void TLVPackage::Construct(
	unsigned char* buffer,
	unsigned int bufferLength,
	TLVEntity* tlvEntity,
	unsigned int& entityLength,
	unsigned int status
)
{
	int currentTLVIndex = 0;
	int currentIndex = 0;
	int currentStatus = 'T'; //״̬�ַ�
	unsigned long valueSize = 0;

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			valueSize = 0;
			//�ж��Ƿ�һ�ṹ
			if ((status == 1 && buffer[currentIndex] & 0x20) != 0x20)
			{
				tlvEntity[currentTLVIndex].Sub_TLVEntity = NULL; //��һ�ṹʱ����Tag�ÿ�
				//�ж��Ƿ���ֽ�Tag
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80); //�жϵڶ����ֽڵ����λ�Ƿ�Ϊ1
					int tagSize = endTagIndex - currentIndex + 1; //����Tag���������ֽ�

					tlvEntity[currentTLVIndex].Tag = new unsigned char[tagSize];
					memcpy(tlvEntity[currentTLVIndex].Tag, buffer + currentIndex, tagSize);
					tlvEntity[currentTLVIndex].Tag[tagSize] = 0;

					tlvEntity[currentTLVIndex].TagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					tlvEntity[currentTLVIndex].Tag = new unsigned char[1];
					memcpy(tlvEntity[currentTLVIndex].Tag, buffer + currentIndex, 1);
					tlvEntity[currentTLVIndex].Tag[1] = 0;

					tlvEntity[currentTLVIndex].TagSize = 1;

					currentIndex += 1;
				}
			}
			else
			{
				//�ж��Ƿ���ֽ�Tag
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80); //�жϵڶ����ֽڵ����λ�Ƿ�Ϊ1
					int tagSize = endTagIndex - currentIndex + 1; //����Tag���������ֽ�

					tlvEntity[currentTLVIndex].Tag = new unsigned char[tagSize];
					memcpy(tlvEntity[currentTLVIndex].Tag, buffer + currentIndex, tagSize);
					tlvEntity[currentTLVIndex].Tag[tagSize] = 0;

					tlvEntity[currentTLVIndex].TagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					tlvEntity[currentTLVIndex].Tag = new unsigned char[1];
					memcpy(tlvEntity[currentTLVIndex].Tag, buffer + currentIndex, 1);
					tlvEntity[currentTLVIndex].Tag[1] = 0;

					tlvEntity[currentTLVIndex].TagSize = 1;

					currentIndex += 1;
				}

				//����SubTag
				int subLength = 0;

				unsigned char* temp;
				if ((buffer[currentIndex] & 0x80) == 0x80)
				{
					for (int index = 0; index < 2; index++)
					{
						subLength += buffer[currentIndex + 1 + index] << (index * 8); //����Length��ĳ���
					}

					temp = new unsigned char[subLength];

					memcpy(temp, buffer + currentIndex + 3, subLength);
				}
				else
				{
					subLength = buffer[currentIndex];

					temp = new unsigned char[subLength];

					memcpy(temp, buffer + currentIndex + 1, subLength);
				}
				temp[subLength] = 0;

				//memcpy(temp, buffer + currentIndex + 1, subLength);
				unsigned int oLength;
				tlvEntity[currentTLVIndex].Sub_TLVEntity = new TLVEntity[1];
				Construct(temp, subLength, tlvEntity[currentTLVIndex].Sub_TLVEntity, oLength);
			}

			currentStatus = 'L';
			break;
		case 'L':
			//�жϳ����ֽڵ����λ�Ƿ�Ϊ1�����Ϊ1������ֽ�Ϊ������չ�ֽڣ�����һ���ֽڿ�ʼ��������
			if ((buffer[currentIndex] & 0x80) != 0x80)
			{
				tlvEntity[currentTLVIndex].Length = new unsigned char[1];
				memcpy(tlvEntity[currentTLVIndex].Length, buffer + currentIndex, 1);
				tlvEntity[currentTLVIndex].Length[1] = 0;
				tlvEntity[currentTLVIndex].LengthSize = 1;

				valueSize = tlvEntity[currentTLVIndex].Length[0];

				currentIndex += 1;
			}
			else
			{
				//Ϊ1�����

				unsigned int lengthSize = buffer[currentIndex] & 0x7f;

				currentIndex += 1; //����һ���ֽڿ�ʼ��Length��

				for (int index = 0; index < lengthSize; index++)
				{
					valueSize += buffer[currentIndex + index] << (index * 8); //����Length��ĳ���
				}

				tlvEntity[currentTLVIndex].Length = new unsigned char[lengthSize];
				memcpy(tlvEntity[currentTLVIndex].Length, buffer + currentIndex, lengthSize);
				tlvEntity[currentTLVIndex].Length[lengthSize] = 0;

				tlvEntity[currentTLVIndex].LengthSize = lengthSize;

				currentIndex += lengthSize;
			}

			currentStatus = 'V';
			break;
		case 'V':
			tlvEntity[currentTLVIndex].Value = new unsigned char[valueSize];
			memcpy(tlvEntity[currentTLVIndex].Value, buffer + currentIndex, valueSize);
			tlvEntity[currentTLVIndex].Value[valueSize] = 0;
			tlvEntity[currentTLVIndex].ValueLength = valueSize;

			currentIndex += valueSize;

			//������һ��TLV����ѭ��
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		default:
			return;
		}
	}

	entityLength = currentTLVIndex;
}

// ����TLV
void TLVPackage::Parse(
	TLVEntity* tlvEntity,
	unsigned int entityLength,
	unsigned char* buffer,
	unsigned int& bufferLength
)
{
	int currentIndex = 0;
	int currentTLVIndex = 0;
	unsigned long valueSize = 0;

	while (currentTLVIndex < entityLength)
	{
		valueSize = 0;
		TLVEntity entity = tlvEntity[currentTLVIndex];

		memcpy(buffer + currentIndex, entity.Tag, entity.TagSize);  //����Tag
		currentIndex += entity.TagSize;

		for (int index = 0; index < entity.LengthSize; index++)
		{
			valueSize += entity.Length[index] << (index * 8); //����Length��ĳ���
		}
		if (valueSize > 127)
		{
			buffer[currentIndex] = 0x80 | entity.LengthSize;
			currentIndex += 1;
		}

		memcpy(buffer + currentIndex, entity.Length, entity.LengthSize);    //����Length
		currentIndex += entity.LengthSize;
		//�ж��Ƿ������Ƕ��TLV
		if (entity.Sub_TLVEntity == NULL)
		{
			memcpy(buffer + currentIndex, entity.Value, valueSize); //����Value
			currentIndex += valueSize;
		}
		else
		{
			unsigned int oLength;
			Parse(entity.Sub_TLVEntity, 1, buffer + currentIndex, oLength); //������Ƕ��TLV
			currentIndex += oLength;
		}

		currentTLVIndex++;
	}
	buffer[currentIndex] = 0;
	bufferLength = currentIndex;
}

int TLVPackage::Hex2Ascii(char* hex, char* ascii, int hexLeng)
{
	int len = hexLeng, tlen, i, cnt;

	for (i = 0, cnt = 0, tlen = 0; i < len; i++)
	{
		char c = toupper(hex[i]);

		if ((c >= '0'&& c <= '9') || (c >= 'A'&& c <= 'F'))
		{
			BYTE t = (c >= 'A') ? c - 'A' + 10 : c - '0';

			if (cnt)
				ascii[tlen++] += t, cnt = 0;
			else
				ascii[tlen] = t << 4, cnt = 1;
		}
	}

	return tlen;
}

int TLVPackage::Ascii2Hex(char* ascii, char* hex)
{
	int i, len = strlen(ascii);
	char chHex[] = "0123456789ABCDEF";

	for (i = 0; i < len; i++)
	{
		hex[i * 3] = chHex[((BYTE)ascii[i]) >> 4];
		hex[i * 3 + 1] = chHex[((BYTE)ascii[i]) & 0xf];
		hex[i * 3 + 2] = ' ';
	}

	hex[len * 3] = '\0';

	return len * 3;

}

int TLVPackage::CONV_BcdAsc(UCHAR *pucDest, UCHAR *pucSrc, int uiSrcLen)
{
	int uiI;
	UCHAR TempBuffer [1024];
	int  charIndex=0;

	/*
	if (uiDestLen % 2)
		*pucDest++ = (*pucSrc++ & 0x0F) + 0x30;
	for (uiI = 0; uiI < (uiDestLen / 2); uiI++)
	{
		*pucDest++ = ((*pucSrc & 0xF0) >> 4) + 0x30;
		*pucDest++ = (*pucSrc++ & 0x0F) + 0x30;
	}*/
	memset(TempBuffer, 0x00, sizeof(TempBuffer));
	for (uiI = 0; uiI < uiSrcLen; uiI++)
	{
		TempBuffer[charIndex++] = ((*pucSrc & 0xF0) >> 4) + 0x30;
		TempBuffer[charIndex++] = (*pucSrc++ & 0x0F) + 0x30;
	}

	for (uiI = 0; uiI < uiSrcLen *2; uiI++) {
		if (TempBuffer[uiI] > (9 + 0x30))
		{
			switch (TempBuffer[uiI])
			{
			case 10 + 0x30:
				TempBuffer[uiI] = 'A';
				break;
			case 11 + 0x30:
				TempBuffer[uiI] = 'B';
				break;
			case 12 + 0x30:
				TempBuffer[uiI] = 'C';
				break;
			case 13 + 0x30:
				TempBuffer[uiI] = 'D';
				break;
			case 14 + 0x30:
				TempBuffer[uiI] = 'E';
				break;
			case 15 + 0x30:
				TempBuffer[uiI] = 'F';
				break;
			default:
				break;
			}
		}
	}
	memcpy(pucDest, TempBuffer, uiSrcLen*2);
	return(uiSrcLen*2);
}


int TLVPackage::CONV_AscBcd(UCHAR *pucDest, UCHAR *pucSrc, int uiSrcLen)
{
	int uiI;
	UCHAR TempBuffer[1024];
	UCHAR TempBuffer2[1024];
	int  HexCharIndex = 0;
	int  AscCharIndex = 0;
	
	memset(TempBuffer, 0x00, sizeof(TempBuffer));
	
	memcpy(TempBuffer, pucSrc, uiSrcLen);
	for (uiI = 0; uiI < uiSrcLen; uiI++) {
		if ((TempBuffer[uiI] >= 'a' && TempBuffer[uiI] <= 'f')
			|| (TempBuffer[uiI] >= 'A' && TempBuffer[uiI] <= 'F')
			|| (TempBuffer[uiI] >= '0' && TempBuffer[uiI] <= '9'))
		{
			switch (TempBuffer[uiI])
			{
			case 'A':
				TempBuffer[uiI] = 0x0A;
				break;
			case 'B':
				TempBuffer[uiI] = 0x0B;
				break;
			case 'C':
				TempBuffer[uiI] = 0x0C;
				break;
			case 'D':
				TempBuffer[uiI] = 0x0D;
				break;
			case 'E':
				TempBuffer[uiI] = 0x0E;
				break;
			case 'F':
				TempBuffer[uiI] = 0x0F;
				break;
			default:
				TempBuffer[uiI] = TempBuffer[uiI] - 0x30;
				break;
			}
		}
		else
			return -1;
	}

	HexCharIndex = 0;
	AscCharIndex = 0;
	memset(TempBuffer2, 0x00, sizeof(TempBuffer2));
	TRACE(" \r\n------------------- \r\n ");
	for (uiI = 0; uiI < ((uiSrcLen+1) / 2); uiI++)
	{
		TempBuffer2[uiI]   |= TempBuffer[AscCharIndex++] <<4;
		TempBuffer2[uiI]   |= TempBuffer[AscCharIndex++];
	}
	TRACE(" \r\n------------------- \r\n ");
	
	memcpy(pucDest, TempBuffer2, (uiSrcLen + 1) / 2);
	return((uiSrcLen + 1) / 2);
}