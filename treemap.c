#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lt) (void* key1, void* key2)) {
    TreeMap * map = (TreeMap *) malloc(sizeof(TreeMap));
    if (map == NULL) return NULL;

    map->root = NULL;
    map->current = NULL;
    map->lower_than = lt;

    return map;
}


void insertTreeMap(TreeMap* tree, void* key, void* value) {
    if (searchTreeMap(tree, key) != NULL)
        return;

    TreeNode* parent = NULL;
    TreeNode* current = tree->root;

    while (current != NULL) {
        parent = current;
        if (tree->lower_than(key, current->pair->key))
            current = current->left;
        else
            current = current->right;
    }

    TreeNode* newNode = createTreeNode(key, value);
    newNode->parent = parent;

    if (parent == NULL) {
        tree->root = newNode;
    } else if (tree->lower_than(key, parent->pair->key)) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }

    tree->current = newNode;
}


TreeNode * minimum(TreeNode * x) {
    if (x == NULL) return NULL;

    while (x->left != NULL) {
        x = x->left;
    }

    return x;
}



void removeNode(TreeMap * tree, TreeNode* node) {
    if (node->left == NULL && node->right == NULL) {
        if (node->parent == NULL) {
            tree->root = NULL;
        } else {
            if (node->parent->left == node)
                node->parent->left = NULL;
            else
                node->parent->right = NULL;
        }
        free(node->pair);
        free(node);
        return;
    }

    if (node->left == NULL || node->right == NULL) {
        TreeNode* child = (node->left != NULL) ? node->left : node->right;

        if (node->parent == NULL) {
            tree->root = child;
        } else {
            if (node->parent->left == node)
                node->parent->left = child;
            else
                node->parent->right = child;
        }

        child->parent = node->parent;

        free(node->pair);
        free(node);
        return;
    }

    TreeNode* minNode = minimum(node->right);

    void* tempKey = minNode->pair->key;
    void* tempValue = minNode->pair->value;

    node->pair->key = tempKey;
    node->pair->value = tempValue;

    removeNode(tree, minNode);
}


void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}




Pair * searchTreeMap(TreeMap* tree, void* key) {
    TreeNode* node = tree->root;

    while (node != NULL) {
        if (is_equal(tree, key, node->pair->key)) {
            tree->current = node;
            return node->pair;
        }
        else if (tree->lower_than(key, node->pair->key)) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    return NULL;
}



Pair * upperBound(TreeMap* tree, void* key) {
    TreeNode* node = tree->root;
    TreeNode* ub_node = NULL;

    while (node != NULL) {
        if (is_equal(tree, key, node->pair->key)) {
            tree->current = node;
            return node->pair;
        }

        if (tree->lower_than(key, node->pair->key)) {
            ub_node = node;
            node = node->left;
        } else {
            node = node->right;
        }
    }

    if (ub_node != NULL) {
        tree->current = ub_node;
        return ub_node->pair;
    }

    return NULL;
}


Pair * firstTreeMap(TreeMap * tree) {
    if (tree->root == NULL) return NULL;

    TreeNode* minNode = minimum(tree->root);
    tree->current = minNode;

    return minNode->pair;
}


Pair * nextTreeMap(TreeMap * tree) {
    if (tree->current == NULL) return NULL;

    TreeNode* node = tree->current;

    if (node->right != NULL) {
        node = minimum(node->right);
        tree->current = node;
        return node->pair;
    }

    TreeNode* parent = node->parent;
    while (parent != NULL && parent->right == node) {
        node = parent;
        parent = parent->parent;
    }

    tree->current = parent;
    if (parent == NULL) return NULL;

    return parent->pair;
}

