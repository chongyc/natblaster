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
 * @file peerdef.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief definitions for peer-specific data
 */
#ifndef __PEERDEF_H__
#define __PEERDEF_H__

#include "def.h"
#include <pcap.h>
#include <pthread.h>

/** @brief macro for the TTL value that is too low to reach the buddy.  In a
 * complete implementation this value would be auto-detected.  Here it is set
 * to a known OK value. */
#define TTL_TOO_LOW		2

/** @brief macro for the TTL value that is high enough to reach the buddy */
#define TTL_OK			64

/** @brief timeout time in seconds to wait for a direct connection flag */
#define DIRECT_CONNECTION_TIMEOUT	180

/** @brief the number of SYNs to send in a SYN flood */
#define SYN_FLOOD_COUNT			502

/** @brief the number of SYN/ACKs to send in a SYN/ACK flood */
#define SYN_ACK_FLOOD_COUNT		502

/** @brief time in seconds to timeout looking for a SYN/ACK flooded packet */
#define FIND_SYN_ACK_TIMEOUT		20

/** @brief struture to hold information pertaining to the birthday paradox */
struct bday_peer {
	/** @brief a flag to indicate whether or not to stop looking for
	 *  a synack */
	flag_t stop_synack_find;
	/** @brief the port that the synack came in on */
	port_t port;
	/** @brief indicates if the port has been set */
	flag_t port_set;
	/** @brief the synack has been found, or an error occured */
	flag_t find_synack_done;
	/** @brief the thread id of the thread started to find the synack */
	pthread_t find_synack_tid;
} __attribute__((__packed__));

/** @brief typedef for the bday structure */
typedef struct bday_peer bday_peer_t;

/** @brief structure with helper connection info */
struct helper_conn {
	/** @brief the port used for the persistent helper connection */
	port_t persistent_port;
	/** @brief the port used for the port prediction second connection */
	port_t prediction_port;
} __attribute__((__packed__));

/** @brief typedef for the helper_conn structure */
typedef struct helper_conn helper_conn_t;

/** @brief structure with all socket descriptors */
struct peer_sock_desc {
	/** @brief the socket used for persistent connection to helper */
	sock_t helper;
	/** @brief the socket used for the pport prediction connection */
	sock_t helper_pred;
	/** @brief the socket created for the connection to buddy */
	sock_t buddy;
} __attribute__((__packed__));

/** @brief typedef for the peer_sock_desc structure */
typedef struct peer_sock_desc peer_sock_desc_t;

/** @brief structure with all the connection information */
struct peer_conn_info {
	/** @brief the helper info */
	helper_info_t helper;
	/** @brief the peer info */
	peer_info_t peer;
	/** @brief the buddy info */
	buddy_info_t buddy;
	/** @brief the socket descriptors */
	peer_sock_desc_t socks;
	/** @brief the helper connection info */
	helper_conn_t helper_conn;
	/** @brief the port allocation type */
	port_alloc_t port_alloc;
	/** @brief the device to connect on (used for libpcap/libnet) */
	char *device;
	/** @brief the syn sent to the buddy */
	tcp_packet_info_t buddy_syn;
	/** @brief the syn/ack to send to the buddy */
	tcp_packet_info_t buddy_syn_ack;
	/** @brief a flag to indicate if the connection attempt to the buddy
	 *  has failed */
	flag_t direct_conn_status;
	/** @brief information about the birthday paradox SYN and SYN/ACK
	 * floods */
	bday_peer_t bday;
} __attribute__((__packed__));

/** @brief typedef for the peer_conn_info structure */
typedef struct peer_conn_info peer_conn_info_t;

#endif /* __PEERDEF_H__ */

