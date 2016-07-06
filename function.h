#define EXT_KEY "abcdefgdhij"//用于后缀加密的KEY
#define EXT_STORAGE_LEN 32//用于存放加密文件的真实后缀信息的最大长度
#define DATA_KEY "abcdefgh"//用于加密文件内容的KEY
#define MYFILE struct myfile
MYFILE{
	char encrypt_file[255];
	char original_file[255];
	char ext_encryp_str[EXT_STORAGE_LEN];
	char *ext;
	bool error;
};

typedef unsigned long long int  uint64;

bool appInit();//程序初始化
bool editPassword(char pw[16]);//设置密码
int encrypFile(char filename[255]);//加密文件
char decryptFile(char *filename);//解密文件
void printStartMsg();//打印启动信息
uint64 fileSize(char filename[255]);//获取文件大小 
void showProgress(int progress);//打印进度
void formatInput(char *input);//格式化输入值

MYFILE pretreatment(char *filename,int mode);


