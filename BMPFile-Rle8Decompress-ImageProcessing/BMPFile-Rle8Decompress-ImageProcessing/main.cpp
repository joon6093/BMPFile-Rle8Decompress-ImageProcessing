#include "ImageProcess.h"
char format[5] = ".bmp";

int main(void)
{
	char filename[50];   //파일명을 입력받기위한 변수

	while (1)          //사용자를 위한 메뉴
	{

		printf("8bit BMP 이미지 파일을 처리합니다.(종료: q!)\n");
		printf("파일명을 입력하세요 >> ");
		scanf("%s", filename);

		if (!strcmp(filename, "q!")) {
			printf("프로그램을 종료합니다.\n");
			return 0;
		}

		strcat(filename, format);

		ImageProcess img(filename);  //영상의 정보들을 들고옴

		if (img.file == NULL)  //영상이 없을 때 오류처리
		{
			printf("\n파일이 존재하지 않습니다.\n");
			continue;
		}

		if (img.fileheader.bfType != 0x4D42)  //영상이 비트맵 형식이 아닐 때 오류 처리
		{
			printf("\n파일이 비트맵이 아닙니다.\n");
			continue;
		}
		if (img.infoheader.biBitCount != 8 || img.infoheader.biClrUsed != 0)  //영상이 8bit가 아니고 Gray-Scale이 아닐때 오류처리 (인덱스드 컬러영상은 8bit이면서 색이있는 영상이다. 이 오류가 없다면 인덱스드컬러영상을 걸러주지못한다)
		{
			printf("\n파일이 8bit 형식이 아니거나 Gray-scale이 아닙니다.\n");
			continue;
		}

		if (img.Check_BitmapCompression() == 0)  //영상이 압축이 되어있지않은지 검사
		{
			printf("이 파일은 압축되어 있지 않습니다.\n");
			printf("수정모드로 넘어갑니다.");
			if (Submenu(img) == 1) {
				return 0;
			}
		}
		else if (img.Check_BitmapCompression() == 1)  //영상이 압축이 되어있는지 검사
		{
			printf("\n이 파일은 압축되어 있습니다.\n");
			printf("압축해제를 진행합니다..\n");
			img.BitmapCompression();               //영상이 압축되어있다면 압축해제를 해주어야함.
			printf("\n수정모드로 넘어갑니다.\n");
			if (Submenu(img) == 1) {
				return 0;
			}
		}
		else             //RLE8형식이 아닌 압축형식으로 압축되어있다면 오류처리
		{
			printf("이 파일은 지원하지 않는 압축형식 입니다.");
		}
	}

}

int Submenu(ImageProcess img)
{
	int user_control;
	char new_filename[50];   //새로운 파일명을 입력 받기 위한 변수

	do                       //영상처리를 위한 기능들 출력
	{
		printf("\n파일 수정하기\n");
		printf("1. 사각형그리기\n");
		printf("2. 밝기반전하기\n");
		printf("3. 밝기조절하기\n");
		printf("4. 좌우반전하기\n");
		printf("5. 상하반전하기\n");
		printf("6. 임계화하기\n");
		printf("7. 레벨분할하기\n");
		printf("8. 다른 이름으로 저장\n");
		printf("9. 뒤로가기\n");
		printf("번호 입력 >> ");

		scanf("%d", &user_control);
		//각각의 모듈실행은 메뉴참고
		if (user_control == 1)
		{
			img.SqareDraw();
		}
		else if (user_control == 2)
		{
			img.InversionColor();
		}
		else if (user_control == 3)
		{
			img.Bright();
		}
		else if (user_control == 4)
		{
			img.XFlip();
		}
		else if (user_control == 5)
		{
			img.YFlip();
		}
		else if (user_control == 6)
		{
			img.Thresholding();
		}
		else if (user_control == 7)
		{
			img.Slicing();
		}
		else if (user_control == 8)       //8번이면 영상을 다른 이름으로 저장
		{
			printf("새로운 파일명 입력 : ");
			scanf("%s", new_filename);
			strcat(new_filename, format);
			img.SaveAsDiff(new_filename);
			printf("프로그램을 종료합니다.\n");
			return 1;
		}
		else
		{
			printf("다시 입력해주세요.\n");
		}
	} while (user_control != 9);

	return 0;
}
