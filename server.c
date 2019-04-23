#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

int totalPackets = 1000; // Not Variable (First Change Struct array size)
int packetSize = 1000; //Not Variable

struct Task
{
    int id;
	long long int timer;
    char data[1001];
};

void bsort(struct Task list[totalPackets], int s)
{
    int i, j;
    long int receievercount = 0;
    struct Task temp;
    for (i=0;i<s-1;i++)
    {
        for (j=0;j<(s-1-i);j++)
        {
            if (list[j].id > list[j+1].id)
            {
                temp = list[j];
                list[j] = list[j+1];
                list[j+1] = temp;
            } 
        }
    }
    printf("The message is:\n");
    for (i=0;i<totalPackets;i++){
        printf("%s",list[i].data);
        receievercount = receievercount + strlen(list[i].data);
    }
    printf("\nTotal Length : %ld",receievercount);
    printf("\n");
}

int main(int argc, char *argv[])
{
    struct Task regn[1000];
    int id;
    long long int timer;
    char data[1000];
	int k =0;
	int sock,length,fromlen,n,i,j,err=0;
	char buffer[1040];
    char id1[10];
	struct sockaddr_in server,from;
	if (argc < 2){	
		printf ("Please provide port no \n");
		exit(0);
	}
	sock = socket(AF_INET,SOCK_DGRAM , 0);
	if (sock < 0){
		printf("Socket Creation Failed\n");
	}

	length = sizeof(server);
	bzero(&server,length);
	server.sin_family = AF_INET; 
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port= htons(atoi(argv[1])); 
    int packetcheck[totalPackets];
    for(i=0;i<totalPackets;i++){
        packetcheck[i] = 0;
    }
	if(bind(sock, ( struct sockaddr *) &server, length) < 0)
	{
		printf( "Binding Failed" );
	}

	fromlen = sizeof (struct sockaddr_in);
    while(k<=totalPackets-1)
    {
        bzero(buffer,1040);
        n = recvfrom ( sock ,buffer,1040,0,(struct sockaddr *)&from ,&fromlen);
        if (n<=0)
            {
                printf("data not received\n");
            }
        else{
            char message[1040];
            strcpy(message,buffer);
            char *token = strtok(message, "-");
            id = atoi(token);
            sprintf(id1,"%s",token);
            token = strtok(NULL, "-"); 
            timer = atoll(token);    
            token = strtok(NULL, "-"); 
            
            for(i=0;i<1001;i++){
                data[i] = token[i];
            }
            regn[k].id = id; 
            regn[k].timer = timer;
            strcpy(regn[k].data,data); 
            
            if(packetcheck[id]==0){
                n = sendto(sock,id1,strlen(id1),0,(struct sockaddr *)&from, fromlen);
                if(n>-1){
                    printf("ACK sent for package no %d\n",id);
                }
                packetcheck[id] = 1;
                    k=k+1;
            }
        }
    }
    bsort(regn,totalPackets);
    printf("\n\n\n");

}