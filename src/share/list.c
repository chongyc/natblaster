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
 * @file list.c
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief A basic list implementation (optimized for sequential accesses).
 */

#include "list.h"
#include "util.h"
#include "debug.h"

errorcode list_init(list_t *list) {

	if (list==NULL)
		return ERROR_NULL_ARG_1;

	list->head=NULL;
	list->last_get = list->head;
	list->last_get_num = 0;
	list->size = 0;

	return SUCCESS;
}

errorcode list_destroy(list_t *list, void (*func)(void*,void*), void *arg) {

	list_node_t *node;

	if (list==NULL)
		return ERROR_NULL_ARG_1;

	while (list->head != NULL) {
		/* make this node point to the old head */
		node = list->head;
		/* set the 2nd node to be the first now */
		list->head = node->next;
		/* use user defined function to clean up item */
		if (func!=NULL)
			func(node->item,arg);
		/* delete the old first node */
		safe_free(node);
	}

	return SUCCESS;
}

errorcode list_find(list_t *list, int (*func)(void*,void*), void *arg,
		    void**found_item) {

	list_node_t *node;

	if (list==NULL)
		return ERROR_NULL_ARG_1;
	if (func==NULL)
		return ERROR_NULL_ARG_2;
	if (found_item==NULL)
		return ERROR_NULL_ARG_4;

	node = list->head;

	while(node!=NULL) {
		switch (func(node->item,arg)) {
			case LIST_FATAL :
				return ERROR_FUNC_POINTER_FUNC_FAILED;
			case LIST_NOT_FOUND :
				node = node->next;
				break;
			case LIST_FOUND :
				*found_item = node->item;
				return SUCCESS;
			default :
				return ERROR_FUNC_POINTER_FUNC_INVALID;
		}
	}

	return ERROR_NOT_FOUND;
}

errorcode list_get(list_t *list, int index, void **item) {

	list_node_t *node, *start;
	int num;

	if (list==NULL)
		return ERROR_NULL_ARG_1;
	if (index<0)
		return ERROR_NEG_ARG_2;
	if (item==NULL)
		return ERROR_NULL_ARG_3;
	if (index>list->size)
		return ERROR_ARG_2;

	start = node = list->last_get;
	num = list->last_get_num;

	do {
		/* if this is the end of the list, loop back to begining */
		if (node==NULL) {
			node = list->head;
			num = 0; /*reset the counter */
			continue;
		}
		/* if this is the index item, return it */
		if (num==index) {
			*item = node->item;
			list->last_get = node;
			list->last_get_num = index;
			return SUCCESS;
		}
		node = node->next;
		num++;
	} while (start!=node); /* loop until the entire list was searched */

	return ERROR_NOT_FOUND;
}

errorcode list_add(list_t *list, void *item) {

	list_node_t *node;

	if (list==NULL)
		return ERROR_NULL_ARG_1;

	if ( (node=(list_node_t*)malloc(sizeof(list_node_t))) == NULL)
		return ERROR_MALLOC_FAILED;

	/* set this node's item */
	node->item = item;
	/* make this node point to the old head */
	node->next = list->head;
	/* make this node the head of the list */
	list->head = node;

	/*reset the last_get pointer */
	list->last_get = list->head;
	list->last_get_num = 0;
	/* increase list size */
	list->size += 1;

	return SUCCESS;
}

errorcode list_remove(list_t *list, int (*func)(void*,void*), void *arg){

	list_node_t *node;
	list_node_t *prev_node;

	if (list==NULL)
		return ERROR_NULL_ARG_1;
	if (func==NULL)
		return ERROR_NULL_ARG_2;

	prev_node = NULL;
	node = list->head;

	while(node!=NULL) {
		switch (func(node->item,arg)) {
			/* if there was a fatal error, handle it */
			case LIST_FATAL :
				return ERROR_FUNC_POINTER_FUNC_FAILED;
			/* if this just wasn't the item look at the next one */
			case LIST_NOT_FOUND :
				prev_node = node;
				node = node->next;
				break;
			/* if the item was found, remove it*/
			case LIST_FOUND :
				/* if this is the first item to remove, set the head */
				if (prev_node==NULL)
					list->head = node->next;
				/* otherwise, set the previous node's next value to the
				 * next node */
				else
					prev_node->next = node->next;
				/* now free the node */
				safe_free(node);
				/* reset the last get pointer */
				list->last_get = list->head;
				list->last_get_num = 0;
				/* decrease list size */
				list->size -= 1;
				return SUCCESS;
			default :
				return ERROR_FUNC_POINTER_FUNC_INVALID;
		}
	}

	return ERROR_NOT_FOUND;
}

int list_count(list_t *list) {

	if (list==NULL)
		return -1;
	return list->size;
}

