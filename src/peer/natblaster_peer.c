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
 * @file natblaster_peer.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains natblaster entrypoint for peer
 */

#include "natblaster_peer.h"

#include "debug.h"
#include "errorcodes.h"
#include "nethelp.h"
#include "peerdef.h"
#include "peerfsm.h"
#include "peercon.h"

int natblaster_connect(ip_t helper_ip, port_t helper_port, ip_t peer_ip,
		       port_t peer_port, ip_t buddy_ext_ip, ip_t buddy_int_ip,
		       port_t buddy_int_port, char *device, flag_t random) {

	peer_conn_info_t info;

	/* the return type is "int", but I return "errorcode"s because I know that
	 * they are the same real type and that all the errorcodes are negative.
	 * This makes debugging easier. */

	/* first find a device if needed */
	if (device==NULL)
		CHECK_FAILED(findDevice(&device),ERROR_NO_DEV_FOUND);

	DEBUG(DBG_VERBOSE, "VERBOSE:connecting to Buddy....%s:%uint\n",
		DBG_IP(buddy_int_ip), DBG_PORT(buddy_int_port));
	DEBUG(DBG_VERBOSE," %sext\n", DBG_IP(buddy_ext_ip));
	DEBUG(DBG_VERBOSE, "VERBOSE:from local address.....%s:%u\n",
		DBG_IP(peer_ip), DBG_PORT(peer_port));
	DEBUG(DBG_VERBOSE, "VERBOSE:with helper............%s:%u\n",
		DBG_IP(helper_ip), DBG_PORT(helper_port));
	DEBUG(DBG_VERBOSE, "VERBOSE:using Device...........%s\n",
		device);
	DEBUG(DBG_VERBOSE, "VERBOSE: this peer is %srandom\n",
		(random==FLAG_SET ? "" : "not "));

	/* now put the info in a conn_info structure */
	info.helper.ip                = helper_ip;
	info.helper.port              = helper_port;
	info.peer.ip                  = peer_ip;
	info.peer.port                = peer_port;
	info.peer.set                 = FLAG_SET;
	info.buddy.int_ip             = buddy_int_ip;
	info.buddy.int_port           = buddy_int_port;
	info.buddy.ext_ip             = buddy_ext_ip;
	info.buddy.identifier         = FLAG_SET;
	info.buddy.ext_port           = PORT_UNKNOWN;
	info.buddy.ext_port_set       =  FLAG_UNSET;
	info.socks.helper             = SOCKET_UNKNOWN;
	info.socks.helper_pred        = SOCKET_UNKNOWN;
	info.socks.buddy              = SOCKET_UNKNOWN;
	info.device                   = device;
	info.direct_conn_status       = FLAG_UNSET;
	info.bday.stop_synack_find    = FLAG_UNSET;
	/* buddy sock gets filled in below */

	/* bind the desired port for a connection to buddy */
	CHECK_FAILED(bindSocket(info.peer.port,&info.socks.buddy),ERROR_1);


	/* bind socket for helper connection (2 before buddy port); */
	if (random==FLAG_UNSET)
		info.helper_conn.persistent_port = PORT_ADD(peer_port,-2);
	else /* bind a "random" port (not the conventional port) */
		info.helper_conn.persistent_port = PORT_ADD(peer_port,-3);
	CHECK_FAILED(bindSocket(info.helper_conn.persistent_port,
		&info.socks.helper),ERROR_2);

	/* bind port for second helper connection */
	info.helper_conn.prediction_port = PORT_ADD(peer_port,-1);
	CHECK_FAILED(bindSocket(info.helper_conn.prediction_port,
				&info.socks.helper_pred),ERROR_3);

	if (FAILED(peer_fsm_start(&info))) {
		/* close the sockets */
		close(info.socks.helper);
		close(info.socks.helper_pred);
		close(info.socks.buddy);
		return ERROR_4;
	}

	/* close helper sockets */
	close(info.socks.helper);
	close(info.socks.helper_pred);

	return info.socks.buddy;
}

