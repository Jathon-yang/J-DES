#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "md5.h"
#include "function.h"

char *progress_str = ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
char *progress_str2 = "==================================================";

//程序初始化
bool appInit(){
	printStartMsg();

	HKEY hKEY;
	char *regname = "Software\\J-DES";

	LONG lRet=RegOpenKeyExA(HKEY_LOCAL_MACHINE,regname,0,KEY_READ,&hKEY);
	if(lRet != ERROR_SUCCESS){//没有相关注册表选项(进行初始化)
		RegCloseKey(hKEY);//关闭注册表

		char pw1[17],pw2[17];
		printf("第一次进入，需要设置一下初始密码。\n");
		printf("请输入你的密码：");
		fgets(pw1,sizeof(pw1),stdin);
		formatInput(pw1);
		if(strlen(pw1) > 16){
			printf("密码长度不能超过16个字符！\n");
			return false;
		}
		printf("请再次输入你的密码：");
		fgets(pw2,sizeof(pw2),stdin);
		formatInput(pw2);
		if(strcmp(pw1, pw2) != 0){
			printf("两次密码不一致！\n");
			return false;
		}

		return editPassword(pw1);
	}else{//非初始化提示输入密码
		char in_pass[17];
		int error = 3;

		char password[33] = {0};
		char decrypt_str[33] = {0}; 
	
		DWORD dwSize;

		dwSize=sizeof(password);
		lRet=RegQueryValueExA(hKEY,"password",NULL,NULL,(LPBYTE)password,&dwSize);
		RegCloseKey(hKEY);//关闭注册表

		while(1){
			printf("请输入密码：");
			fgets(in_pass,sizeof(in_pass),stdin);
			formatInput(in_pass);
			if(strlen(in_pass) > 16){
				printf("密码长度不能超过16个字符！\n");
				return false;
			}

			md5(in_pass,decrypt_str);
			if(strcmp(decrypt_str, password) == 0) return true;
			
			printf("对不起，你输入的密码不正确！还有%d次机会\n",--error);
		
			if(error < 1) return false;
		}
	}
	return true;
}

//设置密码
bool editPassword(char pw[16]){
	HKEY hKEY;
	char *regname = "Software\\J-DES";
	char *lpszValueName = "password";
	DWORD dwDisposition = REG_OPENED_EXISTING_KEY;

	LONG lRet = RegCreateKeyExA(HKEY_LOCAL_MACHINE, regname, NULL, NULL, REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS, NULL, &hKEY, &dwDisposition); 
	if(lRet != ERROR_SUCCESS){
		printf("无权限打开注册表，请检查是否以管理员运行！\n");
		return false;
	}

	char decrypt_str[33] = {0}; 
	md5(pw,decrypt_str);

	lRet = ::RegSetValueExA(hKEY, lpszValueName, NULL, REG_SZ, (BYTE*)decrypt_str, strlen(decrypt_str)+1);
	if(ERROR_SUCCESS != lRet){  
        printf("无权限打开注册表，请检查是否以管理员运行！");
		return false; 
    }
	RegCloseKey(hKEY);
	return true;
}

//加密文件
int encrypFile(char filename[255]){
	MYFILE files;
	char *source,*key,ckey,fBuffer[1],trueFalse[2];
	int index = 0;
	uint64 filesize = fileSize(filename);
	FILE *fSource,*fDest;
	float milestone = 0.02;//完成进度

	if(filesize == 0){
		printf("这是一个空文件:%s\n",filename);
		return 0;
	}

	files = pretreatment(filename,1);
	if(files.error == true) return 0;

	fDest = fopen(files.encrypt_file, "r");
	if(fDest != NULL){//如果文件已经存在时
		fclose(fDest);
		printf("%s 已经存在，是否覆盖？ ",files.encrypt_file);
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

	//计算开始时间
	time_t difftime,starttime;
	starttime = time(0);
	
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
				milestone += 0.02;
			}
             
        }
    }
	printf("[100%%]%s\n\n",progress_str);
    fclose(fSource);
	fclose(fDest);

	difftime = time(0) - starttime;
	printf("本次共耗时%d秒\n",(difftime == 0 ? 1 : difftime));
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
	float milestone = 0.02;

	if(filesize == 0){
		printf("这是一个空文件:%s\n",filename);
		return 0;
	}

	files = pretreatment(filename,2);
	if(files.error == true) return 0;

	fDest = fopen(files.original_file, "r");
	if(fDest != NULL){//如果文件已经存在时
		fclose(fDest);
		printf("%s 已经存在，是否覆盖？ ",files.original_file);
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

	//计算开始时间
	time_t difftime,starttime;
	starttime = time(0);

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
				milestone += 0.02;
			}
		}
    }

	printf("[100%%]%s\n\n",progress_str);
	fclose(fSource);
	fclose(fDest);

	difftime = time(0) - starttime;
	printf("本次共耗时%d秒\n",(difftime == 0 ? 1 : difftime));
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

		bool has_normal = false;
		while(!feof(fSource)){
			fread(fBuffer, 1, 1, fSource);// 读取1字节 
			if(ext_index >= EXT_STORAGE_LEN) break;
		
			*fBuffer = *fBuffer ^ ext_key[ext_index%10];   // xor encrypt
			
			if(*fBuffer == '#'){//遇到#号表示后缀结束
				original_ext[ext_index] = '\0';
				break;
			}

			//第一个字符如果是点的话就合法(不严谨待优化)
			if(ext_index == 0 && *fBuffer == '.') has_normal = true;

			original_ext[ext_index] = *fBuffer;//组装后缀
			ext_index++;
		}
		fclose(fSource);

	
		_splitpath( filename, drive, dir, fname, ext);



		//校验加密文件是否合法
		if(has_normal == false || strcmp(ext,".je") != 0){
			printf("不是一个合法的加密文件: %s\n",filename);
			result.error = true;
			return result;
		}

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
	fSource = fopen(filename, "rb");
	if(fSource == NULL) return 0;
	fseek(fSource,0,2);
	
	filesize = ftell(fSource);
	fclose(fSource);
	return filesize;
}

//打印进度
void showProgress(int progress){
	printf("[%d%%]%s%s\r",progress,progress_str+((100-progress)/2),progress_str2+(progress/2));
	fflush(stdout);
}

//格式化输入值
void formatInput(char *input){
	if(input[strlen(input)-1] == '\n')
		input[strlen(input)-1] = '\0'; 
}