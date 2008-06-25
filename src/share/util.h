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
 * @file util.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains useful utility functions
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include "errorcodes.h"
#include "flag.h"

/**
 * @brief decrements a signed number by one, but not past zero
 *
 * @param x The number to decrement
 * @return The decremented value
 **/
int inline DEC_UNTIL_ZERO(int x);

/** @brief a macro to determine if two boolean values are the same (ie both
 *  true or both false */
#define BOOL_MATCH(x,y) ( (((x)!=0)&&((y)!=0))||(((x)==0)&&((y)==0)) ? 1 : 0)

/**
 * @brief a 'safe' free
 *
 * @param memory pointer to the memory to free
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode safe_free(void *memory);

/**
 * @brief waits for a flag to take one of many specified values
 *
 * Success if the check_flag takes on any one of the stop_flags before timeout
 *
 * @param check_flag pointer to the flag to watch
 * @param stop_flags all the flags to wait for or'ed together.
 * @param timeout the timeout time, in seconds
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode wait_for_flag(flag_t *check_flag, flag_t stop_flags, int timeout);

#endif /* __UTIL_H__ */

