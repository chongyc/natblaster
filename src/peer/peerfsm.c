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
 * @file peerfsm.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains defintions of fsm-like functions to control peer connection
 *        protocol control flow.
 *
 * @bug peer_fsm_forge_syn_ack only forges one SYN/ACK.  Because network could
 *      be lossy, maybe send more than one?
 */


#include "peerfsm.h"
#include "peerfsm_private.h"
#include "peercon.h"
#include "directconn.h"
#include "nethelp.h"
#include "netio.h"
#include "comm.h"
#include "debug.h"
#include "sniff.h"
#include "spoof.h"
#include <time.h>
#include <stdlib.h>

errorcode peer_fsm_start(peer_conn_info_t *info) {

	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	DBG_TIME("time at start of fsm");

	/* create the tcp connection to the helper */
	CHECK_FAILED(tcp_connect(info->helper.ip,info->helper.port,
	                         &(info->socks.helper)),ERROR_TCP_CONNECT);


	/* move into the hello state */
	CHECK_FAILED(peer_fsm_hello(info),ERROR_CALLED_FUNCTION);

	DBG_TIME("time at end of fsm");

	return SUCCESS;

}

errorcode peer_fsm_hello(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_hello_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */

	DBG_TIME("time at start of function");

	/* create the message payload... */
	msg.peer_ip         = info->peer.ip;
	msg.peer_port       = info->peer.port;
	msg.buddy_int_ip    = info->buddy.int_ip;
	msg.buddy_int_port  = info->buddy.int_port;
	msg.buddy_ext_ip    = info->buddy.ext_ip;

	DEBUG(DBG_BDAY,"BDAY: peer internal port: %u\n",
		DBG_PORT(info->peer.port));

	/* ...and send it */
	CHECK_FAILED(sendMsg(info->socks.helper,COMM_MSG_HELLO,&msg,
			     sizeof(msg)),ERROR_NETWORK_SEND);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent HELLO\n");

	/* call next state */
	CHECK_FAILED(peer_fsm_conn_again(info),ERROR_CALLED_FUNCTION);

	DBG_TIME("time at end of function");

	return SUCCESS;
}

errorcode peer_fsm_conn_again(peer_conn_info_t *info) {

	/* declare variables */

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("time at start of function");

	/* get message */
	CHECK_FAILED(readMsg(info->socks.helper,COMM_MSG_CONNECT_AGAIN,NULL,0),
		ERROR_NETWORK_READ);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:received CONNECT_AGAIN\n");

	/* open a second connection */
	CHECK_FAILED(tcp_connect(info->helper.ip,info->helper.port,
			&(info->socks.helper_pred)),ERROR_TCP_CONNECT);

	/* send a message indicating that the second connection has
	 * been made */
	CHECK_FAILED(sendMsg(info->socks.helper,COMM_MSG_CONNECTED_AGAIN,
			NULL,0),ERROR_NETWORK_SEND);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent CONNECTED_AGAIN\n");

	/* enter next state */
	if (FAILED(peer_fsm_check_port_pred(info))) {
		/* close the second connection socket */
		close(info->socks.helper_pred);
		return ERROR_CALLED_FUNCTION;
	}

	/* close the second connection */
	close(info->socks.helper_pred);

	DBG_TIME("time at end of function");

	return SUCCESS;
}

errorcode peer_fsm_check_port_pred(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_pred_port_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("time at start of function");

	/* read the next message */
	CHECK_FAILED(readMsg(info->socks.helper,COMM_MSG_PORT_PRED,
			&msg,sizeof(comm_msg_pred_port_t)),ERROR_NETWORK_READ);

	info->port_alloc.method = msg.port_alloc;

	DEBUG(DBG_PROTOCOL,"PROTOCOL:received PORT_PRED\n");
	DEBUG(DBG_VERBOSE,"VERBOSE:alloc method = %s\n",
		(info->port_alloc.method==COMM_PORT_ALLOC_SEQ) ?
		"sequential" : "random" );

	/* send message saying waiting for buddy information from helper  */
	CHECK_FAILED(sendMsg(info->socks.helper,
		COMM_MSG_WAITING_FOR_BUDDY_ALLOC, NULL, 0),ERROR_NETWORK_SEND);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent WAITING_FOR_BUDDY_ALLOC\n");
	/* enter next state */
	CHECK_FAILED(peer_fsm_buddy_alloc(info),ERROR_CALLED_FUNCTION);

	DBG_TIME("time at end of function");

	return SUCCESS;

}

errorcode peer_fsm_buddy_alloc(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_buddy_alloc_t buddy;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("time at start of function");

	/* receive the buddy info */
	CHECK_FAILED(readMsg(info->socks.helper,COMM_MSG_BUDDY_ALLOC,
		&buddy,sizeof(buddy)),ERROR_NETWORK_READ);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:received BUDDY_ALLOC\n");
	DEBUG(DBG_VERBOSE,"VERBOSE:buddy alloc method: %s\n",
		(buddy.buddy_port_alloc==COMM_PORT_ALLOC_SEQ
			? "sequential" : "random" ));
	DEBUG(DBG_VERBOSE,"VERBOSE:connection is %ssupported\n",
		(buddy.support==COMM_CONNECTION_SUPPORTED ? "" : "not "));


	/* send a message asking for the buddy's external port */
	CHECK_FAILED(sendMsg(info->socks.helper,COMM_MSG_WAITING_FOR_BUDDY_PORT,
		NULL,0),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent WAITING_FOR_BUDDY_PORT\n");

	/* enter next state */
	CHECK_FAILED(peer_fsm_buddy_port(info),ERROR_CALLED_FUNCTION);

	DBG_TIME("time at end of function");

	return SUCCESS;
}

errorcode peer_fsm_buddy_port(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_buddy_port_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("time at start of function");

	CHECK_FAILED(readMsg(info->socks.helper,COMM_MSG_BUDDY_PORT,&msg,
		sizeof(msg)),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received BUDDY_PORT\n");
	DEBUG(DBG_VERBOSE,"VERBOSE:buddy port = %d\n",
		DBG_PORT(msg.ext_port));
	DEBUG(DBG_VERBOSE,"VERBOSE:bday is %sneeded\n",
		(msg.bday == COMM_BDAY_NEEDED ? "" : "not "));

	/* put the port in the correct location in internal data structure */
	info->buddy.ext_port = msg.ext_port;

	/* here there is a deviation from the normal receive-do-send format
	 * of peer_fsm functions.  go to the next state based on if the bday
	 * paradox is needed */

	if (msg.bday == COMM_BDAY_NEEDED)	{
		/* the birthday paradox is needed */
		if (info->port_alloc.method == COMM_PORT_ALLOC_SEQ) {
			/* the peer is sequential and needs to helper predicte
			 * the buddy port */
			CHECK_FAILED(peer_fsm_reply_bday(info),
				ERROR_CALLED_FUNCTION);
		}
		else {
			/* the peer is random is needs to use bday paradox to
			 * determine it's port */
			CHECK_FAILED(peer_fsm_start_bday(info),
				ERROR_CALLED_FUNCTION_1);
		}
	}
	else {
		DEBUG(DBG_PROTOCOL,"PROTOCOL:straight connection will work\n");
		/* a straight connection will work */
		CHECK_FAILED(peer_fsm_start_direct_conn(info),
			ERROR_CALLED_FUNCTION);
	}

	DBG_TIME("time at end of function");

	return SUCCESS;
}

errorcode peer_fsm_start_direct_conn(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_buddy_syn_seq_t msg;

	/* error check arguments */
	CHECK_FAILED(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("time at start of function");

	CHECK_FAILED(start_direct_conn(info),ERROR_CALLED_FUNCTION_1);
	DEBUG(DBG_BDAY,"BDAY:started direct connection\n");
	CHECK_FAILED(capture_peer_to_buddy_syn(info),ERROR_CALLED_FUNCTION_2);

	/* the syn has been found, send it to the helper */
	msg.seq_num = info->buddy_syn.seq_num;
	DEBUG(DBG_VERBOSE,"VERBOSE:sequence number of buddy syn is %u\n",
		DBG_SEQ_NUM(msg.seq_num));

	/* send message */
	CHECK_FAILED(sendMsg(info->socks.helper,COMM_MSG_BUDDY_SYN_SEQ,
		&msg,sizeof(msg)),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent BUDDY_SYN_SEQ message\n");

	/* enter the next state */
	CHECK_FAILED(peer_fsm_forge_syn_ack(info),ERROR_CALLED_FUNCTION_3);

	DBG_TIME("time at end of function");

	return SUCCESS;
}

errorcode peer_fsm_forge_syn_ack(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_peer_syn_seq_t peer_syn_msg;
	comm_msg_goodbye_t goodbye;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("time at start of function");

	/* receive the sequence number to base SYN/ACK on */
	CHECK_FAILED(readMsg(info->socks.helper,COMM_MSG_PEER_SYN_SEQ,
		&peer_syn_msg,sizeof(peer_syn_msg)),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received PEER_SYN_SEQ\n");

	/* fill in info about syn/ack to forge - almost a straight copy of
	 * previous captured syn - just set ACK flag and fill in ack num */
	info->buddy_syn_ack.d_addr   = info->buddy_syn.d_addr;
	info->buddy_syn_ack.s_addr   = info->buddy_syn.s_addr;
	info->buddy_syn_ack.d_port   = info->buddy_syn.d_port;
	info->buddy_syn_ack.s_port   = info->buddy_syn.s_port;
	info->buddy_syn_ack.seq_num  = info->buddy_syn.seq_num;
	info->buddy_syn_ack.ack_num  = SEQ_NUM_ADD(peer_syn_msg.seq_num,1);
	info->buddy_syn_ack.window   = info->buddy_syn.window;
	info->buddy_syn_ack.ack_flag = FLAG_SET;
	info->buddy_syn_ack.syn_flag = FLAG_SET;

	/* forge the SYN/ACK */
	CHECK_FAILED(spoof(&info->buddy_syn_ack,info->device,NULL,0,TTL_OK),
		ERROR_CALLED_FUNCTION);
	DEBUG(DBG_VERBOSE,"VERBOSE:forged SYN/ACK to buddy\n");

	/* now just wait to success (hopefully) */
	CHECK_FAILED(wait_for_direct_conn(&info->direct_conn_status),
		ERROR_1);

	DEBUG(DBG_VERBOSE,"VERBOSE:connection attempt was %ssuccessful\n",
		((info->direct_conn_status==FLAG_SUCCESS) ? "" : "not "));

	/* send confirmation to helper that connection succeeded/failed */
	goodbye.success_or_failure = info->direct_conn_status;
	CHECK_FAILED(sendMsg(info->socks.helper,COMM_MSG_GOODBYE,&goodbye,
		sizeof(goodbye)),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent GOODBYE\n");

	DBG_TIME("time at end of function");

	/* DONE!!! */
	if (info->direct_conn_status == FLAG_FAILED)
		return ERROR_2;
	else
		return SUCCESS;
}

errorcode peer_fsm_start_bday(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_syn_flooded_t msg;
	tcp_packet_info_t skeleton;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */

	DBG_TIME("time at start of function");

	/* seed random sequence number */
	srand(time(NULL));

	/* create tcp skeleton */
	DEBUG(DBG_BDAY,"BDAY:buddy external port = %u\n",
		DBG_PORT(info->buddy.ext_port));
	skeleton.d_addr = info->buddy.ext_ip;
	skeleton.d_port = info->buddy.ext_port;
	skeleton.s_addr = info->peer.ip;
	skeleton.seq_num = (seq_num_t) rand();

	/* do flooding */
	DBG_TIME("starting SYN flood");
	CHECK_FAILED(flood_syns(skeleton,info->device),ERROR_1);
	DBG_TIME("finished SYN flood");

	/* start looking for the SYN/ACK */
	CHECK_FAILED(start_find_synack(info),ERROR_2);

	/* create message to send */
	msg.seq_num = skeleton.seq_num;

	/* send msg */
	CHECK_FAILED(sendMsg(info->socks.helper,COMM_MSG_SYN_FLOODED,&msg,
		sizeof(msg)), ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent SYN_FLOODED\n");

	/* enter next state */
	CHECK_FAILED(peer_fsm_end_bday(info),ERROR_CALLED_FUNCTION);

	DBG_TIME("time st end of function");

	return SUCCESS;
}

errorcode peer_fsm_end_bday(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_bday_success_port_t msg;
	/* error check arguments */
	CHECK_FAILED(info,ERROR_NULL_ARG_1);

	/* do function */

	DBG_TIME("time at start of function");

	/* receive message */
	CHECK_FAILED(readMsg(info->socks.helper,COMM_MSG_BUDDY_SYN_ACK_FLOODED,
		NULL,0),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received BUDDY_SYN_ACK_FLOODED\n");

	DBG_TIME("waiting for SYNACK flood");
	CHECK_FAILED(wait_and_join_find_synack(info),ERROR_1);
	DEBUG(DBG_BDAY,"BDAY:the synack flood was received\n");
	DBG_TIME("done waiting for SYNACK flood");

	/* fill in the message */
	msg.port = info->bday.port;

	DEBUG((DBG_VERBOSE|DBG_BDAY),"VERBOSE|BDAY:bday success port = %u\n",
		DBG_PORT(info->bday.port));

	/* send the message */
	CHECK_FAILED(sendMsg(info->socks.helper,COMM_MSG_BDAY_SUCCESS_PORT,
		&msg,sizeof(msg)),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent BDAY_SUCCESS_PORT\n");

	/* fill in the peer internal port location with the new internal port
	 * since the direct connection needs to know what it is */
	info->peer.port = info->bday.port;

	/* enter the next state */
	/* the next state is recursive, go back and receive another message
	 * with the buddy's port, and then continue with full knowledge */
	CHECK_FAILED(peer_fsm_buddy_port(info),ERROR_CALLED_FUNCTION);

	DBG_TIME("time at end of function");

	return SUCCESS;

}

errorcode peer_fsm_reply_bday(peer_conn_info_t *info) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("time at start of function");

	CHECK_FAILED(sendMsg(info->socks.helper,
		COMM_MSG_WAITING_TO_SYN_ACK_FLOOD,NULL,0),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent WAITING_TO_SYN_ACK_FLOOD\n");

	/* enter next state */
	CHECK_FAILED(peer_fsm_bday_synack_flood(info),ERROR_CALLED_FUNCTION);

	DBG_TIME("time at end of function");

	return SUCCESS;
}

errorcode peer_fsm_bday_synack_flood(peer_conn_info_t *info) {

	/* declare local variables */
	comm_msg_syn_ack_flood_seq_num_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("time at start of function");

	/* receive the message telling the peer to do the bday synack flood */
	CHECK_FAILED(readMsg(info->socks.helper,COMM_MSG_SYN_ACK_FLOOD_SEQ_NUM,
		&msg,sizeof(msg)),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received SYN_ACK_FLOOD_SEQ_NUM\n");

	/* do the synack flood */
	DBG_TIME("starting SYNACK flood");
	CHECK_FAILED(synack_flood(info,msg.seq_num),ERROR_1);
	DBG_TIME("finished SYNACK flood");

	/* send a message indicating the flood happened */
	CHECK_FAILED(sendMsg(info->socks.helper,COMM_MSG_SYN_ACK_FLOOD_DONE,
		NULL,0),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent SYN_ACK_FLOOD_DONE\n");

	/* enter the next state */
	/* this is recursive, go expect another message with the buddy's
	 * port information */
	CHECK_FAILED(peer_fsm_buddy_port(info),ERROR_CALLED_FUNCTION);

	DBG_TIME("time at end of function");
	return SUCCESS;
}

