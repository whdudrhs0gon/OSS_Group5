#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    FILE *fp;
    fp = fopen(argv[1],"r");

    char *temp;
    char *retstr;
    int lenght =0;
    while (fgets(temp, 255, fp) != 0x00) {
        retstr = (char*) malloc( strlen(temp) );
        strncpy(retstr, temp, strlen(temp));
        printf("%s", retstr);
    }    
    printf("\ndone\n");

}