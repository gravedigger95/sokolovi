/* 
 ********************************************************************
 Odsek:          Elektrotehnika i racunarstvo
 Departman:      Racunarstvo i automatika
 Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
 Predmet:        Osnovi Racunarskih Mreza 1
 Godina studija: Treca (III)
 Skolska godina: 2016/2017
 Semestar:       Zimski (V)
 
 website fajla:      server.c
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
#define filename "/home/pi/Projekat/ORMDNS/lab_4/src/rootlista.txt"
struct clients {
	char ip[100];
	char website[100];
};
int main(int argc, char *argv[]) {
	int inList; //check if client made invalid input
	char* message = "Website or IP doesn't exist";
	int socket_desc, server_sock, c, read_size;
	struct sockaddr_in server;
	struct clients client;
	char server_message[DEFAULT_BUFLEN];
	char str[80];
	int k;
	int n = 0;
	int i;
	int total; //total number of elements in arrays
	char ips[TOT][BUF];
	char websites[TOT][BUF];
	FILE *fp = fopen(filename, "r");
	//check if file is opened properly
	if (fp == NULL) {
		printf("File not found!\n");
		return NULL;
	} else {
		printf("Found file %s\n", filename);
	}
	//read file, split strings and fill arrays with ip's and websites
	i = 0;
	while (fscanf(fp, "%s %s\n", client.website, client.ip) != EOF) {
		ips[i][strlen(client.ip) - 1] = '\0';
		strcpy(ips[i], client.ip);
		websites[i][strlen(client.website) - 1] = '\0';
		strcpy(websites[i], client.website);
		i++;
	}
	total = i;
	fclose(fp);

	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(DEFAULT_PORT);

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
	server_sock = accept(socket_desc, (struct sockaddr *) &server,
			(socklen_t*) &c);
	if (server_sock < 0) {
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");

	//Receive a message from client
	while ((read_size = recv(server_sock, server_message, DEFAULT_BUFLEN, 0))
			> 0) {
		*(server_message + read_size) = '\0';
		inList = 0;
		//iterate through arrays and compare with ip's and websites arrays
		for (k = 0; k < total; k++) {
			//client typed website, expecting ip
			if (strcmp(server_message, websites[k]) == 0) {
				//concatenate strings
				strcpy(str, websites[k]);
				strcat(str, " ");
				strcat(str, ips[k]);
				inList = 1;

				//send line to local server
				if ((n = send(server_sock, str, strlen(str), 0)) < 0) {
					puts("Send failed\n");
					return 1;
				}

			}
			//client typed ip, expecting website
			if (strcmp(server_message, ips[k]) == 0) {
				//concatenate strings
				strcpy(str, websites[k]);
				strcat(str, " ");
				strcat(str, ips[k]);
				inList = 1;
				//send line to local server
				if ((n = send(server_sock, str, strlen(str), 0)) < 0) {
					puts("Send failed\n");
					return 1;
				}
			}
		}

		//clients input doesn't exist in root server list
		if (inList == 0) {
			if ((n = send(server_sock, message, strlen(message), 0)) < 0) {
				puts("Send failed\n");
				return 1;

			}
		}
	}

	if (read_size == 0) {
		puts("Server disconnected");
		fflush(stdout);
	} else if (read_size == -1) {
		perror("recv failed");
	}

	close(socket_desc);

	return 0;
}

