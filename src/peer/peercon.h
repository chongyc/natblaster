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
 * @file peercon.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief functions to help the peer make a direct connection
 */

#ifndef __PEERCON_H__
#define __PEERCON_H__

#include "errorcodes.h"
#include "def.h"
#include "peerdef.h"

/**
 * @brief waits until the direct connection flag is set to FLAG_SUCCESS or
  *       FLAG_FAILED
  *
  * @param check_flag pointer to the flag to wait on
  *
  * @return SUCCESS, errorcode on failure
  */
errorcode wait_for_direct_conn(flag_t *check_flag);

/**
 * @brief finds a network devide (requires root privledge)
 *
 * this is basically a wrapper for pcap_lookupdev.  I assume pcap_lookupdev
 * uses malloc to allocate the memory for the returned pointer, but I am
 * not sure.  This needs to be checked.
 *
 * @param dev a pointer to a pointer.  When finished, will point to a string
 *        with the network device to use.
 *
 * @return SUCCESS, neg value on failure
 */
errorcode findDevice(char **dev);

/**
 * @brief function to do the SYN flooding to buddy
 *
 * @param tcp_skeleton a skeleton tcp_packet_info_t to base SYN's on.  The
 *         d_addr, d_port, s_addr, and seq_num fields will be inspected.
 *
 * @param device the device to forge SYNs on
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode flood_syns(tcp_packet_info_t tcp_skeleton, char *device);

/**
 * @brief a function to spawn a thread to look for a SYN/ACK with
 *
 * @param info pointer to the peer's information structure
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode start_find_synack(peer_conn_info_t *info);

/** @brief the entry point for the thread that looks for the SYN/ACK in a
 *  bday flood
 *
 * @param arg the single pthread arg (should be a pointer to the peer
 * information structure)
 *
 * @return SUCCESS, errorcode on failure
 */
void *run_find_synack(void* arg);

/**
 * @brief waits for the find syn ack to finish, within a timeout, and then
 *        joins on the thread.
 *
 * @param info a pointer to the peer_conn_info_t structure
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode wait_and_join_find_synack(peer_conn_info_t *info);

/**
 * @brief sends a bday flood of synacks to the peer
 *
 * @param info pointer to the peer_conn_info_t structure
 * @param seq_num the sequence number used in the SYNs in the other bday flood
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode synack_flood(peer_conn_info_t *info, seq_num_t seq_num);

#endif /* __PEERCON_H__ */

