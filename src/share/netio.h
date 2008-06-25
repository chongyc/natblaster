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
 * @file netio.h
 * @author Daniel Ferullo (ferullo@.cmu.edu)
 *
 * @brief provides functions to do read/write from/to a socket
 */

#ifndef __NETIO_H__
#define __NETIO_H__

#include <stdio.h>

#include "errorcodes.h"
#include "comm.h"
#include "def.h"

/**
 * @brief reads up to buf_len bytes into buf
 *
 * This function checks that as it read messages the length field of the header
 * is correct
 *
 * THIS FUNCTION CONTAINS A COMPLICATED WHILE LOOP THAT IS WORTH A SECOND LOOK!
 *
 * @param sd the socket to read from
 * @param type the message type to read
 * @param buf the buffer to store the message in (the payload only)
 * @param buf_len the length of the buffer
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode readMsg(sock_t sd, comm_type_t type, void* buf, int buf_len);

/**
 * @brief creates a message from the type and payload and sents it
 *
 * @param sd the socket to send on
 * @param type the message type
 * @param payload a pointer to the payload (can be NULL if payload == 0)
 * @param payload_len the length of the payload in bytes
 *
 * @return SUCCESS, neg on failure
 */
errorcode sendMsg(sock_t sd, long type, void* payload, long payload_len);

#endif /* __NETIO_H__ */
