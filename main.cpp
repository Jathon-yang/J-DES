#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include <windows.h>
#include "function.h"

int main(){

	bool init = appInit();//�����ʼ��
	if(init == false) return 0;//��ʼ��ʧ������ֹ����

	int option;//ѡ��
	char filename[255];
	while(1){
		
		printf("1.�����ļ�\n");
		printf("2.�����ļ�\n");
		printf("3.�˳�����\n");
		printf("��������Ҫ����ѡ������ֲ��س�������");
		scanf("%d",&option);
		fflush(stdin);
	
		switch(option){
			case 1:
				printf("��������Ҫ���ܵ��ļ�·��(���� p ����):");
				fgets(filename,sizeof(filename),stdin);
				formatInput(filename);


				if(filename[0] == 'p') break;

				if(strlen(filename) >= 254){
					printf("�ļ�·�����ܳ���255���ֽ�!");
					break;
				}

				printf("�ļ������У����Ժ�...\n");
				if(!encrypFile(filename)){
					printf("����ʧ��!");
					break;
				} 
				printf("�ļ�������ɣ�\n");
				break;
			case 2:
				printf("��������Ҫ���ܵ��ļ�·��(���� p ����):");
				fgets(filename,sizeof(filename),stdin);
				formatInput(filename);

				if(filename[0] == 'p') break;

				if(strlen(filename) >= 254){
					printf("�ļ�·�����ܳ���255���ֽ�!");
					break;
				}

				printf("�ļ������У����Ժ�...\n");
				if(!decryptFile(filename)){
					printf("����ʧ��!");
					break;
				}
				printf("�ļ�������ɣ�\n");
				break;
			case 3 :
				return 0;
				break;
			default :
				printf("����ѡ��Ƿ���\n");
				break;
		}

		printf("\n#########################################################################\n\n");
	}
	

	scanf("%s");
	return 0;
	
}