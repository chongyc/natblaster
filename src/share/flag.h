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
 * @file flag.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief the flag type values.  These are simple values that didn't seem to
 * fit into any other header.
 */

#ifndef __FLAG_H__
#define __FLAG_H__

/** @brief typedef for flag_t type */
typedef char flag_t;

/** @brief macro for the UNSET value */
#define FLAG_UNSET	0x00000000

/** @brief macro for the SET value */
#define FLAG_SET	0x00000001

/** @brief macro for the SUCCESS value */
#define FLAG_SUCCESS	0x00000002

/** @brief macro for the FAILED value */
#define FLAG_FAILED	0x00000004

#endif /* __FLAG_H__ */

