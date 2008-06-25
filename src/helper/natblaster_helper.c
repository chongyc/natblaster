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
 * @file natblaster_helper.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains entrypoint for a helper into natblaster
 */

#include "natblaster_helper.h"

#include "connlist.h"
#include "debug.h"
#include "errorcodes.h"
#include "berkeleyapi.h"
#include "nethelp.h"
#include "helpercon.h"

int natblaster_server(port_t listen_port) {

	sock_t listen_sd;
	connlist_t list;
	observed_data_t data;
	sock_t this_sd;
	struct sockaddr_in peer_con;
	int peer_con_size;

	/* The return type is "int" and the return codes are "errorcodes".  Even
	 * though there aren't strictly the same type, I know they are, and using
	 * the errorcodes makes the code more readable and debug-able. */

	CHECK_FAILED(bindSocket(listen_port,&listen_sd),ERROR_BIND);

	/* initalize the list for connection information */
	CHECK_FAILED(connlist_init(&list),ERROR_INIT);

	if (listen(listen_sd,5)!=0)
		return ERROR_TCP_LISTEN;

	peer_con_size = sizeof(peer_con);
	/* loop forever */
	while (1) {
		this_sd = accept(listen_sd,(struct sockaddr*)&peer_con,
				 &peer_con_size);
		data.ip   = peer_con.sin_addr.s_addr;
		data.port = peer_con.sin_port;
		DEBUG(DBG_NETWORK,"NETWORK:recieved a connection!\n");
		DEBUG(DBG_LIST, "LIST:list size: %d\n",
			connlist_count(&list));
		CHECK_FAILED(create_new_handler(&list,&data,this_sd),ERROR_1);

	}

	/* should never happen */
	return ERROR_1;
}
