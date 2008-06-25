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
 * @file spoof.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief functions to spoof/forge network packets
 */

#include "spoof.h"
#include "spoof_private.h"
#include <libnet.h>
#include "debug.h"
#include "peerdef.h"

errorcode spoof(tcp_packet_info_t *tcp_hdr, char *device, void *payload,
					unsigned long payload_len, short ttl){

	/* declare local variables */
	int c;
	libnet_t *lib;
	libnet_ptag_t t;
	char errbuf[LIBNET_ERRBUF_SIZE];
	unsigned char tcp_flags;

	/* error check arguments */
	CHECK_NOT_NULL(tcp_hdr,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(device,ERROR_NULL_ARG_2);
	if ( (payload==NULL) && (payload_len!=0))
		return ERROR_ARG_3;

	/* do function */

	/* set the tcp_flags */
	tcp_flags = 0;
	tcp_flags |= ( (tcp_hdr->syn_flag==FLAG_SET) ? TH_SYN : 0);
	tcp_flags |= ( (tcp_hdr->ack_flag==FLAG_SET) ? TH_ACK : 0);

	/* Initialize the library.  Root priviledges are required. */
	lib = libnet_init(
		LIBNET_RAW4,                 /* injection type */
		device,                      /* network interface */
		errbuf);                     /* errbuf */

	if (lib == NULL) {
		DEBUG(DBG_SPOOF,"SPOOF:libnet_init() failed\n");
		return ERROR_1;
	}

	/* make sure to put ports, seq_num, ack_num and window in host byte
	 * order because libnet doesn't want them in network byte order. */
	t = libnet_build_tcp(
		PORT_2HBO(tcp_hdr->s_port),     /* source port */
		PORT_2HBO(tcp_hdr->d_port),     /* destination port */
		SEQ_NUM_2HBO(tcp_hdr->seq_num), /* sequence number */
		SEQ_NUM_2HBO(tcp_hdr->ack_num), /* acknowledgement number */
		tcp_flags,                      /* control flags */
		WINDOW_2HBO(tcp_hdr->window),   /* window size */
		0,                              /* checksum */
		0,                              /* urgent pointer */
		LIBNET_TCP_H + payload_len,     /* TCP packet size */
		payload,                        /* payload */
		payload_len,                    /* payload size */
		lib,                            /* libnet handle */
		0                               /* libnet id */
	    );

	if (t == -1) {
		DEBUG(DBG_SPOOF,"SPOOF:can't build TCP header\n");
		libnet_destroy(lib);
		return ERROR_2;
	}

	t = libnet_build_ipv4(
		LIBNET_IPV4_H+LIBNET_TCP_H+payload_len,  /* length */
		0,                                       /* TOS */
		242,                                     /* IP ID */
		0,                                       /* IP Frag */
		ttl,                                      /* TTL */
		IPPROTO_TCP,                             /* protocol */
		0,                                       /* checksum */
		tcp_hdr->s_addr,                         /* source IP */
		tcp_hdr->d_addr,                         /* destination IP */
		NULL,                                    /* payload */
		0,                                       /* payload size */
		lib,                                     /* libnet handle */
		0                                        /* libnet id */
	    );

	if (t == -1) {
		DEBUG(DBG_SPOOF,"SPOOF:can't build IP header\n");
		libnet_destroy(lib);
		return ERROR_3;
	}

	/* Write it to the wire. */
	c = libnet_write(lib);

	if (c == -1) {
		DEBUG(DBG_SPOOF,"SPOOF:write error\n");
		libnet_destroy(lib);
		return ERROR_4;
	}

	libnet_destroy(lib);
	return SUCCESS;
}

