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
 * @file list.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief Functions to implement a list
 */

#ifndef __LIST_H__
#define __LIST_H__

/* custom error codes */
#include "errorcodes.h"
#include <stdlib.h>

/** @brief a macro for the found return value in a match function */
#define LIST_FOUND	1
/** @brief a macro for the not found return value in a match function */
#define LIST_NOT_FOUND	0
/** @brief a macro for the fatal error return value in a match function */
#define LIST_FATAL	-1

/** @brief structure that is a single list node */
struct list_node {
	/** @brief the next list node */
	struct list_node *next;
	/** @brief the contents of this node */
	void *item;
} __attribute__ ((packed));

/** @brief typedef for the list_node structure */
typedef struct list_node list_node_t;

/** @brief structure to contain list information */
struct list {
	/** @brief the head of the list */
	list_node_t *head;
	/** @brief a pointer to the last node an item was returned from with
	 *         get().  This is used to speed up sequential gets()
	 */
	list_node_t *last_get;
	/** @brief the index of the last item gotten */
	int last_get_num;
	/** @brief the number of items in the list */
	int size;
} __attribute__ ((packed));

/** @brief typedef for the list structure */
typedef struct list list_t;

/**
 * @brief initializes the list
 *
 * Use of the list functions before this function is called is undefined
 *
 * @param list a pointer to the list the initalize
 * @return SUCCESS, errorcode on failure
 */
errorcode list_init(list_t *list);

/**
 * @brief deletes all entries from a list
 *
 * Use of the list functions after this function is called is undefined
 *
 * @param list a pointer to the list to destroy
 * @param func a function pointer to a user defined function that will clean up
 *        each item if cleanup is necessary.  If no cleanup is needed, NULL can
 *        be supplied by the user.  This function must be of the form
 *        void func(void*,void*) where the first argument is the current
 *        argument to cleanup, and the second is the user defined argument that
 *        is passed in as the third argument to this function.
 * @param arg the one argument allowed to the user defined cleanup function
 * @return SUCCESS, errorcode on failure
 */
errorcode list_destroy(list_t *list, void (*func)(void*,void*), void *arg);

/**
 * @brief adds an item to the list
 *
 * @param list a pointer to the list to add the item to
 * @param item the item to add to the list
 * @return SUCCESS, errorcode on failure
 */
errorcode list_add(list_t *list, void * item);

/**
 * @brief finds an item and returns a pointer to it through an argument.
 *        matches the item by a user specified function
 *
 * The first item that matches will be returned
 *
 * @param list a pointer to the list to find in
 * @param func a function pointer to the function to match with to find the
 *        item to remove.  Ths function must be of the form
 *        int func(void*,void*)) where it returns LIST_FOUND when the item is
 *        found, LIST_NOT_FOUND on not found and LIST_FATAL on "fatal error, 
 *        stop looking".  The first parameter to the function is the item in
 *        the list, and the second is an optional user defined argument (which
 *        is the third argument to this find function).
 * @param arg the optional func argument
 * @param found_item a pointer to a location to fill in the found item in.  If
 *        the function returns error, this value is undefined.
 * @return SUCCESS (item found) errorcode on failure/not find
 */
errorcode list_find(list_t *list, int (*func)(void*,void*), void* arg,
		    void**found_item);

/**
 * @brief gets the next item in the list after the index passed in
 *
 * an index is a integer which indexes into the list like it is an array.  So,
 * an index of 0 is the first element.  If the index is negative, or higher
 * than the number of elements, an error is returned.  Make no assumptions
 * about what item should be located at a given location, only assume that if
 * this function is called in succession without other list functions called
 * in between then the entire list will be returned with no duplicates.
 *
 * @param list a pointer to the list to get an item from
 * @param index to an item to get
 * @param item a pointer to a pointer that will be filled in with the returned
 *        item upon success
 * @return SUCCESS, errorcode on failure
 */
errorcode list_get(list_t *list, int index, void **item);

/**
 * @brief removes an item from the list, matching the item by the user
 *        specified function
 *
 * Only the first item that matches will be removed
 *
 * @param list a pointer to the list to remove from
 * @param func a function pointer to the function to match with to find the
 *        item to remove.  This function must be of the form int
 *        func(void*,void*) where it return LIST_FOUND when the item is found,
 *        LIST_NOT_FOUND on not found, and LIST_FATAL on "fatal error, stop 
 *        looking".  The first parameter to the function is the item from the
 *        list, and the second is an optional user defined argument (this is 
 *        the third argument to this remove function).
 * @param arg the optional func argument
 * @return SUCCESS (removal or item not found), errorcode on failure
 */
errorcode list_remove(list_t *list, int (*func)(void*,void*), void *arg);

/**
 * @brief gets the number of items in the list
 *
 * @param list a pointer to the list to get the item count from
 * @return the number of items, neg on failure
 */

int list_count(list_t *list);

#endif /* __LIST_H__ */

