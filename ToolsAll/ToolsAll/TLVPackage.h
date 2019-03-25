#pragma once
#if 0
class TLVPackage
{
public:
	TLVPackage();
	~TLVPackage();
};
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
