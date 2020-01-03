#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <unistd.h>
#define N 64
#define M 2*N-1
#define SOU "souceFile.txt"
#define Code "CodeFile"
#define DeCode "DecodeFile.txt"
using namespace std;

typedef struct{
	int weight;
	char data;
	int parent,Lchild,Rchild;
}HTNode,HuffmanTree[M+1];

int book[128]; //记录字符出现的次数
int number=0; //记录出现字符的个数;

char filename[30];
char cpfilename[30];
char uncpfilename[30];

struct Node
{
	int b;			  //字符
	int count;                   //文件中该字符出现的次数
	int parent, lch, rch;        //make a tree
	char bits[256];               //the huffuman code
};
bool cmp(struct Node o1,struct Node o2)
{
	return o1.count > o2.count;
}

bool cmp1(struct Node o1,struct Node o2)
{
	return strlen(o1.bits) < strlen(o2.bits);
}
struct Node HT[256];  //节点树

int flength,n,m;

void openfile()
{
	unsigned char c;
	FILE *infp, *outfp;
	infp = fopen(SOU, "rb");                  //打开原始文件
	if (infp == NULL)
	{
		printf("文件打开失败:%s\n",SOU);
		exit(0);                            	 //如果打开失败，则输出错误信息
	}
	outfp = fopen(Code,"wb");                 //打开压缩后存储信息的文件
	if (outfp == NULL)
	{
		printf("文件打开失败:%s\n",Code);
		exit(0);
	}
	flength = 0;
	while (!feof(infp))
	{
		fread(&c, 1, 1, infp);
//		cout << c << endl;
		HT[c].count++;                       //读文件，统计字符出现次数
		flength++;                               //记录文件的字符总数
	}
	flength--;
	HT[c].count--;

}

void CrtHuffmanTree()
{
	int f,pt1;
	int i,j;
	for (i = 0; i < 256; i ++)                    //HUFFMAN算法中初始节点的设置(128*4)
	{
		if (HT[i].count != 0)
		{
			HT[i].b = (unsigned char) i;
		}
		else
			HT[i].b = -1;
		HT[i].parent = -1;
		HT[i].lch = HT[i].rch = -1;
	}
	
	sort(HT,HT+128,cmp);//将节点按出现次数排序

	for (i = 0; i < 128; i ++)                    //统计不同字符的数量
	{
		if (HT[i].count == 0)
		{
			break;
		}
	}
	n = i;
	m = 2 * n - 1;
	for (i = n; i < m; i ++)
	{
		int minn = 0xffffff;
		for (j = 0; j < i; j ++)
		{
			if (HT[j].parent == -1 && minn > HT[j].count)
			{
				pt1 = j;
				minn = HT[j].count;
				continue;
			}
		}
		HT[i].count = HT[pt1].count;
		HT[pt1].parent = i;
		HT[i].lch = pt1;
		minn = 0xffffff;
		for (j = 0; j < i; j ++)
		{
			if (HT[j].parent == -1 && minn > HT[j].count)
			{
				pt1 = j;
				minn = HT[j].count;
				continue;
			}
		}
		HT[i].count += HT[pt1].count;
		HT[i].rch = pt1;
		HT[pt1].parent = i;
	}
	
	for (i = 0; i < n; i ++)                        //构造huffman树，设置字符的编码
	{
		f = i;
		HT[i].bits[0] = 0;
		while (HT[f].parent != -1)
		{
			j = f;
			f = HT[f].parent;
			if (HT[f].lch == j)
			{
				j = strlen(HT[i].bits);
				memmove(HT[i].bits + 1, HT[i].bits, j + 1);
				HT[i].bits[0] = '0';
			}
			else
			{
				j = strlen(HT[i].bits);
				memmove(HT[i].bits + 1, HT[i].bits, j + 1);
				HT[i].bits[0] = '1';
			}
		}
	}
//	for(int i=0;i<n;i++)
//		cout << HT[i].b << ' '<< HT[i].bits << endl;
}

void printfPercent(int per)
{
	int i = 0;
	printf("|");
	for(i = 0; i < 10; i++)
	{
		if(i < per/10)
			printf(">");
		else
			printf("-");
	}
	printf("|已完成%d%%\n",per);
	usleep(500000);
	fflush(stdout);
	system("clear");
}

//函数：compress()
//作用：读取文件内容并加以压缩
//将压缩内容写入另一个文档
int compress(const char *filename,const char *outputfile)
{
	char buf[512];
	unsigned char c;
	struct Node tmp;
	int i, j,f=0;
	int min1, pt1=8;
	FILE *infp, *outfp;
	int per = 10;
	infp = fopen(filename, "rb");                  //打开原始文件
	if (infp == NULL)
	{
		printf("文件打开失败:%s\n",filename);
		return 0;                             //如果打开失败，则输出错误信息
	}
	outfp = fopen(outputfile,"wb");                 //打开压缩后存储信息的文件
	if (outfp == NULL)
	{
		printf("文件打开失败:%s\n",outputfile);
		return 0;
	}
	//下面的就是读原文件的每一个字符，按照设置好的编码替换文件中的字符
	fseek(infp, 0, SEEK_SET);                                 //将指针定在文件起始位置
	fseek(outfp, 8, SEEK_SET);                                //以8位二进制数为单位进行读取
	buf[0] = 0;
	
	printf("读取将要压缩的文件:%s\n",filename);
	printf("当前文件有:%d字节\n",flength);
	printf("正在压缩\n");
	
	//压缩操作
	while (!feof(infp))
	{
		c = fgetc(infp);
		f++;
		for (i = 0; i < n; i ++)
		{
			if (c == HT[i].b) break;
		}
		strcat(buf, HT[i].bits);
		j = strlen(buf);
		c = 0;
		while (j >= 8)                                             //当剩余字符数量不小于8个时
		{
			for (i = 0; i < 8; i ++)                               //按照八位二进制数转化成十进制ASCII码写入文件一次进行压缩
			{
				if (buf[i] == '1') c = (c << 1) | 1;
				else c = c << 1;
			}
			fwrite(&c, 1, 1, outfp);
			pt1++;
			strcpy(buf, buf + 8);
			j = strlen(buf);
		}
		if(100 * f/flength > per)
		{
			printfPercent(per);
			per += 10;
		}
		if (f == flength)
			break;
	}
	printfPercent(100);

	if (j > 0)                                                      //当剩余字符数量少于8个时
	{
		strcat(buf, "00000000");
		for (i = 0; i < 8; i ++)
		{
			if (buf[i] == '1') c = (c << 1) | 1;
			else c = c << 1;                   	                    //对不足的位数进行补零
		}
		printf("%d\n",c);
		fwrite(&c, 1, 1, outfp);
		pt1++;
	}
	fseek(outfp, 0, SEEK_SET);                                        //将编码信息写入存储文件
	fwrite(&flength,1,sizeof(int),outfp);
	fwrite(&pt1, sizeof(int), 1, outfp);
//	cout << ftell(outfp) << endl;
//	cout << pt1 << endl;
	fseek(outfp, pt1, SEEK_SET);
	fwrite(&n, sizeof(int), 1, outfp);
	
	for (i = 0; i < n; i ++)
	{
		tmp = HT[i];

		fwrite(&(HT[i].b), 1, 1, outfp);
		pt1++;
		c = strlen(HT[i].bits);
		fwrite(&c, 1, 1, outfp);
		pt1++;
		j = strlen(HT[i].bits);

		if (j % 8 != 0)                                             //当位数不满8时，对该数进行补零操作
		{
			for (f = j % 8; f < 8; f ++)
				strcat(HT[i].bits, "0");
		}

		while (HT[i].bits[0] != 0)
		{
			c = 0;
			for (j = 0; j < 8; j ++)
			{
				if (HT[i].bits[j] == '1') c = (c << 1) | 1;
				else c = c << 1;
			}
//			printf("%d\n",c);
			strcpy(HT[i].bits, HT[i].bits + 8);
			fwrite(&c, 1, 1, outfp);                                            //将所得的编码信息写入文件
			pt1++;
		}
		HT[i] = tmp;
	}
	
	fclose(infp);
	fclose(outfp);                       //关闭文件
	
	printf("压缩后文件为:%s\n",outputfile);
	printf("压缩后文件有:%d字节\n",pt1 + 4);
	double bi=100*(pt1+4)/flength;
	printf("压缩比为:%.2f%%\n",bi);
	printf("Enter enter to Continue..\n");
	getchar();
	getchar();
	system("clear");
	
	return 0;
}

int uncompress(const char *codingfile,const char *decodefile)
{
	FILE *infp,*oufp;
	int f=0,flength1=0,num_char;
	unsigned char c;
	int p;
	char buf[256],bx[255];
	int i,j,per=10;
	
	infp = fopen(codingfile,"rb");
	if(infp == NULL)
	{
		printf("%s打开失败!\n",codingfile);
		return 0;
	}
	
	oufp = fopen(decodefile,"wb");
	if(oufp == NULL)
	{
		printf("%s打开失败!\n",decodefile);
		return 0;
	}
	
	fseek(infp,0,SEEK_END);
	int len = ftell(infp);
	fseek(infp,0,SEEK_SET);
	
	printf("读取将要解压的文件:%s\n",codingfile);
	printf("当前文件有:%d字节\n",len);
	printf("正在解压...\n");
	
	//读取源文件长度
	fread(&flength1,sizeof(int),1,infp);
	//读取源文件的参数
	fread(&f, sizeof(int), 1, infp);
	
	fseek(infp, f, SEEK_SET);
	//读取原文件字符个数
	fread(&num_char, sizeof(int), 1, infp);
	int mm;						//bits转化为二进制的每8位的个数
	for(int i=0;i<num_char;i++)
	{
		fread(&(HT[i].b),1,1,infp);
		fread(&c,1,1,infp);
		p= (int)c;
		
		HT[i].count = p;		//
		HT[i].bits[0]=0;
		if(p%8!=0) mm=p/8+1;
		else mm=p/8;
		for(int j=0;j<mm;j++)
		{
			fread(&c,1,1,infp);
			f = c;
			memset(buf,'\0',sizeof(buf));
			while(f)
			{
				if(f&1) strcat(buf,"1");
				else strcat(buf,"0");
				f>>=1;
			}
			reverse(buf, buf+strlen(buf));
			f = strlen(buf);
			for(int l=8;l>f;l--)
			{
				strcat(HT[i].bits,"0");
			}
			strcat(HT[i].bits,buf);
		}
		HT[i].bits[p]=0;
	}
	
	sort(HT,HT+num_char,cmp1);
	
	p = strlen(HT[num_char-1].bits);
	fseek(infp, 8, SEEK_SET);
	mm = 0;
	bx[0] = 0;
	
	while (1)
	{
		while (strlen(bx) < (unsigned int)p)
		{
			fread(&c, 1, 1, infp);
			f = c;
			memset(buf,'\0',sizeof(buf));
			while(f)
			{
				if(f&1) strcat(buf, "1");
				else strcat(buf,"0");
				f>>=1;
			}
			reverse(buf, buf+strlen(buf));
			f = strlen(buf);
			for (int l = 8; l > f; l --)
			{
				strcat(bx, "0");
			}
			strcat(bx, buf);
		}
		for (i = 0; i < num_char; i ++)
		{
			if (memcmp(HT[i].bits, bx, HT[i].count) == 0) break;
		}
		strcpy(bx, bx + HT[i].count);
		c = HT[i].b;
		fwrite(&c, 1, 1, oufp);
		mm++;
		if(100 *  mm/flength1 > per)
		{
			printfPercent(per);
			per += 10;
		}
		if (mm == flength1) break;
	}
	printfPercent(100);

	fclose(infp);
	fclose(oufp);

	printf("解压后文件为:%s\n",decodefile);
	printf("解压后文件有:%d字符\n",flength1);
	printf("Enter enter to Continue..\n");
	getchar();
	getchar();
	system("clear");
	
	return 1;
}

int main(int argc, char *argv[]){
	while(1)
	{
		FILE *fp;
		system("clear");
		printf("$**************************************************************************************************$\n");
		printf("$\t\t\t\t\t1.Compress  \t\t\t\t\t\t   $\n");
		printf("$\t\t\t\t\t2.Uncompress\t\t\t\t\t\t   $\n");
		printf("$\t\t\t\t\t3.exit      \t\t\t\t\t\t   $\n");
		printf("$**************************************************************************************************$\n");
		printf("\t\t\t\tPlease input your choice:");
		string a;
		cin >> a;
		switch (a[0]) {
			
			case '1' :
				if(a.length()!=1)
				{
					printf("\t\t\t\t\t Input ERROR!\nEnter enter to Continue..\n");
					getchar();
					getchar();
					break;
				}
				printf("Please input the filename:");
				scanf("%s",filename);
				fp = fopen(filename,"rb");
				if(fp==NULL)
				{
					printf("%s打开失败..\n",filename);
					getchar();
					getchar();
					break;
				}
				printf("Please input after the compress filename:");
				scanf("%s",cpfilename);
				openfile();
				CrtHuffmanTree(); //建huffman树
				compress(filename,cpfilename);
				break;
			case '2':
				if(a.length()!=1)
				{
					printf("\t\t\t\t\t Input ERROR!\nEnter enter to Continue..\n");
					getchar();
					getchar();
					break;
				}
				printf("Please input the compress filename:");
				scanf("%s",cpfilename);
				fp = fopen(cpfilename,"rb");
				if(fp==NULL)
				{
					printf("%s打开失败..\n",cpfilename);
					getchar();
					getchar();
					system("clear");
					break;
				}
				printf("Please input the uncompress filename:");
				scanf("%s",uncpfilename);
				uncompress(cpfilename,uncpfilename);
				break;
			case '3' :
				if(a.length()!=1)
				{
					printf("\t\t\t\t\t Input ERROR!\nEnter enter to Continue..\n");
					getchar();
					getchar();
					break;
				}
				exit(0);
			default:
				printf("\t\t\t\t\t Input ERROR!\nEnter enter to Continue..\n");
				getchar();
				getchar();
				break;
		}
	}
}