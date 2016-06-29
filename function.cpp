#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.h"

//�����ļ�
int encrypFile(char *filename){
	MYFILE files;
	char *source,*key,ckey,fBuffer[1],trueFalse[2];
	int index = 0;
	uint64 filesize = fileSize(filename);
	FILE *fSource,*fDest;
	float milestone = 0.01;//��ɽ���

	files = pretreatment(filename,1);

	fDest = fopen(files.encrypt_file, "r");
	if(fDest != NULL){//����ļ��Ѿ�����ʱ
		fclose(fDest);
		printf("%s �Ѿ����ڣ��Ƿ񸲸ǣ�\n",files.encrypt_file);
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
				milestone += 0.01;
			}
             
        }
    }
	printf("\n");
    fclose(fSource);
	fclose(fDest);
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
	float milestone = 0.01;

	files = pretreatment(filename,2);

	fDest = fopen(files.original_file, "r");
	if(fDest != NULL){//����ļ��Ѿ�����ʱ
		fclose(fDest);
		printf("%s �Ѿ����ڣ��Ƿ񸲸ǣ�\n",files.original_file);
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
				milestone += 0.01;
			}
		}
    }
	printf("\n");
	fclose(fSource);
	fclose(fDest);

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

		while(!feof(fSource)){
			fread(fBuffer, 1, 1, fSource);// ��ȡ1�ֽ� 
			if(ext_index >= EXT_STORAGE_LEN) break;
		
			*fBuffer = *fBuffer ^ ext_key[ext_index%10];   // xor encrypt
			
			if(*fBuffer == '#'){//����#�ű�ʾ��׺����
				original_ext[ext_index] = '\0';
				break;
			}

			original_ext[ext_index] = *fBuffer;//��װ��׺
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
	fSource = fopen(filename, "r");

	fseek(fSource,0,2);
	
	filesize = ftell(fSource);
	fclose(fSource);
	return filesize;
}

//��ӡ����
void showProgress(int progress){
	char *str = "####################################################################################################";
	char *str2 = "----------------------------------------------------------------------------------------------------";
	printf("[%d%%]%s%s\r",progress,str+(100-progress),str2+progress);
	fflush(stdout);
}
				