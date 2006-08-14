/******** Echo Parse V1.0 ********************
          by KA1JJM 2/03
**********************************************/
/* inputs: 1st is node number, second is variable number */
#include "stdio.h"

main(argc,argv)
int argc;
char **argv;
{
char finline[200];
char saveline[200];
char parstr[8][40];
char nodenum[40];
char ipaddr [40];
char searchstr[60];
FILE *fp;
int x,j;
int fullline=0;
strcpy(nodenum,argv[1]);

if (argc<3) {puts("Not enough parameters"); exit(0); }
if (!strcmp(argv[2],"L")) {
   fullline=1;
   goto notest;
}

x=atoi(argv[2]);
/* printf("Node=%s param=%s\n",nodenum,argv[2]); */
if (x<1 || x >4) { puts("Illegal Var Number"); exit(4); }

notest:
fp=fopen("/home/irlp/custom/echoLinux/echo_hosts","r");

if (fp==NULL) {
    exit(3);
}

while (!feof(fp)) {

      fgets(finline,100,fp);
      strcpy(saveline,finline);
      /* change the spaces to = temporarily so sscanf behaves ***/
      for (j=0; j<strlen(finline); j++) {
         if (finline[j]==0x20) finline[j]='='; 
      }

      sscanf(finline,"%s %s %s %s %s",parstr[1],parstr[2],parstr[3],parstr[4],parstr[5]);
      for (j=0; j<strlen(parstr[2]); j++) {
         if (parstr[2][j]=='=') parstr[2][j]=0x20; 
      }
      for (j=0; j<strlen(parstr[3]); j++) {
         if (parstr[3][j]=='=') parstr[3][j]=0x20; 
      }
      if (!strcmp(nodenum,parstr[3])) {
         if (fullline) {
	    printf("%s",saveline); }
         else { printf("%s\n",parstr[x]);}
            exit(1);
         }
      }
  puts("Not Found");
}
