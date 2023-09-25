#include "ImageProcess.h"
char format[5] = ".bmp";

int main(void)
{
	char filename[50];   //���ϸ��� �Է¹ޱ����� ����

	while (1)          //����ڸ� ���� �޴�
	{

		printf("8bit BMP �̹��� ������ ó���մϴ�.(����: q!)\n");
		printf("���ϸ��� �Է��ϼ��� >> ");
		scanf("%s", filename);

		if (!strcmp(filename, "q!")) {
			printf("���α׷��� �����մϴ�.\n");
			return 0;
		}

		strcat(filename, format);

		ImageProcess img(filename);  //������ �������� ����

		if (img.file == NULL)  //������ ���� �� ����ó��
		{
			printf("\n������ �������� �ʽ��ϴ�.\n");
			continue;
		}

		if (img.fileheader.bfType != 0x4D42)  //������ ��Ʈ�� ������ �ƴ� �� ���� ó��
		{
			printf("\n������ ��Ʈ���� �ƴմϴ�.\n");
			continue;
		}
		if (img.infoheader.biBitCount != 8 || img.infoheader.biClrUsed != 0)  //������ 8bit�� �ƴϰ� Gray-Scale�� �ƴҶ� ����ó�� (�ε����� �÷������� 8bit�̸鼭 �����ִ� �����̴�. �� ������ ���ٸ� �ε������÷������� �ɷ��������Ѵ�)
		{
			printf("\n������ 8bit ������ �ƴϰų� Gray-scale�� �ƴմϴ�.\n");
			continue;
		}

		if (img.Check_BitmapCompression() == 0)  //������ ������ �Ǿ����������� �˻�
		{
			printf("�� ������ ����Ǿ� ���� �ʽ��ϴ�.\n");
			printf("�������� �Ѿ�ϴ�.");
			if (Submenu(img) == 1) {
				return 0;
			}
		}
		else if (img.Check_BitmapCompression() == 1)  //������ ������ �Ǿ��ִ��� �˻�
		{
			printf("\n�� ������ ����Ǿ� �ֽ��ϴ�.\n");
			printf("���������� �����մϴ�..\n");
			img.BitmapCompression();               //������ ����Ǿ��ִٸ� ���������� ���־����.
			printf("\n�������� �Ѿ�ϴ�.\n");
			if (Submenu(img) == 1) {
				return 0;
			}
		}
		else             //RLE8������ �ƴ� ������������ ����Ǿ��ִٸ� ����ó��
		{
			printf("�� ������ �������� �ʴ� �������� �Դϴ�.");
		}
	}

}

int Submenu(ImageProcess img)
{
	int user_control;
	char new_filename[50];   //���ο� ���ϸ��� �Է� �ޱ� ���� ����

	do                       //����ó���� ���� ��ɵ� ���
	{
		printf("\n���� �����ϱ�\n");
		printf("1. �簢���׸���\n");
		printf("2. �������ϱ�\n");
		printf("3. ��������ϱ�\n");
		printf("4. �¿�����ϱ�\n");
		printf("5. ���Ϲ����ϱ�\n");
		printf("6. �Ӱ�ȭ�ϱ�\n");
		printf("7. ���������ϱ�\n");
		printf("8. �ٸ� �̸����� ����\n");
		printf("9. �ڷΰ���\n");
		printf("��ȣ �Է� >> ");

		scanf("%d", &user_control);
		//������ �������� �޴�����
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
		else if (user_control == 8)       //8���̸� ������ �ٸ� �̸����� ����
		{
			printf("���ο� ���ϸ� �Է� : ");
			scanf("%s", new_filename);
			strcat(new_filename, format);
			img.SaveAsDiff(new_filename);
			printf("���α׷��� �����մϴ�.\n");
			return 1;
		}
		else
		{
			printf("�ٽ� �Է����ּ���.\n");
		}
	} while (user_control != 9);

	return 0;
}
