#include "ImageProcess.h"

ImageProcess::ImageProcess(char* file_name)  //영상데이터정보를 들고오는 메소드
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
	if (file == NULL)   //오류처리
	{
		return;
	}

	fread(&fileheader, sizeof(BITMAPFILEHEADER), 1, file);  //비트맵 파일헤더 읽기

	fread(&infoheader, sizeof(BITMAPINFOHEADER), 1, file);  //비트맵 정보헤더 읽기

	RGB = new RGBQUAD[256];            //색상테이블을 담는 변수
	fread(RGB, sizeof(RGBQUAD), 256, file);

	ReadFile = new BYTE[infoheader.biSizeImage];    //압축된영상의 픽셀데이터의 크기는 bisizeimage로의 크기로만 확인가능. 
	fread(ReadFile, sizeof(BYTE), infoheader.biSizeImage, file);

	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	if (infoheader.biWidth % 4 == 0)  //영상의 가로길이가 4의 배수일경우는 픽셀데이터의 크기가 new_width * infoheader.biHeight
	{
		SaveFile = new BYTE[new_width * infoheader.biHeight];
		Sqare_index = new bool[new_width * infoheader.biHeight];
		Sqare_index_temp = new bool[new_width * infoheader.biHeight];
	}
	else                            //영상의 가로길이가 4의 배수가 아닐경우는 픽셀데이터의 크기가 new_width * infoheader.biHeight + 2
	{
		SaveFile = new BYTE[new_width * infoheader.biHeight + 2];
		Sqare_index = new bool[new_width * infoheader.biHeight + 2];
		Sqare_index_temp = new bool[new_width * infoheader.biHeight + 2];
	}
	fclose(file);
}

int ImageProcess::Check_BitmapCompression()   //영상의 압축여부를 확인해주는 메소드
{
	//RLE8방식의 압축
	if (infoheader.biCompression == 1) {
		return 1;
	}
	//압축X 
	else if (infoheader.biCompression == 0) {
		return 0;
	}
	//기타
	else
	{
		return -1;
	}
}

void ImageProcess::BitmapCompression() //RLE 파일 압축 해제 기능을 수행하는 메소드
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth); //영상을 읽어올때 4의배수의 크기로 읽어오기위한 변수
	int firstbyte = 0;   //첫번째 바이트를 저장하기위한 변수
	int secondbyte = 0;  //두번째 바이트를 저장하기위한 변수
	int read_count = 0; //압축된 영상의 픽셀데이터의 위치를 저장할 변수
	int X_save_count = 0;  //픽셀데이터를 이차원배열로 생각했을때 열의 위치
	int Y_save_count = 0;  //픽셀데이터를 이차원배열로 생각했을때 행의 위치
	int NEW_X, NEW_Y = 0;  //MAKER가 0002일때 상대적으로 이동할 오프셋크기를 저장할 변수
	int OLD_X, OLD_Y;   //MAKER가 0002일때 현재 위치를 저장할 변수
	int print_len = 0;  //반복해야할 횟수를 저장하는 변수
	int print_len_count = 0;  //반복문 탈출을 위한 반복한 횟수를 저장하는 변수

	while (read_count < int(infoheader.biSizeImage))
	{
		print_len = 0;  //초기화
		print_len_count = 0;  //초기화
		firstbyte = ReadFile[read_count]; //첫번째 바이트를 읽어옴
		secondbyte = ReadFile[++read_count]; //두번째 바이트를 읽어옴

		if (firstbyte == 0)	//Absolute Mode or Marker
		{
			if (secondbyte >= 3) //두번째 바이트가 3이상일때는 Absolute Mode 
			{
				print_len = secondbyte;  //두번째 바이트가 반복할 횟수
				while (print_len_count < print_len)
				{
					print_len_count++;  //반복문 탈출을 위한 반복한 횟수 증가
					read_count++; //픽셀데이터 위치 증가
					SaveFile[Y_save_count * new_width + X_save_count] = ReadFile[read_count]; //픽셀데이터 위치에 해당하는 값을 행의 위치와 열의 위치를 이용하여 SaveFile에 저장
					X_save_count++;  //저장하였으니까 열의 위치 증가
				}
				if (print_len % 2 == 1)	//길이가 홀수라면
					read_count++;	//마지막값은 복호화 하지않음)   문제의 제시된 Absolute Mode의 특징
			}
			else if (secondbyte == 0)  //두번째 바이트가 0이라면 다음 코드는 새로운 라인을 위한 것이며 현재 라인을 위한 더 이상은 정보는 없음을 의미
			{
				print_len = infoheader.biWidth - X_save_count;  //다음라인으로 가기위해서 반복횟수를 전체 가로길이 - 현재 열의 위치로 설정
				while (print_len_count < print_len)
				{
					print_len_count++;  //반복문 탈출을 위한 반복한 횟수 증가
					SaveFile[Y_save_count * new_width + X_save_count] = 255;  //영상의 가로길이보다 픽셀의 개수가 적으면 디폴트 배경색으로 저장
					X_save_count++; //저장하였으니까 열의 위치 증가
				}
				X_save_count = 0; //다음열로이동했으니 열의 위치 초기화
				Y_save_count++; //다음 행으로 이동했으니 행의 위치 증가

			}
			else if (secondbyte == 1)  //두번째 바이트가 1이라면 부호화 된 데이터의 마지막을 의미
			{
				if (Y_save_count * new_width + X_save_count < infoheader.biWidth * infoheader.biHeight) //현재 위치가 영상의 마지막이 아니라면 
				{
					for (; Y_save_count < infoheader.biHeight; Y_save_count++) //다음 행으로 이동했으니 행의 위치 증가
					{
						for (; X_save_count < infoheader.biWidth; X_save_count++)  //저장하였으니까 열의 위치 증가
						{
							SaveFile[Y_save_count * new_width + X_save_count] = 255; //나머지 영역은 디폴트 배경색으로 저장
						}
						X_save_count = 0; //다음열로이동했으니 열의 위치 초기화
					}
				}
				read_count = infoheader.biSizeImage; //압축해제 반복문 탈출을 위한 read_count를 픽셀데이터의 크기로 지정

			}
			else if (secondbyte == 2) //두번째 바이트가 1이라면 위치로부터 상대적으로 이동할 위치를 나타낸다.
			{

				NEW_X = ReadFile[++read_count];  //첫 번째 바이트는 수평방향의 오프셋
				NEW_Y = ReadFile[++read_count]; //두 번째 바이트는 수직방향의 오프셋
				OLD_X = X_save_count;  //현재 열의 위치 저장
				OLD_Y = Y_save_count; //현재 행의 위치 저장
				while (Y_save_count < OLD_Y + NEW_Y) //현재 행의 위치가 이동할 행의 위치보다 작으면 
				{
					while (X_save_count < infoheader.biWidth) //현재열의 위치가 가로의 길이보다 작다면
					{
						X_save_count++; //현재 열의 위치증가
						SaveFile[Y_save_count * new_width + X_save_count] = 255; //사이의 값은 디폴트 배경색으로 채움
					}
					X_save_count = 0; //다음열로이동했으니 열의 위치 초기화
					Y_save_count++; //다음 행으로 이동했으니 행의 위치 증가
				}

				for (; X_save_count < OLD_X + NEW_X; X_save_count++)  //행을 위에서 처리해주었으니 열을 처리해줌. 현재 열의 위치가 이동할 열의 위치보다 작다면
					SaveFile[Y_save_count * new_width + X_save_count] = 255; //사이의 값은 디폴트 배경색으로 채움
			}
		}
		if (firstbyte != 0)  //첫번째 바이트가 0이아니라면 Encoded mode
		{
			print_len = firstbyte; //첫 번째 바이트는 횟수
			while (print_len_count < print_len)
			{
				print_len_count++; //반복문 탈출을 위해 반복한 값 증가
				SaveFile[Y_save_count * new_width + X_save_count] = secondbyte; //두 번째 바이트는 반복되는 값
				X_save_count++; //열의 위치 증가
			}
		}
		read_count++; //픽셀데이터 위치 증가

	}
	infoheader.biCompression = 0;	//압축되지 않은경우 0의 값을 가짐
	if (infoheader.biWidth % 4 == 0)   //영상의 가로길이가 4의 배수일경우는 픽셀데이터의 크기가 new_width * infoheader.biHeight
	{
		infoheader.biSizeImage = new_width * infoheader.biHeight;  //압축해제된 크기 지정	
	}
	else  //영상의 가로길이가 4의 배수가 아닐경우는 픽셀데이터의 크기가 new_width * infoheader.biHeight + 2
	{
		infoheader.biSizeImage = new_width * infoheader.biHeight + 2;	 //압축해제된 크기 지정
	}

	fileheader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + infoheader.biSizeImage; //총 바이트 크기 저장
	ReadFile = new BYTE[infoheader.biSizeImage]; //ReadFile의 크기는 압축해제전 biSizeImage의 크기이므로 새로운 크기로 배정
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage); //ReadFile에 SaveFile의 값 복사
	Sleep(1000);
	printf("압축해제가 완료됐습니다.\n");
}

void ImageProcess::InversionColor() //입력된 영상의 밝기를 반전한 결과를 제공해주는 메소드
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);  //영상을 읽어올때 4의배수의 크기로 읽어오기위한 변수
	int bit_map_index = 0; //현재 읽고있는 픽셀데이터위치를 저장하기위한 변수
	for (int height = 0; height < infoheader.biHeight; height++) //열의 크기
	{
		for (int weidth = 0; weidth < infoheader.biWidth; weidth++) //행의 크기
		{
			bit_map_index = height * new_width + weidth; //현재 읽고있는 픽셀데이터위치는 행의크기 x 4의배수크기로 나타낸 가로길이 + 열의위치
			if (Sqare_index[bit_map_index] != 1) //사각형을 그린 위치가 아니라면
			{
				SaveFile[bit_map_index] = 255 - ReadFile[bit_map_index]; //255에 현재 픽셀데이터값을 빼주어 밝기 반전 기능 수행
			}
			else
			{
				SaveFile[bit_map_index] = GRAY;    //사각형을 그린 위치라면 사각형 그리기
			}
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage); //ReadFile에 SaveFile의 값 복사
	printf("영상 밝기반전이 완료됨.\n");
}

void ImageProcess::SqareDraw()  //입력된 영상에 사각형을 그려주는 메소드
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	int user_select_x_start = 0; //사용자가 사각형을 그릴 위치를 저장할 변수 (열의 시작 위치)
	int user_select_x_end = 0; //사용자가 사각형을 그릴 위치를 저장할 변수 (열의 끝 위치)
	int user_select_y_start = 0; //사용자가 사각형을 그릴 위치를 저장할 변수 (행의 시작 위치)
	int user_select_y_end = 0; //사용자가 사각형을 그릴 위치를 저장할 변수 (행의 끝 위치)
	while (user_select_x_start<0 or user_select_x_start >= user_select_x_end or user_select_x_end> infoheader.biWidth or user_select_y_start<0 or user_select_y_start >= user_select_y_end or user_select_y_end > infoheader.biHeight) //만약 사용자가 잘못입력했다면 반복
	{
		printf("사용자가 입력한 영상은 가로:%d 세로:%d 의 크기를 가지고 있습니다.\n", infoheader.biWidth, infoheader.biHeight);  //정보제공
		printf("사각형을 그릴 위치 값을 입력해주세요\n영상의 왼쪽 위를 기준으로 X왼쪽: X오른쪽: Y위쪽: Y아래쪽\n");
		scanf("%d %d %d %d", &user_select_x_start, &user_select_x_end, &user_select_y_start, &user_select_y_end);
	}
	for (int Sqare_height = user_select_y_start; Sqare_height < user_select_y_end; Sqare_height++) //사용자가 지정한 사각형의 행의 위치
	{
		for (int Sqare_width = user_select_x_start; Sqare_width < user_select_x_end; Sqare_width++) //사용자가 지정한 사각형의 열의 위치
		{
			bit_map_index = (infoheader.biHeight - 1 - Sqare_height) * new_width + Sqare_width;  //비트맵의 픽셀데이터는 거꾸로 읽어오는 특성을 가진다. 그래서 우리도 픽셀데이터를 읽어올때 반대로 읽어와야 영상의 왼쪽 위를 기준으로 사각형을 그려줄 수 있다.
			ReadFile[bit_map_index] = GRAY; //해당 픽셀데이터의 값 GRAY색으로 변경
			Sqare_index[bit_map_index] = 1; //사각형을 그려준 위치데이터 저장 (사용자가 사각형을 그린후 밝기 조절을 하는 기능을 수행할 시 사각형의 색이 GRAY값에서 달라지게 됨. 사용자 기준으로 봤을 때 고려해야할 사항이라고 생각해서 처리해주었음.
		}
	}
	memcpy(SaveFile, ReadFile, infoheader.biSizeImage);
	printf("영상 사각형그리기 완료됨.\n");
}

void ImageProcess::Bright()  //입력된 영상을 밝기 조절을 해주는 메소드
{
	printf("밝기를 조절할 값을 입력해주세요 -:어둡게 +:밝게\n");
	int bright_control = 0; //사용자가 원하는 밝기조절 값을 받을 변수
	scanf("%d", &bright_control);
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			if (Sqare_index[bit_map_index] != 1) //사각형을 그린 위치가 아니라면
			{
				if (ReadFile[bit_map_index] + bright_control > 255) //사용자가 지정한 밝기조절값과 현재 픽셀데이터의 밝기값을 합쳤을때 255초과이라면 255로 지정
				{
					SaveFile[bit_map_index] = 255;
				}
				else if (ReadFile[bit_map_index] + bright_control < 0)  //사용자가 지정한 밝기조절값과 현재 픽셀데이터의 밝기값을 합쳤을때 0미만 이라면 0으로 지정
				{
					SaveFile[bit_map_index] = 0;
				}
				else
				{
					SaveFile[bit_map_index] = ReadFile[bit_map_index] + bright_control; //위에 두경우가 아니라면 사용자가 지정한 밝기조절값과 현재 픽셀데이터의 밝기값을 합친 값이 현재 픽셀데이터의 밝기 값
				}
			}
			else
			{
				SaveFile[bit_map_index] = GRAY;    //사각형을 그린 위치라면 사각형 그리기
			}
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("영상 밝기조절이 완료됨.\n");
}

void ImageProcess::YFlip()  //입력된 영상을 상하반전해주는 메소드
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	memcpy(Sqare_index_temp, Sqare_index, infoheader.biSizeImage);  //사각형의 위치정보 변환을 위해 복사본에 원본값을 복사해준다.
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			SaveFile[bit_map_index] = ReadFile[((infoheader.biHeight - 1 - height) * new_width) + width]; //상하반전 기능 수행을 위해 아래에서부터 위로 읽어옴
			Sqare_index[bit_map_index] = Sqare_index_temp[((infoheader.biHeight - 1 - height) * new_width) + width];  //사각형을 그린 값 또한 상하반전 기능 수행을 위해 아래에서부터 위로 읽어옴
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("영상 상하반전이 완료됨.\n");
}

void ImageProcess::XFlip() //입력된 영상을 좌우반전해주는 메소드
{
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	memcpy(Sqare_index_temp, Sqare_index, infoheader.biSizeImage);  //사각형의 위치정보 변환을 위해 복사본에 원본값을 복사해준다.
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			SaveFile[bit_map_index] = ReadFile[(height * new_width) + infoheader.biWidth - 1 - width]; //좌우반전 기능 수행을 위해 우에서 좌로 읽어옴
			Sqare_index[bit_map_index] = Sqare_index_temp[(height * new_width) + infoheader.biWidth - 1 - width]; //사각형을 그린 값 또한 좌우반전 기능 수행을 위해 우에서 좌로 읽어옴
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("영상 좌우반전이 완료됨.\n");
}

void ImageProcess::Thresholding()  //입력된 영상을 임계화해주는 메소드
{
	int Thresholding_control = -1; //사용자가 원하는 임계값을 저장할 변수
	while (Thresholding_control < 0 or Thresholding_control > 255) //사용자가 임계값을 잘못입력해주었을 경우 반복
	{
		printf("임계값을 입력해주세요   최소값:0 최대값:255 \n");
		scanf("%d", &Thresholding_control);
	}
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			if (Sqare_index[bit_map_index] != 1)  //마찬가지로 사각형의 밝기 또한 변경될 수 있어서 방지
			{
				if (ReadFile[bit_map_index] > Thresholding_control) //현재 픽셀데이터의 밝기값이 임계값보다 크다면
				{
					SaveFile[bit_map_index] = 255;  //제일 큰 밝기인 하얀색으로 지정
				}
				else if (ReadFile[bit_map_index] <= Thresholding_control)   //현재 픽셀데이터의 밝기값이 임계값보다 작다면
				{
					SaveFile[bit_map_index] = 0; //제일 작은 밝기인 검정색으로 지정
				}
			}
			else
			{
				SaveFile[bit_map_index] = GRAY;
			}
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("영상 임계화가 완료됨.\n");
}

void ImageProcess::Slicing() //입력된 영상을 레벨분할해주는 메소드 
{
	int obj_start = -1;  //사용자가 원하는 객체의 레벨분할을 위해 객체의 밝기 범위의 시작점을 저장할 변수 
	int obj_end = -1;  //사용자가 원하는 객체의 레벨분할을 위해 객체의 밝기 범위의 끝점을 저장할 변수
	int Slicing_control = -1;  //사용자가 원하는 객체의 레벨분할을 위해 바꿀 객체의 밝기값을 저장할 변수
	while (obj_start<0 or obj_start > obj_end or obj_end > 255)  //사용자가 잘못입력하였을 경우 반복
	{
		printf("객체의 밝기범위를 입력해주세요\n");
		scanf("%d %d", &obj_start, &obj_end);
	}
	while (Slicing_control < 0 or Slicing_control > 255) //사용자가 잘못입력하였을 경우 반복
	{
		printf("객체를 강조하기위한 밝기 값을 입력해주세요   최소값:0 최대값:255 \n");
		scanf("%d", &Slicing_control);
	}
	int new_width = NEW_WIDTH(infoheader.biBitCount * infoheader.biWidth);
	int bit_map_index = 0;
	for (int height = 0; height < infoheader.biHeight; height++)
	{
		for (int width = 0; width < infoheader.biWidth; width++)
		{
			bit_map_index = height * new_width + width;
			if (Sqare_index[bit_map_index] != 1)  //마찬가지로 사각형의 밝기 또한 변경될 수 있어서 방지
			{
				if (obj_start <= ReadFile[bit_map_index] and ReadFile[bit_map_index] <= obj_end) //사용자가 원하는 객체의 레벨분할을 위해 객체의 밝기 범위라면
				{
					SaveFile[bit_map_index] = Slicing_control;  //사용자가 원하는 객체의 레벨분할을 위해 바꿀 객체의 밝기값으로 변경
				}
				else
				{
					SaveFile[bit_map_index] = ReadFile[bit_map_index];   //밝기 범위가 아니라면 원래 픽셀데이터의 밝기값으로 지정
				}
			}
			else
			{
				SaveFile[bit_map_index] = GRAY;
			}
		}
	}
	memcpy(ReadFile, SaveFile, infoheader.biSizeImage);
	printf("영상 레벨분할이 완료됨.\n");
}

void ImageProcess::SaveAsDiff(char* new_file_name) //입력된 영상을 새로운 이름으로 저장해주는 메소드
{
	file = fopen(new_file_name, "wb");
	if (file == NULL)  //파일이 열리지않을 경우 오류처리
	{
		printf("파일이 열리지 않습니다.\n");
		return;
	}
	fwrite(&fileheader, sizeof(BITMAPFILEHEADER), 1, file); //비트맵 파일 헤더 저장
	fwrite(&infoheader, sizeof(BITMAPINFOHEADER), 1, file); //비트맵 정보 헤더 저장
	fwrite(RGB, sizeof(RGBQUAD), 256, file);  //색상테이블 저장
	fwrite(SaveFile, sizeof(BYTE), infoheader.biSizeImage, file); //수정된 픽셀데이터 저장
	fclose(file);
}