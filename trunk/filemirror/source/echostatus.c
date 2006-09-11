/**************************************************
Released under the GNU Public Lincense

Program: echostatus (modification of ilinkserver)
Modified by Marcus Xenakis, WA2DCI
Verion 0.2 Beta 3/15/03

Modifications described after the original authors
header that follows.

I sincerely thank Jeff WD4NMQ for his efforts in
making this and other programs that use the 
ilink/echolink protocol avialable as opensource
and distributed as opensource under the 
GNU Public License.

Make sure that you are a registered EchoLink user
and have prior obtained permission to use this 
software from the EchoLink server adminstrators.

Please read the EchoLink Access Policies found at
http://echolink.org/el/access_policies.htm

Please note the following EchoLink policy item,

The "addressing servers" are designed to be accessed
by EchoLink software only.  For the sake of system
security and stability, any other access requires 
the permission of the server administrators.  
As a matter of security, in no event should the IP 
addresses (or Internet hostnames) of 
logged-in stations be displayed to anyone who 
is not a validated, logged-in user. 

/--- Original iLinkserver Information Header -----/

Author: Jeff Pierce, wd4nmq
Version 0.5 Beta

REVISIONS:
  0.5 Beta : 12/25/02
             fixed a bug that allows logon's to latest
             server code 
  0.3 Beta : as suggested by Ken Adkisson, WB4FAY
             I made location a define also.

WARNING: YOU MUST PUT YOUR PASSWORD, CALLSIGN AND 
         LOCATION  IN THE #define'S BELOW!!!

This is test program to access the Amateur Radio 
iLink servers written by Graeme, M0CSH.
THIS IS NOT A FULL BLOWN ILINK CLIENT PROGRAM!!!!
It is program intended only to test out the accessing
of the iLink servers via a sytem other than a Windows
OS on the TCP 5200 port.

This program was developed after using network sniffing 
tools to determine how the iLink client or sysop apps: 
1. Logged on
2. Logged off
3. Got other station's information, including IP address
4. Informed server when it is and isn't busy.

This program can do all of the above.
Now on to discovering what is going on between connecting
stations on UPD ports 5198 and 5199.

iLink must be made to run on OS's other than Windows, especially
the sysop version. Windows is way to un-stable to run ANY 
unattended app. Especially one that controls a radio.
Many sysops are already reporting Windows crashes. And 
quite a number leaving the radio keyed!!! 

So, until Graeme, m0csh sees fit to release the protocols
for other developements, my work will continue.

Again, this code is rough. I can see areas of improvement,
especially in error catching and recovery and not hard coding
 some station data. But, this is only a program for testing out the 

The underlying iLink protocols:
The Answer Is Out There!

/----------- End of original header ------------------/

Ilinkserver.c Modifications:

The modifications to the original ilinkserver code are
very minor in nature.

1) The defines for CALLSIGN, PASSWORD and LOCATION are
read in from an external data file named "userdata.txt".
This data file is also shared with the "EchoLinux"
program by the original author of this program 
Jeff Pierce, WD4NMQ

The format of this plain text file is four records
check the standard EchoLink Windows client for maximum
field lenghts:

<Your Callsign (don't forget the -L or -R as required)>
<Description>
<Location must be mixed case>
<Your EchoLink Password (UPPERCASE)> 

2) The program was modifed to be called as a batch
program and and paramters are passed on the command
line.  This allows it to be called by other programs 
as well as cron.

3) Screen outout not required in batch mode was removed. 

4) Some interactive only functions were removed.

5) The field spacing on the status output was chaged to 
tabs. This allows easier parsing by other programs and
the Linux "cut" command.

******************************************************/
  
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#define MAXCALL 15
#define MAXDATA 45
#define MAXID   8
#define MAXIP   20

FILE *userData;
char CALLSIGN[20], NAME[40], LOCATION[40], PASSWORD[10];

struct stationData {
  char  call[MAXCALL];
  char  data[MAXDATA];
  char  id[MAXID];
  char  ip[MAXIP];
};

struct entry {
  struct stationData  *station;
  struct entry        *next;
};

struct entry *addEntryList(struct entry *, struct stationData *);

int z;
char *srvr_addr = NULL;
char *connected = NULL;

// struct sockaddr_in adr_srvr;/* AF_INET */
// int len_inet;               /* length  */
// int s;                       /* Socket */
struct servent *sp;   /* Service entry */
/* char dtbuf[128]; */     /* Date/Time info */
char dtbuf[256];
struct hostent *hp;
struct stationData  *station;
struct entry        *linkHead, *repeaterHead, *confHead, *stationHead;
struct entry        *messageHead;
/*
 * This function reports the error and
 * exits back to the shell :
 */
static void bail(const char *on_what) {
  fputs(strerror(errno),stderr);
  fputs(": ",stderr);
  fputs(on_what,stderr);
  fputc('\n',stderr);
  exit(1);
}

/*
 * This function is involked when a ctrl-C is
 * entered from the keyboard to do clean up.
 * Free all allocated memory, etc.
 */

void ctrlC(int sig){

  printf("\nReceived a crtl-C, shutting down.\n");
  exit(1);

}

/*
* Now the main program loop
*/
 
int
main(int argc,char **argv) {

  int   quit = 0;
  char  command[10];

  (void) signal(SIGINT, ctrlC);

  station = NULL;
  linkHead = NULL;
  repeaterHead = NULL;
  confHead = NULL;
  stationHead = NULL;

  userData = fopen("/home/irlp/custom/echoLinux/userdata.txt", "r");
  if(userData == NULL){
    perror("userData");
    exit(1);
  }

  fgets(CALLSIGN, 40, userData);
  CALLSIGN[strlen(CALLSIGN)-1] = 0x00;
  fgets(NAME, 40, userData);
  NAME[strlen(NAME)-1] = 0x00;
  fgets(LOCATION, 40, userData);
  LOCATION[strlen(LOCATION)-1] = 0x00;
  fgets(PASSWORD, 40, userData);
  PASSWORD[strlen(PASSWORD)-1] = 0x00;
  fclose(userData);
  
  /*
   * Use a server address from the command
   * line, if one has been provided.
   * Otherwise, this program will default
   * to using the arbitrary address
   * 127.0.0.1 :
   */
  if ( argc >= 2 ) {
    /* Addr on cmdline: */
    srvr_addr = argv[1];
    hp = gethostbyname(srvr_addr);

    if(hp){
      srvr_addr = inet_ntoa(*(struct in_addr *)hp->h_addr_list[0]);
      printf("IP address for %s is %s..\n", argv[1], 
	     inet_ntoa(*(struct in_addr *)hp->h_addr_list[0]));
      printf("IP address for %s is %s..\n", argv[1],
	     srvr_addr);
    }

  } else {
    /* Use default address: */
    srvr_addr = "127.0.0.1";
  }

  if (strcmp(argv[2], "L") == 0) {
    sendLogon();
    printf("Logon\n");
  }    
  else
  if (strcmp(argv[2], "S") == 0) {
    sendLogon();
    getCalls();
    sendLogoff();
  }    
  else
  if (strcmp(argv[2], "R") == 0) {
    getCalls();
  }    
  else
  if (strcmp(argv[2], "B") == 0) {
    connected = argv[3];
    makeBusy();
  }    
  else  
  if (strcmp(argv[2], "O") == 0) {
    sendLogoff();
  }     
  else
  {
    printf("parameter error\n");    
  }

  return 0;

}

/************************************/

int sendLogon(void){

  struct tm *tm_ptr;
  time_t    theTime;
  char      logTime[6];
  unsigned char    sendBuf[50], recvBuf[50];
  int     i, s, z, result;

  /* Now send the 'l' command */

  s = openSocket(srvr_addr);
  memset(sendBuf, 0x6c, 1);
  result = write(s, sendBuf, 1);

  /* Get the local time */
  (void) time(&theTime);
  tm_ptr = localtime(&theTime);
  strftime(logTime, 6, "%H:%M", tm_ptr);

  strcpy(sendBuf, CALLSIGN);
  strcat(sendBuf,"\254\254");
  strcat(sendBuf, PASSWORD);
  strcat(sendBuf, "\015ONLINE3.38(");
  strcat(sendBuf, logTime);
  strcat(sendBuf, ")\015");
  strcat(sendBuf,LOCATION);
  strcat(sendBuf,"\015");

  result = write(s, sendBuf, strlen(sendBuf));

  /* Now get the result */
  result = read(s, recvBuf, 50);
  if(result < 0){
    perror(" Receive ");
    return(result);
  }
  else {
  printf("Logon successful.\n");
  }

  close(s);

}

/************************************/

int sendLogoff(void){

  unsigned char    sendBuf[50], recvBuf[50];
  int     i, s, result;

  s = openSocket(srvr_addr);

  /* Now send the 'l' command */
  memset(sendBuf, 0x6c, 1);
  result = write(s, sendBuf, 1);

  strcpy(sendBuf, CALLSIGN);
  strcat(sendBuf,"\254\254");
  strcat(sendBuf, PASSWORD);
  strcat(sendBuf,"\015OFF-V3.40\015");
  strcat(sendBuf, LOCATION);
  strcat(sendBuf, "\015");

  result = write(s, sendBuf, strlen(sendBuf));

  /* Now get the result */
  result = read(s, recvBuf, 50);
  if(result < 0){
    perror(" Receive ");
    return(result);
  }
  else {
    printf("Logoff successful");
  }

  close(s);

}
/************************************/

int makeBusy(void){

  struct tm *tm_ptr;
  time_t    theTime;
  char      logTime[6];
  unsigned char    sendBuf[50], recvBuf[50];
  int     i, s, result;

  s = openSocket(srvr_addr);

  /* Now send the 'l' command */
  memset(sendBuf, 0x6c, 1);
  result = write(s, sendBuf, 1);

  /* Get the local time */
  (void) time(&theTime);
  tm_ptr = localtime(&theTime);
  strftime(logTime, 6, "%H:%M", tm_ptr);

  strcpy(sendBuf, CALLSIGN);
  strcat(sendBuf,"\254\254");
  strcat(sendBuf, PASSWORD);
  strcat(sendBuf,"\015BUSY3.40(");
  strcat(sendBuf, logTime);
  strcat(sendBuf, ")\015");
  strcat(sendBuf,"In Conference ");
  strcat(sendBuf,connected);

  strcat(sendBuf,"\015");

  result = write(s, sendBuf, strlen(sendBuf));

  /* Now get the result */
  result = read(s, recvBuf, 50);
  if(result < 0){
    perror(" Receive ");
    return(result);
  }
  else {
  printf("Received %d bytes.\n", result);
    printf("Make Busy successful ");
  }

  close(s);

}

/************************/
/**** Open a socket *****/

int  openSocket(char *addr){

  int  sck;
  struct sockaddr_in adr_srvr;/* AF_INET */
  int len_inet;               /* length  */

  memset(&adr_srvr,0,sizeof adr_srvr);
  adr_srvr.sin_family = AF_INET;
  adr_srvr.sin_port = htons(5200);
  adr_srvr.sin_addr.s_addr =
    inet_addr(addr);

  if ( adr_srvr.sin_addr.s_addr == INADDR_NONE )
    bail("bad address.");

  len_inet = sizeof adr_srvr;

  /*
   * Create a TDP/IP socket to use :
   */
  sck = socket(PF_INET,SOCK_STREAM,0);
  if ( sck == -1 )
    bail("socket()");
  /*
   * Connect to the server:
   */
  printf("attempting to connect to %s.\n", srvr_addr);
 
  z = connect(sck, (struct sockaddr *) &adr_srvr,len_inet);
  if ( z == -1 )
    bail("connect(2)");

  printf("Connection a success!!!\n");

  return(sck);

}

/***** Get calls function **********/

int getCalls(void){

  int  s, result, i, j, numRecs;
  unsigned char sendBuf[10], recvBuf[2000];
  char *ptr, *eptr;
  char temp[4][80];
  FILE *tx, *rx;

  s = openSocket(srvr_addr);

  rx = fdopen(s, "r");
  if(rx ==NULL){
    perror("Openning rx stream");
    close(s);
    return(-1);
  }

  tx = fdopen(dup(s), "w");
  if(tx == NULL){
    perror("Openning tx stream");
    fclose(rx);
    return(-1);
  }

  /* Now send the 's' command */
  result = fputc('s', tx);
  if(result < 0){
    perror(" Send the s command ");
    fclose(tx);
    fclose(rx);
    return(-1);
  }
  fflush(tx);

  /* Now read the response */
  result = fscanf(rx, "%s\n", temp[0]);

  /* Now get the number of following records */

  result = fscanf(rx, "%d\n", &numRecs);

  for(i=0;i<numRecs;i++){
    station = malloc(sizeof (struct stationData));
  
    fgets(station->call, MAXCALL, rx);
    station->call[strlen(station->call)-1] = 0x00;
    fgets(station->data, MAXDATA, rx);
    station->data[strlen(station->data)-1] = 0x00;
    fgets(station->id, MAXID, rx);
    station->id[strlen(station->id)-1] = 0x00;
    fgets(station->ip, MAXIP, rx);
    station->ip[strlen(station->ip)-1] = 0x00;
    printf("%s\t%s\t%s\t%s\n", station->call, station->data,
           station->id, station->ip);
    
  }
  printf("There are %d stations online.\n", numRecs);

  fclose(tx);
  fclose(rx);
}

