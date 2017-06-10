###############################################################################
#
# File Name         : Makefile
# Created By        : elcoc0
# Creation Date     : August 31th, 2015
# Last Change       : November 20th, 2015 at 09:55:00 AM
# Last Changed By   : elcoc0
# Purpose           : It's allow to compile our project easier
#                       - all   : Compile all sources
#                       - clean : Clean all objects and executable
#
###############################################################################
#### INITIAL PARAMETERS ####
EXECUTABLE= my_ftp_server
SOURCES=  my_ftp_commands.c my_ftp_server.c 
CFLAGS= -g -Wall -Wextra -ansi -pedantic -std=c99 -D_GNU_SOURCE -lpthread# version for linux
LDFLAGS=
CC=gcc
OBJECTS=$(SOURCES:.c=.o)

#### TARGETS ####
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(CFLAGS)

$(OBJECTS): $(SOURCES)
	$(CC) -c $(SOURCES) $(CFLAGS)

clean:
	rm $(OBJECTS) $(EXECUTABLE)
