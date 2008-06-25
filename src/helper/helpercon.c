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
 * @file helpercon.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains defintions for functions to help with a peer connection
 */

#include <string.h>
#include "helperfsm.h"
#include "helpercon.h"
#include <unistd.h>
#include <pthread.h>
#include "debug.h"
#include "def.h"
#include "util.h"

errorcode create_new_handler(connlist_t *list, observed_data_t *data,
			    sock_t sd) {

	/* declare variables */
	connlist_item_t *item;
	pthread_t tid;
	helper_fsm_thread_arg_t *arg;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(data,ERROR_NULL_ARG_2);

	/* do function */
	if ( (item=(connlist_item_t*)malloc(sizeof(connlist_item_t))) == NULL)
		return ERROR_MALLOC_FAILED_1;

	/* copy the observed data */
	memcpy(&item->obs_data,data,sizeof(observed_data_t));

	/* set the sd for the peer connection */
	item->info.socks.peer = sd;

	/* create new thread */
	if ( (arg = (helper_fsm_thread_arg_t*) malloc(
			sizeof(helper_fsm_thread_arg_t))) == NULL) {
		return ERROR_MALLOC_FAILED_2;
	}

	arg->list = list;
	arg->item = item;

	/* create a thread with the default attributes... */
	if (pthread_create(&tid,NULL,run_helper_fsm_thread,arg)<0) {
		safe_free(arg);
		return ERROR_PTHREAD_CREATE_FAILED;
	}
	/* .. and then detach it! */
	if (pthread_detach(tid)<0)
		return ERROR_PTHREAD_DETACH_FAILED;

	return SUCCESS;
}

void *run_helper_fsm_thread(void *arg) {

	/* declare variables */
	helper_fsm_thread_arg_t *cast_arg;

	/* check arguments */
	CHECK_NOT_NULL(arg,(void*)ERROR_NULL_ARG_1);

	/* do function */
	cast_arg = (helper_fsm_thread_arg_t*)arg;

	if (FAILED(helper_fsm_start(cast_arg->list, cast_arg->item))) {
		safe_free(arg);
		return (void*)ERROR_1;
	}

	safe_free(arg);
	return (void*) SUCCESS;
}

errorcode get_buddy(connlist_t *list, connlist_item_t *item,
				connlist_item_t **found_buddy) {

	/* declare local varibles */
	connlist_item_t *local_item;
	int timer = FIND_BUDDY_TIMEOUT;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(item,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(found_buddy,ERROR_NULL_ARG_3);

	/* do function */
	/* loop looking for the buddy */
	do {
		/* look for the item */
		DEBUG(DBG_BUDDY,"BUDDY:Finding buddy\n");
		if (FAILED(connlist_find(list,connlist_find_buddy,
				&item->info.buddy,&local_item))) {
			/* if it wasn't found, pause */
			sleep(1);
			timer = DEC_UNTIL_ZERO(timer);
		}
		else {
			/* if it was found, break out */
			break;
		}
	} while (timer>0);

	/* if the timer is 0, then the item wasn't found, otherwise it was */
	if (timer==0) {
		return ERROR_NOT_FOUND;
	}

	*found_buddy = local_item;

	return SUCCESS;
}

errorcode wait_for_buddy_port_alloc(helper_conn_info_t *buddy) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_1);

	/* do function */
	CHECK_FAILED(wait_for_flag(&buddy->port_alloc.method_set, FLAG_SET,
		WAIT_FOR_BUDDY_PORT_ALLOC_TIMEOUT),ERROR_CALLED_FUNCTION);

	return SUCCESS;
}

errorcode wait_for_buddy_port_known(helper_conn_info_t *buddy) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_1);

	/* do function */
	CHECK_FAILED(wait_for_flag(&buddy->port_alloc.ext_port_set, FLAG_SET,
		WAIT_FOR_BUDDY_PORT_KNOWN_TIMEOUT),ERROR_CALLED_FUNCTION);

	return SUCCESS;
}

errorcode wait_for_buddy_bday_port(helper_conn_info_t *buddy) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_1);

	/* do function */
	CHECK_FAILED(wait_for_flag(&buddy->bday.port_set, FLAG_SET,
		WAIT_FOR_BUDDY_BDAY_PORT_TIMEOUT),ERROR_CALLED_FUNCTION);

	return SUCCESS;
}

errorcode wait_for_buddy_syn_seq_num(helper_conn_info_t *buddy) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_1);

	/* do function */
	CHECK_FAILED(wait_for_flag(&buddy->buddy_syn.seq_num_set, FLAG_SET,
		WAIT_FOR_BUDDY_SEQ_NUM_TIMEOUT),ERROR_CALLED_FUNCTION);

	return SUCCESS;
}


errorcode wait_for_buddy_syn_flood(helper_conn_info_t *buddy) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(buddy,ERROR_NULL_ARG_1);

	/* do function */
	CHECK_FAILED(wait_for_flag(&buddy->bday.seq_num_set, FLAG_SET,
		WAIT_FOR_BUDDY_SYN_FLOOD_TIMEOUT),ERROR_CALLED_FUNCTION);

	return SUCCESS;
}

errorcode find_conn2(connlist_t *list, observed_data_t *find_data,
				connlist_item_t **found_item) {

	/* declare local variables */
	int timer = FIND_CONN2_TIMEOUT;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(find_data,ERROR_NULL_ARG_2);

	/* do function */
	do {
		if (FAILED(connlist_find(list,connlist_find_pred_port,
			find_data,found_item))) {
			sleep(1);
			timer = DEC_UNTIL_ZERO(timer);
		}
		else {
			return SUCCESS;
		}

	} while (timer>0);

	return ERROR_TIMEOUT;
}

