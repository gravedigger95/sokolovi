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
#define DEFAULT_PORT   27015
#define BUF 128
#define TOT 10
#define filename "/home/pi/Documents/ra193/ORMDNS/lab_4/src/lista.txt"
struct klijenti{
	char ip[100];
	char ime[100];
};

int main(int argc , char *argv[])
{

    int socket_desc , client_sock , c ;
    struct sockaddr_in server , client;
    struct klijenti klijent;
    int pid;
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

/*char line[TOT][BUF];
int i=0;
int total=0;
while(fgets(line[i], BUF, fp)) {

        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    total = i;

    for(i = 0; i < total; ++i)
        printf("%s\n", line[i]);
fclose(fp);
*/

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
	while(1){

    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
	    puts("Connection accepted");
		pid = fork();
		
		if(pid < 0)
		{
			perror("Error on fork");
			return(1);
		}
		
		if(pid == 0)
		{

			close(socket_desc);
			doprocessing(client_sock,ip_s,names,total);
			
			return(0);
		}
		else{
			close(client_sock);
		}
	}
	
	close(socket_desc);

    return 0;
	
}

void doprocessing(int client_sock,char ip_s[][BUF],char names[][BUF],int total){
	int read_size;
	char client_message[DEFAULT_BUFLEN];
	    //Receive a message from client
	int k;
	char* poruka="Unesi ponovo sajt\n";
int n=0;




    while( (read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Bytes received: %d\n", read_size);
		*(client_message + read_size) = '\0';
		//printf("Messagee: %s\n", client_message);
	n=0;
	for(k=0;k<total;k++){
		if(strcmp(client_message,names[k])==0){
		printf("%s\n",ip_s[k]);
		if((n=send(client_sock,ip_s[k]+'\0',strlen(ip_s[k]),0))<0){
		puts("Send failed\n");
	return 1;
}
		printf("%d\n",n);
		
} 
}


if(n==0){
printf("Nije uneo vazeci sajt\n");
	
	if(send(client_sock,poruka,strlen(poruka),0)<0){
	puts("Send failed\n");
	return 1;
}
}





    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

}
}
/*
void upisUFajl(FILE *in, char ime[], char ip[]){
	while(fscanf("%s %s")!=EOF)	
	fprintf(in,"%s %s\n",ime,ip);
}
*/
