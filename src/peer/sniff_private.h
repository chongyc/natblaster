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
 * @file sniff_private.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief prototypes for private sniffing functions
 */

#ifndef __SNIFF_PRIVATE_H__
#define __SNIFF_PRIVATE_H__

#include <pcap.h>

/**
 * @brief initializes the pcap functions
 *
 * @param pcap_desc pointer pointer to fill in with the pcap descriptor
 * @param device the network device to capture on
 * @param timeout timeout in ms to use when capturing packets, -1 = no timeout
 * @param errbuf the pcap error buffer to use
 * @param errbuf_len the length of the error buffer
 *
 * @return SUCCESS, errorcode on failure
 */

errorcode init_packet_capture(pcap_t **pcap_desc, char *device, int timeout,
				char *errbuf, long errbuf_len );

/**
 * @brief finds a tcp packet, looping over all captured packets until the
 * correct one is found
 *
 * If the passed in flag takes on any value other than FLAG_UNSET then this
 * function will return early
 *
 * @param pcap_desc the pcap descriptor
 * @param tcp_skeleton the tcp skeleton to look for.  source and destination
 *        ip/port pairs as well as SYN/ACK flags will be matched on, and the
 *        skeleton will have the seq_num, ack_num fields filled in if there is
 *        a match
 * @param break_flag if the flag value is ever anything except FLAG_UNSET then
 *        the function returns early
 * @param payload a pointer pointer to fill in with a pointer to the payload
 *        if NULL the the value is not set
 * @param payload_len a pointer to a place to put the payload length. Can be
 *        NULL
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode find_tcp_packet(pcap_t *pcap_desc, tcp_packet_info_t *tcp_skeleton,
				flag_t *break_flag, unsigned char **payload,
				unsigned long *payload_len);
/**
 *
 * @param packet pointer the the start of the captured packet
 * @param tcp_skeleton pointer to a tcp_info_t will filled in source and
 *        destination ip/port pairs, along with SYN/ACK flags set/unset.  If
 *        the s/d_ip/port fields are set the the respective unknown values then
 *        they will be ignored when matching against a sniffed packet. The
 *        seq_num, ack_num, and window  fields will be filled in if a packet
 *        matches the other fields
 * @param payload a pointer pointer to fill in with a pointer to the payload.
 *        if NULL then the value is not set.
 * @param payload_len a pointer to a place to put the length the tcp header
 *        says the payload is.  THIS IS NOT NECESSARILY THE LENGTH OF THE
 *        PAYLOAD RETURNED, SINCE PCAP MIGHT NOT RETURN THE ENTIRE PAYLOAD.
 *        This parameter can be NULL.  The pointer points into a buffer
 *        allocated by libpcap, so the value may change in the buffer.
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode process_packet(unsigned char*packet,
		tcp_packet_info_t *tcp_skeleton, unsigned char **payload,
		unsigned long *payload_len);

#endif /* __SNIFF_PRIVATE_H__ */
