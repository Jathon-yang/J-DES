#define EXT_KEY "abcdefgdhij"//用于后缀加密的KEY
#define EXT_STORAGE_LEN 32//用于存放加密文件的真实后缀信息的最大长度
#define DATA_KEY "abcdefgh"//用于加密文件内容的KEY
#define MYFILE struct myfile
MYFILE{
	char encrypt_file[255];
	char original_file[255];
	char ext_encryp_str[EXT_STORAGE_LEN];
	char *ext;
};

int encrypFile(char *filename);//加密文件
char decryptFile(char *filename);//解密文件
void printStartMsg();//打印启动信息
MYFILE pretreatment(char *filename,int mode);
