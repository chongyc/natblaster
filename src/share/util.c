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
 * @file util.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains useful utility functions
 */

#include "util.h"
#include <stdlib.h>
#include <unistd.h>
#include "def.h"

/* inline function */
int inline DEC_UNTIL_ZERO(int x) {
	return (x>=1 ? x-1 : 0);
}

errorcode safe_free(void *memory) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(memory,ERROR_NULL_ARG_1);

	/* do function */
	free(memory);

	return SUCCESS;
}

errorcode wait_for_flag(flag_t *check_flag, flag_t stop_flags, int timeout) {

	/* declare local variables */

	/* error check arguments */
	CHECK_NOT_NULL(check_flag,ERROR_NULL_ARG_1);
	CHECK_NOT_NEG(timeout,ERROR_NEG_ARG_3);

	/* do function */
	do {
		if (!((*check_flag)&stop_flags)) {
			timeout = DEC_UNTIL_ZERO(timeout);
			sleep(1);
		}
		else {
			break;
		}
	} while (timeout>0);

	if (timeout==0)
		return ERROR_TIMEOUT;
	return SUCCESS;
}
