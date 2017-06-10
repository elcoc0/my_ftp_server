/*******************************************************************************
 *
 * File Name         : my_ftp_commands.c
 * Created By        : elcoc0
 * Creation Date     : September 30th, 2015
 * Last Change       : November  8th, 2015 at 11:17:00 PM
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
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <dirent.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include "my_ftp_commons.h"
#include "my_ftp_server.h"
#include "my_ftp_commands.h"

/**
* Handle each command received from the connection socket (stored in state)
* @param message receive from the client (contains arguments)
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int find_and_exec_cmd(const char *message, State *state)
{
	char cmd[BSIZE];
	char args[BSIZE];
	int nb_words = sscanf(message, "%s %s", cmd, args);
	printf("Client exec command : %s\n", message);
	if (nb_words == 1) args[0] = '\0';
	if (strcmp(cmd, "USER") == 0) {
		cmd_user(args, state);
	} else if (strcmp(cmd, "PASS") == 0) {
		cmd_pass(args, state);
	} else if (strcmp(cmd, "SYST") == 0) {
		cmd_syst(state);
	} else if (strcmp(cmd, "FEAT") == 0) {
		cmd_feat(args, state);
	} else if (strcmp(cmd, "PWD") == 0) {
		cmd_pwd(state);
	} else if (strcmp(cmd, "TYPE") == 0) {
		cmd_type(args, state);
	} else if (strcmp(cmd, "PASV") == 0) {
		cmd_pasv(state);
	} else if (strcmp(cmd, "LIST") == 0) {
		cmd_list(args, state);
	} else if (strcmp(cmd, "CWD") == 0) {
		cmd_cwd(args, state);
	} else if (strcmp(cmd, "MKD") == 0) {
		cmd_mkd(args, state);
	} else if (strcmp(cmd, "RETR") == 0) {
		cmd_retr(args, state);
	} else if (strcmp(cmd, "STOR") == 0) {
		cmd_stor(args, state);
	} else if (strcmp(cmd, "ABOR") == 0) {
		cmd_abor(state);
	} else if (strcmp(cmd, "SIZE") == 0) {
		cmd_size(args, state);
	} else if (strcmp(cmd, "DELE") == 0) {
		cmd_dele(args, state);
	} else if (strcmp(cmd, "RMD") == 0) {
		cmd_rmd(args, state);
	} else if (strcmp(cmd, "NOOP") == 0) {
		cmd_noop(state);
	} else if (strcmp(cmd, "HELP") == 0) {
		cmd_help(args, state);
	} else if (strcmp(cmd, "QUIT") == 0) {
		cmd_quit(state);
	} else {
		state->message = MSG_502_ERROR_COMMAND_UNKNOWN;
		send_state(state);
		return 1;
	}
	return 0;
}

/**
* Handle the ftp command USER
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_user(const char *args, State *state)
{
	int user_index = validate_user(args);
	if (user_index == -1) {
		state->message = MSG_430_ERROR_CREDENTIALS;
	} else if (user_index == 0) {
		state->username_index = user_index;
	} else {
		state->username_index = user_index;
		state->message = MSG_331_USERNAME_OK_PASSWORD_PROMPT;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command PASS
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_pass(const char *args, State *state) {

	if (validate_user_pass(args, state->username_index) == 0) {
		state->is_log = 1;
		state->message = MSG_230_USER_LOGGED_IN;
	}else {
		state->message = MSG_430_ERROR_CREDENTIALS;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command SYST
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_syst(State *state)
{
	state->message = MSG_215_NAME_IANA;
	send_state(state);
	return 0;
}

/**
* Handle the ftp command FEAT
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_feat(const char *args, State *state)
{
	state->message = MSG_211_FEAT;
	send_state(state);
	return 0;
}

/**
* Handle the ftp command PWD
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_pwd(State *state)
{
	if (state->is_log==1) {
		char buffer[BSIZE - 20];
		state->message = malloc(sizeof(char) * BSIZE);
		sprintf(state->message,"257 \"%s\"\n", getcwd(buffer, BSIZE - 20));
		send_state(state);
		free(state->message);
	} else {
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	return 0;
}

/**
* Handle the ftp command TYPE
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_type(const char *args, State *state)
{
	if (state->is_log==1) {
		if (strcmp(args, "I") == 0) {
			state->message = MSG_200_TYPE_I;
		}else if (strcmp(args, "A") == 0) {
			state->message = MSG_200_TYPE_A;
		}else {
			state->message = MSG_504_ERROR_PARAMETER_FOR_COMMAND;
		}
	} else {
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command PASV
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_pasv(State *state)
{
	if (state->is_log==1) {
		int ip_server[4] = { 127, 0, 0, 1};
		int port1 = rand_between(EPHEMERAL_PORT_MIN, EPHEMERAL_PORT_MAX) / 256;
		int port2 = rand_between(EPHEMERAL_PORT_MIN, EPHEMERAL_PORT_MAX) % 256;
		state->message = malloc(sizeof(char)*BSIZE);
		sprintf(state->message, MSG_227_PASSIVE_MODE, ip_server[0], ip_server[1], ip_server[2], ip_server[3], port1, port2);
		state->sock_data = create_socket( 256*port1 + port2);
		send_state(state);
		free(state->message);
	} else {
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
		send_state(state);
	}
	return 0;
}

/**
* Handle the ftp command LIST
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_list(const char *args, State *state)
{
	if (state->is_log==1) {
		struct dirent *entry;
		struct stat stat_buffer;
		struct tm *time;
		char timebuff[80], current_dir[BSIZE];
		int connection;
		time_t rawtime;
		char cwd[BSIZE], cwd_orig[BSIZE];
		memset(cwd, 0,  BSIZE);
		memset(cwd_orig, 0,  BSIZE);

    	/* Later we want to go to the original path */
		getcwd(cwd_orig,BSIZE);

	    /* Just chdir to specified path */
	    // if(strlen(cmd->arg)>0&&cmd->arg[0]!='-'){
	    //   chdir(cmd->arg);
	    // }

		getcwd(cwd,BSIZE);
		DIR *dp = opendir(cwd);

		if(!dp){
			state->message = "550 Failed to open directory.\n";
		}else{

			connection = accept_connection(state->sock_data);
			state->message = "150 Here comes the directory listing.\n";

			while(entry=readdir(dp)){
				if(stat(entry->d_name,&stat_buffer)==-1){
					fprintf(stderr, "FTP: Error reading file stats...\n");
				}else{
					char *perms = malloc(9);
					memset(perms,0,9);

	        /* Convert time_t to tm struct */
					rawtime = stat_buffer.st_mtime;
					time = localtime(&rawtime);
					strftime(timebuff,80,"%b %d %H:%M",time);
					str_perm((stat_buffer.st_mode & ALLPERMS), perms);
					dprintf(connection,
						"%c%s %5lu %4lu %4lu %8lu %s %s\r\n", 
						(entry->d_type==DT_DIR)?'d':'-',
						perms,stat_buffer.st_nlink,
						(long int)stat_buffer.st_uid, 
						(long int)stat_buffer.st_gid,
						(long int)stat_buffer.st_size,
						timebuff,
						entry->d_name);
				}
			}
			send_state(state);
			state->message = MSG_226_DIR_LISTED;
			send_state(state);
			close(connection);
			close(state->sock_data);
		}
		closedir(dp);
		chdir(cwd_orig);
	}else{
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
		send_state(state);
	}
	return 0;
}

/**
* Handle the ftp command CWD
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_cwd(const char *args, State *state)
{
	if(state->is_log){
		if(chdir(args)==0){
			state->message = MSG_250_DIRECTORY_CHANGED;
		}else{
			state->message = MSG_550_ERROR_DIRECTORY_CHANGED;
		}
	}else{
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command MKD
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_mkd(const char *args, State *state)
{
	if(state->is_log){
		char cwd[BSIZE];
		char res[BSIZE];
		memset(cwd, 0, BSIZE);
		memset(res, 0, BSIZE);
		getcwd(cwd,BSIZE);

    /* TODO: check if directory already exists with chdir? */

    /* Absolute path */
		if(args[0]=='/'){
			if(mkdir(args,S_IRWXU)==0){
				strcat(res,"257 \"");
				strcat(res,args);
				strcat(res,"\" New directory created.\n");
				state->message = res;
			}else{
				state->message = MSG_550_FACTION_NOT_TAKEN_FILE_UNAVAILABLE;
			}
		}
    /* Relative path */
		else{
			if(mkdir(args,S_IRWXU)==0){
				sprintf(res,"257 \"%s/%s\" New directory created.\n", cwd, args);
				state->message = res;
			}else{
				state->message = MSG_550_FACTION_NOT_TAKEN_FILE_UNAVAILABLE;
			}
		}
	}else{
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command RETR
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_retr(const char *args, State *state)
{

	if(fork()==0){
		int connection;
		int fd;
		struct stat stat_buf;
		off_t offset = 0;
		int sent_total = 0;
		if(state->is_log){

			if(access(args,R_OK)==0 && (fd = open(args,O_RDONLY))){
				fstat(fd,&stat_buf);

				state->message = MSG_150_150_FSTATUS_OK_OPEN_BIN_CONNECTION;

				send_state(state);

				connection = accept_connection(state->sock_data);
				close(state->sock_data);
				if(sent_total = sendfile(connection, fd, &offset, stat_buf.st_size)){

					if(sent_total != stat_buf.st_size){
						fprintf(stderr, "Error cmd_retr, sending file\n");
						exit(EXIT_SUCCESS);
					}
					state->message = MSG_226_FILE_RETRIEVED;
				}else{
					state->message = MSG_550_ERROR_READ_FILE;
				}
			}else{
				state->message = MSG_550_ERROR_GET_FILE;
			}
		}else{
			state->message = MSG_530_ERROR_NOT_LOGGED_IN;
		}
		close(fd);
		close(connection);
		send_state(state);
		exit(EXIT_SUCCESS);
	}
	close(state->sock_data);
	return 0;
}


/**
* Handle the ftp command STOR
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_stor(const char *args, State *state)
{
	if (fork()==0){
		int connection, fd;
		int pipefd[2];
		int res = 1;
		const int buff_size = 8192;

		FILE *fp = fopen(args,"w");

		if (fp==NULL){
			fprintf(stderr, "Error cmd_stor, fopen\n");
		} else if (state->is_log) {

			fd = fileno(fp);
			connection = accept_connection(state->sock_data);
			close(state->sock_data);
			if (pipe(pipefd)==-1) fprintf(stderr, "Error cmd_stor, creating pipe\n");

			state->message = MSG_125_125_DATA_OPENED_TRANSFER_STARTING;
			send_state(state);

        /* Using splice function for file receiving.
         * The splice() system call first appeared in Linux 2.6.17.
        */

         while ((res = splice(connection, 0, pipefd[1], NULL, buff_size, SPLICE_F_MORE | SPLICE_F_MOVE))>0){
         	splice(pipefd[0], NULL, fd, 0, buff_size, SPLICE_F_MORE | SPLICE_F_MOVE);
         }

        /* TODO: signal with ABOR command to exit */

         if(res==-1){
         	fprintf(stderr, "Error cmd_stor, calling splice \n");
         	exit(EXIT_SUCCESS);
         }else{
         	state->message = MSG_226_FILE_STORED;
         }
         close(connection);
         close(fd);
     }else{
     	state->message = MSG_530_ERROR_NOT_LOGGED_IN;
     }
     close(connection);
     send_state(state);
     exit(EXIT_SUCCESS);
 }
 close(state->sock_data);
 return 0;
}

/**
* Handle the ftp command ABOR
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_abor(State *state)
{
	if(state->is_log){
		state->message = MSG_226_DATA_CONNECTION_ABORTED;
		close(state->sock_data);
    //state->message = "225 Data connection open; no transfer in progress.\n";
	}else{
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	send_state(state);
	return 0;

}

/**
* Handle the ftp command SIZE
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_size(const char *args, State *state)
{
	if(state->is_log){
		struct stat stat_buffer;
		char filesize[128];
		memset(filesize, 0, 128);

		if(stat(args,&stat_buffer)==0){
			sprintf(filesize, "213 %lu\n", (long int) stat_buffer.st_size);
			state->message = filesize;
		}else{
			state->message = MSG_550_FACTION_NOT_TAKEN_FILE_UNAVAILABLE;
		}
	}else{
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command DELE
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_dele(const char *args, State *state)
{
	if(state->is_log){
		if (unlink(args) == 0){
			state->message = MSG_250_FACTION_COMPLETED;
		}else{
			state->message = MSG_550_FACTION_NOT_TAKEN_FILE_UNAVAILABLE;
		}
	}else{
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command RMD
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_rmd(const char *args, State *state)
{
	if (state->is_log){
		if(rmdir(args)==0){
			state->message = MSG_250_FACTION_COMPLETED;
		}else{
			state->message = MSG_550_FACTION_NOT_TAKEN_FILE_UNAVAILABLE;
		}
	} else {
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command NOOP
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_noop(State *state)
{
	if (state->is_log){
		state->message = MSG_200_NOOP;
	} else {
		state->message = MSG_530_ERROR_NOT_LOGGED_IN;
	}
	send_state(state);
	return 0;
}

/**
* Handle the ftp command HELP
* @param args arguments passed
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_help(const char *args, State *state)
{

	state->message = MSG_211_HELP;
	send_state(state);
	return 0;
}

/**
* Handle the ftp command QUIT
* @param state Current information (state) connection
* @return 0 in case of success, 1 otherwise 
*/
int cmd_quit(State *state)
{
	state->message = MSG_221_CLOSE_CONNECTION;
	send_state(state);
	close(state->sock_connection);
	return 0;
}

/**
* Formatting a perm code into a string (rwx)
* @param perm Permission to format
* @param str_perm pointer where to store the formatted string
*/
void str_perm(int perm, char *str_perm)
{
	int current_perm = 0;
	int read, write, exec;

  	//Flags buffer */
	char buffer[3];

	read = write = exec = 0;

	int i;
	for(i = 6; i>=0; i-=3){
		//Split permissions, starting by users
		current_perm = ((perm & ALLPERMS) >> i ) & 0x7;

		memset(buffer,0,3);
    	//Check rwx for each
		read = (current_perm >> 2) & 0x1;
		write = (current_perm >> 1) & 0x1;
		exec = (current_perm >> 0) & 0x1;

		sprintf(buffer,"%c%c%c",read?'r':'-' ,write?'w':'-', exec?'x':'-');
		strcat(str_perm,buffer);

	}
}

/**
* Generate a random number between two values
* @param a minimum value
* @param b maximum value
* @return the random number
*/
int rand_between(int a, int b){
	return rand()%(b-a) + a;
}

/**
* Verify if a user exists
* @param user username to validate
* @return the index value where the user is store (in usernames)
* 		if the user does not exist, return -1
* @see my_ftp_commons.h#usernames
*/
int validate_user(const char *user) {
	for (int i = 0; usernames[i]; i++) {
		if (strcmp(user, usernames[i]) == 0) {
			return i;
		}
	}
	return -1;
}

/**
* Verify the password of the user stored at user_index is correct
* @param pass password to verify
* @param user_index User index which need to check the password
* @return 0 if the password matches, 1 otherwise
* @see my_ftp_commons.h#usernames
*/
int validate_user_pass(const char *pass, const int user_index)
{
	return (strcmp(pass, passwords[user_index]) == 0) ? 0 : 1;
}
