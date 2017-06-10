/*******************************************************************************
 *
 * File Name         : my_ftp_server.c
 * Created By        : elcoc0
 * Creation Date     : September 30th, 2015
 * Last Change       : November  8th, 2015 at 11:17:27 PM
 * Last Changed By   : elcoc0
 * Purpose           : Own implementation of a ftp server 
 *
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "my_ftp_commons.h"
#include "my_ftp_commands.h"
#include "my_ftp_server.h"

/**
* Main loop running the server
* Each connection is handlded by a thread
* All connections data are stored inside the structure States_manager
* @param argc number of arguments
* @param argv[1] : server port number
* @return the random number
* @see my_ftp_commands.h
* @see my_ftps_commons.h
*/
int main(int argc, char *argv[])
{
	int sockfd;
	socklen_t clilen;
	char buffer[BSIZE];
	struct sockaddr_in cli_addr;
	int accept_retval;
	int current_co;
	States_manager *states_manager = malloc(sizeof(States_manager));
	states_manager->nb_co_allocated = 0;
	srand(time(NULL));
	int pid = getpid();

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = create_socket(atoi(argv[1]));
	clilen = sizeof(cli_addr);
	memset(buffer, 0,  BSIZE);
	
	while(accept_retval = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) {
		if (accept_retval < 0)  
			fprintf(stderr,"ERROR, on accept\n");

		if ((current_co = create_new_connection(states_manager)) != -1) {
			states_manager->states[current_co]->sock_connection = accept_retval;
			states_manager->states[current_co]->message = MSG_220_WELCOME_MESSAGE;
			send_state(states_manager->states[current_co]);

			pthread_t rcv_thread;
			if (pthread_create( &rcv_thread , NULL ,  connection_handler , (void*) states_manager->states[current_co]) < 0) {
				fprintf(stderr, "ERROR, on creating thread connection_handler\n");
				return 1;
			}
		} else {
			if (send(accept_retval, MSG_10068_LOAD_USERS_EXCEED, strlen(MSG_10068_LOAD_USERS_EXCEED), 0) < 0)
				fprintf(stderr,"ERROR writing to socket\n");
		}
	}
	close(sockfd);

	return 0;  
}
/**
* Connection handler client thread  method
* @param state pointer to the connection state
* @see my_ftp_server.c#main(int argv, char *argv[]
*/
	void *connection_handler(void *state)
	{
		State *state_ptr = (State*)state;
		printf("Connecion thread number : %d\n",state_ptr->sock_connection);
		char buffer[BSIZE];
		int read_size;
		memset(buffer, 0, BSIZE);

		while(read_size = recv(state_ptr->sock_connection, buffer, sizeof(buffer), 0) > 0) {
			sleep(1);
			find_and_exec_cmd(buffer, state_ptr);
			memset(buffer, 0, BSIZE);

		}
	if (read_size < 0) fprintf(stderr,"ERROR reading from socket\n"); //error("ERROR reading from socket");
	if(read_size == 0) {
		printf("Client disconnected \n");
	} else if(read_size == -1) {
		printf("Connection reset by peer \n");
	}
	close(state_ptr->sock_connection);
	state_ptr->is_used = 0;

	return 0;
}

/**
* Send the current state message to the state socket
* @param state pointer to the connection state
*/
void send_state(State *state)
{
	printf("Connection Socket : %d | Message : %s\n", state->sock_connection, state->message);
	if (send(state->sock_connection, state->message, strlen(state->message), 0) < 0) 
		fprintf(stderr,"ERROR writing to socket\n");
}

/**
* Create a socket to the given port
* @param port where the socket is bind
* @return the socket
*/
int create_socket(int port)
{
	int sock;
	struct sockaddr_in serv_addr;
	sock = socket(AF_INET, SOCK_STREAM, 0); 
	
	if (sock < 0) fprintf(stderr,"ERROR, no port provided\n");
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  
		fprintf(stderr,"ERROR, on binding\n");
	listen(sock, NB_BACKLOG_QUEUE);

	return sock;
}

/**
* Call the method accept for a given socket
* @param socket number
* @return the file descriptor
*/
int accept_connection(int socket)
{
	socklen_t addrlen;
	struct sockaddr_in client_address;
	addrlen = sizeof(client_address);

	return accept(socket,(struct sockaddr*) &client_address,&addrlen);
}

/**
* Create a new connection into the States_manager
* @param states_manager pointer to the States_manager
* @return the index where is stored the connection data into the States_manager
*/
int create_new_connection(States_manager *states_manager)
{
	int not_used_index;

	for (not_used_index = 0; not_used_index < states_manager->nb_co_allocated; not_used_index++) {
		if (states_manager->states[not_used_index]->is_used == 0) {
			memset(states_manager->states[not_used_index], 0, sizeof(State));
			states_manager->states[not_used_index]->is_used = 1;
			return not_used_index;
		}
	}
	if (not_used_index < NB_MAX_CONNECTIONS) {
		states_manager->states[not_used_index] = malloc(sizeof(State));
		states_manager->states[not_used_index]->is_used = 1;
		states_manager->nb_co_allocated++;
		return 0;
	} else {
		return -1;
	}
}

/**
* Method for getting the number of connection used
* @param states_manager pointer to the States_manager
* @return the number of connection used
*/
int get_nb_co_used(States_manager *states_manager)
{
	int nb_co_used = 0;

	for (int i = 0; i < states_manager->nb_co_allocated; i++) {
		if (states_manager->states[i]->is_used == 1) {
			nb_co_used++;
		}

	}

	return nb_co_used;
}
