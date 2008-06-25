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
 * @file natblaster_helper.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief prototype for helper natblaster entrypoint
 */

#ifndef __NATBLASTER_HELPER_H__
#define __NATBlASTER_HELPER_H__

#include "def.h"

/**
 * @brief the single function to start a helper application
 *
 * Does not return on success!
 *
 * @param listen_port the port to act as a thrid party server on
 *
 * @return Never returns on success, errorcode on failure.
 */
int natblaster_server(port_t listen_port);

#endif /* __NATBLASTER_HELPER_H__ */

