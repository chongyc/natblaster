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
 * @file connlist.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains prototypes for connection list functions
 *
 * These functions are wrappers for the list.h functions.  They provide
 * abstraction and thread-safety
 */

#ifndef __CONNLIST_H__
#define __CONNLIST_H__

#include "helperdef.h"
#include "list.h"

/** @brief structure for a single connection node */
struct connlist_item {
	/** @brief the helper connection strucure */
	helper_conn_info_t info;
	/** @brief the data observed in the packets */
	observed_data_t obs_data;
	/** @brief the number of threads accessing this item */
	long watchers;
} __attribute__((packed));

/** @brief typedef for the connlist_item structure */
typedef struct connlist_item connlist_item_t;

/** @brief structure for the connlist_t type */
struct connlist {
	/** @brief the pre-existing list type */
	list_t list;
	/** @brief the mutex to provide thread safety */
	pthread_mutex_t mutex;
} __attribute__((packed));

/** @brief typedef for the connlist structure */
typedef struct connlist connlist_t;

/**
 * @brief the function to initialize the list
 *
 * @param list pointer to an allocaed list container
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode connlist_init(connlist_t *list);

/**
 * @brief the function to add an item to the list
 *
 * This function is thread safe.  forget must be called once for this function
 * to remove the item from the list once it has been added.
 *
 * @param list pointer to the connlist_t list
 * @param item pointer to the item to add
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode connlist_add(connlist_t *list, connlist_item_t *item);

/**
 * @brief finds an item in the list
 *
 * This function is thread safe
 *
 * @param list pointer to the connlist_t list
 * @param func function pointer to use in find matching.  Function must meet
 *        requirements specified for function list_find in list.h
 * @param arg the one optional argument passed into the func function
 * @param found_item pointer to a void* to fill in with the found item.  On
 *        error this value is undefined.
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode connlist_find(connlist_t *list, int (*func)(void*,void*), void *arg,
			connlist_item_t **found_item);

/**
 * @brief the function to find a predictied port match (used as a match
 * function for the list implementation).
 *
 * @param this_item the item from the list to check (a connlist_item_t pointer)
 * @param find_item the values to match on (an observed_data_t pointer)
 *
 * @return LIST_FATAL, LIST_FOUND, LIST_NOT_FOUND, as per requirements
 */
int connlist_find_pred_port(void *this_item, void *find_item);


/**
 * @brief the function to find a buddy match (used as a match function for
 * the list implementation).
 *
 * @param this_item the item from the list to check (a connlist_item_t pointer)
 * @param find_item the values to match on (a buddy_info_t pointer)
 *
 * @return LIST_FATAL, LIST_FOUND, LIST_NOT_FOUND, as per requirements
 */
int connlist_find_buddy(void *this_item, void *find_item);

/**
 * @brief the function to remove an item from the list
 *
 * This function is thread safe.
 *
 * This function will only remove the item if there are no watchers left. It
 * must be called exactly once for each time find is called, plus once more
 * for the add call.
 *
 * @param list a pointer to the list to remove from
 * @param func the function to use in matching for the forget
 * @param item a pointer to the item to remove
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode connlist_forget(connlist_t *list,
			  int (*func)(void*,void*), connlist_item_t *item);

/**
 * @brief the matching function for an item in the list
 *
 * @param this_item a pointer to the current item in the list (will be cast to
 *        a connlist_t pointer)
 * @param find_item a pointer to the item to find (will be cast to a
 *        connlist_item_t pointer)
 *
 * @return LIST_FATAL, LIST_FOUND, LIST_NOT_FOUND as list.h requires
 */
int connlist_item_match(void *this_item, void *find_item);

/**
 * @brief gets the number of items in the list
 *
 * This function is thread safe
 *
 * @param list pointer to the connlist
 *
 * @return the number of elements, negative on error
 */
int connlist_count(connlist_t *list);

#endif /* __CONNLIST_H__ */

