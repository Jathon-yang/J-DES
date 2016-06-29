#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.h"

//加密文件
int encrypFile(char *filename){
	MYFILE files;
	char *source,*key,ckey,fBuffer[1],trueFalse[2];
	int index = 0;
	uint64 filesize = fileSize(filename);
	FILE *fSource,*fDest;
	float milestone = 0.01;//完成进度

	files = pretreatment(filename,1);

	fDest = fopen(files.encrypt_file, "r");
	if(fDest != NULL){//如果文件已经存在时
		fclose(fDest);
		printf("%s 已经存在，是否覆盖？\n",files.encrypt_file);
		scanf("%s",&trueFalse);
		if(trueFalse[0] != 'Y' && trueFalse[0] != 'y') return 0;
	}

	fDest = fopen(files.encrypt_file, "wb");
	fSource = fopen(files.original_file, "rb");


	if(fSource == NULL){
		printf("无法打开原文件: %s\n",files.original_file);
		return 0;
	}

	if(fDest == NULL){
		printf("无法生成加密文件：%s\n",files.encrypt_file);
		return 0;
	}
	
	fwrite(files.ext_encryp_str, 1, EXT_STORAGE_LEN, fDest); // 写入文件
	
	key = DATA_KEY;

	while(!feof(fSource)){ 
        fread(fBuffer, 1, 1, fSource);    // 读取1字节  
        if(!feof(fSource)){
            ckey = key[index%7];     // 循环获取key  
            *fBuffer = *fBuffer ^ ckey;   // xor encrypt  
            fwrite(fBuffer, 1, 1, fDest); // 写入文件

			index ++; 

			if(((1.0*index)/filesize) >= milestone){
				showProgress(milestone*100);
				milestone += 0.01;
			}
             
        }
    }
	printf("\n");
    fclose(fSource);
	fclose(fDest);
	printf("加密文件的路径为：%s\n",files.encrypt_file);
	return 1;
}

/**
 * 解密文件
 */
char decryptFile(char *filename){
	char *source,ckey,fBuffer[1],trueFalse[2],*key = DATA_KEY;//文件加密KEY;
	int index = 0,ext_index = 0;
	FILE *fSource,*fDest;
	uint64 filesize = fileSize(filename);
	MYFILE files;
	float milestone = 0.01;

	files = pretreatment(filename,2);

	fDest = fopen(files.original_file, "r");
	if(fDest != NULL){//如果文件已经存在时
		fclose(fDest);
		printf("%s 已经存在，是否覆盖？\n",files.original_file);
		scanf("%s",&trueFalse);
		if(trueFalse[0] != 'Y' && trueFalse[0] != 'y') return 0;
	}


	fSource = fopen(files.encrypt_file, "rb");//读取加密文件
	fDest = fopen(files.original_file, "wb");
	if(fSource == NULL){
		printf("无法打开加密文件: %s\n",files.encrypt_file);
		return 0;
	}

	if(fDest == NULL){
		printf("无法生成解密文件：%s\n",files.original_file);
		return 0;
	}

	fseek(fSource, EXT_STORAGE_LEN, 0);//将文件指针移动到第32位
	while(!feof(fSource)){ 
		fread(fBuffer, 1, 1, fSource);    // 读取1字节 
		if(!feof(fSource)){//防止尾部被追加NUL
			ckey = key[index%7];     // 循环获取key  
			*fBuffer = *fBuffer ^ ckey;   // xor encrypt  
			fwrite(fBuffer, 1, 1, fDest); // 写入文件  
			index ++; 

			if(((1.0*index)/filesize) >= milestone){//显示进度
				showProgress(milestone*100);
				milestone += 0.01;
			}
		}
    }
	printf("\n");
	fclose(fSource);
	fclose(fDest);

	printf("解密文件的路径为：%s\n",files.original_file);
	return 1;

}

//获取加密文件的路径
MYFILE pretreatment(char *filename,int mode){
	char drive[_MAX_DRIVE];

	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char newfilename[255];
	char ext[_MAX_EXT];
	char *ext_key = EXT_KEY;//后缀密钥
	
	MYFILE result;

	if(mode == 1){//加密
		_splitpath( filename, drive, dir, fname, ext);
		_makepath( newfilename, drive, dir, fname, ".je");

		int ext_len = strlen(ext);
		char ext_encryp_str[EXT_STORAGE_LEN];

		for(int i=0;i<EXT_STORAGE_LEN;i++){
			if(i < ext_len){
				ext_encryp_str[i] = ext[i] ^ ext_key[i%10];
			}else if(i == ext_len){
				ext_encryp_str[i] = '#' ^ ext_key[i%10];
			}else{
				ext_encryp_str[i] = '\0';
				break;
			}
		}


		//为结构体属性赋值
		strcpy(result.encrypt_file,newfilename);
		strcpy(result.original_file,filename);
		strcpy(result.ext_encryp_str,ext_encryp_str);
		result.ext = ext;
		
	}else{//解密
		char fBuffer[1];
		int ext_index = 0;
		FILE *fSource;
		
		char original_ext[EXT_STORAGE_LEN];//用于存放解密出来的后缀

		fSource = fopen(filename, "rb");//读取加密文件
		if(fSource == NULL){
			printf("无法打开加密文件: %s\n",filename);
			return result;
		}

		while(!feof(fSource)){
			fread(fBuffer, 1, 1, fSource);// 读取1字节 
			if(ext_index >= EXT_STORAGE_LEN) break;
		
			*fBuffer = *fBuffer ^ ext_key[ext_index%10];   // xor encrypt
			
			if(*fBuffer == '#'){//遇到#号表示后缀结束
				original_ext[ext_index] = '\0';
				break;
			}

			original_ext[ext_index] = *fBuffer;//组装后缀
			ext_index++;
		}
		fclose(fSource);

		_splitpath( filename, drive, dir, fname, ext);
		strcat(fname,"-je");
		_makepath( newfilename, drive, dir, fname, original_ext);

		strcpy(result.original_file,newfilename);
		result.ext = original_ext;
		strcpy(result.encrypt_file,filename);
	}
	
	return result;
}

void printStartMsg(){
	printf("***************** 文本加密解密软件 *****************\n");
	printf("*                                                  *\n");
	printf("*      **************************************      *\n");
	printf("*      *                                    *      *\n");
	printf("*      *  Author：@Jathon                   *      *\n");
	printf("*      *                                    *      *\n");
	printf("*      **************************************      *\n");
	printf("*                                                  *\n");
	printf("****************************************************\n");
}

//获取文件大小 
uint64 fileSize(char filename[255]){
	FILE *fSource;
	uint64 filesize;
	fSource = fopen(filename, "r");

	fseek(fSource,0,2);
	
	filesize = ftell(fSource);
	fclose(fSource);
	return filesize;
}

//打印进度
void showProgress(int progress){
	char *str = "####################################################################################################";
	char *str2 = "----------------------------------------------------------------------------------------------------";
	printf("[%d%%]%s%s\r",progress,str+(100-progress),str2+progress);
	fflush(stdout);
}
				