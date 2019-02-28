//
// Created by pipiFan on 2019/2/28.
//

#ifndef UNTITLED1_DIR_H
#define UNTITLED1_DIR_H

#endif //UNTITLED1_DIR_H
#include <string>
#include <cstring>
#include <io.h>
char files[1000][FILENAME_MAX];
int filenum=0;
using namespace std;
void dir(string path)
{
    long hFile = 0;
    struct _finddata_t fileInfo;
    string pathName, exdName;
    // \\* 代表要遍历所有的类型,如改成\\*.jpg表示遍历jpg类型文件
    if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) {
        return;
    }
    do
    {
        if(!(fileInfo.attrib&_A_SUBDIR)){
            strcpy(files[filenum],fileInfo.name);
            filenum++;
            //判断文件的属性是文件夹还是文件
        }
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
}
