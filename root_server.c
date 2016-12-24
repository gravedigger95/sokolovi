/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2016/2017
    Semestar:       Zimski (V)
    
    Ime fajla:      server.c
    Opis:           TCP/IP server
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27014
#define BUF 128
#define TOT 20
#define filename "/home/pi/Documents/ra193/ORMDNS/lab_4/src/rootlista.txt"
struct klijenti{
	char ip[100];
	char ime[100];
};
int main(int argc , char *argv[])
{
	int in_list;
	char* poruka="Ne postoji sajt ili IP u listi";
    int socket_desc , server_sock , c , read_size;
    struct sockaddr_in server , root_server;
  struct klijenti klijent;
    char server_message[DEFAULT_BUFLEN];
	char str[80];
int k;
   int n=0;
FILE *fp=fopen(filename,"r");
if (fp == NULL) 
{
    printf("File not found!\n");
    return NULL;
}
else 
{
    printf("Found file %s\n", filename);
}

int i=0;
char ip_s[TOT][BUF];
char names[TOT][BUF];
while(fscanf(fp,"%s %s\n",klijent.ime,klijent.ip)!=EOF){
	ip_s[i][strlen(klijent.ip)-1]='\0';
	strcpy(ip_s[i],klijent.ip);
	names[i][strlen(klijent.ime)-1]='\0';
	strcpy(names[i],klijent.ime);
	i++;
}
int total=i;
fclose(fp);

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);
	
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    server_sock = accept(socket_desc, (struct sockaddr *)&server, (socklen_t*)&c);
    if (server_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    //Receive a message from client
    while( (read_size = recv(server_sock , server_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
	printf("Zasto nikad ne odstampas nesto");
        printf("Bytes received: %d\n", read_size);
		*(server_message + read_size) = '\0';
		printf("Messagee: %s\n", server_message);
		in_list=0;
		printf("%d\n",in_list);
		for(k=0;k<total;k++){
		if(strcmp(server_message,names[k])==0){
		strcpy(str,names[k]);
	strcat(str," ");		
strcat(str,ip_s[k]);
		printf("%s\n",str);
		in_list=1;
		printf("%d\n",in_list);
if((n=send(server_sock,str,strlen(str),0))<0){
		puts("Send failed\n");
	return 1;
}
		
	}
printf("Prolazis li prvi if\n");
	
if(strcmp(server_message,ip_s[k])==0){
		strcpy(str,names[k]);
	strcat(str," ");		
strcat(str,ip_s[k]);
		printf("%s\n",str);
		in_list=1;
		printf("%d\n",in_list);
if((n=send(server_sock,str,strlen(str),0))<0){
		puts("Send failed\n");
	return 1;
}
		
	}
printf("Prolazis li drugi if\n");
	
}

printf("prolazis li for???\n");
printf("%s\n",poruka);
if(in_list==0){
if((n=send(server_sock,poruka,strlen(poruka),0))<0){
		puts("Send failed\n");
	return 1;

}
printf("Klijent je uneo glupost");
}
    }

    if(read_size == 0)
    {
        puts("Server disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

	close(socket_desc);

    return 0;
}

