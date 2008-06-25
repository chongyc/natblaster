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
 * @file helperfsm_private.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief prototypes for private helperfsm functions
 */

#ifndef __HELPERFSM_PRIVATE_H__
#define __HELPERFSM_PRIVATE_H__

/**
 * @brief the state to deal with a hello message
 *
 * @param list a pointer to the connection info list
 * @param item a pointer to the connection info item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_hello(connlist_t *list, connlist_item_t *item);

/**
 * @brief handles the second connection state
 *
 * @param list a pointer to the connection list
 * @param item a pointer to the item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_conn2(connlist_t *list, connlist_item_t *item);

/**
 * @brief handles sending a message with buddy info to peers
 *
 * @param list a pointer to the connection list
 * @param item a pointer to the item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_buddy_alloc(connlist_t *list, connlist_item_t *item);

/**
 * @brief handles sending a message with buddy port to peers, or determing
 * port through birthday paradox
 *
 * @param list a pointer to the connection list
 * @param peer a pointer to the peer item for this connection
 * @param buddy a pointer to the buddy item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_buddy_port(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy);

/**
 * @brief handles starting direct connection
 *
 * @param list a pointer to the connection list
 * @param peer a pointer to the peer item for this connection
 * @param buddy a pointer to the buddy item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_start_direct_conn(connlist_t *list, connlist_item_t *peer,
					connlist_item_t *buddy);

/**
 * @brief final state to recieve the peer's goodbye message
 *
 * @param list a pointer to the connection list
 * @param peer a pointer to the peer item for this connection
 * @param buddy a pointer to the buddy item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_goodbye(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy);

/**
 * @brief optional state that handles starting birthday paradox when the peer
 *        is the random one.
 *
 * @param list a pointer to the connection list
 * @param peer a pointer to the peer item for this connection
 * @param buddy a pointer to the buddy item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_start_peer_bday(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy);


/**
 * @brief optional state that handles ending birthday paradox when the peer is
 *        the random one.
 *
 * @param list a pointer to the connection list
 * @param peer a pointer to the peer item for this connection
 * @param buddy a pointer to the buddy item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_end_peer_bday(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy);

/**
 * @brief optional state that handles starting birthday paradox when the buddy
 *        is the random one.
 *
 * @param list a pointer to the connection list
 * @param peer a pointer to the peer item for this connection
 * @param buddy a pointer to the buddy item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_start_buddy_bday(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy);

/**
 * @brief optional state that handles ending birthday paradox when the buddy is
 *        the random one.
 *
 * @param list a pointer to the connection list
 * @param peer a pointer to the peer item for this connection
 * @param buddy a pointer to the buddy item for this connection
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode helper_fsm_end_buddy_bday(connlist_t *list, connlist_item_t *peer,
				connlist_item_t *buddy);

#endif /* __HELPERFSM_PRIVATE_H__ */

