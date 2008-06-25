/*****************************************************************************
 * Copyright 2005 Daniel Ferullo                                             *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *    http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 *                                                                           *
 *****************************************************************************/

/**
 * @file peer.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief a stub program to test the natblaster peer functionality
 */

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "natblaster_peer.h"
#include "errorcodes.h"
#include "def.h"
#include "berkeleyapi.h"
#include "nethelp.h"

/** @brief size of buffer to receive a message from the buddy in */
#define BUFSIZE	64

/**
 * @brief gets arguments from the command line
 *
 * uses errorcodes.h for error codes
 *
 * @param argc the number of arguments passed in
 * @param argv the vector of arguments
 * @param helper_ip a pointer to a pointer.  When finished, will point to a
 *        string with the helper's dotted-decimal IP address
 * @param helper_port pointer to the helper's port (will be filled in)
 * @param peer_ip a pointer to a pointer.  When finished, will point to a
 *        string with the peer's dotted-decimal IP address
 * @param peer_port pointer to the peer's starting port (will be filled in)
 * @param buddy_ext_ip a pointer to a pointer.  When finished, will point to a
 *        string with the buddy's dotted-decimal external IP address
 * @param buddy_int_ip a pointer to a pointer.  When finished, will point to a
 *        string with the buddy's dotted-decimal internal IP address
 * @param buddy_int_port pointer to the buddy's starting port (will be filled
 *        in)
 * @param dev a pointer to a pointer.  When finished, will point to a string
 *        with the network device to use.  If none is specified, will be NULL
 *        on function return
 * @param msg a pointer to a pointer.  When finished, will point to a string
 *        with the message to send to the buddy.
 * @param random a pointer to an flag_t to set to 1 if the peer wants to be
 *        random.
 *
 * @return SUCCESS, neg value on failure
 */
int getArgs(int argc, char *argv[], char **helper_ip,
            port_t *helper_port, char **peer_ip,
			port_t *peer_port, char **buddy_ext_ip,
			char **buddy_int_ip, port_t *buddy_int_port,
			char **dev, char **msg, flag_t *random);

/**
 * @brief prints the program use
 *
 * @return void
 */
void printUse();

/**
 * @brief entry point for peer stub application
 *
 * @param argc the number of elements in the argument vector
 * @param argv the argument vector
 *
 * return 0 on success, neagive on failure
 */
int main(int argc, char *argv[]) {

	char *helper_addr, *peer_addr, *buddy_ext_addr, *buddy_int_addr;
	port_t helper_port, peer_port, buddy_int_port;
	char *dev, *msg;
	sock_t sd;
	char buf[BUFSIZE];
	int nread;
	flag_t random = FLAG_UNSET;
	ip_t helper_num, peer_num, buddy_int_num, buddy_ext_num;

	if(FAILED(getArgs(argc, argv, &helper_addr, &helper_port, &peer_addr,
					  &peer_port, &buddy_ext_addr, &buddy_int_addr,
					  &buddy_int_port, &dev, &msg,&random))) {
		printUse();
		return ERROR_1;
	}

	/* put the ports in network byte order */
	helper_port    = htons( helper_port    );
	peer_port      = htons( peer_port      );
	buddy_int_port = htons( buddy_int_port );

	CHECK_FAILED(resolveIP(helper_addr,&helper_num),ERROR_1);
	CHECK_FAILED(resolveIP(peer_addr,&peer_num),ERROR_2);
	CHECK_FAILED(resolveIP(buddy_int_addr,&buddy_int_num),ERROR_3);
	CHECK_FAILED(resolveIP(buddy_ext_addr,&buddy_ext_num),ERROR_4);

	if ( (sd=natblaster_connect(helper_num,helper_port,peer_num,peer_port,
	                       buddy_ext_num, buddy_int_num, buddy_int_port,
			       dev,random))<0) {
		printf("UNSUCCESSFUL!!!\n");
		return ERROR_2;
	}

	/* send the buddy a small message */
	write(sd, msg, strlen(msg));
	/* and recieve the buddy's message */
	memset(buf, '\0',sizeof(buf));
	if (read(sd, buf, sizeof(buf)-1)>0) {
		printf("Buddy sent message: [%s]\n", buf);
		printf("SUCCESS!!!\n");
	}
	else {
		/* in pratice this case does not occur */
		printf("no message received :(\n");
		printf("QUASI-SUCCESS...\n");
	}
	close(sd);

	return (0);
}


void printUse() {

	printf("options:\n");
	printf("\t--helper_ip      : helper hostname or IP [required]\n");
	printf("\t--helper_port    : helper port to connect to [required]\n");
	printf("\t--local_ip       : local internal hostname or IP [required]\n");
	printf("\t--local_port     : local internal port [required]\n");
	printf("\t--buddy_ext_ip   : external hostname or IP of buddy [required]\n");
	printf("\t--buddy_int_ip   : internal hostname or IP of buddy [required]\n");
	printf("\t--buddy_int_port : internal port of buddy [required]\n");
	printf("\t--device         : device to connect on [optional]\n");
	printf("\t--message        : message to send to buddy (enclosed in quotes if contains white space)\n");
	printf("\t--random         : flag indicating if this peer should pretend to be random\n");

	printf("\n");

	return;
}

int getArgs(int argc, char *argv[], char **helper_ip,
			port_t *helper_port, char **peer_ip,
			port_t *peer_port, char **buddy_ext_ip,
			char **buddy_int_ip, port_t *buddy_int_port,
			char **dev, char **msg, flag_t *random) {

	char c;
	static struct option long_options[] =
	{
		{"helper_ip",      required_argument, 0, 'a'},
		{"helper_port",    required_argument, 0, 'b'},
		{"local_ip",       required_argument, 0, 'c'},
		{"local_port",     required_argument, 0, 'd'},
		{"buddy_ext_ip",   required_argument, 0, 'e'},
		{"buddy_int_ip",   required_argument, 0, 'f'},
		{"buddy_int_port", required_argument, 0, 'g'},
		{"device",         required_argument, 0, 'h'},
		{"message",        required_argument, 0, 'i'},
		{"random",         no_argument,       0, 'j'},
		/* for invalid args */
		{0,               0,                 0,  0 }
	};
		
	CHECK_NOT_NEG(argc,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(argv,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(helper_ip,ERROR_NULL_ARG_3);
	CHECK_NOT_NULL(helper_port,ERROR_NULL_ARG_4);
	CHECK_NOT_NULL(peer_ip,ERROR_NULL_ARG_5);
	CHECK_NOT_NULL(peer_port,ERROR_NULL_ARG_6);
	CHECK_NOT_NULL(buddy_ext_ip,ERROR_NULL_ARG_7);
	CHECK_NOT_NULL(buddy_int_ip,ERROR_NULL_ARG_8);
	CHECK_NOT_NULL(buddy_int_port,ERROR_NULL_ARG_9);
	CHECK_NOT_NULL(dev,ERROR_NULL_ARG_10);
	CHECK_NOT_NULL(msg,ERROR_NULL_ARG_11);
	CHECK_NOT_NULL(random,ERROR_NULL_ARG_12);

	/* set default values */
	*helper_ip = *peer_ip = *buddy_ext_ip = NULL;
	*buddy_int_ip = *dev = *msg = NULL;
	*helper_port = *peer_port = *buddy_int_port = 0 ;

	/* loop over the arguments, and read them in */
	while (1)
	{
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "a:b:c:d:e:f:g:h:i:j:",
			long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
			case 'a' :
				*helper_ip = optarg;
				break;
			case 'b' :
				*helper_port = (unsigned short) atoi(optarg);
				break;
			case 'c' :
				*peer_ip = optarg;
				break;
			case 'd' :
				*peer_port = (unsigned short) atoi(optarg);
				break;
			case 'e' :
				*buddy_ext_ip = optarg;
				break;
			case 'f':
				*buddy_int_ip =optarg;
				break;
			case 'g' :
				*buddy_int_port = (unsigned short) atoi(optarg);
				break;
			case 'h' :
				*dev = optarg;
				break;
			case 'i':
				*msg = optarg;
				break;
			case 'j' :
				*random = FLAG_SET;
				break;
			case '?':
				return ERROR_1;
				break;
			default:
				return ERROR_2;
				break;
		}
	}

	if ( (*helper_ip==NULL) || (*buddy_ext_ip==NULL) ||
	     (buddy_int_ip==NULL) || (*msg==NULL) )
		return ERROR_3;

	if ( (*helper_port==0) || (*peer_port==0) || (*buddy_int_port==0))
		return ERROR_4;

	return SUCCESS;
}

