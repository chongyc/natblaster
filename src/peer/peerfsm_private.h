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
 * @file peerfsm_private.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief prototypes for private peerfsm functions
 */

#ifndef __PEERFSM_PRIVATE_H__
#define __PEERFSM_PRIVATE_H__

/**
 * @brief the hello state in the fsm
 *
 * @param info a pointer to the connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_hello(peer_conn_info_t *info);

/**
 * @brief handles a connect again message
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_conn_again(peer_conn_info_t *info);

/**
 * @brief handles a port prediction message
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_check_port_pred(peer_conn_info_t *info);

/**
 * @brief handles a buddy alloc message
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_buddy_alloc(peer_conn_info_t *info);

/**
 * @brief handles a buddy port message
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_buddy_port(peer_conn_info_t *info);

/**
 * @brief handles starting the direct connection process
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_start_direct_conn(peer_conn_info_t *info);


/**
 * @brief handles forging a SYN/ACK and thus completing direct connection
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_forge_syn_ack(peer_conn_info_t *info);

/**
 * @brief handles starting the bday process (when the peer is the random one)
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_start_bday(peer_conn_info_t *info);

/**
 * @brief handles ending the bday process (when the peer is the random one)
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_end_bday(peer_conn_info_t *info);

/**
 * @brief handles starting the reply bday process (when peer is the seq one)
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_reply_bday(peer_conn_info_t *info);

/**
 * @brief handles sending the flood of synacks to the buddy
 *
 * @param info pointer to the peer connection information
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_bday_synack_flood(peer_conn_info_t *info);


#endif /* __PEERFSM_PRIVATE_H__ */

