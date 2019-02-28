#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <iostream>
#include "process.h"
#include "dir.h"

#define Maxline 1000
///输入输出最大行数
#define Maxchar 1000
///输入输出最大行长度
#define Maxnum 1000
///最大作业数量


struct Table {
    int id;
    int result;
} table[Maxnum];
//定义输出表，包含学号和结果

char res[Maxline][Maxchar];
//程序返回结果
int num = -1;
//正在执行的作业号

char *split(const char *str1, const char *str2, int n) {
    ///定义字符串切割函数，比较常用就拿出来了
    char *str = (char *) malloc(strlen(str1) * sizeof(char));
    strcpy(str, str1);
    char *p;
    p = strtok(str, str2);
    while (p) {
        if (!n)
            break;
        n -= 1;
        p = strtok(NULL, str2);
    }
    return p;
}

int csvsave() {
    ///将table结构体保存到csv
    char resultset[4][20] = {
            "通过",
            "编译错误",
            "超时",
            "输出结果错误"
    };
    FILE *fp = fopen("../result.csv", "w");
    fprintf(fp, "学号,评测结果\n");
    for (int i = 0; table[i].id > 0; i++) {
        fprintf(fp, "%d,%s\n", table[i].id, resultset[table[i].result]);
    }
    return 0;
}

int getFileLength(FILE *fp) {
    ///获取文件长度
    fseek(fp, 0, SEEK_END);
    int size = (int) ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

int iserror() {
    ///读取error文件，判断是否运行错误
    FILE *fp = fopen("../error", "r");
    int size = getFileLength(fp);
    fclose(fp);
    return (bool) size;
}

int compile(const char *cname) {
    ///编译函数：调用gcc进行编译
    char cmd[100];
    sprintf(cmd, "gcc ../inputfiles/%s -o ../testbin/a.exe  2> ../error", cname);
    system(cmd);
    return iserror();
}

int exefile(int &line, const char *exename, const char *inputname) {
    ///执行编译结果
    char cmd[100];//保存运行命令
    sprintf(cmd, R"(..\testbin\%s)", exename);//构造运行命令
    char input[Maxline*Maxchar] = "";//读取输入文件到这
    if (inputname != NULL) {
        FILE *fp0 = fopen(inputname, "r");
        int n = 0;
        int cc=1;
        do{
            cc = fgetc(fp0);
            input[n]=cc;
            n++;
        }while (~cc);
        fclose(fp0);
    }
    in_buffer=input;//将输入交给process处理
    int error=run(cmd);
    line = 0;//计算行数
    for(int i=0;i<strlen(buffer);i++){
        if (buffer[i]=='\n')
            line++;
    }
    for(int i=0;i<line;i++){
        char *str=split(buffer,"\r\n",i);
        strcpy(res[i],str);
    }
    return error;
}

int compare(const char *f2, int fp1line, int mod) {
    ///比较标准答案和程序运行结果 参数分别为程序运行结果，答案行数，比对级别
    FILE *fp2 = fopen(f2, "r");
    char text[Maxline][Maxchar];
    char buf[Maxchar];
    int fp2line = 0;
    while (fgets(buf, Maxchar, fp2) != NULL) {
        strcpy(text[fp2line], split(buf,"\n",0));
        fp2line++;
        //将程序运行结果按行分开
    }
    printf("\n%d %d\n############", fp2line, fp1line);
    if (fp1line != fp2line)//如果行数都不一样那肯定错误了
        return 1;
    switch (mod) {
        case 1: {
            for (int i = 0; i < fp1line; i++) {
                int cmp = strcmp(text[i], res[i]);
                if (cmp) {
                    printf("%s %s %d#\n", text[i], res[i], cmp);
                    return 1;
                }
            }
            return 0;
        }
        case 2:
        {
            int linen[Maxline] = {0};
            for (int i = 0; i < fp1line; i++) {
                int cmp = 0;
                for (int j = 0; j < fp2line; j++) {
                    if (linen[j] == 1)
                        break;
                    cmp = strcmp(text[j], res[i]);
                    if (!cmp) {
                        cmp=0;
                        linen[j] = 1;
                        break;
                    }
                }
                if (cmp) {
                    return 1;
                }
            }
            return 0;
        }
        default:
            printf("输入错误,退出程序\n");
    }
    return 1;
}

int work(const char *cfile,int mod) {
    num++;
    char *idstr = split(cfile, ".", 0);
    int id = atoi(idstr);
    int complierror = compile(cfile);
    if (complierror) {
        table[num].id = id;
        table[num].result = 1;
        printf("编译错误\n");
        return 1;
    }
    int line;
    int exeerror = exefile(line, "..\\testbin\\a.exe", "..\\in");

    if (exeerror) {
        table[num].id = id;
        table[num].result = 2;
        printf("超时\n");
        return 2;
    }
    int resulterror = compare("..\\answer", line, mod);
    if (resulterror) {
        table[num].id = id;
        table[num].result = 3;
        printf("输出错误\n");
        return 3;
    }
    table[num].id = id;
    table[num].result = 0;
    printf("通过\n");
    return 0;
}


int main() {
    int mod;
    puts("请输入比对级别");
    puts("1.严格比对\n2.忽略行顺序");
    scanf("%d",&mod);
    dir("..\\inputfiles");
    for(int i=0;i<filenum;i++) {
        puts(files[i]);
        work(files[i],mod);
    }
    csvsave();
}
