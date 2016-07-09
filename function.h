#define EXT_KEY "ffb46a819408efb50e9bbcd6c99c5016"//用于后缀加密的KEY
#define EXT_STORAGE_LEN 32//用于存放加密文件的真实后缀信息的最大长度
#define DATA_KEY "e10adc3949ba59abbe56e057f20f883e"//用于加密文件内容的KEY
#define PASS_HALT "abcdefgh"//密码加盐(最多10个字符)
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
bool resetPassword();//密码重置
bool editPassword(char pw[16]);//设置密码
int encrypFile(char filename[255]);//加密文件
char decryptFile(char *filename);//解密文件
void printStartMsg();//打印启动信息
uint64 fileSize(char filename[255]);//获取文件大小 
void showProgress(int progress);//打印进度
void formatInput(char *input);//格式化输入值
void haltPassword(char pw[16],char halt[11],char *decrypt_str);//设置哈希密码
bool confirmPasswordMsg();//确认密码并修改
bool checkPassword();//判断密码是否正确
void fgetPassword(char *password,int pass_len);//获取用户输入的密码

MYFILE pretreatment(char *filename,int mode);



