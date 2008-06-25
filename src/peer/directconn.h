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
 * @file directconn.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief functions to help in attempt to make direct connection
 */

#ifndef __DIRECTCONN_H__
#define __DIRECTCONN_H__

#include "errorcodes.h"
#include "peerdef.h"

/** @brief structure to hold argument to started direct connection thread */
struct direct_conn_connect_arg {
	/** @brief the peer's info struct */
	peer_conn_info_t *info;
} __attribute__((packed));

/** @brief typedef for the direct_conn_connect_arg structure */
typedef struct direct_conn_connect_arg direct_conn_connect_arg_t;

/**
 * @brief starts the direct connection process by creating a detached thread
 *        to make the connection
 *
 * Allocates a direct_conn_arg_t structure that it expects the started thread
 * to free.
 *
 * @param info pointer to the peer_conn_info_t structure will all the info
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode start_direct_conn(peer_conn_info_t *info);

#endif /* __DIRECTCONN_H__ */

