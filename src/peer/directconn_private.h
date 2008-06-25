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
 * @file directconn_private.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief prototypes for private direct connection functions
 */

#ifndef __DIRECTCONN_PRIVATE_H__
#define __DIRECTCONN_PRIVATE_H__

/**
 * @brief the entry point for the started thread that actually creates the
 *        direct connection
 *
 * @param arg the sole argument allowed in a pthread.  Will be cast to a
 *        direct_conn_connect_arg_t pointer and freed a thread exit
 *
 * @return SUCCESS, errorcode on failure
 */
void *run_direct_conn_connect(void *arg);

#endif /* __DIRECTCONN_PRIVATE_H__ */

