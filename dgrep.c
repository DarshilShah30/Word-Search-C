/*
SUBMITTED BY:

(1) Darshil Hirenkumar Shah (110059176)
(2) Mansi Hitendrakumar Brahmbhatt (110068455)
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#define PATTERNIDX 3
#define LOCALFILEIDX 4
#define SERVERFILEIDX 5
//./dgrep 137.207.82.52 5000 Hello file1.txt file2.txt


void endMessage(int* server){
	 int converted_number= htonl((uint32_t)-1);
	 int d=write(*server, &converted_number, sizeof(converted_number));
}

int main(int argc, char *argv[]){
  char message[255];
  int server, portNumber, pid, n;
  struct sockaddr_in servAdd;     // server socket address

 if(argc != 6){
    printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    exit(0);
  }

  if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
     fprintf(stderr, "Cannot create socket\n");
     exit(1);
  }

  servAdd.sin_family = AF_INET;
  sscanf(argv[2], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);

  if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){
  fprintf(stderr, " inet_pton() has failed\n");
  exit(2);
}

 if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
  fprintf(stderr, "connect() has failed, exiting\n");
  exit(3);
 }

 int c;
 int converted_number;
 for (int i = 0; i < strlen(argv[SERVERFILEIDX]); ++i) {
	 converted_number= htonl((int)argv[SERVERFILEIDX][i]);
	 int d=write(server, &converted_number, sizeof(converted_number));
 }
 endMessage(&server);
 //send pattern
 for (int i = 0; i < strlen(argv[PATTERNIDX]); ++i) {
	 converted_number= htonl((int)argv[PATTERNIDX][i]);
	 int d=write(server, &converted_number, sizeof(converted_number));
 }
 endMessage(&server);
 FILE * fp;
	 fp = fopen (argv[SERVERFILEIDX], "r");

	if(!fp){
		perror("Error");
	}

	 //send file
	   while(1) {
	      c = fgetc(fp);
	      if( feof(fp) ) {
	         break ;
	      }
	      converted_number= htonl(c);
	      int d=write(server, &converted_number, sizeof(converted_number));
	  	if(!d){
	  		perror("Error");
	  	}
	   }
	 fclose(fp);
	 endMessage(&server);
	 //get local result
	 char command[300];
	 sprintf(command,"grep -w --color=always %s %s /dev/null",argv[PATTERNIDX],argv[LOCALFILEIDX]);
	 int res=system(command);
	 //get server result
	  while(1)                         // reading server's messages
		if(n=read(server, message, 1)){
			if((int)message[0]<0){
				break;
			}
		   message[n]='\0';
		   printf("%s", message);
		  }
}

