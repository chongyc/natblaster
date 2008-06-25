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
 * @file def.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief a handful of useful generic defintions
 */

#ifndef __DEF_H__
#define __DEF_H__

#include "berkeleyapi.h"
#include "flag.h"

/****************************************************************************
 *                        THE PORT TYPE DEFINITIONS                         *
 ****************************************************************************/

/** @brief typedef for port type (16 bit network byte order unsigned short) */
typedef unsigned short port_t;

/** @brief a macro to so port arithmatic */
#define PORT_ADD(x,y) (htons(ntohs(x)+y))

/** @brief a macro to return the port in host byte order */
#define PORT_2HBO(x) (ntohs(x))

/** @brief macro for unknown port value */
#define PORT_UNKNOWN	0


/****************************************************************************
 *                         THE IP TYPE DEFINITIONS                          *
 ****************************************************************************/

/** @brief typedef for an ip type (32 bit network byte order unsigned long ) */
typedef unsigned long ip_t;

/** @brief macro for unknown ip value */
#define IP_UNKNOWN	0

/** @brief the maximum length an IP string can be (xxx.xxx.xxx.xxx+NULL) = 16 */
#define MAX_IP_STR_LEN 16


/****************************************************************************
 *                        THE SOCK TYPE DEFINITIONS                         *
 ****************************************************************************/

/** @brief typedef for socket descriptor type */
typedef int sock_t;

/** @brief macro for an unknown socket value */
#define SOCKET_UNKNOWN	-1


/****************************************************************************
 *                    THE SEQUENCE NUMBER TYPE DEFINITIONS                  *
 ****************************************************************************/

/** @brief sequence number type (32 bit network byte order unsigned long) */
typedef unsigned long seq_num_t;

/** @brief macro to aid in sequence number math */
#define SEQ_NUM_ADD(x,y) (htonl(ntohl(x)+y))

/** @brief a macro to return the sequence number in host byte order */
#define SEQ_NUM_2HBO(x) (ntohl(x))

/** @brief macro for unknown sequence number */
#define SEQ_NUM_UNKNOWN		0

/****************************************************************************
 *                         THE WINDOW TYPE DEFINITIONS                      *
 ****************************************************************************/

/** @brief window type (16 bit network byte order unsigned short) */
typedef unsigned short window_t;

/** @brief a macro to return the window in host byte order */
#define WINDOW_2HBO(x) (ntohs(x))

/** @brief macro for unknown window size */
#define WINDOW_UNKNOWN		0

/** @brief macro for the default window size (used in spoofing) */
#define WINDOW_DEFAULT		0x6815

/****************************************************************************
 *                        THE STRUCTURE TYPE DEFINITIONS                    *
 ****************************************************************************/

/** @brief structure to hold essential information about a tcp packet */
struct tcp_packet_info {
	/** @brief the destination ip address */
	ip_t d_addr;
	/** @brief the source ip address */
	ip_t s_addr;
	/** @brief the destination port */
	port_t d_port;
	/** @brief the source port */
	port_t s_port;
	/** @brief the sequence number */
	seq_num_t seq_num;
	/** @brief the ack number */
	seq_num_t ack_num;
	/** @brief was the SYN flag set in the packet? */
	flag_t syn_flag;
	/** @brief was the ACK flag set in the packet? */
	flag_t ack_flag;
	/** @brief the window for the packet */
	window_t window;
} __attribute__((packed));

/** @brief typedef for the tcp_packet_info structure type */
typedef struct tcp_packet_info tcp_packet_info_t;

/** @brief a structure to contain the port allocation method, and a flag
 *  indicating when it is set */
struct port_alloc {
	/** @brief the port allocation method */
	flag_t method;
	/** @brief the flag indicating if the value is set */
	flag_t method_set;
	/** @brief the external predicted or found port */
	port_t ext_port;
	/** @brief if the external port has been set */
	flag_t ext_port_set;
} __attribute__((packed));

/** @brief typedef for the port alloc type */
typedef struct port_alloc port_alloc_t;

/** @brief structure with all the helper connection informatino */
struct helper_info {
	/** @brief the helper's ip address */
	ip_t ip;
	/** @brief the helper's port */
	port_t port;
} __attribute__((packed));

/** @brief typedef for the helper_info struct */
typedef struct helper_info helper_info_t;

/** @brief structure with all the peer connection information */
struct peer_info {
	/** @brief the peer's ip address */
	ip_t ip;
	/** @brief the peer's port for connection to buddy*/
	port_t port;
	/** @brief a flag indicating if the info has been set */
	flag_t set;
}__attribute__ ((packed));

/** @brief typedef for teh peer_info structure */
typedef struct peer_info peer_info_t;

/** @brief structure with all the buddy connection information */
struct buddy_info {
	/** @brief the buddy's external ip address */
	ip_t ext_ip;
	/** @brief the buddy's internal ip address */
	ip_t int_ip;
	/** @brief the buddy's internal port */
	port_t int_port;
	/** @brief the buddy's external port */
	port_t ext_port;
	/** @brief a flag indicating if the identifier fields of the buddy
	 *  info have been set */
	flag_t identifier;
	/** @brief a flag indicating if the external port has been set */
	flag_t ext_port_set;
} __attribute__((packed));

/** @brief typedef for the buddy_info structure */
typedef struct buddy_info buddy_info_t;

#endif /* __DEF_H__ */

