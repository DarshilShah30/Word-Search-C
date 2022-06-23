/*
SUBMITTED BY:

(1) Darshil Hirenkumar Shah (110059176)
(2) Mansi Hitendrakumar Brahmbhatt (110068455)
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

void endMessage(int* server){
	 int converted_number= htonl((uint32_t)-1);
	 int d=write(*server, &converted_number, sizeof(converted_number));
	 printf("filename %d wrote %d\n", converted_number,d);
}

void child(int);

int main(int argc, char *argv[]){
  int sd, client, portNumber, status;
  struct sockaddr_in servAdd;      // client socket address

 if(argc != 2){
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Cannot create socket\n");
    exit(1);
  }
  sd = socket(AF_INET, SOCK_STREAM, 0);
  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);

  bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
  listen(sd, 5);

  while(1){
    printf("Waiting for a client on port %d \n", portNumber);
    client = accept(sd, NULL, NULL);
    printf("Got a client, start chatting\n");

    if(!fork())
      child(client);

    close(client);
    waitpid(0, &status, WNOHANG);
  }
}

void child(int sd){
	int messagec;
	int n, pid;
	char filename[255];
	char pattern[255];
	int counter=0;
	  while(1){
	   if(n=read(sd, &messagec, sizeof(messagec))){

	     if(messagec<0){
	    	 filename[counter]='\0';
	    	 break;
	     }else{
	    	 filename[counter]=(char)ntohl(messagec);
	     }
	     counter++;
	   }
	  }
	  printf("%s\n",filename);
	  //get pattern
		counter=0;
		  while(1){
		   if(n=read(sd, &messagec, sizeof(messagec))){

		     fprintf(stderr,"%d %d\n",messagec, ntohl(messagec));
		     if(messagec<0){
		    	 pattern[counter]='\0';
		    	 break;
		     }else{
		    	 pattern[counter]=(char)ntohl(messagec);
		     }
		     counter++;
		   }
		  }
		  printf("pattern:%s\n",pattern);
	  //create file
	  FILE * fp;
	  	 fp = fopen (filename, "w+");

	  	if(!fp){
	  		perror("Error");
	  	}

	  	 //send file
	  	   while(1) {
	  		   if(n=read(sd, &messagec, sizeof(messagec))){
	  		     if(messagec<0){
	  		    	 break;
	  		     }else{
	  		    	 fputc(ntohl(messagec),fp);
	  		     }
	  		   }
	  	   }
	  	 fclose(fp);
	  	 //perform grep
	  	 int grepid;
		   int status;
	  	 if(grepid=fork()){
	  		 wait(&status);
	  	 }else{
	  		 int origout=dup(STDOUT_FILENO);
	  		 dup2(sd,1);

	  		 char command[300];
	  		 sprintf(command,"grep -w --color=always %s %s /dev/null",pattern,filename);
	  		 printf("here %s",command);
	  		 int res=system(command);
	  		endMessage(&sd);
	  		 close(sd);
	  		 dup2(origout,1);
	  		 close(origout);
	  	 }
}
