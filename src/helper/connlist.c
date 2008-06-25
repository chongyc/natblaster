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
 * @file connlist.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief Functions to help search and maintain the connection list.  These
 * functions are thread-safe.
 */

#include "connlist.h"
#include <pthread.h>
#include "debug.h"

#include <unistd.h>

errorcode connlist_init(connlist_t *list) {

	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);

	CHECK_FAILED(list_init(&list->list),ERROR_INIT);

	/**
	 * man page says return value is always 0, but I don't trust it, since the
	 * man page for pthreads also says some functions/macros exist that
	 * in fact do not.
	 **/
	if (pthread_mutex_init(&(list->mutex),NULL)<0)
		return ERROR_2;

	return SUCCESS;
}

errorcode connlist_add(connlist_t *list, connlist_item_t *item) {

	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(item,ERROR_NULL_ARG_2);

	/* lock the mutex */
	DEBUG(DBG_THREAD,"THREAD:LOCKING MUTEX\n");
	if (pthread_mutex_lock(&(list->mutex))<0)
		return ERROR_MUTEX_LOCK;
	DEBUG(DBG_THREAD,"THREAD:LOCKED MUTEX\n");
	/* the thread adding the item is a watcher by default */
	item->watchers = 1;

	/* add the item to the list */
	if (FAILED(list_add(&list->list,item))) {
		if(pthread_mutex_unlock(&(list->mutex))<0)
			return ERROR_MUTEX_UNLOCK_1;
		DEBUG(DBG_THREAD,"THREAD:UNLOCKED MUTEX\n");
		return ERROR_LIST_ADD;
	}

	/* unlock the mutex */
	DEBUG(DBG_THREAD,"THREAD:UNLOCKING MUTEX\n");
	if (pthread_mutex_unlock(&(list->mutex))<0)
		return ERROR_MUTEX_UNLOCK_2;
	DEBUG(DBG_THREAD,"THREAD:UNLOCKED MUTEX\n");

	return SUCCESS;
}

errorcode connlist_find(connlist_t *list, int (*func)(void*,void*), void *arg,
			connlist_item_t **found_item){

	/* declare local variables */
	connlist_item_t *cast_item;

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(func,ERROR_NULL_ARG_2);
	CHECK_NOT_NULL(found_item,ERROR_NULL_ARG_4);

	/* do function */

	/* lock the mutex */
	DEBUG(DBG_THREAD,"THREAD:LOCKING MUTEX\n");
	if (pthread_mutex_lock(&(list->mutex))<0)
		return ERROR_MUTEX_LOCK;
	DEBUG(DBG_THREAD,"THREAD:LOCKED MUTEX %p\n",&(list->mutex));

	/* find the item in the list */
	if (FAILED(list_find(&list->list,func,arg,(void**)&cast_item))) {
		if(pthread_mutex_unlock(&(list->mutex))<0)
			return ERROR_MUTEX_UNLOCK_1;
		DEBUG(DBG_THREAD,"THREAD:UNLOCKED MUTEX\n");
		return ERROR_LIST_FIND;
	}

	*found_item = cast_item;

	/* the thread finding this item is a new watcher */
	cast_item->watchers += 1;

	/* unlock the mutex */
	DEBUG(DBG_THREAD,"THREAD:UNLOCKING MUTEX\n");
	if (pthread_mutex_unlock(&(list->mutex))<0)
		return ERROR_MUTEX_UNLOCK_2;
	DEBUG(DBG_THREAD,"THREAD:UNLOCKED MUTEX\n");

	DEBUG(DBG_PORT_PRED,"PORT_PRED:found item\n");
	return SUCCESS;
}

int connlist_find_pred_port(void *this_item, void *find_item) {

	/* declare variables */
	connlist_item_t *cast_item;
	observed_data_t *cast_data;

	/* error check arguments */
	CHECK_NOT_NULL(find_item,LIST_FATAL);
	CHECK_NOT_NULL(this_item,LIST_FATAL);

	/* do function */
	cast_item = (connlist_item_t*) this_item;
	cast_data = (observed_data_t*) find_item;

	DEBUG(DBG_PORT_PRED,"PORT_PRED:item %s:%u\n",
		DBG_IP(cast_item->obs_data.ip),
		DBG_PORT(cast_item->obs_data.port));
	DEBUG(DBG_PORT_PRED,"PORT_PRED:data %s:%u\n",
		DBG_IP(cast_data->ip),DBG_PORT(cast_data->port));

	/* check if the observed data is what is expected */
	if ( (cast_item->obs_data.ip == cast_data->ip) &&
		(cast_item->obs_data.port == cast_data->port) )   {
		DEBUG(DBG_PORT_PRED,"PORT_PRED:matched item\n");
		return LIST_FOUND;
	}

	return LIST_NOT_FOUND;
}

int connlist_find_buddy(void *this_item, void *find_item) {

	/* declare local variables */
	connlist_item_t *cast_item;
	buddy_info_t    *cast_buddy;

	/* error check arguments */
	CHECK_NOT_NULL(find_item,LIST_FATAL);
	CHECK_NOT_NULL(this_item,LIST_FATAL);

	/* do function */
	cast_item   = (connlist_item_t*) this_item;
	cast_buddy  = (buddy_info_t*)    find_item;

	/*
	DEBUG(DBG_LIST,"LIST:match item\n");
	DEBUG(DBG_LIST," cast_buddy:\n");
	DEBUG(DBG_LIST,"  ext_ip...%s\n",DBG_IP(cast_buddy->ext_ip));
	DEBUG(DBG_LIST,"  int_ip...%s\n",DBG_IP(cast_buddy->int_ip));
	DEBUG(DBG_LIST,"  int_port.%u\n",DBG_PORT(cast_buddy->int_port));
	DEBUG(DBG_LIST," cast_item:\n");
	DEBUG(DBG_LIST,"  ext_ip...%s\n",DBG_IP(cast_item->obs_data.ip));
	DEBUG(DBG_LIST,"  int_ip...%s\n",DBG_IP(cast_item->info.peer.ip));
	DEBUG(DBG_LIST,"  int_port.%u\n",DBG_PORT(cast_item->info.peer.port));
	DEBUG(DBG_LIST,"\n");
	*/

	if ( (cast_buddy->int_port==cast_item->info.peer.port) &&
	     (cast_buddy->int_ip==cast_item->info.peer.ip)     &&
	     (cast_buddy->ext_ip==cast_item->obs_data.ip) )     {
		return LIST_FOUND;
	}

	return LIST_NOT_FOUND;
}

errorcode connlist_forget(connlist_t *list,
			  int (*func)(void*,void*), connlist_item_t *item) {

	/* declare variables */

	/* error check arguments */
	CHECK_NOT_NULL(list,ERROR_NULL_ARG_1);
	CHECK_NOT_NULL(item,ERROR_NULL_ARG_2);

	/* lock the mutex */
	DEBUG(DBG_THREAD,"THREAD:LOCKING MUTEX\n");
	if (pthread_mutex_lock(&(list->mutex))<0)
		return ERROR_MUTEX_LOCK;
	DEBUG(DBG_THREAD,"THREAD:LOCKED MUTEX\n");

	/* decrement the number of watchers*/
	item->watchers -= 1;

	if (item->watchers == 0) {
		/* remove the item to the list */
		if (FAILED(list_remove(&list->list,func,item))) {
			if(pthread_mutex_unlock(&(list->mutex))<0)
				return ERROR_MUTEX_UNLOCK_1;
			DEBUG(DBG_THREAD,"THREAD:UNLOCKED MUTEX\n");
			return ERROR_LIST_ADD;
		}
	}

	/* unlock the mutex */
	DEBUG(DBG_THREAD,"THREAD:UNLOCKING MUTEX\n");
	if (pthread_mutex_unlock(&(list->mutex))<0)
		return ERROR_MUTEX_UNLOCK_2;
	DEBUG(DBG_THREAD,"THREAD:UNLOCKED MUTEX\n");
	return SUCCESS;
}

int connlist_item_match(void *this_item, void *find_item) {

	/* declare variables */

	/* error check arguments */
	CHECK_NOT_NULL(this_item,LIST_FATAL);
	CHECK_NOT_NULL(find_item,LIST_FATAL);

	/* just match pointers, that is all that is necessary */
	if (this_item == find_item)
		return LIST_FOUND;

	return LIST_NOT_FOUND;
}

int connlist_count(connlist_t *list) {

	/* declare variables */
	int count;

	/* error check arguments */
	CHECK_NOT_NULL(list,-2);

	/* do function */
	/* lock the mutex */
	DEBUG(DBG_THREAD,"THREAD:LOCKING MUTEX\n");
	if (pthread_mutex_lock(&(list->mutex))<0)
		return -3;
	DEBUG(DBG_THREAD,"THREAD:LOCKED MUTEX\n");

	count = list_count(&list->list);

	/* unlock the mutex */
	DEBUG(DBG_THREAD,"THREAD:UNLOCKING MUTEX\n");
	if (pthread_mutex_unlock(&(list->mutex))<0)
		return -4;
	DEBUG(DBG_THREAD,"THREAD:UNLOCKED MUTEX\n");

	return count;
}
