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
 * @file helperfsm.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains defintions of fsm-like functions to control helper connection
 *        protocol control flow.
 */

#include "helperfsm.h"
#include "helperfsm_private.h"
#include "comm.h"
#include "netio.h"
#include "debug.h"
#include "helpercon.h"
#include <string.h>
#include <unistd.h>

errorcode helper_fsm_start(connlist_t *list, connlist_item_t *item) {

	/* declare variables */
	errorcode ret;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(item,ERROR_NULL_ARG_2);

	/* do function */
	/* set initial values */
	item->info.port_alloc.method        = COMM_PORT_ALLOC_UNKNOWN;
	item->info.port_alloc.method_set    = FLAG_UNSET;
	item->info.port_alloc.ext_port      = PORT_UNKNOWN;
	item->info.port_alloc.ext_port_set  = FLAG_UNSET;
	item->info.peer.ip                  = IP_UNKNOWN;
	item->info.peer.port                = PORT_UNKNOWN;
	item->info.peer.set                 = FLAG_UNSET;
	item->info.buddy.ext_ip             = IP_UNKNOWN;
	item->info.buddy.ext_port           = PORT_UNKNOWN;
	item->info.buddy.int_ip             = IP_UNKNOWN;
	item->info.buddy.int_port           = PORT_UNKNOWN;
	item->info.buddy.identifier         = FLAG_UNSET;
	item->info.buddy.ext_port_set       = FLAG_UNSET;
	item->info.buddy_syn.seq_num        = SEQ_NUM_UNKNOWN;
	item->info.buddy_syn.seq_num_set    = FLAG_UNSET;
	item->info.bday.seq_num             = SEQ_NUM_UNKNOWN;
	item->info.bday.seq_num_set         = FLAG_UNSET;
	item->info.bday.port                = PORT_UNKNOWN;
	item->info.bday.port_set            = FLAG_UNSET;
	item->info.bday.status              = FLAG_UNSET;

	/* add info to the list */
	if(FAILED(connlist_add(list,item))) {
		/* close the socket */
		close(item->info.socks.peer);
		return ERROR_LIST_ADD;
	}

	DEBUG(DBG_LIST,"LIST:item Watchers: %d\n",(int)item->watchers);

	/* call next state */
	ret = helper_fsm_hello(list,item);
	if (FAILED(ret)) {
		if (ret==ERROR_NETWORK_READ){
			DEBUG(DBG_PROTOCOL,"PROTOCOL:no hello message\n");
		}
		/* close the socket */
		close(item->info.socks.peer);
		/* if the state fails, remove the list item */
		CHECK_FAILED(connlist_forget(list,connlist_item_match,item),
			ERROR_LIST_REMOVE_1);
		return ERROR_CALLED_FUNCTION;
	}

	/* close the socket */
	close(item->info.socks.peer);

	CHECK_FAILED(connlist_forget(list,connlist_item_match,item),
			ERROR_LIST_REMOVE_2);

	return SUCCESS;
}

errorcode helper_fsm_hello(connlist_t *list, connlist_item_t *item) {

	/* declare variables */
	comm_msg_hello_t hello;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(item,ERROR_NULL_ARG_2);

	/* do function */
	/* this is a strange case, but it is OK if no acceptable message
	 * is received on this read.  It was probably a port prediction
	 * second connection */
	CHECK_FAILED(readMsg(item->info.socks.peer, COMM_MSG_HELLO,
				&hello, sizeof(hello)), ERROR_NETWORK_READ);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:received HELLO\n");

	/* save out info from the message */
	item->info.peer.port         = hello.peer_port;
	item->info.peer.ip           = hello.peer_ip;
	item->info.peer.set          = FLAG_SET;
	item->info.buddy.int_ip      = hello.buddy_int_ip;
	item->info.buddy.int_port    = hello.buddy_int_port;
	item->info.buddy.ext_ip      = hello.buddy_ext_ip;
	item->info.buddy.identifier  = FLAG_SET;

	DEBUG(DBG_VERBOSE,"VERBOSE:Information from peer hello\n");
	DEBUG(DBG_VERBOSE,"VERBOSE:peer.............%s:%u\n",
		DBG_IP(item->info.peer.ip),
		DBG_PORT(item->info.peer.port));
	DEBUG(DBG_VERBOSE,"VERBOSE:buddy internal...%s:%u\n",
		DBG_IP(item->info.buddy.int_ip),
		DBG_PORT(item->info.buddy.int_port));
	DEBUG(DBG_VERBOSE,"VERBOSE:buddy external...%s\n",
		DBG_IP(item->info.buddy.ext_ip));

	/* send the next message */
	CHECK_FAILED(sendMsg(item->info.socks.peer, COMM_MSG_CONNECT_AGAIN,
		NULL, 0),ERROR_NETWORK_SEND);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent CONNECT_AGAIN\n");

	/* go to next state */
	CHECK_FAILED(helper_fsm_conn2(list,item),ERROR_CALLED_FUNCTION);

	return SUCCESS;
}


errorcode helper_fsm_conn2(connlist_t *list, connlist_item_t *item) {

	/* declare local variables */
	observed_data_t find_data;
	connlist_item_t *port_pred_item = NULL;
	comm_msg_pred_port_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(item,ERROR_NULL_ARG_2);

	/* do function */

	CHECK_FAILED(readMsg(item->info.socks.peer, COMM_MSG_CONNECTED_AGAIN,
			NULL,0),ERROR_NETWORK_READ);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:CONNECTED_AGAIN\n");

	/* look for info about this second connection in the list */
	/* copy of the observed data */
	memcpy(&find_data, &item->obs_data, sizeof(observed_data_t));
	/* and expect the port prediction port to be on the next sequential
	 * port */
	find_data.port = PORT_ADD(find_data.port,1);

	/* wait 2 seconds, to give the second connection time to add
	 * it's item to the list (this is instead of having a timeout, in
	 * essence always wait the full timeout) */

	DEBUG((DBG_PORT_PRED|DBG_LIST), "PORT_PRED|LIST:finding 2nd connection entry in list\n");
	if (FAILED(find_conn2(list,&find_data,&port_pred_item))) {

		DEBUG(DBG_PORT_PRED,"PORT_PRED:couldn't find 2nd connection\n");
		msg.port_alloc = COMM_PORT_ALLOC_RAND;
		/* set the allocation method */
		item->info.port_alloc.method       = COMM_PORT_ALLOC_RAND;
		item->info.port_alloc.method_set   = FLAG_SET;
		/* set the external port definitively to unknown */
		item->info.port_alloc.ext_port     = PORT_UNKNOWN;
		item->info.port_alloc.ext_port_set = FLAG_SET;
	}
	else {
		DEBUG(DBG_PORT_PRED,"PORT_PRED:found 2nd connection\n");
		msg.port_alloc = COMM_PORT_ALLOC_SEQ;
		/* set the port allocation method */
		item->info.port_alloc.method     = COMM_PORT_ALLOC_SEQ;
		item->info.port_alloc.method_set = FLAG_SET;
		/* set the predicted port to 2 higher than the port for this
		 * connection */
		item->info.port_alloc.ext_port     = PORT_ADD(
			item->obs_data.port,2);
		item->info.port_alloc.ext_port_set = FLAG_SET;

		/* forget about the port prediction second connection */
		DEBUG(DBG_LIST, "LIST:forgeting about port pred entry\n");
		CHECK_FAILED(connlist_forget(list,connlist_item_match,
			port_pred_item),ERROR_1);
	}

	DEBUG(DBG_PORT_PRED, "PORT_PRED:port alloc method is %s\n",
		(item->info.port_alloc.method==COMM_PORT_ALLOC_SEQ) ?
		"sequential" : "random" );

	/* tell the peer the port allocation method */
	CHECK_FAILED(sendMsg(item->info.socks.peer, COMM_MSG_PORT_PRED,
		&msg,sizeof(msg)),ERROR_NETWORK_SEND);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent PORT PRED\n");

	/* enter next state */
	CHECK_FAILED(helper_fsm_buddy_alloc(list,item),ERROR_CALLED_FUNCTION);

	return SUCCESS;
}

errorcode helper_fsm_buddy_alloc(connlist_t *list, connlist_item_t *item) {

	/* declare variables */
	connlist_item_t *found_buddy = NULL;
	comm_msg_buddy_alloc_t msg;
	errorcode ret;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(item,ERROR_NULL_ARG_2);

	/* do function */

	ret = SUCCESS;

	/* receive the waiting message */
	CHECK_FAILED(readMsg(item->info.socks.peer,
		COMM_MSG_WAITING_FOR_BUDDY_ALLOC,NULL,0),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received WAITING_FOR_BUDDY_ALLOC\n");


	/* get pointer to buddy's info*/
	if (FAILED(get_buddy(list,item,&found_buddy))){
		DEBUG(DBG_BUDDY,"BUDDY:couldn't find buddy\n");
		return ERROR_1;
	}

	DEBUG(DBG_BUDDY,"BUDDY:found buddy\n");
	/* check to make sure all buddy's info has been filled in,
	 * then fill in the message */

	if (FAILED(wait_for_buddy_port_alloc(&(found_buddy->info)))){
		ret = ERROR_1;
		goto forget_and_return;
	}

	msg.buddy_port_alloc = found_buddy->info.port_alloc.method;
	if ( (found_buddy->info.port_alloc.method == COMM_PORT_ALLOC_RAND) &&
	     (item->info.port_alloc.method == COMM_PORT_ALLOC_RAND)         )
		     msg.support = COMM_CONNECTION_UNSUPPORTED;
	else
		msg.support = COMM_CONNECTION_SUPPORTED;

	/* send out message*/
	if (FAILED(sendMsg(item->info.socks.peer,COMM_MSG_BUDDY_ALLOC,
		&msg,sizeof(comm_msg_buddy_alloc_t)))) {
		ret = ERROR_NETWORK_SEND;
		DEBUG(DBG_PROTOCOL,
			"fsm_buddy_alloc:PROTOCOL:sent BUDDY_ALLOC\n");
		goto forget_and_return;
	}

	if (msg.support == COMM_CONNECTION_UNSUPPORTED) {
		DEBUG(DBG_VERBOSE, "VERBOSE:connection unsupported!\n");
		ret = ERROR_2;
		goto forget_and_return;
	}


	/* enter next state */
	if (FAILED(helper_fsm_buddy_port(list,item,found_buddy))) {
		ret = ERROR_CALLED_FUNCTION;
		goto forget_and_return;
	}

forget_and_return:
	/* forget the buddy's info */
	DEBUG(DBG_LIST, "LIST:forgeting about buddy entry\n");
	CHECK_FAILED(connlist_forget(list,connlist_item_match,found_buddy),
		ERROR_3);
	return ret;
}

errorcode helper_fsm_buddy_port(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy) {
	/* declare variables */
	comm_msg_buddy_port_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(peer,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_3);

	/* do function */

	/* get message from peer */
	CHECK_FAILED(readMsg(peer->info.socks.peer,
		COMM_MSG_WAITING_FOR_BUDDY_PORT,NULL,0),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received WAITING_FOR_BUDDY_PORT\n");

	/* as soon as the buddy's port is known send it to the peer and attach
	 * a note indicating if the peer should do the birthday paradox so it's
	 * port can be detected (the peer should be able to determine on it's
	 * own if this is needed because the helper already told the peer the
	 * peer's port allocation method, so this is for sanity checking only)
	 */

	/* wait for the buddy's port */
	CHECK_FAILED(wait_for_buddy_port_known(&(buddy->info)),ERROR_1);
	/* fill in the message */
	msg.ext_port = buddy->info.port_alloc.ext_port;
	msg.bday = ( ( (peer->info.port_alloc.method == COMM_PORT_ALLOC_RAND)
		|| (buddy->info.port_alloc.method == COMM_PORT_ALLOC_RAND))
			? COMM_BDAY_NEEDED
			: COMM_BDAY_NOT_NEEDED
		   );

	/* send the message */
	CHECK_FAILED(sendMsg(peer->info.socks.peer,COMM_MSG_BUDDY_PORT,
		&msg,sizeof(msg)),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent BUDDY_PORT\n");

	/* enter next state - it depends on port allocation method */
	if (peer->info.port_alloc.method==COMM_PORT_ALLOC_RAND) {
		/* this connections peer is random */
		CHECK_FAILED(helper_fsm_start_peer_bday(list,peer,buddy),
			ERROR_CALLED_FUNCTION_1);
	}
	else if (buddy->info.port_alloc.method==COMM_PORT_ALLOC_RAND) {
		/* the buddy connection is random, the peer is going to
		 * need to help find a port */
		CHECK_FAILED(helper_fsm_start_buddy_bday(list,peer,buddy),
			ERROR_CALLED_FUNCTION_2);
	}
	else {
		/* neither peer is random, go ahead with connection */
		CHECK_FAILED(helper_fsm_start_direct_conn(list,peer,buddy),
			ERROR_CALLED_FUNCTION_3);
	}
	/**
	 * there is no check to make sure both peers aren't random, it is assumed
	 * that this case is handled in an earlier state (since it is
	 * unsupported).
	 **/

	return SUCCESS;
}



errorcode helper_fsm_start_direct_conn(connlist_t *list, connlist_item_t *peer,
					connlist_item_t *buddy) {

	/* declare local variables */
	comm_msg_buddy_syn_seq_t buddy_syn_msg;
	comm_msg_peer_syn_seq_t peer_syn_msg;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(peer,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_3);

	/* do function */

	/* get message from peer with buddy seq num */
	CHECK_FAILED(readMsg(peer->info.socks.peer,COMM_MSG_BUDDY_SYN_SEQ,
		&buddy_syn_msg,sizeof(buddy_syn_msg)),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received BUDDY_SYN_SEQ\n");
	DEBUG(DBG_VERBOSE,"VERBOSE:sequence number is %u\n",
		DBG_SEQ_NUM(buddy_syn_msg.seq_num));

	/* fill in the information about this syn sequence number */
	peer->info.buddy_syn.seq_num     = buddy_syn_msg.seq_num;
	peer->info.buddy_syn.seq_num_set = FLAG_SET;

	/* make payload to send in next message. first wait for the seq num
	 * and then fill it in the payload */
	CHECK_FAILED(wait_for_buddy_syn_seq_num(&(buddy->info)),ERROR_1);
	peer_syn_msg.seq_num = buddy->info.buddy_syn.seq_num;

	/* send the message */
	CHECK_FAILED(sendMsg(peer->info.socks.peer,COMM_MSG_PEER_SYN_SEQ,
		&peer_syn_msg,sizeof(peer_syn_msg)),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent PEER_SYN_SEQ\n");

	/* enter next state */
	CHECK_FAILED(helper_fsm_goodbye(list,peer,buddy),
		ERROR_CALLED_FUNCTION);

	return SUCCESS;
}

errorcode helper_fsm_goodbye(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy) {

	/* declare local variables */
	comm_msg_goodbye_t goodbye;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(peer,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_2);

	/* do function */

	/* receive the message */
	CHECK_FAILED(readMsg(peer->info.socks.peer,COMM_MSG_GOODBYE, &goodbye,
		sizeof(goodbye)),ERROR_NETWORK_READ);

	DEBUG(DBG_PROTOCOL,"PROTOCOL:received GOODBYE\n");

	DEBUG(DBG_VERBOSE,"VERBOSE:peer's connection was %sa success!\n",
		((goodbye.success_or_failure==FLAG_FAILED) ? "not ": ""));

	return SUCCESS;
}

errorcode helper_fsm_start_peer_bday(connlist_t *list, connlist_item_t *peer, 
				connlist_item_t *buddy) {

	/* declare local variables */
	comm_msg_syn_flooded_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(peer,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_3);

	/* do function */

	/* receive message indicating that the flood happened */
	CHECK_FAILED(readMsg(peer->info.socks.peer,COMM_MSG_SYN_FLOODED,
		&msg,sizeof(msg)),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received SYN_FLOODED\n");

	/* set the value for the sequence number in the peer's info */
	peer->info.bday.seq_num = msg.seq_num;
	peer->info.bday.seq_num_set = FLAG_SET;

	/* send message indicating the buddy is about to commence sending the
	 * SYN/ACKs */
	CHECK_FAILED(sendMsg(peer->info.socks.peer,
		COMM_MSG_BUDDY_SYN_ACK_FLOODED,NULL,0),ERROR_NETWORK_SEND);

	/* enter next state */
	CHECK_FAILED(helper_fsm_end_peer_bday(list,peer,buddy),
		ERROR_CALLED_FUNCTION);

	return ERROR_1;
}

errorcode helper_fsm_end_peer_bday(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy) {

	/* declare local variables */
	comm_msg_bday_success_port_t receive_msg;
	comm_msg_buddy_port_t send_msg;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(peer,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_3);

	/* do function */

	if (FAILED(readMsg(peer->info.socks.peer,COMM_MSG_BDAY_SUCCESS_PORT,
		&receive_msg,sizeof(receive_msg)))) {
		peer->info.bday.status = FLAG_FAILED;
		return ERROR_NETWORK_READ;
	}

	/* set the port value */
	peer->info.bday.port               = receive_msg.port;
	peer->info.bday.port_set           = FLAG_SET;
	peer->info.bday.status             = FLAG_SUCCESS;
	/* this location for the external port is no longer valid, since the
	 * flag was already set, but I set it here just in case I screwed up
	 * somewhere else in the code and look at it */
	peer->info.port_alloc.ext_port     = receive_msg.port;
	peer->info.port_alloc.ext_port_set = FLAG_SET;

	/* send a message with the buddy's port, to go back to the buddy port
	 * state.  there is no need to wait for the port value to be set, it
	 * is obviously set, since it was set before */
	send_msg.ext_port = buddy->info.port_alloc.ext_port;
	send_msg.bday     =  COMM_BDAY_NOT_NEEDED;

	CHECK_FAILED(sendMsg(peer->info.socks.peer,COMM_MSG_BUDDY_PORT,
		&send_msg,sizeof(send_msg)),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent BUDDY_PORT...again\n");

	/* go to the start direct conncetion state now that all ports are
	 * known */
	CHECK_FAILED(helper_fsm_start_direct_conn(list,peer,buddy),
		ERROR_CALLED_FUNCTION);

	return SUCCESS;

}

errorcode helper_fsm_start_buddy_bday(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy) {

	/* declare local variables */
	comm_msg_syn_ack_flood_seq_num_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(peer,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_3);

	/* do function */

	CHECK_FAILED(readMsg(peer->info.socks.peer,
		COMM_MSG_WAITING_TO_SYN_ACK_FLOOD,NULL,0),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received WAITING_TO_SYN_ACK_FLOOD\n");

	/* as soon as the bday.seq_num_set flag is set, it is time for this
	 * peer to flood synacks */
	CHECK_FAILED(wait_for_buddy_syn_flood(&buddy->info),ERROR_1);

	/* make the message... */
	msg.seq_num = buddy->info.bday.seq_num;
	/* ...and sent it */
	CHECK_FAILED(sendMsg(peer->info.socks.peer,
		COMM_MSG_SYN_ACK_FLOOD_SEQ_NUM,&msg,sizeof(msg)),
		ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent SYN_ACK_FLOOD_SEQ_NUM\n");

	/* enter the next state */
	CHECK_FAILED(helper_fsm_end_buddy_bday(list,peer,buddy),
		ERROR_CALLED_FUNCTION);

	return ERROR_1;
}

errorcode helper_fsm_end_buddy_bday(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy) {

	/* declare local variables */
	comm_msg_buddy_port_t msg;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(peer,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_3);

	/* do function */

	/* receive the message indicating the synack flood was done */
	CHECK_FAILED(readMsg(peer->info.socks.peer,COMM_MSG_SYN_ACK_FLOOD_DONE,
		NULL,0),ERROR_NETWORK_READ);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:received SYN_ACK_FLOOD_DONE\n");

	/* wait for the buddy to set the external port */
	CHECK_FAILED(wait_for_buddy_bday_port(&(buddy->info)),ERROR_1);

	/* now, resend the COMM_MSG_BUDDY_PORT message, but this time mark
	 * the bday flag as unneeded
	 */
	msg.ext_port = buddy->info.bday.port;
	msg.bday     = COMM_BDAY_NOT_NEEDED;

	/* now send the message */
	CHECK_FAILED(sendMsg(peer->info.socks.peer,COMM_MSG_BUDDY_PORT,
		&msg,sizeof(msg)),ERROR_NETWORK_SEND);
	DEBUG(DBG_PROTOCOL,"PROTOCOL:sent BUDDY_PORT...again\n");

	/* go to the start direct conncetion state now that all ports are
	 * known */
	CHECK_FAILED(helper_fsm_start_direct_conn(list,peer,buddy),
		ERROR_CALLED_FUNCTION);

	return SUCCESS;

}

