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
 * @file netio_private.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief private functions to help with communication protocol IO
 */

#ifndef __NETIO_PRIVATE_H__
#define __NETIO_PRIVATE_H__

#include "errorcodes.h"

/**
 * @brief checks that an message is as long as the length field says it is
 *
 * @param msg a pointer to the message
 * @param msg_len the length of msg (including header)
 *
 * @return OK if the message is ok, NOT_OK if it is to small, other neg value
 *         otherwise
 */
errorcode checkMsgLen(char *msg, int msg_len);

#endif /* __NETIO_PRIVATE_H__ */

