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
 * @file nethelp.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief provides helpful basic network functions
 */
#include <pcap.h>
#include <string.h>
#include "nethelp.h"
#include "berkeleyapi.h"
#include "debug.h"

errorcode resolveIP(char *ip_or_name, ip_t *ip) {

	/* declare local variables */
	struct hostent *hp;

	/* error check arguments */
	CHECK_NOT_NULL(ip_or_name,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(ip,ERROR_NULL_ARG_2);

	/* do function */
	hp = gethostbyname(ip_or_name);

	if (hp==NULL)
		return ERROR_HOST_NAME_LOOKUP;

	if (hp->h_addr==NULL)
		return ERROR_1;

	memcpy(ip,hp->h_addr,sizeof(ip));
	if (*ip==-1)
		return ERROR_2;

	return SUCCESS;
}

errorcode bindSocket(port_t port_to_bind, sock_t *sd) {

	/* declare local variables */
	int buddy_sd;
	struct sockaddr_in server;

	/* error check arguments */
	CHECK_NOT_NULL(sd,ERROR_NULL_ARG_2);

	/* do function */

	/* create stream socket using TCP */
	if( (buddy_sd=socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		return ERROR_SOCKET_CREATE;
	}

	/* put the clients info in the sockaddr_in struct */
	server.sin_family = AF_INET;
	server.sin_port = port_to_bind;
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind the port to use for the direct connection*/
	if (bind(buddy_sd, (struct sockaddr*)&server, sizeof(server))<0) {
		DEBUG(DBG_NETWORK, "NETWORK:couldn't bind port %u\n",
			DBG_PORT(port_to_bind));
		return ERROR_BIND;
	}
	/* done binding to port for direct connection */
	*sd = buddy_sd;

	return SUCCESS;
}

errorcode tcp_connect(ip_t ip, port_t port, sock_t *sd) {

	/* declare local variables */
	struct in_addr addr;
	struct sockaddr_in con_to;

	/* error check arguments */
	CHECK_NOT_NULL(sd,ERROR_NULL_ARG_3);

	/* do function */

	if (*sd <= 0) {
		DEBUG(DBG_ALL, "ALL:WARNING: tcp_connect: calling socket (bad b/c sd should already exist)\n");
		if ( (*sd=socket(AF_INET,SOCK_STREAM,0)) < 0)
			return ERROR_TCP_SOCKET;
	}

	addr.s_addr = ip;

	con_to.sin_family = AF_INET;
	con_to.sin_port   = port;
	con_to.sin_addr   = addr;
	memset(con_to.sin_zero,0,8);

	if (connect(*sd, (struct sockaddr*)&con_to, sizeof(con_to))<0)
		return ERROR_TCP_CONNECT;

	return SUCCESS;
}

