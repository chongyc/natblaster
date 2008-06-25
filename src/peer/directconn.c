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
 * @file directconn.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief functions to help in attempt to make direct connection
 */

#include "directconn.h"
#include "directconn_private.h"
#include <stdlib.h>
#include <pthread.h>
#include "util.h"
#include "debug.h"
#include "berkeleyapi.h"
#include <unistd.h>

errorcode start_direct_conn(peer_conn_info_t *info) {

	/* declare local variables */
	direct_conn_connect_arg_t *arg;
	pthread_t tid;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */

	/* create argument */
	if ( (arg = (direct_conn_connect_arg_t*) malloc (
			sizeof(direct_conn_connect_arg_t))) == NULL)
		return ERROR_1;
	arg->info = info;

	/* start the direct connection thread... */
	if (pthread_create(&tid,NULL,run_direct_conn_connect,arg)<0) {
		safe_free(arg);
		return ERROR_PTHREAD_CREATE_FAILED;
	}
	/* ...and the detach it! */
	if (pthread_detach(tid)<0)
		return ERROR_PTHREAD_DETACH_FAILED;

	return SUCCESS;
}

void *run_direct_conn_connect(void *arg) {

	/* declare local variables */
	direct_conn_connect_arg_t *cast_arg;
	struct sockaddr_in server;
	int ttl;

	/* error check arguments */
	CHECK_NOT_NULL(arg,(void*)ERROR_NULL_ARG_1);

	/* do function */

	sleep(1);/* HACK!!! to slow things down */

	cast_arg = (direct_conn_connect_arg_t*)arg;

	DEBUG(DBG_DIR_CONN,"DIR_CONN:starting connection\n");

	/* put the server info in the sockaddr_in struct */
	server.sin_family      = AF_INET;
	server.sin_port        = cast_arg->info->buddy.ext_port;
	server.sin_addr.s_addr = cast_arg->info->buddy.ext_ip;

	/* set the TTL too low */
	ttl = TTL_TOO_LOW;
	setsockopt(cast_arg->info->socks.buddy, IPPROTO_IP, IP_TTL,
			&ttl, sizeof(ttl));

	/* connect to the server */
	if( connect(cast_arg->info->socks.buddy, (struct sockaddr *)&server,
			sizeof(server)) < 0) {
		cast_arg->info->direct_conn_status = FLAG_FAILED;
		safe_free(arg);
		DEBUG(DBG_DIR_CONN,"DIR_CONN:Direction connection failed\n");
		return (void*)ERROR_TCP_CONNECT;
	}

	/* set the TTL back high */
	ttl = TTL_OK;
	setsockopt(cast_arg->info->socks.buddy, IPPROTO_IP, IP_TTL,
			&ttl, sizeof(ttl));

	DEBUG(DBG_DIR_CONN,"DIR_CONN:direct connection made!\n");

	cast_arg->info->direct_conn_status = FLAG_SUCCESS;
	safe_free(arg);
	return (void*)SUCCESS;
}

