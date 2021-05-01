/**
 * @file		list.c
 * @brief		This file contains a generic implementation of a double linked list
 *
 * @date		Dec 7, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 * @author	Simone Ruffini[simone.ruffini@tutanota.com]
 */

#include "llist.h"

#include <stdlib.h>
#include <string.h>

typedef struct node {
	struct node *prev;
	llist_node data;
	struct node *next;
} _node;

typedef struct {
	size_t count;
	comparator comp_func;
	equal equal_func;
	_node *head;
	_node *tail;
} _list;

LLIST_RETURN _list_allocate(_node **new_node, llist_node new_data) {
	if (new_data == NULL) {
		return LLIST_NULL_ARGUMENT;
	}
	*new_node = malloc(sizeof(_node));

	if (*new_node == NULL) {
		return LLIST_MALLOC_ERROR;
	}

	(*new_node)->prev = NULL;
	(*new_node)->next = NULL;
	(*new_node)->data = new_data;

	return LLIST_SUCCESS;
}

LLIST_RETURN _list_delete_node(_list *list, _node *node) {
	if ((list == NULL) || (node == NULL))
		return LLIST_NULL_ARGUMENT;

	if (node->next != NULL) {
		// If node is not tail (because it has a successor)
		(node->next)->prev = node->prev;
	}

	if (node->prev != NULL) {
		// If node is not head (bracuase it has a predecessor)
		(node->prev)->next = node->next;
	}

	//if we are deleting head
	if (node == list->head) {
		//just move the hed pointer
		list->head = node->next;
		//this works even if the list is of one element (head == tail) because node->next == NULL
		//and by removing the element we are deleting the list: head == tail == NULL
	}
	//if we are deleting tail
	if (node == list->tail) {
		// sets tail to penultimate element in list
		list->tail = node->prev;
	}

	free(node->data);
	free(node);
	list->count--;

	return LLIST_SUCCESS;
}

LLIST_RETURN _list_insert_prev(_list *list, _node *pos_node, llist_node new_data) {
	if ((list == NULL) || (new_data == NULL)) {
		return LLIST_NULL_ARGUMENT;
	}

	_node *node = NULL;
	uint8_t ret = _list_allocate(&node, new_data);
	if (ret != LLIST_SUCCESS) {
		return ret;
	}

	// Update new node's prev
	node->prev = (pos_node)->prev;
	// Update pos_node's prev to link to node
	(pos_node)->prev = node;
	// Update new node's next to link to pos_node
	node->next = pos_node;

	if (pos_node == list->head) {
		// If we're on head
		list->head = node;
	} else {
		//since we are not inserting in place of head we will have a prev node
		//that can be deferenced and updated to point to the new node
		node->prev->next = node;
	}

	list->count++;
	return LLIST_SUCCESS;
}

LLIST_RETURN _list_get(_list *list, size_t index, _node **node) {
	if (list == NULL || node == NULL) {
		return LLIST_NULL_ARGUMENT;
	}

	if (index <= list->count / 2) {
		_node *iterator = list->head;
		for (size_t i = 0; i < list->count; i++) {
			if (i == index) {
				*node = iterator;
				return LLIST_SUCCESS;
			}

			iterator = iterator->next;
		}
	} else {
		_node *iterator = list->tail;

		for (size_t i = list->count - 1; i >= 0 && i < list->count; i--) {
			if (i == index) {
				*node = iterator;
				return LLIST_SUCCESS;
			}

			iterator = iterator->prev;
		}
	}

	return LLIST_NODE_NOT_FOUND;
}

llist llist_init(comparator compare_func, equal equal_func) {
	_list *new_list;

	new_list = malloc(sizeof(_list));

	if (new_list == NULL)
		return NULL;

	new_list->equal_func = equal_func;
	new_list->comp_func = compare_func;

	// Reset the list
	new_list->count = 0;
	new_list->head = NULL;
	new_list->tail = NULL;
	return new_list;
}

LLIST_RETURN llist_destroy(llist list) {
	if (list == NULL)
		return LLIST_NULL_ARGUMENT;

	_node *iterator;
	_node *next;

	// Delete the data contained in the nodes
	iterator = ((_list *)list)->head;

	while (iterator != NULL) {
		free(iterator->data);

		next = iterator->next;
		free(iterator);	 // Delete's the container
		iterator = next;
	}

	//release the list
	free(list);

	return LLIST_SUCCESS;
}

LLIST_RETURN llist_push(llist list, llist_node new_node) {
	//if the list is empty head==tail
	if (((_list *)list)->count == 0) {
		LLIST_RETURN ret = _list_allocate(&((_list *)list)->head, new_node);
		if (ret != LLIST_SUCCESS) {
			return ret;
		}

		((_list *)list)->tail = ((_list *)list)->head;
		((_list *)list)->count++;

		return LLIST_SUCCESS;
	}

	return _list_insert_prev((_list *)list, ((_list *)list)->head, new_node);
}

LLIST_RETURN llist_push_back(llist list, llist_node new_node) {
	if ((list == NULL) || (new_node == NULL)) {
		return LLIST_NULL_ARGUMENT;
	}

	//if the list is empty
	if (((_list *)list)->count == 0) {
		return llist_push(list, new_node);
	}

	_node *node = NULL;
	LLIST_RETURN ret = _list_allocate(&node, new_node);
	if (ret != LLIST_SUCCESS) {
		return ret;
	}

	node->next = NULL;	//(((_list *)list)->tail)->next;
	node->prev = ((_list *)list)->tail;
	(((_list *)list)->tail)->next = node;
	((_list *)list)->tail = node;

	((_list *)list)->count++;

	return LLIST_SUCCESS;
}

LLIST_RETURN llist_insert_by_node(llist list, llist_node node, llist_node new_node) {
	if (list == NULL || new_node == NULL || node == NULL) {
		return LLIST_NULL_ARGUMENT;
	}

	_node *iterator = ((_list *)list)->head;

	for (size_t count = 0; count < ((_list *)list)->count; count++) {
		if (((_list *)list)->equal_func(node, iterator->data)) {
			return _list_insert_prev((_list *)list, iterator, new_node);
		}
		iterator = iterator->next;
	}

	//if the node was not found
	return LLIST_NODE_NOT_FOUND;
}

LLIST_RETURN llist_insert_by_index(llist list, size_t index, llist_node new_node) {
	if (list == NULL || new_node == NULL) {
		return LLIST_NULL_ARGUMENT;
	}

	if (index > (((_list *)list)->count - 1)) {
		return LLIST_NODE_NOT_FOUND;
	}

	_node *node;
	LLIST_RETURN ret = _list_get((_list *)list, index, &node);

	if (ret != LLIST_SUCCESS) {
		return ret;
	}

	return _list_insert_prev((_list *)list, node, new_node);
}

LLIST_RETURN llist_insert_priority(llist list, llist_node new_node) {
	if ((list == NULL) || (new_node == NULL)) {
		return LLIST_NULL_ARGUMENT;
	}
	if (((_list *)list)->comp_func == NULL) {
		return LLIST_COMPARATOR_MISSING;
	}
	_node *iterator = ((_list *)list)->head;

	for (size_t count = 0; count < ((_list *)list)->count; count++) {
		//while new_node has less priority then iterator: cycle

		//if new_node has more or same priority as iterator data: replace iterator node
		// with a node containing new_node(remember that new_node is the data of the list)
		if (((_list *)list)->comp_func(new_node, iterator->data) >= 0) {
			return _list_insert_prev((_list *)list, iterator, new_node);
		}

		iterator = iterator->next;
	}

	// if new_data has less prioirity then all nodes data than place it at the end
	// make the node of new_node(type= llist_node) as the new tail of the list

	//this is needed if the llist is empty too
	return llist_push_back(list, new_node);
}

LLIST_RETURN llist_find(llist list, llist_node node, size_t *index) {
	if (list == NULL || node == NULL || index == NULL) {
		return LLIST_NULL_ARGUMENT;
	}

	if (((_list *)list)->equal_func == NULL) {
		return LLIST_EQUAL_MISSING;
	}

	_node *iterator = ((_list *)list)->head;

	for (size_t count = 0; count < ((_list *)list)->count; count++) {
		if (((_list *)list)->equal_func(node, iterator->data) == 1) {
			*index = count;
			node = iterator->data;
			return LLIST_SUCCESS;
		}

		iterator = iterator->next;
	}

	return LLIST_NODE_NOT_FOUND;
}

LLIST_RETURN llist_get(llist list, size_t index, llist_node *node) {
	_node *temp;

	LLIST_RETURN ret = _list_get((_list *)list, index, &temp);
	if (ret != LLIST_SUCCESS) {
		return ret;
	}

	*node = temp->data;
	return LLIST_SUCCESS;
}

llist_node llist_get_head(llist list) {
	if (list == NULL)
		return NULL;
	if (llist_size(list) == 0)
		return NULL;
	return ((_list *)list)->head->data;
}

llist_node llist_get_tail(llist list) {
	if (list == NULL)
		return NULL;
	if (llist_size(list) == 0)
		return NULL;
	return ((_list *)list)->tail->data;
}

size_t llist_size(llist list) {
	if (list == NULL)
		return 0;

	return ((_list *)list)->count;
}

LLIST_RETURN llist_export(llist list, void *array, size_t node_size) {
	if (list == NULL || array == NULL) {
		return LLIST_NULL_ARGUMENT;
	}

	_node *iterator = ((_list *)list)->head;
	for (size_t i = 0; i < ((_list *)list)->count; i++) {
		memcpy(array + (i * node_size), iterator->data, node_size);
		iterator = iterator->next;
	}

	return LLIST_SUCCESS;
}

LLIST_RETURN llist_remove_by_node(llist list, llist_node node) {
	if (list == NULL || node == NULL) {
		return LLIST_NULL_ARGUMENT;
	}

	_node *iterator = ((_list *)list)->head;

	for (size_t count = 0; count < ((_list *)list)->count; count++) {
		if (((_list *)list)->equal_func(node, iterator->data)) {
			return _list_delete_node((_list *)list, iterator);
		}
		iterator = iterator->next;
	}

	//if the node was not found
	return LLIST_NODE_NOT_FOUND;
}

LLIST_RETURN llist_remove_by_index(llist list, size_t index) {
	if (list == NULL) {
		return LLIST_NULL_ARGUMENT;
	}

	_node *node;
	LLIST_RETURN ret = _list_get((_list *)list, index, &node);
	if (ret != LLIST_SUCCESS) {
		return ret;
	}

	return _list_delete_node((_list *)list, node);
}
