/**
 * @file		llist.h
 * @brief		This file contains a generic implementation of a double linked list
 *
 * @date		Dec 7, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 * @author	Simone Ruffini[simone.ruffini@tutanota.com]
 */

#ifndef LLIST_H
#define LLIST_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Return values for list manipulating functions
 */
typedef enum {
	LLIST_SUCCESS,			  /**< Operating success */
	LLIST_NODE_NOT_FOUND,	  /**< Error: No matching node found*/
	LLIST_EQUAL_MISSING,	  /**< Error: Equal function is missing*/
	LLIST_COMPARATOR_MISSING, /**< Error: Comparator function is missing*/
	LLIST_NULL_ARGUMENT,	  /**< Error: NULL argument*/
	LLIST_MALLOC_ERROR,		  /**< Error: Memory allocation error*/
	LLIST_ERROR				  /**< Error: Generic error*/
} LLIST_RETURN;

typedef void *llist;
typedef void *llist_node;

/**
* @brief Compares two nodes in a list
* @param[in] first llist_node 
* @param[in] second llist_node
* @return an integer less than, equal to, or greater than zero if first,
* respectively, to be less than, to match, or be greater than second.
*/
typedef int8_t (*comparator)(llist_node first, llist_node second);

/**
* @brief Check if two nodes are equal
* @param[in] first llist_node
* @param[in] second llist_node
* @return true if the nodes are equal, false otherwise
*/
typedef bool (*equal)(llist_node, llist_node);

#define LIST_INITALIZER \
	{ 0, NULL, NULL, NULL, NULL }

/**
 * @brief Initialize a list
 * @param[in] compare_func a function used to compare elements in the list
 * @param[in] equal_func a function used to check if two elements are equal
 * @return new list if success, NULL on error
 */
llist llist_init(comparator compare_func, equal equal_func);

/**
 * @brief Destroys a list
 * @warning Call this function only if the list was created with llist_init
 *          Static initializer created list cannot be destroyed using this function
 * @param[in] list The list to destroy
 *			  if NULL is provided standard library c free() will be used
 * @return LLIST_SUCCESS if list was destroyed
 */
LLIST_RETURN llist_destroy(llist list);

/**
 * @brief Add a node to a list on top(head)
 * @param[in] list the list to operate upon
 * @param[in] new_node the node to add (must be malloc outside)
 * @return int LLIST_SUCCESS if success
 */
LLIST_RETURN llist_push(llist list, llist_node new_node);

/**
 * @brief Add a node to a list on bottom (tail)
 * @param[in] list the list to operator upon
 * @param[in] new_node the node to add (must be malloc outside)
 * @return int LLIST_SUCCESS if success
 */
LLIST_RETURN llist_push_back(llist list, llist_node new_node);

/**
 * @brief remove the node from the list 
 * @param[in] list the list to operate on
 * @param[in] node the node to remove 
 * @return LLIST_SUCCSESS if success
 */
LLIST_RETURN llist_remove_by_node(llist list, llist_node node);

/**
 * @brief remove the node in position index from the list 
 * @param[in] list the list to operate on
 * @return LLIST_SUCCSESS if success
 */
LLIST_RETURN llist_remove_by_index(llist list, size_t index);

/**
 * @brief Insert new_node in the list in place of node, (the llist_node node becomes next of new_nocde)
 * @param[in] list list structure
 * @param[in] node the node on which to insert 
 * @param[in] new_node the data to add (must be malloc outside)
 * @return int LLIST_SUCCESS if success
 */
LLIST_RETURN llist_insert_by_node(llist list, llist_node node, llist_node new_node);

/**
 * @brief Insert new_node in the list at index pos, (node on index becomes index+1)
 * @param[in] list list structure
 * @param[in] index the position in the list where to insert 
 * @param[in] new_node the data to add (must be malloc outside)
 * @return int LLIST_SUCCESS if success
 */
LLIST_RETURN llist_insert_by_index(llist list, size_t index, llist_node new_node);

/**
 * @brief Insert a node in a priority queue-like structure
 * @param[in] list list structure
 * @param[in] new_node the data to add (must be malloc outside)
 * @return int LLIST_SUCCESS if success
 */
LLIST_RETURN llist_insert_priority(llist list, llist_node new_node);

/**
 * @brief Returns the head node of the list
 * @param[in] list the list to operate on
 * @return the head node, NULL on error
 */
llist_node llist_get_head(llist list);

/**
 * @brief Returns the tail node of the list
 * @param[in] list the list to operate on
 * @return the tail node, NULL on error
 */
llist_node llist_get_tail(llist list);

/**
 * @brief exports the list into an array
 * @details the array must be list->size long
 * 
 * @param[in] list	the list to operate on
 * @param[out] array	the array to export to (its memory address)
 * 
 * @return int LIST_SUCCEES for success
 */
LLIST_RETURN llist_export(llist list, void *array, size_t node_size);
/**
 * @brief return the number of elements in the list
 * @param[in] list the list to operate on
 * @return the size of the list 0 if empty 
 */
size_t llist_size(llist list);

/**
 * @brief Finds a node in a list and returns its index
 * @details If the node is found, the node variable is set to it
 * 
 * @param[in]  list the list to operator upon
 * @param[in]  node the node to find
 * @param[in/out] index the index of the found node (0 if empty).
 *		this pointer is the refence to the address of index (&index) 
 * @return LLIST_SUCCESS if success
 */
LLIST_RETURN llist_find(llist list, llist_node node, size_t *index);

/**
 * @brief Finds the node given an index in the list
 * @param[in] list the list to operator upon
 * @param[in] index the index of the node.
 * @param[in/out]  node the found node(NULL if not present)
 *		this pointer is the refence to the address of node(&node) 
 * @return LLIST_SUCCESS if success
 */
LLIST_RETURN llist_get(llist list, size_t index, llist_node *node);

#endif