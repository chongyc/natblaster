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
 * @file helperdef.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief definitions for helper-specific data
 */
#ifndef __HELPERDEF_H__
#define __HELPERDEF_H__

#include "def.h"

/**
 * The timeouts specified below are based on experience on how long it really
 * takes for natblaster to succeed.  They are longer than is probably needed.
 **/

/** @brief the time in seconds to look for buddy info when searching */
#define FIND_BUDDY_TIMEOUT			20

/** @brief the time in seconds to look for a second connection */
#define FIND_CONN2_TIMEOUT			5

/** @brief the time in seconds to wait for the buddy port alloc to be set */
#define WAIT_FOR_BUDDY_PORT_ALLOC_TIMEOUT	20

/** @brief time in seconds to wait for the buddy's external port to be set */
#define WAIT_FOR_BUDDY_PORT_KNOWN_TIMEOUT	20

/** @brief time in seconds to wait for the buddy's SYN sequence number */
#define WAIT_FOR_BUDDY_SEQ_NUM_TIMEOUT		20

/** @brief time in seconds to wait for the buddy to send SYN a SYN FLOOD */
#define WAIT_FOR_BUDDY_SYN_FLOOD_TIMEOUT	20

/** @brief time in seconds to wait for the buddy external port to be set
 * through use of the birthday paradox */
#define WAIT_FOR_BUDDY_BDAY_PORT_TIMEOUT	20

/** @brief a structure to hold information about a bday flood */
struct bday_helper {
	/** @brief the sequence number in the SYN packets half of the flood */
	seq_num_t seq_num;
	/** @brief a flag indicating if the sequence number has been set */
	flag_t seq_num_set;
	/** @brief the port that the successful synack was sent to */
	port_t port;
	/** @brief indicates if the port value has been set */
	flag_t port_set;
	/** @brief a flag indicating if the bday port prediction failed */
	flag_t status;
} __attribute__((__packed__));

/** @brief a typedef for the bday_helper structure */
typedef struct bday_helper bday_helper_t;


/** @brief a structure to hold the sequence number from a SYN packet */
struct buddy_syn_seq_num {
	/** @brief the sequence number to the buddy */
	seq_num_t seq_num;
	/** @brief a flag indicating if the sequence number has been set */
	flag_t seq_num_set;
} __attribute__((__packed__));

/** @brief a typedef for teh buddy_syn_seq_num structure */
typedef struct buddy_syn_seq_num buddy_syn_seq_num_t;

/** @brief structure with all socket descriptors */
struct helper_sock_desc {
	/** @brief the socket used for persistent connection to peer */
	sock_t peer;
}__attribute__((__packed__));

/** @brief typedef for the helper_sock_desc structure */
typedef struct helper_sock_desc helper_sock_desc_t;

/** @brief structure with all the connection information */
struct helper_conn_info {
	/** @brief the peer info */
	peer_info_t peer;
	/** @brief the buddy info */
	buddy_info_t buddy;
	/** @brief the socket descriptors */
	helper_sock_desc_t socks;
	/** @brief the port allocation method */
	port_alloc_t port_alloc;
	/** @brief the sequence number to buddy */
	buddy_syn_seq_num_t buddy_syn;
	/** @brief information about a bday attempt */
	bday_helper_t bday;
} __attribute__((__packed__));

/** @brief typedef for teh helper_conn_info structure */
typedef struct helper_conn_info helper_conn_info_t;

/** @brief structure to hold observed connection data */
struct observed_data {
	/** @brief the peer's ip */
	ip_t ip;
	/** @brief the peer's port */
	port_t port;
} __attribute__((__packed__));

/** @brief typedef for the observed_data structure */
typedef struct observed_data observed_data_t;

#endif /* __HELPERDEF_H__ */

