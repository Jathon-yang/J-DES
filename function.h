#define EXT_KEY "ffb46a819408efb50e9bbcd6c99c5016"//���ں�׺���ܵ�KEY
#define EXT_STORAGE_LEN 32//���ڴ�ż����ļ�����ʵ��׺��Ϣ����󳤶�
#define DATA_KEY "e10adc3949ba59abbe56e057f20f883e"//���ڼ����ļ����ݵ�KEY
#define PASS_HALT "abcdefgh"//�������(���10���ַ�)
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
bool resetPassword();//��������
bool editPassword(char pw[16]);//��������
int encrypFile(char filename[255]);//�����ļ�
char decryptFile(char *filename);//�����ļ�
void printStartMsg();//��ӡ������Ϣ
uint64 fileSize(char filename[255]);//��ȡ�ļ���С 
void showProgress(int progress);//��ӡ����
void formatInput(char *input);//��ʽ������ֵ
void haltPassword(char pw[16],char halt[11],char *decrypt_str);//���ù�ϣ����
bool confirmPasswordMsg();//ȷ�����벢�޸�
bool checkPassword();//�ж������Ƿ���ȷ
void fgetPassword(char *password,int pass_len);//��ȡ�û����������

MYFILE pretreatment(char *filename,int mode);



