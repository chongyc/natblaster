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
 * @file helper.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief stub demo helper server
 */

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "def.h"
#include "berkeleyapi.h"
#include "errorcodes.h"
#include "natblaster_helper.h"

/**
 * @brief gets arguments from the command line
 *
 * uses errorcodes.h for error codes
 *
 * @param argc the number of arguments passed in
 * @param argv the vector of arguments
 * @param helper_port pointer to the helper's port (will be filled in)
 *
 * @return SUCCESS, errorcode on failure
 */
int getArgs(int argc, char *argv[], port_t *helper_port);

/**
 * @brief prints the program use
 *
 * @return void
 */
void printUse();

/**
 * @brief stud entry point
 *
 * @param argc the number of elements in the argument vector
 * @param argv the argument vector
 *
 * @return 0 on success, neg on failure
 */
int main(int argc, char *argv[]) {

	port_t port;

	if (FAILED(getArgs(argc,argv,&port))) {
		printUse();
		return (-1);
	}

	port = htons(port);

	CHECK_FAILED(natblaster_server(port),-2);

	return (0);

}

void printUse() {

	printf("options:\n");
	printf("\t--listen_port : port to listen for peer connections on [required]\n");
	printf("\n");

	return;
}

int getArgs(int argc, char *argv[], port_t *helper_port) {

	char c;

	static struct option long_options[] =
	{
		{"listen_port",     required_argument, 0, 'a'},
		{0, 0, 0, 0 } /* for invalid args */
	};
	
	if (argc < 0)
		return ERROR_NEG_ARG_1;
	if (argv==NULL)
		return ERROR_NULL_ARG_2;
	if (helper_port==NULL)
		return ERROR_NULL_ARG_3;

	/* set default values */
	*helper_port = 0 ;

	/* loop over the arguments, and read them in */
	while (1)
	{
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "a:b:",
			long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
			case 'a' :
				*helper_port = (port_t) atoi(optarg);
				break;
			case '?':
				return ERROR_1;
				break;
			default:
				return ERROR_2;
				break;
		}
	}

	if (*helper_port==0)
		return ERROR_3;

	return SUCCESS;
}

