#include<stdio.h>
#include<stdlib.h>
#include <windows.h>
#include<string.h>
#define NEW_WIDTH(bits) (((bits)+31)/32*4)  //영상을 4의배수크기로 읽어오기위해 사용한 공식 선언
#define GRAY 170;        //사용자가 그려준 사각형의 색 지정
#pragma warning(disable:4996)

class ImageProcess {
public:
	FILE* file;    //영상을 담는 멤버변수
	BITMAPFILEHEADER fileheader;  //비트맵 파일헤더를 담는 멤버변수
	BITMAPINFOHEADER infoheader;  //비트맵 정보헤더를 담는 멤버변수
	RGBQUAD* RGB;    //비트맵 색상테이블을 담는 멤버변수
	BYTE* ReadFile;   //비트맵 픽셀데이터를 담는 멤버변수
	BYTE* SaveFile;  //수정된 비트맵 픽셀데이터를 저장하는 멤버변수
	bool* Sqare_index;  //사각형 그리기 기능 수행 후 사각형에대한 위치정보를 저장하는 멤버변수
	bool* Sqare_index_temp;  //사각형의 위치 변환을 위해 필요한 복사본을 저장할 멤버변수
public:
	ImageProcess(char* filename);   //영상데이터정보를 들고오는 메소드
	int Check_BitmapCompression();  //영상의 압축여부를 확인해주는 메소드
	void BitmapCompression();    //RLE 파일 압축 해제 기능을 수행하는 메소드
	void InversionColor();  //입력된 영상의 밝기를 반전한 결과를 제공해주는 메소드
	void SqareDraw();  //입력된 영상에 사각형을 그려주는 메소드
	void Bright();   //입력된 영상을 밝기 조절을 해주는 메소드
	void YFlip();  //입력된 영상을 상하반전해주는 메소드
	void XFlip();  //입력된 영상을 좌우반전해주는 메소드
	void Thresholding();  //입력된 영상을 임계화해주는 메소드
	void Slicing();   //입력된 영상을 레벨분할해주는 메소드
	void SaveAsDiff(char* new_file_name);  //입력된 영상을 새로운 이름으로 저장해주는 메소드
};
int Submenu(ImageProcess img);  //영상처리 메뉴