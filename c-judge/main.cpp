#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <iostream>
#include "process.h"
#include "dir.h"

#define Maxline 1000
///��������������
#define Maxchar 1000
///�����������г���
#define Maxnum 1000
///�����ҵ����


struct Table {
    int id;
    int result;
} table[Maxnum];
//�������������ѧ�źͽ��

char res[Maxline][Maxchar];
//���򷵻ؽ��
int num = -1;
//����ִ�е���ҵ��

char *split(const char *str1, const char *str2, int n) {
    ///�����ַ����и�����Ƚϳ��þ��ó�����
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
    ///��table�ṹ�屣�浽csv
    char resultset[4][20] = {
            "ͨ��",
            "�������",
            "��ʱ",
            "����������"
    };
    FILE *fp = fopen("../result.csv", "w");
    fprintf(fp, "ѧ��,������\n");
    for (int i = 0; table[i].id > 0; i++) {
        fprintf(fp, "%d,%s\n", table[i].id, resultset[table[i].result]);
    }
    return 0;
}

int getFileLength(FILE *fp) {
    ///��ȡ�ļ�����
    fseek(fp, 0, SEEK_END);
    int size = (int) ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

int iserror() {
    ///��ȡerror�ļ����ж��Ƿ����д���
    FILE *fp = fopen("../error", "r");
    int size = getFileLength(fp);
    fclose(fp);
    return (bool) size;
}

int compile(const char *cname) {
    ///���뺯��������gcc���б���
    char cmd[100];
    sprintf(cmd, "gcc ../inputfiles/%s -o ../testbin/a.exe  2> ../error", cname);
    system(cmd);
    return iserror();
}

int exefile(int &line, const char *exename, const char *inputname) {
    ///ִ�б�����
    char cmd[100];//������������
    sprintf(cmd, R"(..\testbin\%s)", exename);//������������
    char input[Maxline*Maxchar] = "";//��ȡ�����ļ�����
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
    in_buffer=input;//�����뽻��process����
    int error=run(cmd);
    line = 0;//��������
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
    ///�Ƚϱ�׼�𰸺ͳ������н�� �����ֱ�Ϊ�������н�������������ȶԼ���
    FILE *fp2 = fopen(f2, "r");
    char text[Maxline][Maxchar];
    char buf[Maxchar];
    int fp2line = 0;
    while (fgets(buf, Maxchar, fp2) != NULL) {
        strcpy(text[fp2line], split(buf,"\n",0));
        fp2line++;
        //���������н�����зֿ�
    }
    printf("\n%d %d\n############", fp2line, fp1line);
    if (fp1line != fp2line)//�����������һ���ǿ϶�������
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
            printf("�������,�˳�����\n");
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
        printf("�������\n");
        return 1;
    }
    int line;
    int exeerror = exefile(line, "..\\testbin\\a.exe", "..\\in");

    if (exeerror) {
        table[num].id = id;
        table[num].result = 2;
        printf("��ʱ\n");
        return 2;
    }
    int resulterror = compare("..\\answer", line, mod);
    if (resulterror) {
        table[num].id = id;
        table[num].result = 3;
        printf("�������\n");
        return 3;
    }
    table[num].id = id;
    table[num].result = 0;
    printf("ͨ��\n");
    return 0;
}


int main() {
    int mod;
    puts("������ȶԼ���");
    puts("1.�ϸ�ȶ�\n2.������˳��");
    scanf("%d",&mod);
    dir("..\\inputfiles");
    for(int i=0;i<filenum;i++) {
        puts(files[i]);
        work(files[i],mod);
    }
    csvsave();
}
