#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include "function.h"

int main(){
	int option;
	int in_pass,password = 123456;

	int error = 3;
	printStartMsg();

	/*while(1){
		printf("������6λ���룺");
		scanf("%d",&in_pass);

		if(password == in_pass) break;
			
		printf("�Բ�������������벻��ȷ������%d�λ���\n",--error);
		
		if(error < 1) return 0;
		
	}*/
	
	char filename[255];
	while(1){
		
		printf("1.�����ļ�\n");
		printf("2.�����ļ�\n");
		printf("3.�˳�����\n");
		printf("��������Ҫ����ѡ������ֲ��س�������");
		scanf("%d",&option);
		fflush(stdin);
		//option = 1;


	

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
			default :
				printf("����ѡ��Ƿ���\n");
				break;
		}

		printf("\n#########################################################################\n\n");
	}
	

	scanf("%s");
	return 0;
	
}