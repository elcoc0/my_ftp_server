/*******************************************************************************
*
* File Name         : my_ftp_commons.h
* Created By        : elcoc0
* Creation Date     : September 30th, 2015
* Last Change       : November  8th, 2015 at 11:17:37 PM
* Last Changed By   : elcoc0
* Purpose           : Own implementation of a ftp server
*
*******************************************************************************/

#ifndef _MY_FTPS_COMMONS_HEADER_
#define _MY_FTPS_COMMONS_HEADER_

	//General value of buffer
	#define BSIZE 1024

	//In passive mode, port as generated randomly, here is the range of this random generated port
	#define EPHEMERAL_PORT_MIN 32768
	#define EPHEMERAL_PORT_MAX 61000
	
	//Number maximal of simultaneously connections
	#define NB_MAX_CONNECTIONS 50

	//Number of backlog queue client (socket)
	#define NB_BACKLOG_QUEUE 5

	//Structure used for each connection, it stores all related information about the connection state
	typedef struct State
	{

	  	// Boolean to know if the user is log
	  	int is_log;

	  	/* Is this username allowed? */
	  	int username_index;
	  	char *username;

	  	/* Response message to client e.g. 220 Welcome */
	  	char *message;

  		//Control connection socket
		int sock_connection;

	  	// Data socket
	  	int sock_data;

	  	// Connection currently used
	  	int is_used;
	} State;

	//States that stored all connection states
	typedef struct States_manager {
    	State *states[NB_MAX_CONNECTIONS];
    	int nb_co_allocated;
	} States_manager;
	
	// usernames
	static const char *usernames[] = { "anonymous","foo","bar" };

	// Valid passwords 
	static const char *passwords[] = { "","foopw","barpw" };

	void send_state(State *state);
	int create_socket(int port);
	int accept_connection(int socket);
	int create_new_connection(States_manager *states_manager);
	int get_nb_co_used(States_manager *states);

	#define MSG_110_RESTART_MARKER_REPLAY "110 Restart marker replay.\n"
	#define MSG_120_SERVICE_BE_READY "120 Service ready in few minutes.\n"
	#define MSG_125_125_DATA_OPENED_TRANSFER_STARTING "125 Data connection already open; transfer starting.\n"
	#define MSG_150_150_FSTATUS_OK_OPEN_BIN_CONNECTION "150 File status okay, Opening BINARY data connection.\n"
	#define MSG_150_150_FSTATUS_OK_OPEN_ASCII_CONNECTION "150 File status okay, Opening ASCII data connection.\n"
	#define MSG_200_TYPE_A "200 Setting ASCII Mode.\n"
	#define MSG_200_TYPE_I "200 Setting Binary Mode.\n"
	#define MSG_200_NOOP "200 OK Noop.\n"
	#define MSG_202_COMMAND_NOT_IMPLEMENTED "202 Command not implemented, superfluous at this site.\n"
	#define MSG_211_HELP 	"211-\n"\
								"+-------+------+-------------------+------+------+------------------+\n"\
								"| cmd   | FEAT | description       | type | conf | RFC#s/References |\n"\
								"|       | Code |                   |      |      | and Notes        |\n"\
							   	"+-------+------+-------------------+------+------+------------------+\n"\
							   	"| ABOR  | base | Abort             | s    | m    | 959              |\n"\
							   	"| ACCT  | base | Account           | a    | m    | 959              |\n"\
							   	"| ADAT  | secu | Authentication/   | a    | o    | 2228, 2773, 4217 |\n"\
							   	"|       |      | Security Data     |      |      |                  |\n"\
							   	"| ALLO  | base | Allocate          | s    | m    | 959              |\n"\
							   	"| APPE  | base | Append (with      | s    | m    | 959              |\n"\
							   	"|       |      | create)           |      |      |                  |\n"\
							   	"| AUTH  | secu | Authentication/   | a    | o    | 2228             |\n"\
							   	"|       |      | Security          |      |      |                  |\n"\
							   	"|       |      | Mechanism         |      |      |                  |\n"\
							   	"| AUTH+ | AUTH | Authentication/   | a    | o    | 2773, 4217 #2    |\n"\
							   	"|       |      | Security          |      |      |                  |\n"\
							   	"|       |      | Mechanism         |      |      |                  |\n"\
							   	"| CCC   | secu | Clear Command     | a    | o    | 2228             |\n"\
							   	"|       |      | Channel           |      |      |                  |\n"\
							   	"| CDUP  | base | Change to Parent  | a    | o    | 959              |\n"\
							   	"|       |      | Directory         |      |      |                  |\n"\
							   	"| CONF  | secu | Confidentiality   | a    | o    | 2228             |\n"\
							   	"|       |      | Protected Command |      |      |                  |\n"\
							   	"| CWD   | base | Change Working    | a    | m    | 959              |\n"\
							   	"|       |      | Directory         |      |      |                  |\n"\
							   	"| DELE  | base | Delete File       | s    | m    | 959              |\n"\
							   	"| ENC   | secu | Privacy Protected | a    | o    | 2228, 2773, 4217 |\n"\
							   	"|       |      | Command           |      |      |                  |\n"\
							   	"| EPRT  | nat6 | Extended Port     | p    | o    | 2428             |\n"\
							   	"| EPSV  | nat6 | Extended Passive  | p    | o    | 2428             |\n"\
							   	"|       |      | Mode              |      |      |                  |\n"\
							   	"+-------+------+-------------------+------+------+------------------+\n"\
								"211  End\n"

	#define MSG_211_FEAT		"211-\n"\
							   	"+------+------+\n"\
								"| cmd  | FEAT |\n"\
							   	"+------+------+\n"\
							   	"| ABOR | base |\n"\
							   	"| ACCT | base |\n"\
							   	"| ADAT | secu |\n"\
							   	"|      |      |\n"\
							   	"| ALLO | base |\n"\
							   	"| APPE | base |\n"\
							   	"|      |      |\n"\
							   	"| AUTH | secu |\n"\
							   	"|      |      |\n"\
							   	"|      |      |\n"\
							   	"| AUTH+| AUTH |\n"\
							   	"|      |      |\n"\
							   	"|      |      |\n"\
							   	"| CCC  | secu |\n"\
							   	"|      |      |\n"\
							   	"| CDUP | base |\n"\
							   	"|      |      |\n"\
							   	"| CONF | secu |\n"\
							   	"|      |      |\n"\
							   	"| CWD  | base |\n"\
							   	"|      |      |\n"\
							   	"| DELE | base |\n"\
							   	"| ENC  | secu |\n"\
							   	"|      |      |\n"\
							   	"| EPRT | nat6 |\n"\
							   	"| EPSV | nat6 |\n"\
							   	"|      |      |\n"\
							   	"+------+------+\n"\
								"211  End\n"
	#define MSG_212_DIR_STATUS "212 Directory status.\n"
	#define MSG_213_FILE_STATUS "213 File status.\n"
	#define MSG_214_HELP_USER "214 Help message.On how to use the server or the meaning of a particular non-standard command. This reply is useful only to the human user.\n"
	#define MSG_215_NAME_IANA "215 UNIX elcoco home made FTP\n"
	#define MSG_220_WELCOME_MESSAGE "220 Welcome, service ready for new user.\n"
	#define MSG_221_CLOSE_CONNECTION "221 Service closing control connection.\n"
	#define MSG_225_DATA_CONNECTION_OPEN_NO_TRANSFER "225 Data connection open; no transfer in progress.\n"
	#define MSG_226_FILE_RETRIEVED "226 Closing data connection. Requested file retrieved\n"
	#define MSG_226_FILE_STORED "226 Closing data connection. Requested file stored\n"
	#define MSG_226_DIR_LISTED "226 Closing data connection. Listing Directory completed\n"
	#define MSG_226_DATA_CONNECTION_ABORTED "226 Aborting data connection. \n"
	#define MSG_227_PASSIVE_MODE "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\n"
	#define MSG_228_LONG_PASSIVE_MODE "228 Entering Long Passive Mode (long address, port).\n"
	#define MSG_229_EXTENDED_PASSIVE_MODE "229 Entering Extended Passive Mode (|||port|).\n"
	#define MSG_230_USER_LOGGED_IN "230 User logged in, proceed. Logged out if appropriate.\n"
	#define MSG_231_USER_LOGGED_OUT "231 User logged out; service terminated.\n"
	#define MSG_232_LOGOUT_AFTER_TRANSFER "232 Logout command noted, will complete when transfer done.\n"
	#define MSG_234_AUTH_MECHANISM_ACCEPTED "234 Specifies that the server accepts the authentication mechanism specified by the client, and the exchange of security data is complete. A higher level nonstandard code created by Microsoft.\n"
	#define MSG_250_FACTION_COMPLETED "250 Requested file action okay, completed.\n"
	#define MSG_250_DIRECTORY_CHANGED "250 Directory successfully changed.\n"
	#define MSG_331_USERNAME_OK_PASSWORD_PROMPT "331 User name okay, need password.\n"
	#define MSG_332_NEED_ACCOUNT "332 Need account for login.\n"
	#define MSG_350_FACTION_PENDING "350 Requested file action pending further information\n"
	#define MSG_421_SERVICE_UNAVAILABLE "421 Service not available, closing control connection.\n"
	#define MSG_425_ERROR_DATA_CONNECTION_OPEN "425 Can't open data connection.\n"
	#define MSG_426_CONNECTION_CLOSED_ABORT "426 Connection closed; transfer aborted.\n"
	#define MSG_430_ERROR_CREDENTIALS "430 Invalid username or password\n"
	#define MSG_434_HOST_UNAVAILABLE "434 Requested host unavailable.\n"
	#define MSG_450_ERROR_FACTION_NOT_TAKEN "450 Requested file action not taken.\n"
	#define MSG_451_ERROR_FACTION_ABORTED "451 Requested action aborted. Local error in processing.\n"
	#define MSG_452_ERROR_FACTION_STORAGE_UNAVAILABLE "452 Requested action not taken. Insufficient storage space in system.File unavailable (e.g., file busy).\n"
	#define MSG_501_ERROR_SYNTAX_PARAMETERS "501 Syntax error in parameters or arguments.\n"
	#define MSG_502_ERROR_COMMAND_UNKNOWN "502 Command not implemented.\n"
	#define MSG_503_ERROR_BAD_SEQUENCE_COMMANDS "503 Bad sequence of commands.\n"
	#define MSG_504_ERROR_PARAMETER_FOR_COMMAND "504 Command not implemented for that parameter.\n"
	#define MSG_530_ERROR_NOT_LOGGED_IN "530 You need to be logged in to perform this action.\n"
	#define MSG_532_NEED_ACCOUNT_STORAGE "532 Need account for storing files.\n"
	
	#define MSG_550_FACTION_NOT_TAKEN_FILE_UNAVAILABLE "550 Requested action not taken. File unavailable (e.g., file not found, no access).\n"
	#define MSG_550_ERROR_DIRECTORY_CHANGED "550 Failed to change directory.\n"
	#define MSG_550_ERROR_READ_FILE "550 Requested action not taken. Can not read file.\n"
	#define MSG_550_ERROR_GET_FILE "550 Requested action not taken. Can not get file\n"

	#define MSG_551_FACTION_ABORTED_PAGE_UNUKNOWN "551 Requested action aborted. Page type unknown.\n"
	#define MSG_552_FACTION_ABORTED_STORAGE "552 Requested file action aborted. Exceeded storage allocation (for current directory or dataset).\n"
	#define MSG_553_FACTION_NOT_TAKEN_FILENAME_NOT_ALLOWED "553 Requested action not taken. File name not allowed.\n"
	#define MSG_631_INTEGRITY "631 Integrity protected reply.\n"
	#define MSG_632_CONFIDENTIALITY_AND_INTEGRITY "632 Confidentiality and integrity protected reply.\n"
	#define MSG_633_CONFIDENTIALITY "633 Confidentiality protected reply.\n"
	#define MSG_10054_CONNECTION_RESET_BY_PEER "10054 Connection reset by peer. The connection was forcibly closed by the remote host.\n"
	#define MSG_10060_CONNECTION_REMOTE_SERVER_UNAVAILABLE "10060 Cannot connect to remote server.\n"
	#define MSG_10061_CONNECTION_REMOTE_SERVER_REFUSED "10061 Cannot connect to remote server. The connection is actively refused by the server.\n"
	#define MSG_10066_DIR_NOT_EMPTY "10066 Directory not empty.\n"
	#define MSG_10068_LOAD_USERS_EXCEED "10068 Too many users, server is full.\n"

#endif

