#define EXT_KEY "abcdefgdhij"//���ں�׺���ܵ�KEY
#define EXT_STORAGE_LEN 32//���ڴ�ż����ļ�����ʵ��׺��Ϣ����󳤶�
#define DATA_KEY "abcdefgh"//���ڼ����ļ����ݵ�KEY
#define MYFILE struct myfile
MYFILE{
	char encrypt_file[255];
	char original_file[255];
	char ext_encryp_str[EXT_STORAGE_LEN];
	char *ext;
	bool error;
};

typedef unsigned long long int  uint64;

bool appInit();//�����ʼ��
bool editPassword(char pw[16]);//��������
int encrypFile(char filename[255]);//�����ļ�
char decryptFile(char *filename);//�����ļ�
void printStartMsg();//��ӡ������Ϣ
uint64 fileSize(char filename[255]);//��ȡ�ļ���С 
void showProgress(int progress);//��ӡ����
void formatInput(char *input);//��ʽ������ֵ

MYFILE pretreatment(char *filename,int mode);


