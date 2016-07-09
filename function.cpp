#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include "md5.h"
#include "function.h"

char *progress_str = ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";//��ɽ����ַ�
char *progress_str2 = "==================================================";//δ��ɽ����ַ�
char *regname = "Software\\J-DES";//ע���·��

//�����ʼ��
bool appInit(){
	
	printStartMsg();//��ӡ����̨��ʾ

	HKEY hKEY;
	LONG lRet=RegOpenKeyExA(HKEY_LOCAL_MACHINE,regname,0,KEY_READ,&hKEY);//��ע���

	if(lRet != ERROR_SUCCESS){//û�����ע���ѡ��(���г�ʼ��)
		RegCloseKey(hKEY);//�ر�ע���
		return confirmPasswordMsg();
		
	}else{//�ǳ�ʼ����ʾ��������
		RegCloseKey(hKEY);//�ر�ע���
		int error = 3;//����������
		while(1){			
			if(checkPassword()) return true;
			if(error <= 1){
				printf("�Բ�������������벻��ȷ!");
				return false;
			}
			printf("�Բ�������������벻��ȷ������%d�λ���\n",--error);
			
		}
	}
	return true;
}

//��������
bool resetPassword(){
	if(checkPassword()){
		return confirmPasswordMsg();
	}
	return false;
}

//�����ļ�
int encrypFile(char filename[255]){
	MYFILE files;
	char ckey,fBuffer[1],trueFalse[2];
	int index = 0;
	uint64 filesize = fileSize(filename);
	FILE *fSource,*fDest;
	errno_t fDest_err,fSourse_err;
	float milestone = 0.02f;//��ɽ���

	if(filesize == 0){
		printf("����һ�����ļ�:%s\n",filename);
		return 0;
	}

	files = pretreatment(filename,1);
	if(files.error == true) return 0;


	fDest_err = fopen_s(&fDest, files.encrypt_file, "r");
	if(fDest_err == 0){//����ļ��Ѿ�����ʱ
		fclose(fDest);
		printf("%s �Ѿ����ڣ��Ƿ񸲸ǣ� ",files.encrypt_file);
		fflush(stdin);
		scanf_s("%s",&trueFalse,2);
		if(trueFalse[0] != 'Y' && trueFalse[0] != 'y') return 0;
	}

	fDest_err = fopen_s(&fDest, files.encrypt_file, "wb");
	fSourse_err = fopen_s(&fSource, files.original_file, "rb");


	if(fSourse_err != 0){
		printf("�޷���ԭ�ļ�: %s\n",files.original_file);
		return 0;
	}

	if(fDest_err != 0){
		printf("�޷����ɼ����ļ���%s\n",files.encrypt_file);
		return 0;
	}

	//���㿪ʼʱ��
	time_t difftime,starttime;
	starttime = time(0);
	
	fwrite(files.ext_encryp_str, 1, EXT_STORAGE_LEN, fDest); // д���ļ�
	
	unsigned int key_len = strlen(DATA_KEY);//��ȡ��Կ����

	while(!feof(fSource)){ 
        fread(fBuffer, 1, 1, fSource);    // ��ȡ1�ֽ�  
        if(!feof(fSource)){
            ckey = DATA_KEY[index%key_len];     // ѭ����ȡkey  
            *fBuffer = *fBuffer ^ ckey;   // xor encrypt  
            fwrite(fBuffer, 1, 1, fDest); // д���ļ�

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
	printf("���ι���ʱ%d��\n",(difftime == 0 ? 1 : difftime));
	printf("�����ļ���·��Ϊ��%s\n",files.encrypt_file);
	return 1;
}

/**
 * �����ļ�
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
		printf("����һ�����ļ�:%s\n",filename);
		return 0;
	}

	files = pretreatment(filename,2);
	if(files.error == true) return 0;

	fDest_err = fopen_s(&fDest, files.original_file, "r");
	if(fDest_err == 0){//����ļ��Ѿ�����ʱ
		fclose(fDest);
		printf("%s �Ѿ����ڣ��Ƿ񸲸ǣ� ",files.original_file);
		fflush(stdin);
		scanf_s("%s",&trueFalse,2);
		if(trueFalse[0] != 'Y' && trueFalse[0] != 'y') return 0;
	}


	fSource_err = fopen_s(&fSource, files.encrypt_file, "rb");//��ȡ�����ļ�
	fDest_err = fopen_s(&fDest, files.original_file, "wb");
	if(fSource_err != 0){
		printf("�޷��򿪼����ļ�: %s\n",files.encrypt_file);
		return 0;
	}

	if(fDest_err != 0){
		printf("�޷����ɽ����ļ���%s\n",files.original_file);
		return 0;
	}

	//���㿪ʼʱ��
	time_t difftime,starttime;
	starttime = time(0);

	unsigned int key_len = strlen(DATA_KEY);//��ȡ��Կ����

	fseek(fSource, EXT_STORAGE_LEN, 0);//���ļ�ָ���ƶ�����32λ
	while(!feof(fSource)){ 
		fread(fBuffer, 1, 1, fSource);    // ��ȡ1�ֽ� 
		if(!feof(fSource)){//��ֹβ����׷��NUL
			ckey = DATA_KEY[index%key_len];     // ѭ����ȡkey  
			*fBuffer = *fBuffer ^ ckey;   // xor encrypt  
			fwrite(fBuffer, 1, 1, fDest); // д���ļ�  
			index ++; 

			if(((1.0*index)/filesize) >= milestone){//��ʾ����
				showProgress((int)(milestone*100));
				milestone += 0.02f;
			}
		}
    }

	printf("[100%%]%s\n\n",progress_str);
	fclose(fSource);
	fclose(fDest);

	difftime = time(0) - starttime;
	printf("���ι���ʱ%d��\n",(difftime == 0 ? 1 : difftime));
	printf("�����ļ���·��Ϊ��%s\n",files.original_file);
	return 1;

}

//��ȡ�����ļ���·��
MYFILE pretreatment(char *filename,int mode){
	char drive[_MAX_DRIVE];

	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char newfilename[255];
	char ext[_MAX_EXT];
	unsigned int ext_key_len = strlen(EXT_KEY);//�����׺��Կ����
	
	MYFILE result = {'\0','\0','\0','\0',false};

	if(mode == 1){//����
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


		//Ϊ�ṹ�����Ը�ֵ
		strcpy_s(result.encrypt_file,newfilename);
		strcpy_s(result.original_file,filename);
		strcpy_s(result.ext_encryp_str,ext_encryp_str);
		result.ext = ext;
		
	}else{//����
		char fBuffer[1];
		int ext_index = 0;
		FILE *fSource;
		errno_t fSource_err;
		
		char original_ext[EXT_STORAGE_LEN];//���ڴ�Ž��ܳ����ĺ�׺

		fSource_err = fopen_s(&fSource, filename, "rb");//��ȡ�����ļ�
		if(fSource == NULL){
			printf("�޷��򿪼����ļ�: %s\n",filename);
			return result;
		}

		bool has_normal = false;
		while(!feof(fSource)){
			fread(fBuffer, 1, 1, fSource);// ��ȡ1�ֽ� 
			if(ext_index >= EXT_STORAGE_LEN) break;
		
			*fBuffer = *fBuffer ^ EXT_KEY[ext_index%ext_key_len];   // xor encrypt
			
			if(*fBuffer == '#'){//����#�ű�ʾ��׺����
				original_ext[ext_index] = '\0';
				break;
			}

			//��һ���ַ�����ǵ�Ļ��ͺϷ�(���Ͻ����Ż�)
			if(ext_index == 0 && *fBuffer == '.') has_normal = true;

			original_ext[ext_index] = *fBuffer;//��װ��׺
			ext_index++;
		}
		fclose(fSource);

	
		_splitpath_s( filename, drive, dir, fname, ext);



		//У������ļ��Ƿ�Ϸ�
		if(has_normal == false || strcmp(ext,".je") != 0){
			printf("����һ���Ϸ��ļ����ļ�: %s\n",filename);
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
	printf("***************** �ı����ܽ������ *****************\n");
	printf("*                                                  *\n");
	printf("*      **************************************      *\n");
	printf("*      *                                    *      *\n");
	printf("*      *  Author��@Jathon                   *      *\n");
	printf("*      *                                    *      *\n");
	printf("*      **************************************      *\n");
	printf("*                                                  *\n");
	printf("****************************************************\n");
}

//��ȡ�ļ���С 
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

//��ӡ����
void showProgress(int progress){
	printf("[%d%%]%s%s\r",progress,progress_str+((100-progress)/2),progress_str2+(progress/2));
	fflush(stdout);
}

//��ʽ������ֵ
void formatInput(char *input){
	if(input[strlen(input)-1] == '\n')
		input[strlen(input)-1] = '\0'; 
}

//���ù�ϣ����
void haltPassword(char pw[16],char halt[11],char *decrypt_str){
	char halt_pass[40] = {0};//����ַ��ϲ�������
	
	strcpy_s(halt_pass,halt);//�����ַ�
	strcat_s(halt_pass,pw);//�����ַ�
	strcat_s(halt_pass,PASS_HALT);//�����ַ�

	md5(pw,decrypt_str);
}

//ȷ�����벢�޸�
bool confirmPasswordMsg(){
	char password[17]={0},pw2[17];
	printf("��һ�ν��룬��Ҫ����һ�³�ʼ���롣\n");

	bool loop = true;
	while(loop){
		printf("������������룺");
		fgetPassword(password,16);//��ȡ���������
		if(*password == '\0'){
			printf("���벻��Ϊ�գ�\n");
			continue;
		}
		if(strlen(password) > 16){
			printf("���볤�Ȳ��ܳ���16���ַ���\n");
			continue;
		}
		printf("���ٴ�����������룺");
		fgetPassword(pw2,16);//��ȡ���������
		if(strcmp(password, pw2) != 0){
			printf("�������벻һ�£�\n");
			continue;
		}
		loop = false;
	}
	
	return editPassword(password);
}

//�ж������Ƿ���ȷ
bool checkPassword(){
	HKEY hKEY;

	LONG lRet=RegOpenKeyExA(HKEY_LOCAL_MACHINE,regname,0,KEY_READ,&hKEY);

	char in_pass[17];//�������뻺����

	char password[33] = {0};//ע������ݻ�����
	char halt[11] = {0};//ע������ݻ�����
	char decrypt_str[33] = {0};//��ϣ���뻺����

	DWORD dwSize,haltSize;

	dwSize=sizeof(password);
	haltSize=sizeof(halt);

	lRet=RegQueryValueExA(hKEY,"password",NULL,NULL,(LPBYTE)password,&dwSize);
	lRet=RegQueryValueExA(hKEY,"addtime",NULL,NULL,(LPBYTE)halt,&haltSize);
	RegCloseKey(hKEY);//�ر�ע���

	if(*password == '\0'){
		printf("�Բ��𣬳��������𻵣������������������룡\n");
		return confirmPasswordMsg();
	}

	printf("���������룺");
	fgetPassword(in_pass,16);//��ȡ���������
	if(strlen(in_pass) > 16){
		printf("���볤�Ȳ��ܳ���16���ַ���\n");
		return false;
	}

	haltPassword(in_pass,halt,decrypt_str);
	if(strcmp(decrypt_str, password) == 0) return true;
	return false;
}

//��������
bool editPassword(char pw[16]){
	if(strlen(PASS_HALT) > 10){
		printf("�������ֵ��������10���ַ�");
		return false;
	}

	HKEY hKEY;
	char *lpszValueName = "password";
	DWORD dwDisposition = REG_OPENED_EXISTING_KEY;

	LONG lRet = RegCreateKeyExA(HKEY_LOCAL_MACHINE, regname, NULL, NULL, REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS, NULL, &hKEY, &dwDisposition); 
	if(lRet != ERROR_SUCCESS){
		printf("��Ȩ�޴�ע��������Ƿ��Թ���Ա���У�\n");
		return false;
	}

	char int_to_string[11] = {0};//����ת�ַ�������
	char decrypt_str[33] = {0};//��ϣ���뻺����
	time_t addtime = time(0);//ʱ���(10λ)
	_itoa_s((unsigned)addtime,int_to_string,10);//������ת�����ַ�

	haltPassword(pw,int_to_string,decrypt_str);

	lRet = ::RegSetValueExA(hKEY, lpszValueName, NULL, REG_SZ, (BYTE*)decrypt_str, strlen(decrypt_str)+1);
	lRet = ::RegSetValueExA(hKEY, "addtime", NULL, REG_SZ, (BYTE*)int_to_string, strlen(int_to_string)+1);
	if(ERROR_SUCCESS != lRet){  
        printf("��Ȩ�޴�ע��������Ƿ��Թ���Ա���У�");
		return false; 
    }
	RegCloseKey(hKEY);
	return true;
}

//��ȡ�û����������
void fgetPassword(char *password,int pass_len){
	int i = 0;
	char ch = '*';
	while(i <= pass_len && (password[i] = _getch()) != '\r'){

		if(i >= pass_len && password[i] != '\b') continue;//�������Ȳ���������(ɾ������)
			
		if(password[i]=='\b'){//ɾ��ʱ����
			if(i <= 0) continue;
			putchar('\b');
			putchar(' ');
			putchar('\b');
			i--;
		}else{//д���Ǻ�
			putchar(ch);
			i++;
		}
     }
    password[i]='\0';//��ȫ�ַ���
	printf("\n");//��ӡ����
}