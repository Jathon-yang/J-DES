#define EXT_KEY "abcdefgdhij"//���ں�׺���ܵ�KEY
#define EXT_STORAGE_LEN 32//���ڴ�ż����ļ�����ʵ��׺��Ϣ����󳤶�
#define DATA_KEY "abcdefgh"//���ڼ����ļ����ݵ�KEY
#define MYFILE struct myfile
MYFILE{
	char encrypt_file[255];
	char original_file[255];
	char ext_encryp_str[EXT_STORAGE_LEN];
	char *ext;
};

int encrypFile(char *filename);//�����ļ�
char decryptFile(char *filename);//�����ļ�
void printStartMsg();//��ӡ������Ϣ
MYFILE pretreatment(char *filename,int mode);
