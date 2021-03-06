/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <stdarg.h>
#include "ubt.h"


/* PROTOTYPE DECLARATION */
static int new_node(struct Node **node, char *key, void *value);
static int add_impl(struct Node *node, char *key, void *value,
        struct Node *parent);
static int delete_impl(struct Node *node, char *key,
        struct Node *parent, struct Node **del);
static int lookup_impl(struct Node *node, char *key,
        struct Node *parent, void **value);
static int min_node(struct Node *node, struct Node **result);
static int max_node(struct Node *node, struct Node **result);


int
initialize(struct Node *tree)
{ /* {{{ */
    int s;

    if (tree == NULL) {
        return EINVAL;
    }

    s = pthread_mutex_init(&tree->mtx, NULL);
    if (s != 0) {
        return s;
    }
    tree->key = NULL;
    tree->value = NULL;
    tree->left = NULL;
    tree->right = NULL;
    return 0;
} /* }}} */


/* helper function
 * <node> will be initialized with key-value pair.
 * If the node is initialized successfully, this return 0.
 * Otherwise, this return error code (>0).
 */
static int
new_node(struct Node **node, char *key, void *value)
{ /* {{{ */
    int s;

    *node = malloc(sizeof(struct Node));
    if (*node == NULL) {
        return -1;
    }
    s = pthread_mutex_init(&(*node)->mtx, NULL);
    if (s != 0) {
        free(*node);
        *node = NULL;
        return s;
    }
    (*node)->key = key;
    (*node)->value = value;
    (*node)->left = NULL;
    (*node)->right = NULL;

    return 0;
} /* }}} */


int
add(struct Node *tree, char *key, void *value)
{ /* {{{ */
    int s;

    if (tree == NULL || key == NULL) {
        return EINVAL;
    }

    s = pthread_mutex_lock(&tree->mtx);
    if (s != 0) {
        return s;
    }

    /* if tree is empty, create the top node */
    if (tree->key == NULL) {
        tree->key = key;
        tree->value = value;
        assert(tree->left == NULL);
        assert(tree->right == NULL);

        s = pthread_mutex_unlock(&tree->mtx);
        if (s != 0) {
            return s;
        }
        return 0;
    }

    return add_impl(tree, key, value, NULL);
} /* }}} */


/* add_impl is helper function for add
 * <node>:
 *  The top node of (sub)tree.
 * <key>:
 *  The key of key-value which will be inserted in the tree.
 * <value>:
 *  The value of key-value which will be inserted in the tree.
 * <parent>:
 *  The parent node of <node>.
 *  <parent> is NULL, if <node> is top of the tree.
 * --- CONDITION ---
 * BEFORE CALL:
 *   If <parent> is NULL,
 *      <node> is already locked.
 *   If <parent> is not NULL,
 *      <parent> is already locked, but <node> is NOT locked.
 * AFTER CALL:
 *   Parent will be unlocked.
 *   All node of subtree <node>, which was locked by this thread,
 *      will be unlocked.
 */
static int
add_impl(struct Node *node, char *key, void *value, struct Node *parent)
{ /* {{{ */
    int s, cmp;

    /* Parent is already locked, if parent is not NULL. */
    /* Node is already locked, if parent is NULL. */
    if (parent == NULL) {
        assert(node != NULL);
    }

    if (parent != NULL) {
        s = pthread_mutex_lock(&node->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&parent->mtx);
            return s;
        }
        s =  pthread_mutex_unlock(&parent->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&node->mtx);
            return s;
        }
    }

    cmp = strcmp(node->key, key);
    if (cmp == 0) { /* same key is found */
        s = pthread_mutex_unlock(&node->mtx);
        if (s != 0) {
            return s;
        }
        return -1;
    }
    if (cmp > 0) { /* maybe go into left sub-tree */
        if (node->left == NULL) {
            s = new_node(&node->left, key, value);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
            s = pthread_mutex_unlock(&node->mtx);
            if (s != 0) {
                return s;
            }
            return 0;
        }
        return add_impl(node->left, key, value, node);
    } else if (cmp < 0) { /* maybe go into right sub-tree */
        if (node->right == NULL) {
            s = new_node(&node->right, key, value);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
            s = pthread_mutex_unlock(&node->mtx);
            if (s != 0) {
                return s;
            }
            return 0;
        }
        return add_impl(node->right, key, value, node);
    }

    /* cannot reach */
    assert(false);
} /* }}} */


int
delete(struct Node *tree, char *key)
{ /* {{{ */
    int s;
    struct Node *del;

    if (tree == NULL || key == NULL) {
        return EINVAL;
    }

    s = pthread_mutex_lock(&tree->mtx);
    if (s != 0) {
        return s;
    }

    /* Tree is empty */
    if (tree->key == NULL) {
        s = pthread_mutex_unlock(&tree->mtx);
        if (s != 0) {
            return s;
        }
        return -1;
    }

    del = NULL;
    s = delete_impl(tree, key, NULL, &del);
    if (s != 0) {
        return s;
    }

    if (del == tree) {
        /* delete itself */
        assert(tree->left == NULL);
        assert(tree->right == NULL);
        tree->key = NULL;
        tree->value = NULL;
        s = pthread_mutex_unlock(&tree->mtx);
        if (s != 0) {
            return s;
        }
        return 0;
    } else if (del != NULL) {
        assert(del->left == NULL);
        assert(del->right == NULL);
        tree->key = del->key;
        tree->value = del->value;
        (void) pthread_mutex_unlock(&del->mtx);
        (void) pthread_mutex_destroy(&del->mtx);
        free(del);
        s = pthread_mutex_unlock(&tree->mtx);
        if (s != 0) {
            return s;
        }
        return 0;
    }

    return 0;
} /* }}} */


/* delete_impl is helper function for delete
 * <node>:
 *  The top node of (sub)tree.
 * <key>:
 *  The key of key-value which will be deleted from the tree.
 * <parent>:
 *  The parent node of <node>.
 *  <parent> is NULL, if <node> is top of the tree.
 * <del>:
 *  The pointer to the node which will be deleted.
 *  If parent is NULL, this function cannot delete the key-value pair.
 *  Caller (delete or delete_impl) must delete the key-value pair
 *  using this value when this function returned.
 * --- CONDITION ---
 * BEFORE CALL:
 *   If <parent> is NULL,
 *      <node> is already locked.
 *   If <parent> is not NULL,
 *      <parent> is already locked, but <node> is NOT locked.
 * AFTER CALL:
 *   Parent will be unlocked.
 *   ALl node of subtree <node>, which was locked by this thread,
 *      will be unlocked.
 */
static int
delete_impl(struct Node *node, char *key, struct Node *parent, struct Node **del)
{ /* {{{ */
    int s, cmp;
    struct Node *alt;

    /* Parent is already locked, if parent is not NULL */
    /* Node is already locked, if parent is NULL */

    if (parent != NULL) {
        s = pthread_mutex_lock(&node->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&parent->mtx);
            return s;
        }
    }

    cmp = strcmp(node->key, key);
    if (cmp == 0) {
        if (node->left == NULL && node->right == NULL) {
            /* foud the node which will be deleted */
            if (parent != NULL) {
                if (parent->left == node) {
                    parent->left = NULL;
                } else if (parent->right == node) {
                    parent->right = NULL;
                } else {
                    assert(false);
                }
                (void) pthread_mutex_unlock(&node->mtx);
                (void) pthread_mutex_destroy(&node->mtx);
                free(node);
                s = pthread_mutex_unlock(&parent->mtx);
                if (s != 0) {
                    return s;
                }
            } else {
                *del = node;
            }
            return 0;
        }

        /* move the another node to this place */
        if (parent != NULL) {
            s = pthread_mutex_unlock(&parent->mtx);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
        }

        if (node->left != NULL) {
            s = pthread_mutex_lock(&node->left->mtx);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
            s = max_node(node->left, &alt);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->left->mtx);
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
            assert(alt != NULL);
            node->key = alt->key;
            node->value = alt->value;
            s = pthread_mutex_unlock(&alt->mtx);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
            if (node->left == alt) {
                (void) pthread_mutex_destroy(&alt->mtx);
                free(alt);
                node->left = NULL;
            } else {
                s = delete_impl(node->left, alt->key, NULL, NULL);
                if (s != 0) {
                    (void) pthread_mutex_unlock(&node->mtx);
                    return s;
                }
            }
            s = pthread_mutex_unlock(&node->mtx);
            if (s != 0) {
                return s;
            }
            return 0;
        }

        if (node->right != NULL) {
            s = pthread_mutex_lock(&node->right->mtx);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
            s = min_node(node->right, &alt);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->right->mtx);
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
            assert(alt != NULL);
            node->key = alt->key;
            node->value = alt->value;
            s = pthread_mutex_unlock(&alt->mtx);
            if (s != 0) {
                (void) pthread_mutex_unlock(&node->mtx);
                return s;
            }
            if (node->right == alt) {
                (void) pthread_mutex_destroy(&alt->mtx);
                free(alt);
                node->right = NULL;
            } else {
                s = delete_impl(node->right, alt->key, NULL, NULL);
                if (s != 0) {
                    (void) pthread_mutex_unlock(&node->mtx);
                    return s;
                }
            }
            s = pthread_mutex_unlock(&node->mtx);
            if (s != 0) {
                return s;
            }
            return 0;
        }

        /* cannot reach */
        assert(false);
    }

    /* key may be found in sub-tree */
    if (parent != NULL) {
        s = pthread_mutex_unlock(&parent->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&node->mtx);
            return s;
        }
    }
    if (cmp > 0) { /* maybe go into left sub-tree */
        if (node->left == NULL) {
            s = pthread_mutex_unlock(&node->mtx);
            if (s != 0) {
                return s;
            }
            return -1;
        }
        return delete_impl(node->left, key, node, del);
    }
    if (cmp < 0) { /* maybe go into right sub-tree */
        if (node->right == NULL) {
            s = pthread_mutex_unlock(&node->mtx);
            if (s != 0) {
                return s;
            }
            return -1;
        }
        return delete_impl(node->right, key, node, del);
    }

    /* cannot reach */
    assert(false);
} /* }}} */


int
lookup(struct Node *tree, char *key, void **value)
{ /* {{{ */
    int s;

    if (tree == NULL || key == NULL || value == NULL) {
        return EINVAL;
    }

    s = pthread_mutex_lock(&tree->mtx);
    if (s != 0) {
        return s;
    }

    /* The tree is empty, the key is not found */
    if (tree->key == NULL) {
        value = NULL;
        s = pthread_mutex_unlock(&tree->mtx);
        if (s != 0) {
            return s;
        }
        return -1;
    }

    return lookup_impl(tree, key, NULL, value);
} /* }}} */


/* lookup_impl is helper function for lookup
 * <node>:
 *  The top node of (sub)tree.
 * <key>:
 *  The key of key-value which will be found from the tree.
 * <parent>:
 *  The parent node of <node>.
 *  <parent> is NULL, if <node> is top of the tree.
 * <value>:
 *  The pointer to the value which will be found.
 * --- CONDITION ---
 * BEFORE CALL:
 *   If <parent> is NULL,
 *      <node> is already locked.
 *   If <parent> is not NULL,
 *      <parent> is already locked, but <node> is NOT locked.
 * AFTER CALL:
 *   Parent will be unlocked.
 *   ALl node of subtree <node>, which was locked by this thread,
 *      will be unlocked.
 */
static int
lookup_impl(struct Node *node, char *key, struct Node *parent, void **value)
{ /* {{{ */
    int s, cmp;

    /* node is already locked, if parent is NULL*/
    if (parent != NULL) {
        s = pthread_mutex_lock(&node->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&parent->mtx);
            return s;
        }
        s = pthread_mutex_unlock(&parent->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&node->mtx);
            return s;
        }
    }

    cmp = strcmp(node->key, key);
    if (cmp == 0) {
        /* found the key */
        *value = node->value;
        s = pthread_mutex_unlock(&node->mtx);
        if (s != 0) {
            return s;
        }
        return 0;
    }
    if (cmp > 0) {
        /* The key may be in left sub-tree */
        if (node->left == NULL) {
            s = pthread_mutex_unlock(&node->mtx);
            if (s != 0) {
                return s;
            }
            return -1;
        }
        return lookup_impl(node->left, key, node, value);
    } else if (cmp < 0) {
        /* The key may be in right sub-tree */
        if (node->right == NULL) {
            s = pthread_mutex_unlock(&node->mtx);
            if (s != 0) {
                return s;
            }
            return -1;
        }
        return lookup_impl(node->right, key, node, value);
    }

    /* cannot reach */
    assert(false);
} /* }}} */


/* min_node find the minimum key-value pair from the tree
 * <node>:
 *   The top node of (sub)tree.
 * <result>:
 *   The node with minimal key-value
 * --- CONDITION ---
 *  BEFORE CALL:
 *    <node> is already locked.
 *  AFTER CALL:
 *    <*result> is still locked.
 */
static int
min_node(struct Node *node, struct Node **result)
{ /* {{{ */
    int s;

    if (node == NULL || result == NULL) {
        return EINVAL;
    }

    /* node is already locked */
    if (node->left == NULL && node->right == NULL) {
        *result = node;
        return 0;
    } else if (node->left == NULL) {
        s = pthread_mutex_lock(&node->right->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&node->mtx);
            return s;
        }
        s = pthread_mutex_unlock(&node->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&node->right->mtx);
            return s;
        }
        return min_node(node->right, result);
    }

    s = pthread_mutex_lock(&node->left->mtx);
    if (s != 0) {
        (void) pthread_mutex_unlock(&node->mtx);
        return s;
    }
    s = pthread_mutex_unlock(&node->mtx);
    if (s != 0) {
        (void) pthread_mutex_unlock(&node->left->mtx);
        return s;
    }
    return min_node(node->left, result);
} /* }}} */


/* max_node find the maximal key-value pair from the tree
 * see also min_node
 */
static int
max_node(struct Node *node, struct Node **result)
{ /* {{{ */
    int s;

    if (node == NULL || result == NULL) {
        return EINVAL;
    }

    /* node is already locked */
    if (node->left == NULL && node->right == NULL) {
        *result = node;
        return 0;
    } else if (node->right == NULL) {
        s = pthread_mutex_lock(&node->left->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&node->mtx);
            return s;
        }
        s = pthread_mutex_unlock(&node->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&node->left->mtx);
            return s;
        }
        return max_node(node->left, result);
    }

    s = pthread_mutex_lock(&node->right->mtx);
    if (s != 0) {
        (void) pthread_mutex_unlock(&node->mtx);
        return s;
    }
    s = pthread_mutex_unlock(&node->mtx);
    if (s != 0) {
        (void) pthread_mutex_unlock(&node->right->mtx);
        return s;
    }
    return max_node(node->right, result);
} /* }}} */


/* print_tree_impl is helper function for print_tree
 * <tree>:
 *  the (sub)tree
 * <level>:
 *  indent level [depth of (sub)tree].
 * --- CONDITION ---
 *  BEFORE CALL:
 *   If level is 0, <tree> is not locked.
 *   Otherwise, <tree> is already locked.
 *  AFTER CALL:
 *   ALl node of subtree <node>, which was locked by this thread,
 *      will be unlocked.
 */
static int
print_tree_impl(struct Node *node, int level)
{ /* {{{ */
    int s;
    struct Node *left, *right;

    if (node == NULL) {
        printf("%*c%c\n", level * 4, ' ', '-');
        return 0;
    }

    /* If level is 0, tree is not already locked */
    if (level == 0) {
        s = pthread_mutex_lock(&node->mtx);
        if (s != 0) {
            return s;
        }
    }

    printf("%*c%s %p\n", level * 4, ' ', node->key, node->value);
    left = node->left;
    right = node->right;

    if (left == NULL && right == NULL) {
        s = pthread_mutex_unlock(&node->mtx);
        if (s != 0) {
            return s;
        }
        return 0;
    }

    if (left != NULL) {
        s = pthread_mutex_lock(&left->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&node->mtx);
            return s;
        }
    }
    if (right != NULL) {
        s = pthread_mutex_lock(&right->mtx);
        if (s != 0) {
            (void) pthread_mutex_unlock(&left->mtx);
            (void) pthread_mutex_unlock(&node->mtx);
            return s;
        }
    }
    s = pthread_mutex_unlock(&node->mtx);
    if (s != 0) {
        (void) pthread_mutex_unlock(&right->mtx);
        (void) pthread_mutex_unlock(&left->mtx);
        return s;
    }

    s = print_tree_impl(left, level+1);
    if (s != 0) {
        (void) pthread_mutex_unlock(&right->mtx);
        return s;
    }
    s = print_tree_impl(right, level+1);
    if (s != 0) {
        return s;
    }

    return 0;
} /* }}} */


int
print_tree(struct Node *tree)
{ /* {{{ */
    if (tree == NULL) {
        return EINVAL;
    }
    return print_tree_impl(tree, 0);
} /* }}} */
