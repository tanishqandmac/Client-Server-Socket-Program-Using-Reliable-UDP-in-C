#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <inttypes.h>
#include <time.h>
#define _POSIX_C_SOURCE 200809L
#include <math.h>
#include <unistd.h>

long int timeDiffThreshold = 100000; //100000 = 1 second
int totalPackets = 1000;
int packetSize = 1000;

struct Task
{
    int id;
	long long int timer;
    char data[1001];
};

long long int currentTime(void)
{
    struct timespec tms;
    long long int k;
    if (clock_gettime(CLOCK_REALTIME,&tms)) {
        return -1;
    }
    int64_t micros = tms.tv_sec * 1000000;
    micros += tms.tv_nsec/1000;
    if (tms.tv_nsec % 1000 >= 500) {
        ++micros;
    }
    k = (long long int)micros;
    return k;
}

int timerExpired(long long int t) {
    long long int currentTimer = currentTime();
    long timeDiff = llabs(currentTimer - t);
    if (timeDiff > timeDiffThreshold) {
        return 1;
    }
    return 0;
}

void error( char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sock,length,n,i,j,err=0;
    int j1,k;
	k=0;
	char str[1000000];	
    char message[1040];
    
	for (int i = 0; i < 1000000; i++) {										
		str[i]= 'a';																	
    }
	struct sockaddr_in server,from;
	struct hostent *hp;
	if (argc != 4) {	
		printf(" Please enter IP Address and Port No and Window Size\n ");
		exit(1);
	}
	sock = socket(AF_INET,SOCK_DGRAM , 0);
	if (sock < 0){
        error("Error opening socket");
    }
	server.sin_family = AF_INET;
	hp =gethostbyname(argv[1]);
	if (hp==0){		
			error("Unknown Host");
		}
    
	bcopy((char *)hp->h_addr, (char *)&server.sin_addr,hp->h_length);
	server.sin_port = htons(atoi(argv[2]));
	length = sizeof(struct sockaddr_in);
	struct Task regn[1000] ;

    char id1[10];
    int ack,count=0;
    int check = -1;
    int checkcount[totalPackets];
    int errorcheck = 0;
    for(i=0;i<totalPackets;i++){
        checkcount[i] = 0;
    }
    int windowsize =atoi(argv[3]);
    int m;
    int totalSize = totalPackets;
    
    //First N Packets
    for(m=0;m<windowsize;m++)
        {
            regn[m].id = m;
            regn[m].timer = currentTime();
            for(j1=0;j1<1001;j1++)
            {
                if(j1!=1000){
                regn[m].data[j1] = str[k];
                k=k+1;
                }
                else{
                    regn[m].data[j1] = '\0';
                }
            }
            sprintf(message,"%d-%lld-%s",regn[m].id,regn[m].timer,regn[m].data);
            sendto(sock,message,strlen(message),0,(struct sockaddr_in *)&server,length);	
            bzero(message,1040);
            printf("Packet %d Sent!",m);
            printf("\n");
        }
    
    while(count<totalSize && ack!= totalPackets-1){
        n = recvfrom (sock,id1,sizeof(id1),0,(struct sockaddr_in *) &server,&length); 
        ack=atoi(id1);
        printf("Ack Recieved for packet no: %d\n",ack);
       
        if (n>0 && m<totalPackets)
        {
            checkcount[ack] = 1;
            regn[m].id = m;
            regn[m].timer = currentTime();
            for(j1=0;j1<1001;j1++)
            {
                if(j1!=1000){
                    regn[m].data[j1] = str[k];
                    k=k+1;
                }
                else{
                    regn[m].data[j1] = '\0';
                }
            }
            sprintf(message,"%d-%lld-%s",regn[m].id,regn[m].timer,regn[m].data);
            sendto(sock,message,strlen(message),0,(struct sockaddr_in *)&server,length);	
            bzero(message,1040);
            printf("Packet %d Sent!",m);
            printf("\n");
            m= m+1;
        }
        count=count+1;
        for(i=count;i<m;i++){
        if(checkcount[i]==0){
        errorcheck = timerExpired(regn[i].timer);
        if(errorcheck==1){
            printf("Time Out! Packet no %d sent again\n",i);
            sprintf(message,"%d-%lld-%s",regn[i].id,regn[i].timer,regn[i].data);
            sendto(sock,message,strlen(message),0,(struct sockaddr_in *)&server,length);
            totalSize = totalSize +1;
                }
            }
        }
        printf("\n");
    }
}
