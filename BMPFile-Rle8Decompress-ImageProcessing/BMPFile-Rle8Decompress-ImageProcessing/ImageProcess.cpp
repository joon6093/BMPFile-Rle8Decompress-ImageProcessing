#include "ImageProcess.h"

ImageProcess::ImageProcess(char* file_name)  //�������������� ������ �޼ҵ�
{
	file = NULL;
	fileheader = {};
	infoheader = {};
	RGB = NULL;
	ReadFile = NULL;
	SaveFile = NULL;
	Sqare_index = NULL;
	Sqare_index_temp = NULL;

	file = fopen(file_name, "rb");
	if (file == NULL)   //����ó��
	{
		return;
	}

	fread(&fileheader, sizeof(BITMAPFILEHEADER), 1, file);  //��Ʈ�� ������� �б�

	fread(&infoheader, sizeof(BITMAPINFOHEADER), 1, file);  //��Ʈ�� ������� �б�

	RGB = new RGBQUAD[256];            //�������̺��� ��� ����
	fread(RGB, sizeof(RGBQUAD), 256, file);

	ReadFile = new BYTE[infoheader.biSizeImage];    //����ȿ����� �ȼ��������� ũ��� bisizeimage���� ũ��θ� Ȯ�ΰ���. 
	fread(ReadFile, sizeof(BYTE), infoheader.biSizeImage, file);

	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	if (infoheader.biWidth % 4 == 0)  //������ ���α��̰� 4�� ����ϰ��� �ȼ��������� ũ�Ⱑ new_width * infoheader.biHeight
	{
		SaveFile = new BYTE[new_width * infoheader.biHeight];
		Sqare_index = new bool[new_width * infoheader.biHeight];
		Sqare_index_temp = new bool[new_width * infoheader.biHeight];
	}
	else                            //������ ���α��̰� 4�� ����� �ƴҰ��� �ȼ��������� ũ�Ⱑ new_width * infoheader.biHeight + 2
	{
		SaveFile = new BYTE[new_width * infoheader.biHeight + 2];
		Sqare_index = new bool[new_width * infoheader.biHeight + 2];
		Sqare_index_temp = new bool[new_width * infoheader.biHeight + 2];
	}
	fclose(file);
}

int ImageProcess::Check_BitmapCompression()   //������ ���࿩�θ� Ȯ�����ִ� �޼ҵ�
{
	//RLE8����� ����
	if (infoheader.biCompression == 1) {
		return 1;
	}
	//����X 
	else if (infoheader.biCompression == 0) {
		return 0;
	}
	//��Ÿ
	else
	{
		return -1;
	}
}

void ImageProcess::BitmapCompression() //RLE ���� ���� ���� ����� �����ϴ� �޼ҵ�
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth); //������ �о�ö� 4�ǹ���� ũ��� �о�������� ����
	int firstbyte = 0;   //ù��° ����Ʈ�� �����ϱ����� ����
	int secondbyte = 0;  //�ι�° ����Ʈ�� �����ϱ����� ����
	int read_count = 0; //����� ������ �ȼ��������� ��ġ�� ������ ����
	int X_save_count = 0;  //�ȼ������͸� �������迭�� ���������� ���� ��ġ
	int Y_save_count = 0;  //�ȼ������͸� �������迭�� ���������� ���� ��ġ
	int NEW_X, NEW_Y = 0;  //MAKER�� 0002�϶� ��������� �̵��� ������ũ�⸦ ������ ����
	int OLD_X, OLD_Y;   //MAKER�� 0002�϶� ���� ��ġ�� ������ ����
	int print_len = 0;  //�ݺ��ؾ��� Ƚ���� �����ϴ� ����
	int print_len_count = 0;  //�ݺ��� Ż���� ���� �ݺ��� Ƚ���� �����ϴ� ����

	while (read_count < int(infoheader.biSizeImage))
	{
		print_len = 0;  //�ʱ�ȭ
		print_len_count = 0;  //�ʱ�ȭ
		firstbyte = ReadFile[read_count]; //ù��° ����Ʈ�� �о��
		secondbyte = ReadFile[++read_count]; //�ι�° ����Ʈ�� �о��

		if (firstbyte == 0)	//Absolute Mode or Marker
		{
			if (secondbyte >= 3) //�ι�° ����Ʈ�� 3�̻��϶��� Absolute Mode 
			{
				print_len = secondbyte;  //�ι�° ����Ʈ�� �ݺ��� Ƚ��
				while (print_len_count < print_len)
				{
					print_len_count++;  //�ݺ��� Ż���� ���� �ݺ��� Ƚ�� ����
					read_count++; //�ȼ������� ��ġ ����
					SaveFile[Y_save_count * new_width + X_save_count] = ReadFile[read_count]; //�ȼ������� ��ġ�� �ش��ϴ� ���� ���� ��ġ�� ���� ��ġ�� �̿��Ͽ� SaveFile�� ����
					X_save_count++;  //�����Ͽ����ϱ� ���� ��ġ ����
				}
				if (print_len % 2 == 1)	//���̰� Ȧ�����
					read_count++;	//���������� ��ȣȭ ��������)   ������ ���õ� Absolute Mode�� Ư¡
			}
			else if (secondbyte == 0)  //�ι�° ����Ʈ�� 0�̶�� ���� �ڵ�� ���ο� ������ ���� ���̸� ���� ������ ���� �� �̻��� ������ ������ �ǹ�
			{
				print_len = infoheader.biWidth - X_save_count;  //������������ �������ؼ� �ݺ�Ƚ���� ��ü ���α��� - ���� ���� ��ġ�� ����
				while (print_len_count < print_len)
				{
					print_len_count++;  //�ݺ��� Ż���� ���� �ݺ��� Ƚ�� ����
					SaveFile[Y_save_count * new_width + X_save_count] = 255;  //������ ���α��̺��� �ȼ��� ������ ������ ����Ʈ �������� ����
					X_save_count++; //�����Ͽ����ϱ� ���� ��ġ ����
				}
				X_save_count = 0; //���������̵������� ���� ��ġ �ʱ�ȭ
				Y_save_count++; //���� ������ �̵������� ���� ��ġ ����

			}
			else if (secondbyte == 1)  //�ι�° ����Ʈ�� 1�̶�� ��ȣȭ �� �������� �������� �ǹ�
			{
				if (Y_save_count * new_width + X_save_count < infoheader.biWidth * infoheader.biHeight) //���� ��ġ�� ������ �������� �ƴ϶�� 
				{
					for (; Y_save_count < infoheader.biHeight; Y_save_count++) //���� ������ �̵������� ���� ��ġ ����
					{
						for (; X_save_count < infoheader.biWidth; X_save_count++)  //�����Ͽ����ϱ� ���� ��ġ ����
						{
							SaveFile[Y_save_count * new_width + X_save_count] = 255; //������ ������ ����Ʈ �������� ����
						}
						X_save_count = 0; //���������̵������� ���� ��ġ �ʱ�ȭ
					}
				}
				read_count = infoheader.biSizeImage; //�������� �ݺ��� Ż���� ���� read_count�� �ȼ��������� ũ��� ����

			}
			else if (secondbyte == 2) //�ι�° ����Ʈ�� 1�̶�� ��ġ�κ��� ��������� �̵��� ��ġ�� ��Ÿ����.
			{

				NEW_X = ReadFile[++read_count];  //ù ��° ����Ʈ�� ��������� ������
				NEW_Y = ReadFile[++read_count]; //�� ��° ����Ʈ�� ���������� ������
				OLD_X = X_save_count;  //���� ���� ��ġ ����
				OLD_Y = Y_save_count; //���� ���� ��ġ ����
				while (Y_save_count < OLD_Y + NEW_Y) //���� ���� ��ġ�� �̵��� ���� ��ġ���� ������ 
				{
					while (X_save_count < infoheader.biWidth) //���翭�� ��ġ�� ������ ���̺��� �۴ٸ�
					{
						X_save_count++; //���� ���� ��ġ����
						SaveFile[Y_save_count * new_width + X_save_count] = 255; //������ ���� ����Ʈ �������� ä��
					}
					X_save_count = 0; //���������̵������� ���� ��ġ �ʱ�ȭ
					Y_save_count++; //���� ������ �̵������� ���� ��ġ ����
				}

				for (; X_save_count < OLD_X + NEW_X; X_save_count++)  //���� ������ ó�����־����� ���� ó������. ���� ���� ��ġ�� �̵��� ���� ��ġ���� �۴ٸ�
					SaveFile[Y_save_count * new_width + X_save_count] = 255; //������ ���� ����Ʈ �������� ä��
			}
		}
		if (firstbyte != 0)  //ù��° ����Ʈ�� 0�̾ƴ϶�� Encoded mode
		{
			print_len = firstbyte; //ù ��° ����Ʈ�� Ƚ��
			while (print_len_count < print_len)
			{
				print_len_count++; //�ݺ��� Ż���� ���� �ݺ��� �� ����
				SaveFile[Y_save_count * new_width + X_save_count] = secondbyte; //�� ��° ����Ʈ�� �ݺ��Ǵ� ��
				X_save_count++; //���� ��ġ ����
			}
		}
		read_count++; //�ȼ������� ��ġ ����

	}
	infoheader.biCompression = 0;	//������� ������� 0�� ���� ����
	if (infoheader.biWidth % 4 == 0)   //������ ���α��̰� 4�� ����ϰ��� �ȼ��������� ũ�Ⱑ new_width * infoheader.biHeight
	{
		infoheader.biSizeImage = new_width * infoheader.biHeight;  //���������� ũ�� ����	
	}
	else  //������ ���α��̰� 4�� ����� �ƴҰ��� �ȼ��������� ũ�Ⱑ new_width * infoheader.biHeight + 2
	{
		infoheader.biSizeImage = new_width * infoheader.biHeight + 2;	 //���������� ũ�� ����
	}

	fileheader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + infoheader.biSizeImage; //�� ����Ʈ ũ�� ����
	ReadFile = new BYTE[infoheader.biSizeImage]; //ReadFile�� ũ��� ���������� biSizeImage�� ũ���̹Ƿ� ���ο� ũ��� ����
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage); //ReadFile�� SaveFile�� �� ����
	Sleep(1000);
	printf("���������� �Ϸ�ƽ��ϴ�.\n");
}

void ImageProcess::InversionColor() //�Էµ� ������ ��⸦ ������ ����� �������ִ� �޼ҵ�
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);  //������ �о�ö� 4�ǹ���� ũ��� �о�������� ����
	int bit_map_index = 0; //���� �а��ִ� �ȼ���������ġ�� �����ϱ����� ����
	for (int height = 0; height < infoheader.biHeight; height++) //���� ũ��
	{
		for (int weidth = 0; weidth < infoheader.biWidth; weidth++) //���� ũ��
		{
			bit_map_index = height * new_width + weidth; //���� �а��ִ� �ȼ���������ġ�� ����ũ�� x 4�ǹ��ũ��� ��Ÿ�� ���α��� + ������ġ
			if (Sqare_index[bit_map_index] != 1) //�簢���� �׸� ��ġ�� �ƴ϶��
			{
				SaveFile[bit_map_index] = 255 - ReadFile[bit_map_index]; //255�� ���� �ȼ������Ͱ��� ���־� ��� ���� ��� ����
			}
			else
			{
				SaveFile[bit_map_index] = GRAY;    //�簢���� �׸� ��ġ��� �簢�� �׸���
			}
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage); //ReadFile�� SaveFile�� �� ����
	printf("���� �������� �Ϸ��.\n");
}

void ImageProcess::SqareDraw()  //�Էµ� ���� �簢���� �׷��ִ� �޼ҵ�
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	int user_select_x_start = 0; //����ڰ� �簢���� �׸� ��ġ�� ������ ���� (���� ���� ��ġ)
	int user_select_x_end = 0; //����ڰ� �簢���� �׸� ��ġ�� ������ ���� (���� �� ��ġ)
	int user_select_y_start = 0; //����ڰ� �簢���� �׸� ��ġ�� ������ ���� (���� ���� ��ġ)
	int user_select_y_end = 0; //����ڰ� �簢���� �׸� ��ġ�� ������ ���� (���� �� ��ġ)
	while (user_select_x_start<0 or user_select_x_start >= user_select_x_end or user_select_x_end> infoheader.biWidth or user_select_y_start<0 or user_select_y_start >= user_select_y_end or user_select_y_end > infoheader.biHeight) //���� ����ڰ� �߸��Է��ߴٸ� �ݺ�
	{
		printf("����ڰ� �Է��� ������ ����:%d ����:%d �� ũ�⸦ ������ �ֽ��ϴ�.\n", infoheader.biWidth, infoheader.biHeight);  //��������
		printf("�簢���� �׸� ��ġ ���� �Է����ּ���\n������ ���� ���� �������� X����: X������: Y����: Y�Ʒ���\n");
		scanf("%d %d %d %d", &user_select_x_start, &user_select_x_end, &user_select_y_start, &user_select_y_end);
	}
	for (int Sqare_height = user_select_y_start; Sqare_height < user_select_y_end; Sqare_height++) //����ڰ� ������ �簢���� ���� ��ġ
	{
		for (int Sqare_width = user_select_x_start; Sqare_width < user_select_x_end; Sqare_width++) //����ڰ� ������ �簢���� ���� ��ġ
		{
			bit_map_index = (infoheader.biHeight - 1 - Sqare_height) * new_width + Sqare_width;  //��Ʈ���� �ȼ������ʹ� �Ųٷ� �о���� Ư���� ������. �׷��� �츮�� �ȼ������͸� �о�ö� �ݴ�� �о�;� ������ ���� ���� �������� �簢���� �׷��� �� �ִ�.
			ReadFile[bit_map_index] = GRAY; //�ش� �ȼ��������� �� GRAY������ ����
			Sqare_index[bit_map_index] = 1; //�簢���� �׷��� ��ġ������ ���� (����ڰ� �簢���� �׸��� ��� ������ �ϴ� ����� ������ �� �簢���� ���� GRAY������ �޶����� ��. ����� �������� ���� �� ����ؾ��� �����̶�� �����ؼ� ó�����־���.
		}
	}
	memcpy(SaveFile, ReadFile, infoheader.biSizeImage);
	printf("���� �簢���׸��� �Ϸ��.\n");
}

void ImageProcess::Bright()  //�Էµ� ������ ��� ������ ���ִ� �޼ҵ�
{
	printf("��⸦ ������ ���� �Է����ּ��� -:��Ӱ� +:���\n");
	int bright_control = 0; //����ڰ� ���ϴ� ������� ���� ���� ����
	scanf("%d", &bright_control);
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			if (Sqare_index[bit_map_index] != 1) //�簢���� �׸� ��ġ�� �ƴ϶��
			{
				if (ReadFile[bit_map_index] + bright_control > 255) //����ڰ� ������ ����������� ���� �ȼ��������� ��Ⱚ�� �������� 255�ʰ��̶�� 255�� ����
				{
					SaveFile[bit_map_index] = 255;
				}
				else if (ReadFile[bit_map_index] + bright_control < 0)  //����ڰ� ������ ����������� ���� �ȼ��������� ��Ⱚ�� �������� 0�̸� �̶�� 0���� ����
				{
					SaveFile[bit_map_index] = 0;
				}
				else
				{
					SaveFile[bit_map_index] = ReadFile[bit_map_index] + bright_control; //���� �ΰ�찡 �ƴ϶�� ����ڰ� ������ ����������� ���� �ȼ��������� ��Ⱚ�� ��ģ ���� ���� �ȼ��������� ��� ��
				}
			}
			else
			{
				SaveFile[bit_map_index] = GRAY;    //�簢���� �׸� ��ġ��� �簢�� �׸���
			}
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("���� ��������� �Ϸ��.\n");
}

void ImageProcess::YFlip()  //�Էµ� ������ ���Ϲ������ִ� �޼ҵ�
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	memcpy(Sqare_index_temp, Sqare_index, infoheader.biSizeImage);  //�簢���� ��ġ���� ��ȯ�� ���� ���纻�� �������� �������ش�.
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			SaveFile[bit_map_index] = ReadFile[((infoheader.biHeight - 1 - height) * new_width) + width]; //���Ϲ��� ��� ������ ���� �Ʒ��������� ���� �о��
			Sqare_index[bit_map_index] = Sqare_index_temp[((infoheader.biHeight - 1 - height) * new_width) + width];  //�簢���� �׸� �� ���� ���Ϲ��� ��� ������ ���� �Ʒ��������� ���� �о��
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("���� ���Ϲ����� �Ϸ��.\n");
}

void ImageProcess::XFlip() //�Էµ� ������ �¿�������ִ� �޼ҵ�
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	memcpy(Sqare_index_temp, Sqare_index, infoheader.biSizeImage);  //�簢���� ��ġ���� ��ȯ�� ���� ���纻�� �������� �������ش�.
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			SaveFile[bit_map_index] = ReadFile[(height * new_width) + infoheader.biWidth - 1 - width]; //�¿���� ��� ������ ���� �쿡�� �·� �о��
			Sqare_index[bit_map_index] = Sqare_index_temp[(height * new_width) + infoheader.biWidth - 1 - width]; //�簢���� �׸� �� ���� �¿���� ��� ������ ���� �쿡�� �·� �о��
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("���� �¿������ �Ϸ��.\n");
}

void ImageProcess::Thresholding()  //�Էµ� ������ �Ӱ�ȭ���ִ� �޼ҵ�
{
	int Thresholding_control = -1; //����ڰ� ���ϴ� �Ӱ谪�� ������ ����
	while (Thresholding_control < 0 or Thresholding_control > 255) //����ڰ� �Ӱ谪�� �߸��Է����־��� ��� �ݺ�
	{
		printf("�Ӱ谪�� �Է����ּ���   �ּҰ�:0 �ִ밪:255 \n");
		scanf("%d", &Thresholding_control);
	}
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			if (Sqare_index[bit_map_index] != 1)  //���������� �簢���� ��� ���� ����� �� �־ ����
			{
				if (ReadFile[bit_map_index] > Thresholding_control) //���� �ȼ��������� ��Ⱚ�� �Ӱ谪���� ũ�ٸ�
				{
					SaveFile[bit_map_index] = 255;  //���� ū ����� �Ͼ������ ����
				}
				else if (ReadFile[bit_map_index] <= Thresholding_control)   //���� �ȼ��������� ��Ⱚ�� �Ӱ谪���� �۴ٸ�
				{
					SaveFile[bit_map_index] = 0; //���� ���� ����� ���������� ����
				}
			}
			else
			{
				SaveFile[bit_map_index] = GRAY;
			}
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("���� �Ӱ�ȭ�� �Ϸ��.\n");
}

void ImageProcess::Slicing() //�Էµ� ������ �����������ִ� �޼ҵ� 
{
	int obj_start = -1;  //����ڰ� ���ϴ� ��ü�� ���������� ���� ��ü�� ��� ������ �������� ������ ���� 
	int obj_end = -1;  //����ڰ� ���ϴ� ��ü�� ���������� ���� ��ü�� ��� ������ ������ ������ ����
	int Slicing_control = -1;  //����ڰ� ���ϴ� ��ü�� ���������� ���� �ٲ� ��ü�� ��Ⱚ�� ������ ����
	while (obj_start<0 or obj_start > obj_end or obj_end > 255)  //����ڰ� �߸��Է��Ͽ��� ��� �ݺ�
	{
		printf("��ü�� �������� �Է����ּ���\n");
		scanf("%d %d", &obj_start, &obj_end);
	}
	while (Slicing_control < 0 or Slicing_control > 255) //����ڰ� �߸��Է��Ͽ��� ��� �ݺ�
	{
		printf("��ü�� �����ϱ����� ��� ���� �Է����ּ���   �ּҰ�:0 �ִ밪:255 \n");
		scanf("%d", &Slicing_control);
	}
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			if (Sqare_index[bit_map_index] != 1)  //���������� �簢���� ��� ���� ����� �� �־ ����
			{
				if (obj_start <= ReadFile[bit_map_index] and ReadFile[bit_map_index] <= obj_end) //����ڰ� ���ϴ� ��ü�� ���������� ���� ��ü�� ��� �������
				{
					SaveFile[bit_map_index] = Slicing_control;  //����ڰ� ���ϴ� ��ü�� ���������� ���� �ٲ� ��ü�� ��Ⱚ���� ����
				}
				else
				{
					SaveFile[bit_map_index] = ReadFile[bit_map_index];   //��� ������ �ƴ϶�� ���� �ȼ��������� ��Ⱚ���� ����
				}
			}
			else
			{
				SaveFile[bit_map_index] = GRAY;
			}
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("���� ���������� �Ϸ��.\n");
}

void ImageProcess::SaveAsDiff(char* new_file_name) //�Էµ� ������ ���ο� �̸����� �������ִ� �޼ҵ�
{
	file = fopen(new_file_name, "wb");
	if (file == NULL)  //������ ���������� ��� ����ó��
	{
		printf("������ ������ �ʽ��ϴ�.\n");
		return;
	}
	fwrite(&fileheader, sizeof(BITMAPFILEHEADER), 1, file); //��Ʈ�� ���� ��� ����
	fwrite(&infoheader, sizeof(BITMAPINFOHEADER), 1, file); //��Ʈ�� ���� ��� ����
	fwrite(RGB, sizeof(RGBQUAD), 256, file);  //�������̺� ����
	fwrite(SaveFile, sizeof(BYTE), infoheader.biSizeImage, file); //������ �ȼ������� ����
	fclose(file);
}