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
 * @file sniff.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief functions to aid in sniffing packets off the wire
 */

#include "sniff.h"
#include "sniff_private.h"
#include "debug.h"
#include "util.h"
#include <string.h>
#include "nethelp.h"

errorcode capture_peer_to_buddy_syn(peer_conn_info_t *info) {

	/* declare local variables */
	char errbuf[PCAP_ERRBUF_SIZE]; /* pcap error string buffer */
	pcap_t *pcap_desc = NULL; /* pcap capture descriptor */

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */

	/* initalize the pcap functions */
	CHECK_FAILED(init_packet_capture(&pcap_desc,info->device,1000,
		errbuf,	PCAP_ERRBUF_SIZE),ERROR_1);

	/* fill in the syn info to look for */
	info->buddy_syn.d_addr    = info->buddy.ext_ip;
	info->buddy_syn.d_port    = info->buddy.ext_port;
	info->buddy_syn.s_addr    = info->peer.ip;
	info->buddy_syn.s_port    = info->peer.port;
	info->buddy_syn.syn_flag  = FLAG_SET;
	info->buddy_syn.ack_flag  = FLAG_UNSET;

	/* now loop, checking packets for the desired SYN */
	CHECK_FAILED(find_tcp_packet(pcap_desc, &info->buddy_syn,
		&info->direct_conn_status,NULL,NULL),ERROR_1);

	return SUCCESS;
}

errorcode capture_flooded_synack(peer_conn_info_t *info) {

	/* declare local variables */
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *pcap_desc = NULL;
	tcp_packet_info_t skeleton;
	unsigned char *payload = NULL;
	unsigned long payload_len = 0;

	/* error check arguments */
	CHECK_NOT_NULL(info,ERROR_NULL_ARG_1);

	/* do function */

	CHECK_FAILED(init_packet_capture(&pcap_desc,info->device,1000,
		errbuf, PCAP_ERRBUF_SIZE), ERROR_1);

	/* fill in the syn ack info */
	skeleton.d_addr   = info->peer.ip;
	skeleton.d_port   = PORT_UNKNOWN;
	skeleton.s_addr   = info->buddy.ext_ip;
	skeleton.s_port   = info->buddy.ext_port;
	skeleton.ack_flag = FLAG_SET;
	skeleton.syn_flag = FLAG_SET;

	/* now loop checking packets for the desired SYN/ACK */
	CHECK_FAILED(find_tcp_packet(pcap_desc, &skeleton,
		&info->bday.stop_synack_find,&payload,&payload_len), ERROR_1);

	DEBUG(DBG_BDAY,"DBAY:payload size is %u\n",(unsigned int)payload_len);

	if (payload == NULL)
		return ERROR_1;
	if (payload_len != sizeof(port_t))
		return ERROR_2;

	/* set the port value */
	memcpy(&info->bday.port,payload,sizeof(info->bday.port));
	info->bday.port_set = FLAG_SET;

	/* rebind the buddy socket to the new internal port */
	close(info->socks.buddy);
	CHECK_FAILED(bindSocket(skeleton.d_port,&info->socks.buddy),
		ERROR_CALLED_FUNCTION);

	return SUCCESS;
}

errorcode init_packet_capture(pcap_t **pcap_desc, char *device, int timeout,
				char *errbuf, long errbuf_len) {

	/* declare local variables */
	char *filter = "tcp";
	struct bpf_program fp;
	bpf_u_int32 maskp;
	bpf_u_int32 netp;

	/* error check arguments */
	CHECK_NOT_NULL(pcap_desc,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(device,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(errbuf,ERROR_NULL_ARG_4);
	CHECK_GREATER_THAN(errbuf_len,PCAP_ERRBUF_SIZE-1,ERROR_ARG_5);

	/* do function */
	if ( pcap_lookupnet(device,&netp,&maskp,errbuf) < 0 ) {
		DEBUG(DBG_SNIFF,
			"SNIFF:did you forget to run this program as root?\n");
		return ERROR_1;
	}

	if  ( (*pcap_desc=pcap_open_live(device,BUFSIZ,0,
					timeout,errbuf)) == NULL )
		return ERROR_2;

	/* make sure the user is on ethernet (that is the only supported
	 * data link layer right now */
	if ( pcap_datalink(*pcap_desc) != DLT_EN10MB)
		return ERROR_2;

	/* compile the filter */
	if ( pcap_compile(*pcap_desc,&fp,filter,0,netp) < 0 )
		return ERROR_4;

	/* set the filter */
	if ( pcap_setfilter(*pcap_desc,&fp) == -1 )
		return ERROR_5;

	/* set pcap_next to be non-blocking */
	if ( pcap_setnonblock(*pcap_desc,1,errbuf) < 0 )
		return ERROR_6;

	return SUCCESS;
}

errorcode find_tcp_packet(pcap_t *pcap_desc, tcp_packet_info_t *tcp_skeleton,
			flag_t *break_flag, unsigned char **payload,
			unsigned long *payload_len) {

	/* declare local variables */
	struct pcap_pkthdr hdr;
	const u_char *packet;

	/* error check arguments */
	CHECK_NOT_NULL(pcap_desc,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(tcp_skeleton,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(break_flag,ERROR_NULL_ARG_3);

	/* do function */

	/* loop as long as the brak_flag isn't set */
	while ( *break_flag == FLAG_UNSET) {
		/* capture the next packet, if timeout occurs, then result is
		 * NULL */
		if ( (packet=pcap_next(pcap_desc,&hdr)) != NULL) {
			/* proces the packet, and if it is "the one" then
			 * return success */
			if (!(FAILED(process_packet((unsigned char*) packet,
					tcp_skeleton,payload,payload_len)))) {
				return SUCCESS;
			}
		}
	}

	DEBUG(DBG_SNIFF,"SNIFF:could not find syn to buddy\n");

	return ERROR_2;
}

errorcode process_packet(unsigned char*packet,
		tcp_packet_info_t *tcp_skeleton, unsigned char **payload,
		unsigned long *payload_len) {

	/* declare local variables */
	struct ether_header *ether  = NULL; /* from net/ethernet.h */
	struct iphdr        *ip     = NULL; /* from linux/ip.h */
	struct tcphdr       *tcp    = NULL; /* from netinet/tcp.h */
	int ip_hdr_len              = 0;

	/* error check arguments */
	CHECK_NOT_NULL(packet,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(tcp_skeleton,ERROR_NULL_ARG_2);

	/* do function */

	/* align different structure pointers to correct location */
	ether = (struct ether_header*) (packet);

	ip    = (struct iphdr*) ((char*)ether + sizeof(struct ether_header));
	/* determine how long the ip hdr is (multiply by 4 because the ihl
	 * field is in 32 bit words, but I want it in bytes */
	ip_hdr_len = 4 * (unsigned int) ip->ihl;

	tcp   = (struct tcphdr*) ( (char*)ip + ip_hdr_len);

	/*
	DEBUG(DBG_SNIFF,"RECEIVED PACKET:\n");
	DEBUG(DBG_SNIFF,"SNIFF:ip.s_addr:  %s\n",DBG_IP(ip->saddr));
	DEBUG(DBG_SNIFF,"SNIFF:ip.d_addr:  %s\n",DBG_IP(ip->daddr));
	DEBUG(DBG_SNIFF,"SNIFF:tcp.s_port: %d\n",DBG_PORT(tcp->th_sport));
	DEBUG(DBG_SNIFF,"SNIFF:tcp.d_port: %d\n",DBG_PORT(tcp->th_dport));
	DEBUG(DBG_SNIFF,"SNIFF:tcp ACK bit is %sset\n",
		(tcp->th_flags&TH_ACK) ? "" : "not ");
	DEBUG(DBG_SNIFF,"SNIFF:tcp SYN bit is %sset\n",
		(tcp->th_flags&TH_SYN) ? "" : "not ");
	DEBUG(DBG_SNIFF,"LOOKING FOR:\n");
	DEBUG(DBG_SNIFF,"SNIFF:skeleton.s_addr:  %s\n",
		DBG_IP(tcp_skeleton->s_addr));
	DEBUG(DBG_SNIFF,"SNIFF:skeleton.d_addr:  %s\n",
		DBG_IP(tcp_skeleton->d_addr));
	DEBUG(DBG_SNIFF,"SNIFF:skeleton.s_port: %d\n",
		DBG_PORT(tcp_skeleton->s_port));
	DEBUG(DBG_SNIFF,"SNIFF:skeleton.d_port: %d\n",
		DBG_PORT(tcp_skeleton->d_port));
	DEBUG(DBG_SNIFF,"SNIFF:skeleton ACK bit is %sset\n",
		(tcp_skeleton->ack_flag==FLAG_SET) ? "" : "not ");
	DEBUG(DBG_SNIFF,"SNIFF:skeleton SYN bit is %sset\n",
		(tcp_skeleton->syn_flag==FLAG_SET) ? "" : "not ");
	DEBUG(DBG_SNIFF,"\n");
	*/

	/* if statement to check if this is the desired packet */
	if (	( (ip->saddr     !=  tcp_skeleton->s_addr)   &&
		  (tcp_skeleton->s_addr != IP_UNKNOWN)     ) ||
		( (ip->daddr     !=  tcp_skeleton->d_addr)   &&
		  (tcp_skeleton->d_addr != IP_UNKNOWN)    ) ||
		( (tcp->th_sport !=  tcp_skeleton->s_port)   &&
		  (tcp_skeleton->s_port != PORT_UNKNOWN)   ) ||
		( (tcp->th_dport !=  tcp_skeleton->d_port)   &&
		  (tcp_skeleton->d_port != PORT_UNKNOWN)   ) ||
		!BOOL_MATCH(tcp->th_flags&TH_ACK,
			(tcp_skeleton->ack_flag==FLAG_SET)) ||
		!BOOL_MATCH(tcp->th_flags&TH_SYN,
			(tcp_skeleton->syn_flag==FLAG_SET))
	   )  {
		/* if here, at least one of the above didn't match */
		return NOT_OK;
	}

	DEBUG(DBG_SNIFF,"SNIFF:matched packet\n");

	DEBUG(DBG_SNIFF,"SNIFF:ip.s_addr:  %s\n",DBG_IP(ip->saddr));
	DEBUG(DBG_SNIFF,"SNIFF:ip.d_addr:  %s\n",DBG_IP(ip->daddr));
	DEBUG(DBG_SNIFF,"SNIFF:tcp.s_port: %d\n",DBG_PORT(tcp->th_sport));
	DEBUG(DBG_SNIFF,"SNIFF:tcp.d_port: %d\n",DBG_PORT(tcp->th_dport));

	DEBUG(DBG_SNIFF,"SNIFF:tcp ACK bit is %sset\n",
		(tcp->th_flags&TH_ACK) ? "" : "not ");
	DEBUG(DBG_SNIFF,"SNIFF:tcp SYN bit is %sset\n",
		(tcp->th_flags&TH_SYN) ? "" : "not ");


	/* set the sequence number and ack number */
	tcp_skeleton->seq_num = tcp->th_seq;
	tcp_skeleton->ack_num = tcp->th_ack;
	tcp_skeleton->window  = tcp->th_win;

	/* set the other information */
	tcp_skeleton->s_addr = ip->saddr;
	tcp_skeleton->d_addr = ip->daddr;
	tcp_skeleton->s_port = tcp->th_sport;
	tcp_skeleton->d_port = tcp->th_dport;

	/* set the payload pointer */
	if (payload != NULL)
		*payload = (unsigned char*) tcp + sizeof(struct tcphdr);
	if (payload_len != NULL)
		*payload_len = ntohs(ip->tot_len) - sizeof(struct tcphdr) -
							sizeof(struct iphdr);


	return SUCCESS;
}

