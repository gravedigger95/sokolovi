/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2016/2017
    Semestar:       Zimski (V)
    
    Ime fajla:      client.c
    Opis:           TCP/IP klijent
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>      //printf
#include<stdlib.h>
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int main(int argc , char *argv[])
{
    int sock;
int num;
char buff[DEFAULT_BUFLEN];

    struct sockaddr_in server;
    char *message;
	int messLen = 0;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("192.168.0.25");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");
	
	while(1) {
		puts("Enter a message:");
		char *c = (char *)malloc(1);

		message = (char *)malloc(1);
		memset(message, '\0', 1);
		messLen = 0;

		while( read(0, c, 1) > 0) {
			if( *c == '\n') break;
			message = (char *)realloc(message, messLen + 1);
			messLen++;
			strcat(message, c);
		}

		

		//Send some data
		if( send(sock , message , strlen(message), 0) < 0)
		{
			puts("Send failed");
			return 1;
		}

if((num = recv(sock, buff, DEFAULT_BUFLEN,0))>0){

    buff[num] = '\0';

    printf("Message received: %s\nNumber of bytes received: %d\n", buff,num);

}
bzero(buff,sizeof(buff));


		puts("Client message:");
		puts(message);
		free(message);
	}
    close(sock);

    return 0;
}

