#include<stdio.h>
#include<stdlib.h>
#include <windows.h>
#include<string.h>
#define NEW_WIDTH(bits) (((bits)+31)/32*4)  //������ 4�ǹ��ũ��� �о�������� ����� ���� ����
#define GRAY 170;        //����ڰ� �׷��� �簢���� �� ����
#pragma warning(disable:4996)

class ImageProcess {
public:
	FILE* file;    //������ ��� �������
	BITMAPFILEHEADER fileheader;  //��Ʈ�� ��������� ��� �������
	BITMAPINFOHEADER infoheader;  //��Ʈ�� ��������� ��� �������
	RGBQUAD* RGB;    //��Ʈ�� �������̺��� ��� �������
	BYTE* ReadFile;   //��Ʈ�� �ȼ������͸� ��� �������
	BYTE* SaveFile;  //������ ��Ʈ�� �ȼ������͸� �����ϴ� �������
	bool* Sqare_index;  //�簢�� �׸��� ��� ���� �� �簢�������� ��ġ������ �����ϴ� �������
	bool* Sqare_index_temp;  //�簢���� ��ġ ��ȯ�� ���� �ʿ��� ���纻�� ������ �������
public:
	ImageProcess(char* filename);   //�������������� ������ �޼ҵ�
	int Check_BitmapCompression();  //������ ���࿩�θ� Ȯ�����ִ� �޼ҵ�
	void BitmapCompression();    //RLE ���� ���� ���� ����� �����ϴ� �޼ҵ�
	void InversionColor();  //�Էµ� ������ ��⸦ ������ ����� �������ִ� �޼ҵ�
	void SqareDraw();  //�Էµ� ���� �簢���� �׷��ִ� �޼ҵ�
	void Bright();   //�Էµ� ������ ��� ������ ���ִ� �޼ҵ�
	void YFlip();  //�Էµ� ������ ���Ϲ������ִ� �޼ҵ�
	void XFlip();  //�Էµ� ������ �¿�������ִ� �޼ҵ�
	void Thresholding();  //�Էµ� ������ �Ӱ�ȭ���ִ� �޼ҵ�
	void Slicing();   //�Էµ� ������ �����������ִ� �޼ҵ�
	void SaveAsDiff(char* new_file_name);  //�Էµ� ������ ���ο� �̸����� �������ִ� �޼ҵ�
};
int Submenu(ImageProcess img);  //����ó�� �޴�