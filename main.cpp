#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include <windows.h>
#include "function.h"

int main(){

	bool init = appInit();//程序初始化
	if(init == false) return 0;//初始化失败则中止程序

	int option;//选项
	char filename[255];
	while(1){
		
		printf("1.加密文件\n");
		printf("2.解密文件\n");
		printf("3.退出程序\n");
		printf("请输入你要操作选项的数字并回车继续：");
		scanf("%d",&option);
		fflush(stdin);
	
		switch(option){
			case 1:
				printf("请输入需要加密的文件路径(输入 p 返回):");
				fgets(filename,sizeof(filename),stdin);
				formatInput(filename);


				if(filename[0] == 'p') break;

				if(strlen(filename) >= 254){
					printf("文件路径不能超过255个字节!");
					break;
				}

				printf("文件加密中，请稍后...\n");
				if(!encrypFile(filename)){
					printf("加密失败!");
					break;
				} 
				printf("文件加密完成！\n");
				break;
			case 2:
				printf("请输入需要解密的文件路径(输入 p 返回):");
				fgets(filename,sizeof(filename),stdin);
				formatInput(filename);

				if(filename[0] == 'p') break;

				if(strlen(filename) >= 254){
					printf("文件路径不能超过255个字节!");
					break;
				}

				printf("文件解密中，请稍后...\n");
				if(!decryptFile(filename)){
					printf("解密失败!");
					break;
				}
				printf("文件解密完成！\n");
				break;
			case 3 :
				return 0;
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