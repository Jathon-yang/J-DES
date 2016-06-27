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
		printf("请输入6位密码：");
		scanf("%d",&in_pass);

		if(password == in_pass) break;
			
		printf("对不起，你输入的密码不正确！还有%d次机会\n",--error);
		
		if(error < 1) return 0;
		
	}
	
	while(1){
		printf("请输入你要操作选项的数字并回车继续：\n");
		printf("1.加密文件\n");
		printf("2.解密文件\n");
		scanf("%d",&option);
		//option = 2;

		char filename[255];
	
		printf("请输入需要加/解密的文件路径:");
		scanf("%s",&filename);

		if(strlen(filename) > 255){
			printf("文件路径不能超过255个字节!");
			return 0;
		}

	
		switch(option){
			case 1:
				printf("文件加密中，请稍后...\n");
				if(!encrypFile(filename)){
					printf("加密失败!");
					break;
				} 
				printf("文件加密完成！\n");
				break;
			case 2:
				printf("文件解密中，请稍后...\n");
				if(!decryptFile(filename)){
					printf("解密失败!");
					break;
				}
				printf("文件解密完成！\n");
				break;
			default :
				printf("输入选项非法！\n");
				break;
		}

		printf("\n#########################################################################\n\n");
	}
	

	scanf("%s");
	return 0;
	
}