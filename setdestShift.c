/*
 * NS2のsetdestで作成するシナリオファイルの座標をシフトする
 * Grid operate directorは削除される
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX1LINE 256

int main(int argc, char *argv[]){

    if(argc != 5){
        fprintf(stderr, "Usage : %s inputFileName NodeNumShift X-coordinate Y-coordinate\n", argv[0]);
        return 1;

    }

    FILE *fp, *fp2, *fp3;
    char inputFilename[128];
    strcpy(inputFilename,argv[1]);

    char outputFilename[128];
    sprintf(outputFilename,"%s_out",inputFilename);

    char str[MAX1LINE];

    fp = fopen(inputFilename, "r");
    fp2 = fopen("temp", "w");

    double shift_Node = atoi(argv[2]);
    double shift_X = atof(argv[3]);
    double shift_Y = atof(argv[4]);


    //file open check
    if(fp == NULL) {
        printf("%s file not open!\n", inputFilename);
        return -1;
    }
    else{
        printf("inputfilename: %s\n",inputFilename);
    }
    if(fp2 == NULL) {
        printf("temp file not open!\n");
        return -1;
    }
    else{
        printf("temp file open\n");
    }

    int count = 0;
    int i = 0;
    char *addr, *addr2, *addr3;
    char *addr_front, *addr_x, *addr_y, *addr_v;
    char addr_after[MAX1LINE];
    double value = 0;
    double value_x = 0, value_y = 0, value_v = 0;
    bool isEditFlag;
    int nodeNum = 0, numLen=0;

    char buf[8];
    char buf2[MAX1LINE];

    while(fgets(str, MAX1LINE, fp) != NULL) {
        isEditFlag = false;

        //初期X座標のシフト
        //setXを探す
        addr = NULL;
        addr2 = NULL;
        addr = strstr(str,"set X_");

        if(addr != NULL) {
            printf("%d:",count);
            for(i = 0; i < strlen(addr); i++){
                printf("%c",addr[i]);
            }
            //Xを探す
            addr2 = strchr(addr, (int)'X');
            if(addr2 != NULL){
                printf("%s",addr2 + 3);
                value = atof(addr2 + 3);

                printf("double:%.12f\n",value);
                value += shift_X;

                printf("shiftX= %.12f\n",value);
                snprintf(addr2 + 3, 20, "%.13f", value);
                fprintf(stdout,"%s",str);
                //fprintf(fp2,"%s",str);
                str[strlen(str)-1] = '\n';
            }
            isEditFlag = true;
        }

        //初期Y座標のシフト
        addr = NULL;
        addr = strstr(str,"set Y_");
        if(addr != NULL) {
            printf("%d:",count);
            for(i = 0; i < strlen(addr); i++){
                printf("%c",addr[i]);
            }
            //Yを探す
            addr2 = strchr(addr, (int)'Y');
            if(addr2 != NULL){
                printf("%s",addr2 + 3);
                value = atof(addr2 + 3);

                printf("double:%.12f\n",value);
                value += shift_Y;

                printf("shiftX= %.12f\n",value);
                snprintf(addr2 + 3, 20, "%.13f", value);
                fprintf(stdout,"%s",str);
                //fprintf(fp2,"%s",str);
                str[strlen(str)-1] = '\n';
            }
            isEditFlag = true;
        }
        //Zも初期配置しておく
        addr = NULL;
        addr = strstr(str,"set Z_");
        if(addr != NULL) {
            isEditFlag = true;
        }

        //コメントもコピー
        addr = NULL;
        addr = strstr(str,"#");
        if(addr != NULL) {
            isEditFlag = true;
        }

        //シミュレーション開始後setdestのシフト
        addr = NULL;
        addr = strstr(str,"setdest");
        addr_x = NULL;
        addr_y = NULL;
        if(addr != NULL) {
            printf("start:%s",str);

            addr_x = addr + 8;
            printf("addrX:%s\n", addr_x);

            value_x = atof(addr + 8);
            printf("value_x:%.12f\n", value_x);
            value_x += shift_X;
            //snprintf(addr + 8, strlen(addr + 8), "%.13f", value);


            value = 0;
            for(i = 8; ;i++){
                if(addr[i] == ' '){
                    i++;
                    break;
                }
            }
            addr_y = addr + i;
            printf("addrY:%s\n", addr_y);
            value_y = atof(addr_y);
            printf("value_y:%.12f\n", value_y);
            value_y += shift_Y;


            int xlen = addr_y - addr_x - 1;
            printf("Xlen:%d\n", xlen);

            for(i = 8 + xlen + 1; ;i++){
                if(addr[i] == ' '){
                    i++;
                    break;
                }
            }
            printf("v:%s\n", addr + i);
            addr_v = addr + i;
            value_v = atof(addr_v);

            int ylen = addr_v - addr_y - 1;
            printf("%s:%d\n",addr_y, ylen);

            printf("%f %f %f\n",value_x, value_y, value_v);

            snprintf(addr_x, 50, "%f %f %f\"\n", value_x,value_y,value_v);
            printf("@:%s\n",str);
            //fprintf(fp2,"%s",str);
            isEditFlag = true;

        }

        printf("\n---\n");

        //ファイルに出力
        if(isEditFlag == true) {
            fprintf(fp2, "%s", str);
        }


        if(count >=40){
            //break;
        }
        count++;

    }

    fclose(fp); // ファイルを閉じる
    fclose(fp2);

    //node番号を書き換える
    fp2 = fopen("temp", "r");
    if(fp2 == NULL) {
        printf("temp file not open!\n");
        return -1;
    }
    else{
        printf("temp file open\n");
    }

    fp3 = fopen(outputFilename, "w");
    if(fp3 == NULL) {
        printf("%s file not open!\n", inputFilename);
        return -1;
    }
    else{
        printf("inputfilename: %s\n",inputFilename);
    }

    while(fgets(str, MAX1LINE, fp2) != NULL) {
        isEditFlag = false;

        addr = NULL;
        addr2 = NULL;
        addr3 = NULL;
        addr = strstr(str, "node_(");
        if (addr != NULL) {
            printf("---\n");
            printf("%s",str);
            printf("%s",addr);

            addr2 = &addr[6];
            printf("%s",addr2);
            printf("---\n");
            // printff("%c")
            if (addr2 != NULL) {
                addr3 = strchr(addr2, (int) ')');
                numLen = addr3 - addr2;
                printf("num length:%d\n", numLen);

                //")"以降をbuf2にコピー
                strcpy(buf2,addr3);
                printf("%s",addr3);
                printf("%s",buf2);

                //ノード番号を加算
                nodeNum = atoi(addr2);
                nodeNum += shift_Node;
                printf("nodeNum:%d\n", nodeNum);

                //加算した後の桁数を求める
                /*snprintf(buf, 8, "%d", nodeNum);
                numLen = strlen(buf);*/
                printf("numlen:%d\n", numLen);
                printf("addr2:%s",addr2);
                printf("addr3:%s",addr3);

                snprintf(addr2, 100, "%d%s", nodeNum,buf2);
                printf("addr2:%s",addr2);
                str[strlen(str)-1] = '\n';

                printf("\nend\n");
                printf("str:%s",str);

            }
            isEditFlag = true;
        }

        //コメントもコピー
        addr = NULL;
        addr = strstr(str,"#");
        if(addr != NULL) {
            isEditFlag = true;
        }

        //ファイルに出力
        if(isEditFlag == true) {
            fprintf(fp3, "%s", str);
        }
    }


    return 0;
}

