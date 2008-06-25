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
 * @file helpercon.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains prototypes for functions for a peer connection from a helper
 */

#ifndef __HELPERCON_H__
#define __HELPERCON_H__

#include "helperdef.h"
#include "connlist.h"
#include "errorcodes.h"
#include "def.h"

/** @brief structure to hold data passed into a helper_fsm thread */
struct helper_fsm_thread_arg {
	/** @brief the connection list */
	connlist_t *list;
	/** @brief the connection info */
	connlist_item_t *item;
} __attribute__((packed));

/** @brief typedef for the helper_fsm_thread_arg structure */
typedef struct helper_fsm_thread_arg helper_fsm_thread_arg_t;

/**
 * @brief creates a new detached thread to handle a peer connection
 *
 * @param list a pointer to the list to share data with other threads thru
 * @param data a pointer to the observed connection data, a copy of this data
 *        is made.
 * @param sd the socket descriptor for the connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode create_new_handler(connlist_t *list, observed_data_t *data,
			     sock_t sd);

/**
 * @brief a wrapper function for the helper fsm entry point
 *
 * This pthread should be started detacted, but it returns errorcodes just
 * in case.
 *
 * @param arg the sole void * pthread argument.  This arugment will be cast to
 *        a helper_fsm_thread_arg_t in the function.  THIS ARGUMENT MUST BE
 *        ALLOCATED ON THE HEAP WITH MALLOC, AS IT WILL BE FREED.
 * @return SUCCESS, errorcode on failure
 */
void *run_helper_fsm_thread(void *arg);

/**
 * @brief finds and returns buddy info from the thread-shared list
 *
 * This function looks for FIND_BUDDY_TIMEOUT seconds, and then gives up
 *
 * @param list pointer to the connlist_t structure to find the buddy in
 * @param item pointer to the item for the looking peer info
 * @param found_buddy pointer pointer to fill in with buddy info
 *
 * @return SUCCESS, errorcode on failure or timeout
 */
errorcode get_buddy(connlist_t *list, connlist_item_t *item,
			connlist_item_t **found_buddy);
/**
 * @brief returns success once the buddy's port allocation method has been
 *        set.  timeouts out after WAIT_FOR_BUDDY_PORT_ALLOC_TIMEOUT
 *
 * @param buddy pointer to the buddy's helper_conn_info_t structure
 *
 * @return SUCCESS when alloc method set, errorcode on error or timeout
 */
errorcode wait_for_buddy_port_alloc(helper_conn_info_t *buddy);

/**
 * @brief returns success once the buddy's external port has been set.
 *        timeouts out after WAIT_FOR_BUDDY_PORT_KNOWN_TIMEOUT
 *
 * @param buddy pointer to the buddy's helper_conn_info_t structure
 *
 * @return SUCCESS when port known is set, errorcode on error or timeout
 */
errorcode wait_for_buddy_port_known(helper_conn_info_t *buddy);

/**
 * @brief returns success once the buddy's external port has been set through
 *        the birthday paradox.  timeouts out after
 *        WAIT_FOR_BUDDY_BDAY_PORT_TIMEOUT
 *
 * @param buddy pointer to the buddy's helper_conn_info_t structure
 *
 * @return SUCCESS when port known is set, errorcode on error or timeout
 */
errorcode wait_for_buddy_bday_port(helper_conn_info_t *buddy);


/**
 * @brief returns success once the buddy's SYN sequence number has been set.
 *        timeouts out after WAIT_FOR_BUDDY_SEQ_NUM_TIMEOUT
 *
 * @param buddy pointer to the buddy's helper_conn_info_t structure
 *
 * @return SUCCESS when seq num flag is set, errorcode on error or timeout
 */
errorcode wait_for_buddy_syn_seq_num(helper_conn_info_t *buddy);

/**
 * @brief returns success once the buddy's SYN sequence number has been set.
 *        timeouts out after WAIT_FOR_BUDDY_SYN_FLOOD_TIMEOUT
 *
 * @param buddy pointer to the buddy's helper_conn_info_t structure
 *
 * @return SUCCESS when seq num flag is set, errorcode on error or timeout
 */
errorcode wait_for_buddy_syn_flood(helper_conn_info_t *buddy);

/**
 * @brief finds a second connection, timing out if it takes too long
 *
 * timeout id FIND_CONN2_TIMEOUT
 *
 * @param list pointer to the list to look in
 * @param find_data the data to match on when searching
 * @param found_item pointer to a pointer to set to the found item
 *
 * @return SUCCESS, errorcode on timeout or failure
 */
errorcode find_conn2(connlist_t *list, observed_data_t *find_data,
				connlist_item_t **found_item);

#endif /* __HELPERCON_H__ */
