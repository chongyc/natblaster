/*****************************************************************************
 * Copyright 2005 Daniel Ferullo, Andrew Biggadike                           *
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
 * @file debug.h
 * @author Andrew Biggadike (biggadike@cmu.edu)
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief debug macros, stolen from andrew and tweaked
 */

#include "berkeleyapi.h"
#include <unistd.h>
#include <sys/time.h>

#ifndef __DEBUG_H__
#define __DEBUG_H__

/** @brief the ALL debug level:
 *         if debuging is on at any level then it is forced on at this level
 */
#define DBG_ALL				(0xffffffff)

/** @brief the VERBOSE debug level:
 *         basic verbose information
 */
#define DBG_VERBOSE			(0x00000001)

/** @brief the NETWORK debug level:
 *         information about basic networking
 */
#define DBG_NETWORK			(0x00000002)

/** @brief the LIST debug level:
 *         information relating the the shared peer list structure
 */
#define DBG_LIST			(0x00000004)

/** @brief the PROTOCOL debug level:
 *         information about the natblaster communnication protocol
 */

#define DBG_PROTOCOL		(0x00000008)

/** @brief the THREAD debug level:
 *         information about the list multi-threading
 */
#define DBG_THREAD			(0x00000010)

/** @brief the PORT_PRED debug level:
 *         information about port prediction
 */
#define DBG_PORT_PRED		(0x00000020)

/** @brief the BUDDY debug level:
 *         information about dealing with a peer's buddy
 */
#define DBG_BUDDY			(0x00000040)

/** @brief the DIR_CONN debug level:
 *         information relating directly to the direct connection attempt
 */
#define DBG_DIR_CONN		(0x00000080)

/** @brief the SNIFF debug level:
 *         information pertaining to the process of sniffing packets
 */
#define DBG_SNIFF			(0x00000100)

/** @brief the SPOOF debug level:
 *         information pertaining to spoofing packets
 */
#define DBG_SPOOF			(0x00000200)

/** @brief the BDAY debug level:
 *         information pertaining to the birthday paradox port determination
 */
#define DBG_BDAY			(0x00000400)

/** @brief all the debug levels that are turned on */
#define DBG_LEVEL (DBG_PROTOCOL | DBG_VERBOSE | DBG_NETWORK | DBG_PORT_PRED \
| DBG_SNIFF | DBG_SPOOF | DBG_BDAY)

/** @brief a macro to allow easy debugging info to be turned on and off */
#define DEBUG(level,fmt,args...) \
	if (level & DBG_LEVEL) fprintf(stderr,"%s:" fmt, __FUNCTION__, ##args)

/** @brief a macro to put ip_t in pretty-print format for debugging */
#define DBG_IP(x) ((char*)inet_ntoa(*(struct in_addr*)&x))

/** @brief a macro to put port_t in pretty-print format for debugging */
#define DBG_PORT(x) ((unsigned short)(ntohs(x)))

/** @brief a macro to put seq_num_t in pretty-print format for debugging */
#define DBG_SEQ_NUM(x) ((unsigned int)ntohl(x))

/** @brief a macro to print the current system time */
#define DBG_TIME(x)  ({ \
	struct timeval val; \
	if (gettimeofday(&val,NULL)<0)  \
		{fprintf(stderr,"TIME:%s:%s <?>\n",__FUNCTION__,x);} \
	else \
		{fprintf(stderr,"TIME:%s:%s <%u.%u>\n",__FUNCTION__, x, (unsigned int) val.tv_sec, (unsigned int)val.tv_usec);} \
})

#endif /* __DEBUG_H__ */
