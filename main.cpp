#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include "function.h"


int main(){
	int option;
	int in_pass,password = 123456;

	int error = 3;
	printStartMsg();

	while(1){
		printf("������6λ���룺");
		scanf("%d",&in_pass);

		if(password == in_pass) break;
			
		printf("�Բ�������������벻��ȷ������%d�λ���\n",--error);
		
		if(error < 1) return 0;
		
	}
	
	while(1){
		printf("��������Ҫ����ѡ������ֲ��س�������\n");
		printf("1.�����ļ�\n");
		printf("2.�����ļ�\n");
		scanf("%d",&option);
		//option = 2;

		char filename[255];
	
		printf("��������Ҫ��/���ܵ��ļ�·��:");
		scanf("%s",&filename);

		if(strlen(filename) > 255){
			printf("�ļ�·�����ܳ���255���ֽ�!");
			return 0;
		}

	
		switch(option){
			case 1:
				printf("�ļ������У����Ժ�...\n");
				if(!encrypFile(filename)){
					printf("����ʧ��!");
					break;
				} 
				printf("�ļ�������ɣ�\n");
				break;
			case 2:
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