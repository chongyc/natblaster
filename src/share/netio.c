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
 * @file netio.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief provides functions to read and write to sockets specific to
 *        the messages that should be sent for the communication protocol.
 */

#include "netio.h"
#include "netio_private.h"
#include "berkeleyapi.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "debug.h"

errorcode readMsg(sock_t sd, comm_type_t type, void* buf, int buf_len) {

	/* this function does not assume that the entire network message is
	 * received at once.  it allows for piecemeal reception of a message */

	/* declare local variables */
	int bytes_read, prev_bytes_read=0, ret, large_buf_len;
	char *large_buf, *buf_p;
	comm_type_t received_type;
	/* need to more all others up here too... */

	/* error check arguments */
	CHECK_NOT_NEG(sd,ERROR_NEG_ARG_1);
	CHECK_NOT_NEG(buf_len, ERROR_NEG_ARG_4);

	/* do function */

	/* declare the buffer to receive data into */
	if (buf != NULL)
		memset(buf,'\0',buf_len);

	/* create a buffer to store the entire message, then later
	 * keep only the payload */
	if ( (large_buf = (char*)malloc(buf_len+COMM_HEADER_LEN))==NULL)
		return ERROR_MALLOC_FAILED;
	memset(large_buf,0,buf_len+COMM_HEADER_LEN);
	large_buf_len = buf_len+COMM_HEADER_LEN;

	/* this pointer will move throught the buf in the loop below */
	buf_p = large_buf;

	/* read the next message loop incase only part of the message is
	 * received in one call to read() (allow reassembly of the fragments)
	 */
	while (1) {
		if ((bytes_read=read(sd, buf_p,
					large_buf_len-prev_bytes_read))<0) {
			safe_free(large_buf);
			return ERROR_1;
		}
		if (bytes_read==0) {
			safe_free(large_buf);
			return ERROR_TCP_READ;
		}
		if (bytes_read+prev_bytes_read > COMM_MAX_LEN) {
			DEBUG(DBG_ALL,"ALL:WARNING: POSSIBLE BUFFER OVERRUN\n");
			safe_free(large_buf);
			return ERROR_2;
		}
		if (bytes_read + prev_bytes_read < COMM_MIN_LEN ) {
			/* the entire header wasn't even received... */
			prev_bytes_read += bytes_read;
			buf_p = buf_p + bytes_read; /* pointer arithmatic */
			continue;
		}
		/* cast the second 4 bytes as a unsigned long and check if the
		 * length is ok see communication.h for explanation of why
		 * those 4 bytes should be checked */
		if ((ret=checkMsgLen(large_buf,bytes_read+prev_bytes_read))<0){
			/* entire message not received */
			if (ret!=NOT_OK) {
				/* there is a problem beyond that the message
				 * not entirely receieved */
				 safe_free(large_buf);
				return ERROR_3;
			}
			/* wait for the rest of the message */
			prev_bytes_read += bytes_read;
			buf_p = buf_p + bytes_read; /* pointer arithmatic */
			continue;
		}
		/* the entire message is receieved... */
		break;
	}

	/* now copy out the payload, if a buffer was passed in */
	if (buf != NULL)
		memcpy(buf,large_buf+COMM_HEADER_LEN,buf_len);

	/* make sure it is the correct type */
	memcpy(&received_type,large_buf,4);

	safe_free(large_buf);

	if (ntohl(received_type) != type)
		return ERROR_4;

	return SUCCESS;
}

errorcode checkMsgLen(char *msg, int msg_len) {

	unsigned long len;

	CHECK_NOT_NEG(msg,ERROR_NULL_ARG_1);
	CHECK_NOT_NEG(msg_len,ERROR_NEG_ARG_2);
	if (msg_len < COMM_MIN_LEN)
		return ERROR_1;

	/* now cast two header fields into long and unsigned long
	 * (per communication protocol)*/
	memcpy(&len, msg+COMM_TYPE_LEN, COMM_LENGTH_LEN);
	len = ntohl(len);

	if (msg_len > COMM_MAX_LEN)
		return ERROR_2; /* message is longer than permisable size */
	else if (len < (msg_len - COMM_HEADER_LEN))
		return ERROR_3; /* length field says the message is smaller
				 * than what was received */

	if (len > (msg_len - COMM_HEADER_LEN) )
		return NOT_OK; /* the length field says the message is longer
				* than what was received */

	return OK;
}

errorcode sendMsg(sock_t sd, long type, void* payload, long payload_len) {

	char *buf;
	long tmp;
	unsigned long utmp;
	
	CHECK_NOT_NEG(sd,ERROR_NEG_ARG_1);
	CHECK_NOT_NEG(payload_len,ERROR_NEG_ARG_4);
	if ((payload==NULL)&&(payload_len!=0))
		return ERROR_NULL_ARG_3;

	buf = (char*)malloc(COMM_HEADER_LEN+payload_len);
	if (buf==NULL)
		return ERROR_MALLOC_FAILED;

	/* make a message and send it */

	/* add type */
	tmp = htonl(type);
	memcpy(buf, &tmp, COMM_TYPE_LEN);

	/* add length */
	utmp = htonl(payload_len);
	memcpy(buf+COMM_TYPE_LEN, &utmp, COMM_LENGTH_LEN);

	/* add payload */
	if (payload_len != 0)
		memcpy(buf+COMM_TYPE_LEN+COMM_LENGTH_LEN,payload,payload_len);

	/* send the message */
	if( write(sd, buf, COMM_HEADER_LEN+payload_len) < 0 ) {
		DEBUG(DBG_NETWORK,"NETWORK:Failed to send data to socket.\n");
		safe_free(buf);
		return ERROR_TCP_SEND;
	}

	safe_free(buf);
	return SUCCESS;
}

