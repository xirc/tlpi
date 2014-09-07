/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef __UBT_H__
#define __UBT_H__


/* Binary Tree */
struct Node {
    char *key;
    void *value;
    struct Node *left;
    struct Node *right;
    pthread_mutex_t mtx;
};


/* Initialize the tree.
 * If the tree is initialized successfully, this return 0.
 * Otherwise, this return error code (>0).
 */
int initialize(struct Node *tree);


/* Insert key-value pair into the tree.
 * If the key-value pair is inserted successfully, this return 0.
 * If the same key is found in the tree, this return -1.
 * Otherwise, this return error code (>0).
 */
int add(struct Node *tree, char *key, void *value);


/* Delete key-value pair from the tree.
 * If the key-value pair is deleted successfully, this return 0.
 * If the key is NOT found in the tree, this return -1.
 * Otherwise, this return error code (>0).
 */
int delete(struct Node *tree, char *key);


/* Find key-value pair in the tree.
 * If the key-value pair is found successfully,
 *    value is stored, and this return 0.
 * If the key-value pair is not found, this return -1.
 * Otherwise, this return error code (>0).
 */
int lookup(struct Node *tree, char *key, void **value);


/* Print the tree as human redable text format
 * If tree is printed successfully, this return 0.
 * Otherwise, this return error code (>0).
 */
int print_tree(struct Node *tree);


#endif
