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
 * @file comm.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains data structures and messages used in communication
 *        between a peer and the helper
 */

#ifndef __COMM_H__
#define __COMM_H__

#include "def.h"

/*********************************************************************
 * All communication packets are of the form:                        *
 *                                                                   *
 * Message Type [4 byte long value]                                  *
 * Length after this field [4 byte unsigned long]                    *
 * Rest of Message [a predefined structure]                          *
 *                                                                   *
 * When sent across the network, it is important to make sure that   *
 * EVERYTHING is net network byte order (big endian).  That includes *
 * the messsage type and message length fields!!!                    *
 *                                                                   *
 * The message length field isn't technically needed, but it is sent *
 * in case any future messages can have variable length.             *
 *                                                                   *
 *********************************************************************/

/*****************************************************************************
 *                    Useful Communication Constants                         *
 *****************************************************************************/
/** @brief the length of the message type field */
#define COMM_TYPE_LEN 4

/** @brief the length of the length field */
#define COMM_LENGTH_LEN 4

/** @brief the minimum comminication length (header size) */
#define COMM_MIN_LEN (COMM_TYPE_LEN + COMM_LENGTH_LEN)

/** @brief the length of the header (same as COMM_MIN_LEN) */
#define COMM_HEADER_LEN COMM_MIN_LEN

/** @brief the absolute maximum a message (including payload) can be */
#define COMM_MAX_LEN 1024


/*****************************************************************************
 *                    Useful Typedef's for COMM Info                         *
 *****************************************************************************/

/** @brief a typedef for the comm type field */
typedef long comm_type_t;

/** @brief a typdef for the comm length field */
typedef unsigned long comm_len_t;

/*****************************************************************************
 *                            Communication Types                            *
 *****************************************************************************/

/** @brief an intial message from a peer to helper starting everything
 * (message includes a payload of type comm_peer_info_t) */
#define COMM_MSG_HELLO				0x0001

/** @brief a message from the helper to the peer asking for a second connection
 *  to facilitate port prediction */
#define COMM_MSG_CONNECT_AGAIN			0x1000

/** @brief a message from the peer to the helper indicating that a second
    port prediction connection has been made */
#define COMM_MSG_CONNECTED_AGAIN		0x0002

/** @brief a message from the helper to the peer indicating the success of
    the first port prediction attempt */
#define COMM_MSG_PORT_PRED			0x1002

/** @brief a message from the peer to the helper indicating the peer is now
    waiting to get buddy info (this message only exists to maintain the
    ping/pong message flow) */
#define COMM_MSG_WAITING_FOR_BUDDY_ALLOC	0x0003

/** @brief a message from the helper to the peer informing the peer of the
 *  the buddy's port alloc method, and whether there is support for the
 *  connection */
#define COMM_MSG_BUDDY_ALLOC			0x1003

/** @brief a message from the peer to the helper asking for the buddy's
 *  external port */
#define COMM_MSG_WAITING_FOR_BUDDY_PORT		0x0004

/** @brief a message from the helper to the peer with info about the buddy */
#define COMM_MSG_BUDDY_PORT			0x1004

/** @brief message from the peer to the helper with sequence number in the SYN
 *  packet sent to the buddy */
#define COMM_MSG_BUDDY_SYN_SEQ			0x0005

/** @brief message from helper to peer relaying the sequence number in the SYN
 *  from the buddy that the peer should base a forged SYN/ACK on */
#define COMM_MSG_PEER_SYN_SEQ			0x1005

/** @brief a message from the peer to the helper terminating the protocol */
#define COMM_MSG_GOODBYE			0x0006

/** @brief a message from the peer to the helper indicating the birthay SYN
 *  flood has be done */
#define COMM_MSG_SYN_FLOODED			0x0101

/** @brief a message from the helper to the peer indicating that the buddy has
 *  commenced the bday synack flood */
#define COMM_MSG_BUDDY_SYN_ACK_FLOODED		0x1101

/** @brief a message from the peer to the helper indicating the port that made
 *  if through the NAT */
#define COMM_MSG_BDAY_SUCCESS_PORT		0x0102

/** @brief a message from the peer to the helper indicating the peer is ready
 *  to be told to do a birthday flood */
#define COMM_MSG_WAITING_TO_SYN_ACK_FLOOD	0x0201

/** @brief a message from the helper to the peer with the sequence number that
 *  the peer used in the SYN flood */
#define COMM_MSG_SYN_ACK_FLOOD_SEQ_NUM		0x1201

/** @brief a message from the peer to the helper indicating that the synack
 *  flood has been completed */
#define COMM_MSG_SYN_ACK_FLOOD_DONE		0x0202

/*****************************************************************************
 *                           Port Allocation Types                           *
 *****************************************************************************/

/** @brief port allocation method is unknown */
#define COMM_PORT_ALLOC_UNKNOWN		-1

/** @brief port allocation method is sequential */
#define COMM_PORT_ALLOC_SEQ		1

/** @brief port allocation method is random */
#define COMM_PORT_ALLOC_RAND		2

/*****************************************************************************
 *                        Communication Support Types                        *
 *****************************************************************************/
/** @brief unsupported connection */
#define COMM_CONNECTION_UNSUPPORTED	0

/** @brief supported connection */
#define COMM_CONNECTION_SUPPORTED	1

/*****************************************************************************
 *                      Flags indicating if DBay is needed                   *
 *****************************************************************************/
/** @brief birthday paradox not needed for peer */
#define COMM_BDAY_NOT_NEEDED		0

/** @brief birthday paradox needed for peer */
#define COMM_BDAY_NEEDED		1

/*****************************************************************************
 *                     Possible Communication Payloads                       *
 *****************************************************************************/

/** @brief structure for a COMM_MSG_HELLO message payload */
struct comm_msg_hello {
	/** @brief the peer's internal ip */
	ip_t peer_ip;
	/** @brief the peer's internal port */
	port_t peer_port;
	/** @brief the buddy's internal ip */
	ip_t buddy_int_ip;
	/** @brief the buddy's internal port */
	port_t buddy_int_port;
	/** @brief the buddy's external ip */
	ip_t buddy_ext_ip;
} __attribute__((__packed__));

/** @brief typedef for the COMM_MSG_HELLO payload structure */
typedef struct comm_msg_hello comm_msg_hello_t;



/** @brief structure to hold payload for a COMM_MSG_PORT_PRED message */
struct comm_msg_pred_port {
	/** @brief the port allocation method */
	flag_t port_alloc;
} __attribute__((__packed__));

/** @brief typedef for the COMM_MSG_PORT_PRED payload structure */
typedef struct comm_msg_pred_port comm_msg_pred_port_t;



/** @brief structure to hold payload for a COMM_MSG_BUDDY_ALLOC message */
struct comm_msg_buddy_alloc {
	/** @brief the buddy's port allocation method */
	flag_t buddy_port_alloc;
	/** @brief whether or not there is support for the connection */
	flag_t support;
} __attribute__((__packed__));

/** @brief typedef for the COMM_MSG_BUDDY_ALLOC payload structure */
typedef struct comm_msg_buddy_alloc comm_msg_buddy_alloc_t;



/** @brief structure to hold payload for a COMM_MSG_BUDDY_INFO message */
struct comm_msg_buddy_info {
	/** @brief the external port the buddy will work on */
	port_t ext_port;
	/** @brief whether the birthday paradox is needed to determine
	 *  peer's external port.  if it is, then the peer should start
	 *  the paradox */
	flag_t bday;
} __attribute__((__packed__));

/** @brief typedef for the COMM_MSG_BUDDY_PORT payload structure */
typedef struct comm_msg_buddy_info comm_msg_buddy_port_t;



/** @brief structure to hold payload for a COMM_MSG_SYN_TO_BUDDY_SEQ message */
struct comm_msg_buddy_syn_seq {
	/** @brief the sequence number of the SYN */
	seq_num_t seq_num;
} __attribute__((__packed__));

/** @brief a typedef for the COMM_MSG_BUDDY_SYN_SEQ message */
typedef struct comm_msg_buddy_syn_seq comm_msg_buddy_syn_seq_t;



/** @brief structure to hold the COMM_MSG_PEER_SYN_SEQ payload */
struct comm_msg_peer_syn_seq {
	/** @brief the sequence number from the peer's SYN to the buddy */
	seq_num_t seq_num;
} __attribute__((__packed__));

/** @brief typedef for the COMM_MSG_PEER_SYN_SEQ payload structure */
typedef struct comm_msg_peer_syn_seq comm_msg_peer_syn_seq_t;



/** @brief structure to hold the COMM_MSG_GOODBYE payload */
struct comm_msg_goodbye {
	/** @brief a flag indicating if the connection attempt worked */
	flag_t success_or_failure;
} __attribute__((__packed__));

/** @brief typedef for the COMM_MSG_GOODBYE structure */
typedef struct comm_msg_goodbye comm_msg_goodbye_t;


/** @brief structure to hold the COMM_MSG_SYN_FLOODED payload */
struct comm_msg_syn_flooded {
	/** @brief the sequence number used in the flood */
	seq_num_t seq_num;
} __attribute__((__packed__));

/** @brief a typedef for the COMM_MSG_SYN_FLOODED payload structure */
typedef struct comm_msg_syn_flooded comm_msg_syn_flooded_t;


/** @brief structure to hold the COMM_MSG_BDAY_SUCCESS_PORT payload */
struct comm_msg_bday_success_port {
	/** @brief the port that a synack was sent to in the bday flood */
	port_t port;
} __attribute__((__packed__));

/** @brief typedef for the COMM_MSG_BDAY_SUCCESS_PORT payload structure */
typedef struct comm_msg_bday_success_port comm_msg_bday_success_port_t;


/** @brief structure to hold the COMM_MSG_SYN_ACK_FLOOD_SEQ_NUM payload */
struct comm_msg_syn_ack_flood_seq_num {
	/** @brief the sequence number used in the SYNs sent by the buddy */
	seq_num_t seq_num;
} __attribute__((__packed__));

/** @brief typedef for the COMM_MSG_SYN_ACK_FLOOD_SEQ_NUM payload structure */
typedef struct comm_msg_syn_ack_flood_seq_num comm_msg_syn_ack_flood_seq_num_t;

#endif /* __COMM_H__ */

