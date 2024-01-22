#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "leak_detector_c.h"

#define MAXLEN 30

typedef struct itemNode
{
    char name[MAXLEN];
    int count;
    struct itemNode *left, *right;
} itemNode;

typedef struct treeNameNode
{
    char treeName[MAXLEN];
    struct treeNameNode *left, *right;
    itemNode *theTree;
} treeNameNode;

treeNameNode *createTreeNameNode(char *name)
{
    treeNameNode *nameNode = (treeNameNode *)malloc(sizeof(treeNameNode));
    strcpy(nameNode->treeName, name);
    nameNode->left = NULL;
    nameNode->right = NULL;
    nameNode->theTree = NULL;
    return nameNode;
}

// Create an item within the tree
itemNode *createItemNode(char *name, int count)
{
    itemNode *node = (itemNode *)malloc(sizeof(itemNode));
    strcpy(node->name, name);
    node->count = count;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Add new tree into root
treeNameNode *insertNameNode(treeNameNode *root, treeNameNode *newNode)
{
    if (root == NULL)
        return newNode;
    else
    {
        if (strcmp(root->treeName, newNode->treeName) < 0)
        {
            if (root->right != NULL)
                root->right = insertNameNode(root->right, newNode);
            else
                root->right = newNode;
        }
        else
        {
            if (root->left != NULL)
                root->left = insertNameNode(root->left, newNode);
            else
                root->left = newNode;
        }
    }
    return root;
}

// Add new item into tree
itemNode *insertItemNode(itemNode *treeRoot, itemNode *newNode)
{
    if (treeRoot == NULL)
        return newNode;
    else
    {
        if (strcmp(treeRoot->name, newNode->name) < 0)
        {
            if (treeRoot->right != NULL)
                treeRoot->right = insertItemNode(treeRoot->right, newNode);
            else
                treeRoot->right = newNode;
        }
        else
        {
            if (treeRoot->left != NULL)
                treeRoot->left = insertItemNode(treeRoot->left, newNode);
            else
                treeRoot->left = newNode;
        }
    }
    return treeRoot;
}

// From the input file, take all tree names and create a tree for each
treeNameNode *buildNameTree(FILE *inFile, int N)
{
    if (inFile == NULL)
        return NULL;

    treeNameNode *nameRoot = NULL;
    char name[MAXLEN];
    for (int i = 0; i < N; i++)
    {
        fscanf(inFile, "%s", name);
        treeNameNode *newNameNode = createTreeNameNode(name);
        nameRoot = insertNameNode(nameRoot, newNameNode);
    }
    return nameRoot;
}

// Find a specific tree from treeName
treeNameNode *searchNameNode(treeNameNode *root, char treeName[MAXLEN])
{
    if (root == NULL)
        return NULL;
    treeNameNode *nameNode;
    if (strcmp(root->treeName, treeName) == 0)
        return root;
    else if (strcmp(root->treeName, treeName) > 0)
        nameNode = searchNameNode(root->left, treeName);
    else
        nameNode = searchNameNode(root->right, treeName);
    return nameNode;
}

// From the input file, fill the created trees with the nodes
void populateTrees(FILE *inFile, treeNameNode *nameRoot, int I)
{
    if (inFile == NULL)
        return;
    else
    {
        int count;
        char treeName[MAXLEN];
        char itemName[MAXLEN];
        for (int i = 0; i < I; i++)
        {
            fscanf(inFile, "%s %s %d", treeName, itemName, &count);
            itemNode *newItemNode = createItemNode(itemName, count);
            treeNameNode *treeNode = searchNameNode(nameRoot, treeName);
            treeNode->theTree = insertItemNode(treeNode->theTree, newItemNode);
        }
    }
}

// Display the elements of a tree in order alphabetically
void displayInOrderNameTree(treeNameNode *root, FILE *ofp)
{
    if (root != NULL)
    {
        displayInOrderNameTree(root->left, ofp);
        printf("%s ", root->treeName);
        fprintf(ofp, "%s ", root->treeName);
        displayInOrderNameTree(root->right, ofp);
    }
}

// Traverses the itemNode sub tree
void traverseSubTree(itemNode *root, FILE *ofp)
{
    if (root != NULL)
    {
        traverseSubTree(root->left, ofp);
        printf("%s ", root->name);
        fprintf(ofp, "%s ", root->name);
        traverseSubTree(root->right, ofp);
    }
}

// Traverses the tree and uses traverse subtree and prints it out
void traverse_in_traverse(treeNameNode *root, FILE *ofp)
{
    if (root != NULL)
    {
        traverse_in_traverse(root->left, ofp);
        printf("\n===%s===\n", root->treeName);
        fprintf(ofp, "\n===%s===\n", root->treeName);
        traverseSubTree(root->theTree, ofp);
        traverse_in_traverse(root->right, ofp);
    }
}

// Free a specifc given node
void freeNode(itemNode *root)
{
    if (root != NULL)
    {
        freeNode(root->left);
        freeNode(root->right);
        free(root);
    }
}

// Free specific tree
void freeTree(treeNameNode *root)
{
    if (root != NULL)
    {
        freeTree(root->left);
        freeTree(root->right);
        freeNode(root->theTree);
        free(root);
    }
}

// Counts how many elements are before ther given node
int count_before(itemNode *root, char *nodeName)
{
    if (root == NULL)
        return 0;
    if (strcmp(root->name, nodeName) == 0)
        return 1;
    else if (strcmp(root->name, nodeName) > 0)
        return 1 + count_before(root->left, nodeName);
    else
        return 1 + count_before(root->right, nodeName);
}

// Finds the item before the given node
void item_before(treeNameNode *root, char *treeName, char *nodeName, FILE *ofp)
{
    treeNameNode *tree = searchNameNode(root, treeName);
    int count = count_before(tree->theTree, nodeName);
    printf("\nitem before %s: %d", nodeName, count + 1);
    fprintf(ofp, "\nitem before %s: %d", nodeName, count + 1);
}

// Returns a pointer to the node storing the minimum value in the tree
// with the root, root. Will not work if called with an empty tree.
itemNode *minVal(itemNode *root)
{
    // Root stores the minimal value.
    if (root->left == NULL)
        return root;

    // The left subtree of the root stores the minimal value.
    else
        return minVal(root->left);
}

// Returns 1 iff node has a left child and no right child.
int hasOnlyLeftChild(itemNode *node)
{
    return (node->left != NULL && node->right == NULL);
}

// Returns 1 iff node has a right child and no left child.
int hasOnlyRightChild(itemNode *node)
{
    return (node->left == NULL && node->right != NULL);
}

// Returns 1 if node is a leaf node, 0 otherwise.
int isLeaf(itemNode *node)
{
    return (node->left == NULL && node->right == NULL);
}

itemNode *parent(itemNode *root, itemNode *node)
{
    // Take care of NULL cases.
    if (root == NULL || root == node)
        return NULL;

    // // The root is the direct parent of node.
    if (root->left == node || root->right == node)
        return root;

    // // Look for node's parent in the left side of the tree.
    if (strcmp(node->name, root->name) < 0)
    {
        return parent(root->left, node);
    }

    // // Look for node's parent in the right side of the tree.
    else if (strcmp(node->name, root->name) > 0)
        return parent(root->right, node);

    return NULL; // Catch any other extraneous cases.
}

// Finds a node from a node name and given tree
itemNode *findNode(itemNode *current_ptr, char *nodeName)
{
    // Check if there are nodes in the tree.
    if (current_ptr != NULL)
    {
        // Found the value at the root.
        if (strcmp(current_ptr->name, nodeName) == 0)
        {
            return current_ptr;
        }

        // Search to the left.
        if (strcmp(nodeName, current_ptr->name) < 0)
            return findNode(current_ptr->left, nodeName);

        // Or...search to the right.
        else
            return findNode(current_ptr->right, nodeName);
    }
    else
        return NULL; // No node found.
}

// Delete a node from a node name
itemNode *delete_node(itemNode *root, char *nodeName)
{
    itemNode *delnode, *new_del_node, *save_node;
    itemNode *par;
    char save_val[MAXLEN];

    delnode = findNode(root, nodeName); // Get a pointer to the node to delete.
    // strcpy(delnode->name, "");
    par = parent(root, delnode); // Get the parent of this node.

    // Take care of the case where the node to delete is a leaf node.
    if (isLeaf(delnode))
    { // case 1

        // Deleting the only node in the tree.
        if (par == NULL)
        {
            free(root); // free the memory for the node.
            return NULL;
        }

        // Deletes the node if it's a left child.
        if (strcmp(nodeName, par->name) > 0)
        {
            free(par->left); // Free the memory for the node.
            par->left = NULL;
        }

        // Deletes the node if it's a right child.
        else
        {
            free(par->right); // Free the memory for the node.
            par->right = NULL;
        }
        strcpy(delnode->name, "");
        return root; // Return the root of the new tree.
    }

    // Take care of the case where the node to be deleted only has a left
    // child.
    if (hasOnlyLeftChild(delnode))
    {

        // Deleting the root node of the tree.
        if (par == NULL)
        {
            save_node = delnode->left;
            free(delnode);    // Free the node to delete.
            return save_node; // Return the new root node of the resulting tree.
        }

        // Deletes the node if it's a left child.
        if (strcmp(nodeName, par->name) > 0)
        {
            save_node = par->left;       // Save the node to delete.
            par->left = par->left->left; // Readjust the parent pointer.
            free(save_node);             // Free the memory for the deleted node.
        }

        // Deletes the node if it's a right child.
        else
        {
            save_node = par->right;        // Save the node to delete.
            par->right = par->right->left; // Readjust the parent pointer.
            free(save_node);               // Free the memory for the deleted node.
        }
        strcpy(delnode->name, "");
        return root; // Return the root of the tree after the deletion.
    }

    // Takes care of the case where the deleted node only has a right child.
    if (hasOnlyRightChild(delnode))
    {

        // Node to delete is the root node.
        if (par == NULL)
        {
            save_node = delnode->right;
            free(delnode);
            return save_node;
        }

        // Delete's the node if it is a left child.
        if (strcmp(nodeName, par->name) > 0)
        {
            save_node = par->left;
            par->left = par->left->right;
            free(save_node);
        }

        // Delete's the node if it is a right child.
        else
        {
            save_node = par->right;
            par->right = par->right->right;
            free(save_node);
        }
        return root;
    }
    // if your code reaches hear it means delnode has two children
    //  Find the new physical node to delete.
    new_del_node = minVal(delnode->right);
    strcpy(save_val, new_del_node->name);
    delete_node(root, save_val); // Now, delete the proper value.

    // Restore the data to the original node to be deleted.
    strcpy(delnode->name, "");
    delnode->count = 0;

    return root;
}

// Deletes a given node from a tree and node name
void delete_query(treeNameNode *root, char *treeName, char *nodeName, FILE *ofp)
{
    treeNameNode *tree = searchNameNode(root, treeName);
    if (tree == NULL)
    {
        printf("\n%s does not exist", treeName);
        fprintf(ofp, "\n%s does not exist", treeName);
    }
    else
    {
        tree->theTree = delete_node(tree->theTree, nodeName);
        printf("\n%s deleted from %s", nodeName, treeName);
        fprintf(ofp, "\n%s deleted from %s", nodeName, treeName);
    }
}

// Adds up total of all nodes
int count_all(itemNode *root)
{
    if (root == NULL)
        return 0;
    else
        return root->count + count_all(root->left) + count_all(root->right);
}

// Count the number of nodes in a tree
void count_nodes(treeNameNode *root, char *treeName, FILE *ofp)
{
    treeNameNode *tree = searchNameNode(root, treeName);
    if (tree == NULL)
    {
        printf("\n%s does not exist", treeName);
        fprintf(ofp, "\n%s does not exist", treeName);
    }
    else
    {
        int count = count_all(tree->theTree);
        printf("\n%s count %d", treeName, count);
        fprintf(ofp, "\n%s count %d", treeName, count);
    }
}

// Used for height_balance to find the right height
int calculate_right(itemNode *root)
{
    int right = 0;
    while (root->right != NULL)
    {
        right++;
        root = root->right;
    }
    if (root->left != NULL)
    {
        while (root->left != NULL)
        {
            right++;
            root = root->left;
        }
    }
    return right;
}

// Used for height_balance to find the left height
int calculate_left(itemNode *root)
{
    int left = 0;
    while (root->left != NULL)
    {
        left++;
        root = root->left;
    }
    if (root->right != NULL)
    {
        while (root->right != NULL)
        {
            left++;
            root = root->right;
        }
    }
    return left;
}

// Determine if a tree is balanced or not
void height_balance(treeNameNode *root, char *treeName, FILE *ofp)
{
    treeNameNode *tree = searchNameNode(root, treeName);
    int left;
    int right;
    int height;
    if (tree->theTree->left == NULL)
        left = -1;
    if (tree->theTree->right == NULL)
        right = -1;
    if (tree->theTree->left != NULL)
    {
        left = calculate_left(tree->theTree->left);
    }
    if (tree->theTree->right != NULL)
    {
        right = calculate_right(tree->theTree->right);
    }
    height = abs(right - left);
    printf("\n%s: left height %d, right height %d, difference %d, %s",
           treeName, left, right, height, height == 0 ? "balanced" : "not balanced");
    fprintf(ofp, "\n%s: left height %d, right height %d, difference %d, %s",
            treeName, left, right, height, height == 0 ? "balanced" : "not balanced");
}

// Searches tree for a given node and returns the count of that node
int searchTree(itemNode *root, char *nodeName)
{
    if (root == NULL)
        return 0;
    if (strcmp(root->name, nodeName) == 0)
        return root->count;
    else if (strcmp(root->name, nodeName) > 0)
        return searchTree(root->left, nodeName);
    else
        return searchTree(root->right, nodeName);
}

// Finds the tree to search for, then calls search tree to search for the specific node
void search(treeNameNode *root, char *treeName, char *nodeName, FILE *ofp)
{
    treeNameNode *tree = searchNameNode(root, treeName);
    if (tree == NULL)
    {
        printf("\n%s does not exist", treeName);
        fprintf(ofp, "\n%s does not exist", treeName);
    }
    else
    {
        int count = searchTree(tree->theTree, nodeName);
        if (count == 0)
        {
            printf("\n%s not found in %s", nodeName, treeName);
            fprintf(ofp, "\n%s not found in %s", nodeName, treeName);
        }
        else
        {
            printf("\n%d %s found in %s", count, nodeName, treeName);
            fprintf(ofp, "\n%d %s found in %s", count, nodeName, treeName);
        }
    }
}

// Finds an element from a given tree and node and reduces it's count. If it is <=0, delete the node
void reduce(treeNameNode *root, char *treeName, char *nodeName, int count, FILE *ofp)
{
    treeNameNode *tree = searchNameNode(root, treeName);
    if (tree != NULL)
    {
        itemNode *temp = findNode(tree->theTree, nodeName);
        temp->count -= count;
        printf("\n%s reduced", nodeName);
        fprintf(ofp, "\n%s reduced", nodeName);
        if (temp->count <= 0)
            delete_node(tree->theTree, nodeName);
    }
}

// Delete an entire tree
void delete_name(treeNameNode *root, char *treeName, FILE *ofp)
{
    treeNameNode *tree = searchNameNode(root, treeName);
    strcpy(tree->treeName, "");
    tree = NULL;
    freeTree(tree);
    printf("\n%s deleted", treeName);
    fprintf(ofp, "\n%s deleted", treeName);
}

// This function will handle the queries
void queries(treeNameNode *root, FILE *inFile, FILE *ofp, int Q)
{
    for (int i = 0; i < Q; i++)
    {
        char query[MAXLEN];
        char treeName[MAXLEN];
        char nodeName[MAXLEN];
        int count;

        // Scan for query
        fscanf(inFile, "%s", query);

        // Based off the query, determine what other inputs are needed
        if (strcmp(query, "search") == 0 || strcmp(query, "item_before") == 0 || strcmp(query, "delete") == 0)
            fscanf(inFile, "%s %s", treeName, nodeName);
        else if (strcmp(query, "height_balance") == 0 || strcmp(query, "count") == 0 || strcmp(query, "delete_name") == 0)
            fscanf(inFile, "%s", treeName);
        else if (strcmp(query, "reduce") == 0)
            fscanf(inFile, "%s %s %d", treeName, nodeName, &count);

        // Based off of the query, do the action the query asks for
        if (strcmp(query, "search") == 0)
            search(root, treeName, nodeName, ofp);
        else if (strcmp(query, "item_before") == 0)
            item_before(root, treeName, nodeName, ofp);
        else if (strcmp(query, "height_balance") == 0)
            height_balance(root, treeName, ofp);
        else if (strcmp(query, "delete") == 0)
            delete_query(root, treeName, nodeName, ofp);
        else if (strcmp(query, "count") == 0)
            count_nodes(root, treeName, ofp);
        else if (strcmp(query, "decrease") == 0)
            reduce(root, treeName, nodeName, count, ofp);
        else if (strcmp(query, "delete_name") == 0)
            delete_name(root, treeName, ofp);
    }
}

// Main funciton will init nameRoot and call necessary functions, then free memory and close files.
int main()
{
    atexit(report_mem_leak);
    int N, I, Q;
    FILE *inFile = fopen("in2.txt", "r");
    FILE *ofp = fopen("out333.txt", "w");

    fscanf(inFile, "%d %d %d", &N, &I, &Q);

    treeNameNode *root = buildNameTree(inFile, N);
    populateTrees(inFile, root, I);
    displayInOrderNameTree(root, ofp);
    traverse_in_traverse(root, ofp);

    queries(root, inFile, ofp, Q);
    printf("\n");

    freeTree(root);
    fclose(inFile);
    fclose(ofp);
}