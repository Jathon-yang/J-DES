#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include "md5.h"
#include "function.h"

char *progress_str = ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";//完成进度字符
char *progress_str2 = "==================================================";//未完成进度字符
char *regname = "Software\\J-DES";//注册表路径

//程序初始化
bool appInit(){
	
	printStartMsg();//打印控制台提示

	HKEY hKEY;
	LONG lRet=RegOpenKeyExA(HKEY_LOCAL_MACHINE,regname,0,KEY_READ,&hKEY);//打开注册表

	if(lRet != ERROR_SUCCESS){//没有相关注册表选项(进行初始化)
		RegCloseKey(hKEY);//关闭注册表
		return confirmPasswordMsg();
		
	}else{//非初始化提示输入密码
		RegCloseKey(hKEY);//关闭注册表
		int error = 3;//允许错误次数
		while(1){			
			if(checkPassword()) return true;
			if(error <= 1){
				printf("对不起，你输入的密码不正确!");
				return false;
			}
			printf("对不起，你输入的密码不正确！还有%d次机会\n",--error);
			
		}
	}
	return true;
}

//密码重置
bool resetPassword(){
	if(checkPassword()){
		return confirmPasswordMsg();
	}
	return false;
}

//加密文件
int encrypFile(char filename[255]){
	MYFILE files;
	char ckey,fBuffer[1],trueFalse[2];
	int index = 0;
	uint64 filesize = fileSize(filename);
	FILE *fSource,*fDest;
	errno_t fDest_err,fSourse_err;
	float milestone = 0.02f;//完成进度

	if(filesize == 0){
		printf("这是一个空文件:%s\n",filename);
		return 0;
	}

	files = pretreatment(filename,1);
	if(files.error == true) return 0;


	fDest_err = fopen_s(&fDest, files.encrypt_file, "r");
	if(fDest_err == 0){//如果文件已经存在时
		fclose(fDest);
		printf("%s 已经存在，是否覆盖？ ",files.encrypt_file);
		fflush(stdin);
		scanf_s("%s",&trueFalse,2);
		if(trueFalse[0] != 'Y' && trueFalse[0] != 'y') return 0;
	}

	fDest_err = fopen_s(&fDest, files.encrypt_file, "wb");
	fSourse_err = fopen_s(&fSource, files.original_file, "rb");


	if(fSourse_err != 0){
		printf("无法打开原文件: %s\n",files.original_file);
		return 0;
	}

	if(fDest_err != 0){
		printf("无法生成加密文件：%s\n",files.encrypt_file);
		return 0;
	}

	//计算开始时间
	time_t difftime,starttime;
	starttime = time(0);
	
	fwrite(files.ext_encryp_str, 1, EXT_STORAGE_LEN, fDest); // 写入文件
	
	unsigned int key_len = strlen(DATA_KEY);//获取密钥长度

	while(!feof(fSource)){ 
        fread(fBuffer, 1, 1, fSource);    // 读取1字节  
        if(!feof(fSource)){
            ckey = DATA_KEY[index%key_len];     // 循环获取key  
            *fBuffer = *fBuffer ^ ckey;   // xor encrypt  
            fwrite(fBuffer, 1, 1, fDest); // 写入文件

			index ++; 

			if(((1.0*index)/filesize) >= milestone){
				showProgress((int)(milestone*100));
				milestone += 0.02f;
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
	char ckey,fBuffer[1],trueFalse[2];
	int index = 0,ext_index = 0;
	FILE *fSource,*fDest;
	uint64 filesize = fileSize(filename);
	MYFILE files;
	float milestone = 0.02f;

	errno_t fSource_err,fDest_err;

	if(filesize == 0){
		printf("这是一个空文件:%s\n",filename);
		return 0;
	}

	files = pretreatment(filename,2);
	if(files.error == true) return 0;

	fDest_err = fopen_s(&fDest, files.original_file, "r");
	if(fDest_err == 0){//如果文件已经存在时
		fclose(fDest);
		printf("%s 已经存在，是否覆盖？ ",files.original_file);
		fflush(stdin);
		scanf_s("%s",&trueFalse,2);
		if(trueFalse[0] != 'Y' && trueFalse[0] != 'y') return 0;
	}


	fSource_err = fopen_s(&fSource, files.encrypt_file, "rb");//读取加密文件
	fDest_err = fopen_s(&fDest, files.original_file, "wb");
	if(fSource_err != 0){
		printf("无法打开加密文件: %s\n",files.encrypt_file);
		return 0;
	}

	if(fDest_err != 0){
		printf("无法生成解密文件：%s\n",files.original_file);
		return 0;
	}

	//计算开始时间
	time_t difftime,starttime;
	starttime = time(0);

	unsigned int key_len = strlen(DATA_KEY);//获取密钥长度

	fseek(fSource, EXT_STORAGE_LEN, 0);//将文件指针移动到第32位
	while(!feof(fSource)){ 
		fread(fBuffer, 1, 1, fSource);    // 读取1字节 
		if(!feof(fSource)){//防止尾部被追加NUL
			ckey = DATA_KEY[index%key_len];     // 循环获取key  
			*fBuffer = *fBuffer ^ ckey;   // xor encrypt  
			fwrite(fBuffer, 1, 1, fDest); // 写入文件  
			index ++; 

			if(((1.0*index)/filesize) >= milestone){//显示进度
				showProgress((int)(milestone*100));
				milestone += 0.02f;
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
	unsigned int ext_key_len = strlen(EXT_KEY);//计算后缀密钥长度
	
	MYFILE result = {'\0','\0','\0','\0',false};

	if(mode == 1){//加密
		_splitpath_s( filename, drive, dir, fname, ext);
		_makepath_s( newfilename, drive, dir, fname, ".je");

		int ext_len = strlen(ext);
		char ext_encryp_str[EXT_STORAGE_LEN];

		for(int i=0;i<EXT_STORAGE_LEN;i++){
			if(i < ext_len){
				ext_encryp_str[i] = ext[i] ^ EXT_KEY[i%ext_key_len];
			}else if(i == ext_len){
				ext_encryp_str[i] = '#' ^ EXT_KEY[i%ext_key_len];
			}else{
				ext_encryp_str[i] = '\0';
				break;
			}
		}


		//为结构体属性赋值
		strcpy_s(result.encrypt_file,newfilename);
		strcpy_s(result.original_file,filename);
		strcpy_s(result.ext_encryp_str,ext_encryp_str);
		result.ext = ext;
		
	}else{//解密
		char fBuffer[1];
		int ext_index = 0;
		FILE *fSource;
		errno_t fSource_err;
		
		char original_ext[EXT_STORAGE_LEN];//用于存放解密出来的后缀

		fSource_err = fopen_s(&fSource, filename, "rb");//读取加密文件
		if(fSource == NULL){
			printf("无法打开加密文件: %s\n",filename);
			return result;
		}

		bool has_normal = false;
		while(!feof(fSource)){
			fread(fBuffer, 1, 1, fSource);// 读取1字节 
			if(ext_index >= EXT_STORAGE_LEN) break;
		
			*fBuffer = *fBuffer ^ EXT_KEY[ext_index%ext_key_len];   // xor encrypt
			
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

	
		_splitpath_s( filename, drive, dir, fname, ext);



		//校验加密文件是否合法
		if(has_normal == false || strcmp(ext,".je") != 0){
			printf("不是一个合法的加密文件: %s\n",filename);
			result.error = true;
			return result;
		}

		strcat_s(fname,"-je");
		_makepath_s( newfilename, drive, dir, fname, original_ext);

		strcpy_s(result.original_file,newfilename);
		result.ext = original_ext;
		strcpy_s(result.encrypt_file,filename);
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
	errno_t fSource_err;
	fSource_err = fopen_s(&fSource, filename, "rb");
	if(fSource_err != 0) return 0;
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

//设置哈希密码
void haltPassword(char pw[16],char halt[11],char *decrypt_str){
	char halt_pass[40] = {0};//多个字符合并缓冲区
	
	strcpy_s(halt_pass,halt);//连接字符
	strcat_s(halt_pass,pw);//连接字符
	strcat_s(halt_pass,PASS_HALT);//连接字符

	md5(pw,decrypt_str);
}

//确认密码并修改
bool confirmPasswordMsg(){
	char password[17]={0},pw2[17];
	printf("第一次进入，需要设置一下初始密码。\n");

	bool loop = true;
	while(loop){
		printf("请输入你的密码：");
		fgetPassword(password,16);//获取输入的密码
		if(*password == '\0'){
			printf("密码不能为空！\n");
			continue;
		}
		if(strlen(password) > 16){
			printf("密码长度不能超过16个字符！\n");
			continue;
		}
		printf("请再次输入你的密码：");
		fgetPassword(pw2,16);//获取输入的密码
		if(strcmp(password, pw2) != 0){
			printf("两次密码不一致！\n");
			continue;
		}
		loop = false;
	}
	
	return editPassword(password);
}

//判断密码是否正确
bool checkPassword(){
	HKEY hKEY;

	LONG lRet=RegOpenKeyExA(HKEY_LOCAL_MACHINE,regname,0,KEY_READ,&hKEY);

	char in_pass[17];//输入密码缓冲区

	char password[33] = {0};//注册表数据缓冲区
	char halt[11] = {0};//注册表数据缓冲区
	char decrypt_str[33] = {0};//哈希密码缓冲区

	DWORD dwSize,haltSize;

	dwSize=sizeof(password);
	haltSize=sizeof(halt);

	lRet=RegQueryValueExA(hKEY,"password",NULL,NULL,(LPBYTE)password,&dwSize);
	lRet=RegQueryValueExA(hKEY,"addtime",NULL,NULL,(LPBYTE)halt,&haltSize);
	RegCloseKey(hKEY);//关闭注册表

	if(*password == '\0'){
		printf("对不起，程序数据损坏，请重新设置您的密码！\n");
		return confirmPasswordMsg();
	}

	printf("请输入密码：");
	fgetPassword(in_pass,16);//获取输入的密码
	if(strlen(in_pass) > 16){
		printf("密码长度不能超过16个字符！\n");
		return false;
	}

	haltPassword(in_pass,halt,decrypt_str);
	if(strcmp(decrypt_str, password) == 0) return true;
	return false;
}

//设置密码
bool editPassword(char pw[16]){
	if(strlen(PASS_HALT) > 10){
		printf("密码加盐值不允许超过10个字符");
		return false;
	}

	HKEY hKEY;
	char *lpszValueName = "password";
	DWORD dwDisposition = REG_OPENED_EXISTING_KEY;

	LONG lRet = RegCreateKeyExA(HKEY_LOCAL_MACHINE, regname, NULL, NULL, REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS, NULL, &hKEY, &dwDisposition); 
	if(lRet != ERROR_SUCCESS){
		printf("无权限打开注册表，请检查是否以管理员运行！\n");
		return false;
	}

	char int_to_string[11] = {0};//整形转字符缓冲区
	char decrypt_str[33] = {0};//哈希密码缓冲区
	time_t addtime = time(0);//时间戳(10位)
	_itoa_s((unsigned)addtime,int_to_string,10);//将整形转化成字符

	haltPassword(pw,int_to_string,decrypt_str);

	lRet = ::RegSetValueExA(hKEY, lpszValueName, NULL, REG_SZ, (BYTE*)decrypt_str, strlen(decrypt_str)+1);
	lRet = ::RegSetValueExA(hKEY, "addtime", NULL, REG_SZ, (BYTE*)int_to_string, strlen(int_to_string)+1);
	if(ERROR_SUCCESS != lRet){  
        printf("无权限打开注册表，请检查是否以管理员运行！");
		return false; 
    }
	RegCloseKey(hKEY);
	return true;
}

//获取用户输入的密码
void fgetPassword(char *password,int pass_len){
	int i = 0;
	char ch = '*';
	while(i <= pass_len && (password[i] = _getch()) != '\r'){

		if(i >= pass_len && password[i] != '\b') continue;//超过长度不允许输入(删除除外)
			
		if(password[i]=='\b'){//删除时回退
			if(i <= 0) continue;
			putchar('\b');
			putchar(' ');
			putchar('\b');
			i--;
		}else{//写入星号
			putchar(ch);
			i++;
		}
     }
    password[i]='\0';//补全字符串
	printf("\n");//打印换行
}