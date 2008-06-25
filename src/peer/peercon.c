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
 * @file peercon.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief functions to assist the peer in making a direct connection
 */

#include "peercon.h"
#include "util.h"
#include "peerdef.h"
#include <time.h>
#include <stdlib.h>
#include "spoof.h"
#include "sniff.h"
#include "debug.h"

errorcode wait_for_direct_conn(flag_t *check_flag) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(check_flag,ERROR_NULL_ARG_1);

	/* do function */
	CHECK_FAILED(wait_for_flag(check_flag,FLAG_FAILED|FLAG_SUCCESS,
		DIRECT_CONNECTION_TIMEOUT),ERROR_1);

	return SUCCESS;
}

errorcode findDevice(char **dev) {

	/* declare local variables */
	char errbuf[PCAP_ERRBUF_SIZE];

	/* error check arguments */
	CHECK_NOT_NULL(dev,ERROR_NULL_ARG_1);

	/* do function */
	*dev = pcap_lookupdev(errbuf);

	if (*dev==NULL)
		return ERROR_NO_DEV_FOUND;

	return SUCCESS;
}

errorcode flood_syns(tcp_packet_info_t tcp_skeleton, char *device) {

	/* declare local variables */
	int i;

	/* error check arguments */
	CHECK_NOT_NULL(device,ERROR_NULL_ARG_1);

	/* do function */

	/* fill in missing tcp_skeleton fields */
	tcp_skeleton.syn_flag = FLAG_SET;
	tcp_skeleton.ack_flag = FLAG_UNSET;
	tcp_skeleton.window   = WINDOW_DEFAULT;
	tcp_skeleton.ack_num  = 0;

	DEBUG(DBG_BDAY,"DBAY:d_port = %u\n",DBG_PORT(tcp_skeleton.d_port));

	/* seed the random number generator */
	srand(time(NULL));

	for (i=0;i<SYN_FLOOD_COUNT;i++) {
		tcp_skeleton.s_port = (port_t)rand();
		CHECK_FAILED(spoof(&tcp_skeleton,device,NULL,0,TTL_TOO_LOW),
			ERROR_CALLED_FUNCTION);
	}

	return SUCCESS;
}

errorcode start_find_synack(peer_conn_info_t *info) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	info->bday.stop_synack_find = FLAG_UNSET;
	info->bday.find_synack_done = FLAG_UNSET;
	info->bday.port_set         = FLAG_UNSET;
	if (pthread_create(&info->bday.find_synack_tid,NULL,
						run_find_synack,info)<0)
		return ERROR_PTHREAD_CREATE_FAILED;

	return SUCCESS;
}

void *run_find_synack(void* arg) {

	/* declare local variables */
	peer_conn_info_t *cast_arg;
	errorcode ret;

	/* error check arguments */
	CHECK_NOT_NULL(arg,(void*)ERROR_NULL_ARG_1);

	/* do function*/
	cast_arg = (peer_conn_info_t*) arg;
	ret = capture_flooded_synack(cast_arg);

	cast_arg->bday.find_synack_done = FLAG_SET;

	if (FAILED(ret))
		return (void*)ERROR_CALLED_FUNCTION;
	else
		return (void*)SUCCESS;
}

errorcode wait_and_join_find_synack(peer_conn_info_t *info) {

	/* declare local variables */
	errorcode wait_ret;
	errorcode thread_ret;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */
	DBG_TIME("waiting for SYNACK flood listening thread to finished");
	/* wait for the find synack thread to finish */
	wait_ret = wait_for_flag(&info->bday.find_synack_done,FLAG_SET,
		FIND_SYN_ACK_TIMEOUT);

	/* force the thread to finish even if it wasn't done */
	info->bday.stop_synack_find = FLAG_SET;

	/* join on the thread */
	if (pthread_join(info->bday.find_synack_tid,(void**)&thread_ret)<0)
		return ERROR_PTHREAD_JOIN;
	DBG_TIME("finished waiting for SYNACK flood listening thread to finish");

	/* make sure no timeout happened, and that the thread was successful */
	if (FAILED(wait_ret))
		return ERROR_1;
	if (FAILED(thread_ret))
		return ERROR_2;

	return SUCCESS;
}

errorcode synack_flood(peer_conn_info_t *info, seq_num_t seq_num) {

	/* declare local variables */
	int i;
	tcp_packet_info_t skeleton;
	port_t port;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */

	/* fill in the tcp packet skeleton */
	skeleton.d_addr   = info->buddy.ext_ip;
	skeleton.s_addr   = info->peer.ip;
	skeleton.s_port   = info->peer.port;
	skeleton.seq_num  = (seq_num_t)rand();
	skeleton.ack_num  = SEQ_NUM_ADD(seq_num,1);
	skeleton.ack_flag = FLAG_SET;
	skeleton.syn_flag = FLAG_SET;
	skeleton.window   = WINDOW_DEFAULT;

	/* seed the random number generator */
	srand(time(NULL));

	/* do the flood */
	for (i=0;i<SYN_ACK_FLOOD_COUNT;i++) {
		skeleton.d_port = port = (port_t)rand();
		CHECK_FAILED(spoof(&skeleton, info->device,&port,sizeof(port),
			TTL_OK),ERROR_1);
	}

	return SUCCESS;
}

