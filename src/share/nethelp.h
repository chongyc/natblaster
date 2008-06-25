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
 * @file nethelp.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief provides prototypes for basic helpful network functions
 */

#ifndef __NETHELP_H__
#define __NETHELP_H__

#include "def.h"
#include "errorcodes.h"

/**
 * @brief resolves a hostname or IP to a 32bit IP number
 *
 * @param ip_or_name the hostname or IP as a string
 * @param ip the IP as a 32bit number
 */
errorcode resolveIP(char *ip_or_name, ip_t *ip);

/**
 * @brief binds a socket
 *
 * @param port_to_bind the desired port
 * @param sd a pointer to an int to fill in with the socket descriptor for the
 *        bound socket, if successful.  On error the value is undefined.
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode bindSocket(port_t port_to_bind, sock_t *sd);

/**
 * @brief creates a TCP connection
 *
 * @param ip the network byte order 32 bit ip address to connect to
 * @param port the port to connect to
 * @param sd a pointer to a sd to fill in with the connection sd (value
 *        undefined on error)  If the socket descriptor is greater than zero
 *        then the current socket descriptor will be used, otherwise a new one
 *        will be created.
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode tcp_connect(ip_t ip, port_t port, sock_t *sd);

#endif /* __NETHELP_H__ */

