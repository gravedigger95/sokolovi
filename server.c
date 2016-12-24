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
#define TOT 20
#define filename "/home/pi/Documents/ra193/ORMDNS/lab_4/src/lista.txt"
#define filename1 "/home/pi/Documents/ra193/ORMDNS/lab_4/src/rootlista.txt"
struct klijenti {
	char ip[100];
	char ime[100];
};

int main(int argc, char *argv[]) {

	int socket_desc, client_sock, c, root_sock;
	struct sockaddr_in server, client, root_server;
	int pid;
	int n = 0;

	//Create socket

	root_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (root_sock == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(DEFAULT_PORT);

	root_server.sin_addr.s_addr = inet_addr("192.168.0.25");
	root_server.sin_family = AF_INET;
	root_server.sin_port = htons(27014);

	if (connect(root_sock, (struct sockaddr *) &root_server,
			sizeof(root_server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}

	puts("Connected\n");

	//Bind
	if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(socket_desc, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	while (1) {

		client_sock = accept(socket_desc, (struct sockaddr *) &client,
				(socklen_t*) &c);
		if (client_sock < 0) {
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");
		pid = fork();

		if (pid < 0) {
			perror("Error on fork");
			return (1);
		}

		if (pid == 0) {

			close(socket_desc);
			doprocessing(client_sock, root_sock);

			return (0);
		} else {
			close(client_sock);

		}
	}
	close(root_sock);
	close(socket_desc);

	return 0;

}

void doprocessing(int client_sock, int root_sock) {
	int read_size;
	char client_message[DEFAULT_BUFLEN];
	//Receive a message from client
	int k;
	char buff[100];
	char line[100];
	int x, y;
	int flag = 0;
	char* ptr;
	char* poruka = "Ne mogu da razresim upit.\n Saljem korenskom...\n";
	int n = 0;
	int num;
	int total;
	int xyz;
	int i = 0;
	char ip_s[TOT][BUF];
	char names[TOT][BUF];
	int nje;
	int flag_ip, flag_name;
		char* poruka1="Ne postoji sajt ili IP u listi";
	struct klijenti klijent;
		
	while ((read_size = recv(client_sock, client_message, DEFAULT_BUFLEN, 0))
			> 0) {	
i=0;
		FILE *fp = fopen(filename, "r");
		printf("STAMPAM LISTU\n\n");
		while (fscanf(fp, "%s %s\n", klijent.ime, klijent.ip) != EOF) {
			ip_s[i][strlen(klijent.ip)] = '\0';
			strcpy(ip_s[i], klijent.ip);
			names[i][strlen(klijent.ime)] = '\0';
			strcpy(names[i], klijent.ime);
			printf("%s\n",names[i]);
			printf("%s\n",ip_s[i]);
			i++;
		}
	
printf("\nSkenirao fajl i upisao u niz\n");
	total = i;
		fclose(fp);
	flag_ip=0;
	flag_name=0;
		printf("Bytes received: %d\n", read_size);
		*(client_message + read_size) = '\0';
		printf("Messagee: %s\n", client_message);
		n = 0;
		for (k = 0; k < total; k++) {
			if (strcmp(client_message, names[k]) == 0) {
				
				printf("Sajt koji si uneo se nalazi u listi(strcmp):\n");
				printf("%s\n",client_message);
				printf("%s\n",names[k]);
				printf("IP tog sajta je:\n");				
				printf("--------------\n");
				printf("%s", ip_s[k]);
				printf("\t%d\n",strlen(ip_s[k]));
				printf("------------\n");
				
				if ((n = send(client_sock, ip_s[k], strlen(ip_s[k]), 0))
						< 0) {
					puts("Send failed\n");
					return 1;
				} 

				printf("Vratio sam ip unetog sajta klijentu\n");

				printf("%d\n", n);
				break;

			}
			

			 if(strcmp(client_message,ip_s[k])==0){
				printf("IP koji si uneo se nalazi u listi(strcmp):\n");
				printf("Sajt je:\n");				
				printf("%s\n",names[k]);
			 if((n=send(client_sock,names[k],strlen(names[k]),0))<0){
			 puts("Send failed\n");
			 return 1;
			 }
			printf("Vratio sam naziv sajta klijentu\n");
			 printf("%d\n",n);
			 
			 }
			 
		}

		if (n == 0) {
			printf("Sajt ili IP se ne nalazi u lokalnoj listi!!!\n");
			if (send(root_sock, client_message, strlen(client_message), 0)
					< 0) {
				puts("Send failed");
				return 1;
			}
			printf("Saljem korenskom\n");
			
			
			if ((num = recv(root_sock, buff, sizeof(buff), 0)) != -1) {
				buff[num]='\0';
				puts(buff);
				puts(poruka1);
				if((xyz=strcmp(buff,poruka1))!=0){
				printf("%d\n",xyz);
				buff[num] = '\0';
				printf("Korenski mi vraca celu liniju sa sajtom i IPem\n");
				printf("Message received: %s\nNumber of bytes received: %d\n",
						buff, num);
				




							FILE *fp4 = fopen(filename, "r");
					printf("\nSTAMPAM LISTU PRE UPISA\n\n");
				while (fgets(line, sizeof(line), fp4) != NULL) {
					flag = 0;
					printf("%s\n",line);					
					if (strcmp(buff, line) == 0) {
						flag++;
						break;
					}

				}
				fclose(fp4);
				printf("Iscitao sam fajl i proverio da li je sajt vec u listi\n");

				if (flag == 0) {
					flag_name=0;
					flag_ip=0;			
					printf("Sajt nije u listi\n");
									FILE *fp2 = fopen(filename, "a+");
					fprintf(fp2, "%s\n", buff);
					fclose(fp2);
							FILE *fp3 = fopen(filename, "r");
								printf("\nSTAMPAM LISTU POSLE UPISA\n\n");
						while (fgets(line, sizeof(line), fp3) != NULL) {
					printf("%s\n",line);					

				}

					fclose(fp3);
					printf("Unesen je sajt u bazu\n");
					strtok_r(buff, " ", &ptr);
					puts(client_message);
					puts(buff);
					puts(ptr);
					printf("%d\n",strlen(ptr));
					printf("%d\n",strlen(client_message));					
					printf("Razbijam liniju od korenskog na ip i naziv\n");
					if(strcmp(client_message,buff)==0){
						flag_name=1;
						printf("Klijent je uneo sajt...trazi IP nazad\n");
					}
					
					if(strcmp(client_message,ptr)==0){
						flag_ip=1;
						printf("Klijent je uneo IP...trazi sajt nazad\n");
					}
					//puts("evo ti sajt:");
					 //puts(buff);
					 //puts("evo ti ip:");
					 //puts(ptr);
				printf("FLAG_IP: %d\n", flag_ip);
		printf("FLAG_NAME: %d\n", flag_name);
					 if(flag_name==1){

					 puts("Saljem IP\n");
					 
					if ((send(client_sock, ptr, strlen(ptr), 0)) < 0) {
						puts("Send failed\n");
						return 1;
					}
						printf("Poslao sam klojentu IP\n");
					} else if(flag_ip==1){
					 puts("Saljem ime\n");

					 if((send(client_sock,buff,strlen(buff),0))<0)
					 {

					 puts("Send failed\n");
					 return 1;
					 }
						printf("Poslao sam klijentu sajt\n");

					 }else {
						printf("INVALID INPUT\n");
						 if((send(client_sock,"Invalid!!!\n",strlen("Invalid!!!\n"),0))<0)
					 {

					 puts("Send failed\n");
					 return 1;
					 }
		
						}
					 
				} else {
					printf("Sajt vec postoji\n");
				}

				

				bzero(buff, sizeof(buff));
			}else{
		 if((send(client_sock,"Invalid!!!\n",strlen("Invalid!!!\n"),0))<0)
					 {

					 puts("Send failed\n");
					 return 1;
					 }
	}
		}
		printf("FLAG_IP: %d\n", flag_ip);
		printf("FLAG_NAME: %d\n", flag_name);

		for (x = 0; x < TOT; x++) {
			for (y = 0; y < BUF; y++) {
				ip_s[x][y] = '\0';
				names[x][y] = '\0';
			}
		}
		printf("Praznim listu\n");

	}
		if (read_size == 0) {
			puts("Client disconnected");
			fflush(stdout);
		} else if (read_size == -1) {
			perror("recv failed");
		}

	}
}
