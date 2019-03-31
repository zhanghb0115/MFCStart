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

// 构造TLV
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
	int currentStatus = 'T'; //状态字符
	unsigned long valueSize = 0;

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			valueSize = 0;
			//判断是否单一结构
			if ((status == 1 && buffer[currentIndex] & 0x20) != 0x20)
			{
				tlvEntity[currentTLVIndex].Sub_TLVEntity = NULL; //单一结构时将子Tag置空
				//判断是否多字节Tag
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80); //判断第二个字节的最高位是否为1
					int tagSize = endTagIndex - currentIndex + 1; //计算Tag包含多少字节

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
				//判断是否多字节Tag
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80); //判断第二个字节的最高位是否为1
					int tagSize = endTagIndex - currentIndex + 1; //计算Tag包含多少字节

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

				//分析SubTag
				int subLength = 0;

				unsigned char* temp;
				if ((buffer[currentIndex] & 0x80) == 0x80)
				{
					for (int index = 0; index < 2; index++)
					{
						subLength += buffer[currentIndex + 1 + index] << (index * 8); //计算Length域的长度
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
			//判断长度字节的最高位是否为1，如果为1，则该字节为长度扩展字节，由下一个字节开始决定长度
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
				//为1的情况

				unsigned int lengthSize = buffer[currentIndex] & 0x7f;

				currentIndex += 1; //从下一个字节开始算Length域

				for (int index = 0; index < lengthSize; index++)
				{
					valueSize += buffer[currentIndex + index] << (index * 8); //计算Length域的长度
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

			//进入下一个TLV构造循环
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		default:
			return;
		}
	}

	entityLength = currentTLVIndex;
}

// 解析TLV
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

		memcpy(buffer + currentIndex, entity.Tag, entity.TagSize);  //解析Tag
		currentIndex += entity.TagSize;

		for (int index = 0; index < entity.LengthSize; index++)
		{
			valueSize += entity.Length[index] << (index * 8); //计算Length域的长度
		}
		if (valueSize > 127)
		{
			buffer[currentIndex] = 0x80 | entity.LengthSize;
			currentIndex += 1;
		}

		memcpy(buffer + currentIndex, entity.Length, entity.LengthSize);    //解析Length
		currentIndex += entity.LengthSize;
		//判断是否包含子嵌套TLV
		if (entity.Sub_TLVEntity == NULL)
		{
			memcpy(buffer + currentIndex, entity.Value, valueSize); //解析Value
			currentIndex += valueSize;
		}
		else
		{
			unsigned int oLength;
			Parse(entity.Sub_TLVEntity, 1, buffer + currentIndex, oLength); //解析子嵌套TLV
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