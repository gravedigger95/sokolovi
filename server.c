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
#define DEFAULT_PORT   27015
#define BUF 128
#define TOT 20
#define filename "/home/pi/Projekat/ORMDNS/lab_4/src/lista.txt"
#define filename1 "/home/pi/Projekat/ORMDNS/lab_4/src/rootlista.txt"

void DoProcessing(int client_sock, int root_sock);

struct clients {
	char ip[100];
	char website[100];
};

int main(int argc, char *argv[]) {

	int socket_desc, client_sock, c, root_sock;
	struct sockaddr_in server, client, root_server;
	int pid;

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

	if (connect(root_sock, (struct sockaddr *) &root_server, sizeof(root_server)) < 0) {
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

		client_sock = accept(socket_desc, (struct sockaddr *) &client,(socklen_t*) &c);
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
			DoProcessing(client_sock, root_sock);

			return (0);
		} else {
			close(client_sock);

		}
	}
	close(root_sock);
	close(socket_desc);

	return 0;

}

void DoProcessing(int client_sock, int root_sock) {

	int read_size;
	char client_message[DEFAULT_BUFLEN];
	int k;
	char buff[100];
	int x, y; //iterators for cleaning arrays
	int inLocalList = 0; //flag for checking is ip or website sent from root server already in local server list
	char* ptr;
	int n = 0; //flag for checking is ip or website in local server list
	int num;
	int total; //total number of elements in arrays
	int invalid; //flag for checking if client typed invalid ip or website
	int i = 0;
	char ips[TOT][BUF];
	char websites[TOT][BUF];
	int flag_ip, flag_website; //flags for knowing what to return to client(ip or website)
	char* message = "Website or IP doesn't exist";
	struct clients client;

	//Receive a message from client

	while ((read_size = recv(client_sock, client_message, DEFAULT_BUFLEN, 0))
			> 0) {
		i = 0;
		FILE *fp = fopen(filename, "r");
		//scan file and split each line to website and ip and fill arrays
		while (fscanf(fp, "%s %s\n", client.website, client.ip) != EOF) {
			ips[i][strlen(client.ip)] = '\0';
			strcpy(ips[i], client.ip);
			websites[i][strlen(client.website)] = '\0';
			strcpy(websites[i], client.website);
			i++;
		}

		total = i;
		fclose(fp);
		flag_ip = 0;
		flag_website = 0;
		*(client_message + read_size) = '\0';
		n = 0;
		//iterate through arrays and compare with ip's and websites arrays
		for (k = 0; k < total; k++) {
			//client typed website, expecting ip
			if (strcmp(client_message, websites[k]) == 0) {
				
				//send ip to client
				if ((n = send(client_sock, ips[k], strlen(ips[k]), 0)) < 0) {
					puts("Send failed\n");
					return 1;
				}
				break;

			}

			//client typed ip, expecting website
			if (strcmp(client_message, ips[k]) == 0) {
				//send website to client
				if ((n = send(client_sock, websites[k], strlen(websites[k]), 0))
						< 0) {
					puts("Send failed\n");
					return 1;
				}
			}
		}
		//site or ip not in list
		if (n == 0) {
			//proceed request to root server
			if (send(root_sock, client_message, strlen(client_message), 0) < 0) {
				puts("Send failed");
				return 1;
			}
			//printf("Saljem korenskom\n");

			//receive response from root server
			if ((num = recv(root_sock, buff, sizeof(buff), 0)) != -1) {
				buff[num] = '\0';
				
				//if client typed correct input
				if ((invalid = strcmp(buff, message)) != 0) {
					//printf("%d\n",invalid);
					buff[num] = '\0';
		
					//if line not in local list expand list with new website
					if (inLocalList == 0) {
						flag_website = 0;
						flag_ip = 0;
						
						FILE *fp2 = fopen(filename, "a+");
						fprintf(fp2, "%s\n", buff);
						fclose(fp2);
			
						//split string received from root server to website and ip
						strtok_r(buff, " ", &ptr);
						
						//check if client typed website
						if (strcmp(client_message, buff) == 0) {
							flag_website = 1;
						}
						//check if client typed ip
						if (strcmp(client_message, ptr) == 0) {
							flag_ip = 1;
						}
				
						//return ip to client
						if (flag_website == 1) {
							if ((send(client_sock, ptr, strlen(ptr), 0)) < 0) {
								puts("Send failed\n");
								return 1;
							}
						} else if (flag_ip == 1) {
							//return website to client
							if ((send(client_sock, buff, strlen(buff), 0)) < 0) {

								puts("Send failed\n");
								return 1;
							}
						}
					}

					bzero(buff, sizeof(buff));
				} else {
					if ((send(client_sock, "Invalid!!!\n",strlen("Invalid!!!\n"), 0)) < 0) {

						puts("Send failed\n");
						return 1;
					}
				}
			}
			//clean arrays
			for (x = 0; x < TOT; x++) {
				for (y = 0; y < BUF; y++) {
					ips[x][y] = '\0';
					websites[x][y] = '\0';
				}
			}
		}
		if (read_size == 0) {
			puts("Client disconnected");
			fflush(stdout);
		} else if (read_size == -1) {
			perror("recv failed");
		}

	}
}
