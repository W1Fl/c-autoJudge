//
// Created by wifi on 19-2-26.
//

#include <stdio.h>
int main()
{
    int a,b;
    while(1){
        scanf("%d%d",&a,&b);
        if(0==b&&a==0)
            return 0;
        printf("%d\n",a+b);
    }
}