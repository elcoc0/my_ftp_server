/*******************************************************************************
*
* File Name         : my_ftp_commands.h
* Created By        : elcoc0
* Creation Date     : September 30th, 2015
* Last Change       : November  8th, 2015 at 11:17:22 PM
* Last Changed By   : elcoc0
* Purpose           : Own implementation of a ftp server
*
*******************************************************************************/

#ifndef _MY_FTP_COMMANDS_HEADER_
#define _MY_FTP_COMMANDS_HEADER_

int find_and_exec_cmd(const char *message, State *state);
int cmd_user(const char *args, State *state);
int cmd_pass(const char *args, State *state);
int cmd_syst(State *state);
int cmd_feat(const char *args, State *state);
int cmd_pwd(State *state);
int cmd_type(const char *args, State *state);
int cmd_pasv(State *state);
int cmd_list(const char *args, State *state);
int cmd_cwd(const char *args, State *state);
int cmd_mkd(const char *args, State *state);
int cmd_retr(const char *args, State *state);
int cmd_stor(const char *args, State *state);
int cmd_abor(State *state);
int cmd_size(const char *args, State *state);
int cmd_dele(const char *args, State *state);
int cmd_rmd(const char *args, State *state);
int cmd_noop(State *state);
int cmd_help(const char *args, State *state);
int cmd_quit(State *state);
int rand_between(int a, int b);
int validate_user(const char *user);
int validate_user_pass(const char *pass, const int user_index);
void str_perm(int perm, char *str_perm);


#endif

