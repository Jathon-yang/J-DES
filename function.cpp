#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "md5.h"
#include "function.h"

char *progress_str = ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
char *progress_str2 = "==================================================";

//�����ʼ��
bool appInit(){
	printStartMsg();

	HKEY hKEY;
	char *regname = "Software\\J-DES";

	LONG lRet=RegOpenKeyExA(HKEY_LOCAL_MACHINE,regname,0,KEY_READ,&hKEY);
	if(lRet != ERROR_SUCCESS){//û�����ע���ѡ��(���г�ʼ��)
		RegCloseKey(hKEY);//�ر�ע���

		char pw1[17],pw2[17];
		printf("��һ�ν��룬��Ҫ����һ�³�ʼ���롣\n");
		printf("������������룺");
		fgets(pw1,sizeof(pw1),stdin);
		formatInput(pw1);
		if(strlen(pw1) > 16){
			printf("���볤�Ȳ��ܳ���16���ַ���\n");
			return false;
		}
		printf("���ٴ�����������룺");
		fgets(pw2,sizeof(pw2),stdin);
		formatInput(pw2);
		if(strcmp(pw1, pw2) != 0){
			printf("�������벻һ�£�\n");
			return false;
		}

		return editPassword(pw1);
	}else{//�ǳ�ʼ����ʾ��������
		char in_pass[17];
		int error = 3;

		char password[33] = {0};
		char decrypt_str[33] = {0}; 
	
		DWORD dwSize;

		dwSize=sizeof(password);
		lRet=RegQueryValueExA(hKEY,"password",NULL,NULL,(LPBYTE)password,&dwSize);
		RegCloseKey(hKEY);//�ر�ע���

		while(1){
			printf("���������룺");
			fgets(in_pass,sizeof(in_pass),stdin);
			formatInput(in_pass);
			if(strlen(in_pass) > 16){
				printf("���볤�Ȳ��ܳ���16���ַ���\n");
				return false;
			}

			md5(in_pass,decrypt_str);
			if(strcmp(decrypt_str, password) == 0) return true;
			
			printf("�Բ�������������벻��ȷ������%d�λ���\n",--error);
		
			if(error < 1) return false;
		}
	}
	return true;
}

//��������
bool editPassword(char pw[16]){
	HKEY hKEY;
	char *regname = "Software\\J-DES";
	char *lpszValueName = "password";
	DWORD dwDisposition = REG_OPENED_EXISTING_KEY;

	LONG lRet = RegCreateKeyExA(HKEY_LOCAL_MACHINE, regname, NULL, NULL, REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS, NULL, &hKEY, &dwDisposition); 
	if(lRet != ERROR_SUCCESS){
		printf("��Ȩ�޴�ע��������Ƿ��Թ���Ա���У�\n");
		return false;
	}

	char decrypt_str[33] = {0}; 
	md5(pw,decrypt_str);

	lRet = ::RegSetValueExA(hKEY, lpszValueName, NULL, REG_SZ, (BYTE*)decrypt_str, strlen(decrypt_str)+1);
	if(ERROR_SUCCESS != lRet){  
        printf("��Ȩ�޴�ע��������Ƿ��Թ���Ա���У�");
		return false; 
    }
	RegCloseKey(hKEY);
	return true;
}

//�����ļ�
int encrypFile(char filename[255]){
	MYFILE files;
	char *source,*key,ckey,fBuffer[1],trueFalse[2];
	int index = 0;
	uint64 filesize = fileSize(filename);
	FILE *fSource,*fDest;
	float milestone = 0.02;//��ɽ���

	if(filesize == 0){
		printf("����һ�����ļ�:%s\n",filename);
		return 0;
	}

	files = pretreatment(filename,1);
	if(files.error == true) return 0;

	fDest = fopen(files.encrypt_file, "r");
	if(fDest != NULL){//����ļ��Ѿ�����ʱ
		fclose(fDest);
		printf("%s �Ѿ����ڣ��Ƿ񸲸ǣ� ",files.encrypt_file);
		scanf("%s",&trueFalse);
		if(trueFalse[0] != 'Y' && trueFalse[0] != 'y') return 0;
	}

	fDest = fopen(files.encrypt_file, "wb");
	fSource = fopen(files.original_file, "rb");


	if(fSource == NULL){
		printf("�޷���ԭ�ļ�: %s\n",files.original_file);
		return 0;
	}

	if(fDest == NULL){
		printf("�޷����ɼ����ļ���%s\n",files.encrypt_file);
		return 0;
	}

	//���㿪ʼʱ��
	time_t difftime,starttime;
	starttime = time(0);
	
	fwrite(files.ext_encryp_str, 1, EXT_STORAGE_LEN, fDest); // д���ļ�
	
	key = DATA_KEY;

	while(!feof(fSource)){ 
        fread(fBuffer, 1, 1, fSource);    // ��ȡ1�ֽ�  
        if(!feof(fSource)){
            ckey = key[index%7];     // ѭ����ȡkey  
            *fBuffer = *fBuffer ^ ckey;   // xor encrypt  
            fwrite(fBuffer, 1, 1, fDest); // д���ļ�

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
	printf("���ι���ʱ%d��\n",(difftime == 0 ? 1 : difftime));
	printf("�����ļ���·��Ϊ��%s\n",files.encrypt_file);
	return 1;
}

/**
 * �����ļ�
 */
char decryptFile(char *filename){
	char *source,ckey,fBuffer[1],trueFalse[2],*key = DATA_KEY;//�ļ�����KEY;
	int index = 0,ext_index = 0;
	FILE *fSource,*fDest;
	uint64 filesize = fileSize(filename);
	MYFILE files;
	float milestone = 0.02;

	if(filesize == 0){
		printf("����һ�����ļ�:%s\n",filename);
		return 0;
	}

	files = pretreatment(filename,2);
	if(files.error == true) return 0;

	fDest = fopen(files.original_file, "r");
	if(fDest != NULL){//����ļ��Ѿ�����ʱ
		fclose(fDest);
		printf("%s �Ѿ����ڣ��Ƿ񸲸ǣ� ",files.original_file);
		scanf("%s",&trueFalse);
		if(trueFalse[0] != 'Y' && trueFalse[0] != 'y') return 0;
	}


	fSource = fopen(files.encrypt_file, "rb");//��ȡ�����ļ�
	fDest = fopen(files.original_file, "wb");
	if(fSource == NULL){
		printf("�޷��򿪼����ļ�: %s\n",files.encrypt_file);
		return 0;
	}

	if(fDest == NULL){
		printf("�޷����ɽ����ļ���%s\n",files.original_file);
		return 0;
	}

	//���㿪ʼʱ��
	time_t difftime,starttime;
	starttime = time(0);

	fseek(fSource, EXT_STORAGE_LEN, 0);//���ļ�ָ���ƶ�����32λ
	while(!feof(fSource)){ 
		fread(fBuffer, 1, 1, fSource);    // ��ȡ1�ֽ� 
		if(!feof(fSource)){//��ֹβ����׷��NUL
			ckey = key[index%7];     // ѭ����ȡkey  
			*fBuffer = *fBuffer ^ ckey;   // xor encrypt  
			fwrite(fBuffer, 1, 1, fDest); // д���ļ�  
			index ++; 

			if(((1.0*index)/filesize) >= milestone){//��ʾ����
				showProgress(milestone*100);
				milestone += 0.02;
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
	char *ext_key = EXT_KEY;//��׺��Կ
	
	MYFILE result;

	if(mode == 1){//����
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


		//Ϊ�ṹ�����Ը�ֵ
		strcpy(result.encrypt_file,newfilename);
		strcpy(result.original_file,filename);
		strcpy(result.ext_encryp_str,ext_encryp_str);
		result.ext = ext;
		
	}else{//����
		char fBuffer[1];
		int ext_index = 0;
		FILE *fSource;
		
		char original_ext[EXT_STORAGE_LEN];//���ڴ�Ž��ܳ����ĺ�׺

		fSource = fopen(filename, "rb");//��ȡ�����ļ�
		if(fSource == NULL){
			printf("�޷��򿪼����ļ�: %s\n",filename);
			return result;
		}

		bool has_normal = false;
		while(!feof(fSource)){
			fread(fBuffer, 1, 1, fSource);// ��ȡ1�ֽ� 
			if(ext_index >= EXT_STORAGE_LEN) break;
		
			*fBuffer = *fBuffer ^ ext_key[ext_index%10];   // xor encrypt
			
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

	
		_splitpath( filename, drive, dir, fname, ext);



		//У������ļ��Ƿ�Ϸ�
		if(has_normal == false || strcmp(ext,".je") != 0){
			printf("����һ���Ϸ��ļ����ļ�: %s\n",filename);
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
	fSource = fopen(filename, "rb");
	if(fSource == NULL) return 0;
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