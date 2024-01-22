/* COP 3502C Programming Assignment 4
    - Almost a Forest -
This program is written by: Clarence Gomez */

// header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

// Constants
#define MAXLEN 30

// Item Struct
typedef struct itemNode
{
    char name[MAXLEN];
    int count;
    struct itemNode *left, *right;
} itemNode;

// Tree Struct
typedef struct treeNameNode
{
    char treeName[MAXLEN];
    struct treeNameNode *left, *right;
    itemNode *theTree;
} treeNameNode;

// function prototypes
treeNameNode *create_tNode(char tName[MAXLEN]);
treeNameNode *buildNameTree(treeNameNode *root, int N, char tn[N][MAXLEN]);
treeNameNode *insert(treeNameNode *root, treeNameNode *element);
treeNameNode *addItems(treeNameNode *root, int I, char nameType[I][MAXLEN], char iNames[I][MAXLEN], int iCount[I]);
void inorder(treeNameNode *ptr, FILE *ofile);
treeNameNode *searchNameNode(treeNameNode *root, char treeName[50]);
void traverse_in_traverse(treeNameNode *root, FILE *ofile);
itemNode *create_iNode(char iName[MAXLEN], int cnt);
itemNode *itemInsert(itemNode *root, itemNode *item);
void itemInorder(itemNode *ptr, FILE *ofile);
void free_forest(treeNameNode *forest);
void freeNode(itemNode *root);
void find(itemNode *root, char name[MAXLEN], char itemName[MAXLEN], FILE *ofile);
int count_before(itemNode *root, char val[MAXLEN]);
int count(itemNode *root);
void QueryHandler(treeNameNode *root, int Q, char query[Q][100], FILE *ofp);
int height(itemNode *root);
void check_balance(treeNameNode *tree, FILE *ofp);
void delete_name(treeNameNode *root, char treeName[MAXLEN], FILE *ofp);
void decrease(itemNode *root, char name[MAXLEN], char itemName[MAXLEN], int dAmnt, FILE *ofp);
itemNode *findItem(itemNode *current_item, char itemName[MAXLEN]);
itemNode *minVal(itemNode *root);
itemNode *maxVal(itemNode *root);
int isLeaf(itemNode *tree);
int hasOnlyLeftChild(itemNode *tree);
int hasOnlyRightChild(itemNode *tree);
itemNode *parent(itemNode *root, itemNode *item);
itemNode *removeItem(itemNode *root, char delItem[MAXLEN]);

// driver function
int main()
{
    atexit(report_mem_leak);
    //  N - # of Tree Names
    //  I - Total # items in the list to be inserted to all trees
    //  Q - # of queries listed in the input
    FILE *fForest = fopen("out.txt", "w");

    int N, I, Q, i;
    char c;
    scanf("%d %d %d", &N, &I, &Q);
    char treeNames[N][MAXLEN], itemTName[I][MAXLEN], itemNames[I][MAXLEN], query[Q][100];
    int itemCount[I];

    // get the tree names
    for (i = 0; i < N; i++)
        scanf("%s[^\n]", treeNames[i]);

    // now get the item names
    for (i = 0; i < I; i++)
        scanf("%s %s %d", itemTName[i], itemNames[i], &itemCount[i]);

    // now get the query items
    for (i = 0; i < Q; i++)
    {
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        scanf("%[^\n]", query[i]);
    }

    treeNameNode *forest = NULL;
    forest = buildNameTree(forest, N, treeNames);
    forest = addItems(forest, I, itemTName, itemNames, itemCount);

    // output the items added
    inorder(forest, fForest);
    traverse_in_traverse(forest, fForest);
    QueryHandler(forest, Q, query, fForest);
    printf("\n");
    fprintf(fForest, "\n");

    free_forest(forest);
    fclose(fForest);

    return 0;
}

// creating an item node
itemNode *create_iNode(char iName[MAXLEN], int cnt)
{
    itemNode *newItem = (itemNode *)malloc(sizeof(itemNode));
    strcpy(newItem->name, iName);
    newItem->count = cnt;
    newItem->left = NULL;
    newItem->right = NULL;

    return newItem;
}
// creating the name nodes
treeNameNode *create_tNode(char tName[MAXLEN])
{
    treeNameNode *newName = (treeNameNode *)malloc(sizeof(treeNameNode));
    strcpy(newName->treeName, tName);
    newName->theTree = NULL;
    newName->left = NULL;
    newName->right = NULL;

    return newName;
}

// inserts to name tree and returns name tree root
treeNameNode *buildNameTree(treeNameNode *root, int N, char tn[N][MAXLEN])
{
    // create the nodes
    for (int i = 0; i < N; i++)
    {
        treeNameNode *newName = create_tNode(tn[i]);
        root = insert(root, newName);
    }

    return root;
}

// prints the data of the name tree and the corresponding item trees
void traverse_in_traverse(treeNameNode *root, FILE *ofile)
{
    if (root != NULL)
    {
        traverse_in_traverse(root->left, ofile); // Go Left.
        printf("\n###%s###\n", root->treeName);
        fprintf(ofile, "\n###%s###\n", root->treeName); // Print the root.
        itemInorder(root->theTree, ofile);
        traverse_in_traverse(root->right, ofile);
    }
}

// search for a name in the name tree and returns that node
treeNameNode *searchNameNode(treeNameNode *root, char treeName[50])
{
    if (root != NULL)
    {
        // Found the value at the root.
        if (strcmp(root->treeName, treeName) == 0)
            return root;

        // Search to the left.
        if (strcmp(treeName, root->treeName) < 0)
            return searchNameNode(root->left, treeName);

        // Or...search to the right.
        else
            return searchNameNode(root->right, treeName);
    }
    else
        return NULL;
}

// Name tree insertion function
treeNameNode *insert(treeNameNode *root, treeNameNode *element)
{
    // Inserting into an empty tree.
    if (root == NULL)
        return element;
    else
    {
        if (strcmp(element->treeName, root->treeName) < 0)
        {
            if (root->left != NULL)
                insert(root->left, element);
            else
                root->left = element;
        }
        else
        {
            if (root->right != NULL)
                insert(root->right, element);
            else
                root->right = element;
        }
    }
    return root;
}

// Item Tree insertion function
itemNode *itemInsert(itemNode *root, itemNode *item)
{
    // Inserting into an empty tree.
    if (root == NULL)
        return item;
    else
    {
        if (strcmp(item->name, root->name) < 0)
        {
            if (root->left != NULL)
                itemInsert(root->left, item);
            else
                root->left = item;
        }
        else
        {
            if (root->right != NULL)
                itemInsert(root->right, item);
            else
                root->right = item;
        }
    }
    return root;
}
// inorder function
void inorder(treeNameNode *ptr, FILE *ofile)
{
    // Only traverse the node if it's not null.
    if (ptr != NULL)
    {
        inorder(ptr->left, ofile); // Go Left.
        printf("%s ", ptr->treeName);
        fprintf(ofile, "%s ", ptr->treeName); // Print the root.
        inorder(ptr->right, ofile);           // Go Right.
    }
}
// item inorder function
void itemInorder(itemNode *ptr, FILE *ofile)
{
    if (ptr != NULL)
    {
        itemInorder(ptr->left, ofile);
        printf("%s ", ptr->name);
        fprintf(ofile, "%s ", ptr->name);
        itemInorder(ptr->right, ofile);
    }
}

// Adding items to their specified BST function
treeNameNode *addItems(treeNameNode *root, int I, char nameType[I][MAXLEN], char iNames[I][MAXLEN], int iCount[I])
{
    int i, j;

    for (i = 0; i < I; i++)
    {
        treeNameNode *currentName = searchNameNode(root, nameType[i]);
        itemNode *newItem = create_iNode(iNames[i], iCount[i]);
        currentName->theTree = itemInsert(currentName->theTree, newItem);
    }
    return root;
}
// -- functions needed to process Queries --

// Query Handler Function
void QueryHandler(treeNameNode *root, int Q, char query[Q][100], FILE *ofile)
{
    char command[MAXLEN], name[MAXLEN], itemName[MAXLEN];

    for (int i = 0; i < Q; i++)
    {
        sscanf(query[i], "%s %s", command, name);
        // check if the given tree exists
        if (searchNameNode(root, name) == NULL)
        {
            printf("\n%s does not exist", name);
            fprintf(ofile, "\n%s does not exist", name);
        }
        else
        {
            // get the tree
            treeNameNode *tree = searchNameNode(root, name);

            // find command
            if (strcmp(command, "find") == 0)
            {
                sscanf(query[i], "%s %s %s", command, name, itemName);
                find(tree->theTree, name, itemName, ofile);
            }
            // count_before command
            else if (strcmp(command, "count_before") == 0)
            {
                sscanf(query[i], "%s %s %s", command, name, itemName);
                printf("\nitem before %s: %d", itemName, count_before(tree->theTree, itemName) + 1);
                fprintf(ofile, "\nitem before %s: %d", itemName, count_before(tree->theTree, itemName) + 1);
            }

            // check_balance command
            else if (strcmp(command, "check_balance") == 0)
            {
                check_balance(tree, ofile);
            }
            // count command
            else if (strcmp(command, "count") == 0)
            {
                printf("\n%s count %d", tree->treeName, count(tree->theTree));
                fprintf(ofile, "\n%s count %d", tree->treeName, count(tree->theTree));
            }
            // decrease command
            else if (strcmp(command, "decrease") == 0)
            {
                int reduce;
                sscanf(query[i], "%s %s %s %d", command, name, itemName, &reduce);
                decrease(tree->theTree, name, itemName, reduce, ofile);
            }
            // remove command
            else if (strcmp(command, "remove") == 0)
            {
                sscanf(query[i], "%s %s %s", command, name, itemName);
                if (findItem(tree->theTree, itemName) == NULL)
                {
                    printf("\n%s not found in %s", itemName, name);
                    fprintf(ofile, "\n%s not found in %s", itemName, name);
                }
                else
                {
                    removeItem(tree->theTree, itemName);
                    printf("\n%s deleted from %s", itemName, name);
                    fprintf(ofile, "\n%s deleted from %s", itemName, name);
                }
            }
            // delete command
            else if (strcmp(command, "delete_name") == 0)
            {
                delete_name(tree, name, ofile);
            }
            else
            {
                printf("\ncommand not found");
                fprintf(ofile, "\ncommand not found");
            }
        }
    }
}

// freeing the forest memory
void free_forest(treeNameNode *forest)
{
    if (forest != NULL)
    {
        free_forest(forest->left);
        free_forest(forest->right);
        freeNode(forest->theTree);
        free(forest);
    }
}
// frees a tree node
void freeNode(itemNode *root)
{
    if (root != NULL)
    {
        freeNode(root->left);
        freeNode(root->right);
        free(root);
    }
}

// delete_name function
void delete_name(treeNameNode *root, char treeName[MAXLEN], FILE *ofile)
{
    if (root != NULL)
    {
        strcpy(root->treeName, "");
        root = NULL;
        free_forest(root);
        printf("\n%s deleted", treeName);
        fprintf(ofile, "\n%s deleted", treeName);
    }
}
// decrease function
void decrease(itemNode *root, char name[MAXLEN], char itemName[MAXLEN], int dAmnt, FILE *ofile)
{
    itemNode *item = findItem(root, itemName);
    if (item == NULL)
    {
        printf("\n%s deleted from %s", itemName, name);
        fprintf(ofile, "\n%s deleted from %s", itemName, name);
    }
    if (item->count - dAmnt <= 0)
    {
        removeItem(root, itemName);
        printf("\n%s deleted", itemName);
        fprintf(ofile, "\n%s deleted", itemName);
    }
    else
    {
        item->count -= dAmnt;
        printf("\n%s reduced", itemName);
        fprintf(ofile, "\n%s reduced", itemName);
    }
}
// find function
void find(itemNode *root, char name[MAXLEN], char itemName[MAXLEN], FILE *ofile)

{

    if (root != NULL)
    {
        // Found the value at the root.
        if (strcmp(root->name, itemName) == 0)
        {
            printf("\n%d %s found in %s", root->count, root->name, name);
            fprintf(ofile, "\n%d %s found in %s", root->count, root->name, name);
        }
        // Search to the left.
        if (strcmp(itemName, root->name) < 0)
            return find(root->left, name, itemName, ofile);

        // Or...search to the right.
        if (strcmp(itemName, root->name) > 0)
            return find(root->right, name, itemName, ofile);
    }
    else
        printf("\n%s not found in %s", itemName, name);
}

// counts the items in a given tree coming before a given item name
int count_before(itemNode *root, char itemName[MAXLEN])
{
    if (root == NULL)
        return 0;
    if (strcmp(root->name, itemName) == 0)
        return 1;
    else if (strcmp(root->name, itemName) > 0)
        return 1 + count_before(root->left, itemName);
    else
        return 1 + count_before(root->right, itemName);
}

// counts the amount of items in a tree name
int count(itemNode *root)

{
    if (root != NULL)
        return root->count + count(root->left) +
               count(root->right);
    else
        return 0;
}

// height function
int height(itemNode *root)
{
    if (root == NULL)
        return 0;

    int rightHeight, leftHeight;
    rightHeight = height(root->right);
    leftHeight = height(root->left);

    if (leftHeight > rightHeight)
        return leftHeight + 1;
    return rightHeight + 1;
}

// check balance function
void check_balance(treeNameNode *tree, FILE *ofile)
{
    // If tree is empty then return true
    if (tree == NULL)
        return;

    // getting the height of left and right sub trees
    int l = height(tree->theTree->left);
    int r = height(tree->theTree->right);

    // adjusting the height
    if (l == 1)
        l = 0;
    if (l == 0)
        l = -1;
    else
        l--;
    if (r == 1)
        r = 0;
    if (r == 0)
        r = -1;
    else
        r--;

    int diff = abs(l - r);

    if (diff == 0)
    {
        printf("\n%s: left height %d, right height %d, difference %d, balanced", tree->treeName, l, r, diff);
        fprintf(ofile, "\n%s: left height %d, right height %d, difference %d, balanced", tree->treeName, l, r, diff);
    }
    else
    {
        printf("\n%s: left height %d, right height %d, difference %d, not balanced", tree->treeName, l, r, diff);
        fprintf(ofile, "\n%s: left height %d, right height %d, difference %d, not balanced", tree->treeName, l, r, diff);
    }
}

/*      - functions needed for the remove function -    */

// if the itemNode is a leaf
int isLeaf(itemNode *tree)
{
    return (tree->left == NULL && tree->right == NULL);
}

// Returns 1 if node has a left child and no right child.
int hasOnlyLeftChild(itemNode *tree)
{
    return (tree->left != NULL && tree->right == NULL);
}

// Returns 1 if node has a right child and no left child.
int hasOnlyRightChild(itemNode *tree)
{
    return (tree->left == NULL && tree->right != NULL);
}
// parent function
itemNode *parent(itemNode *root, itemNode *item)
{

    // Take care of NULL cases.
    if (root == NULL || root == item)
        return NULL;

    // The root is the direct parent of node.
    if (root->left == item || root->right == item)
        return root;

    // Look for node's parent in the left side of the tree.
    if (strcmp(item->name, root->name) < 0)
        return parent(root->left, item);

    // Look for node's parent in the right side of the tree.
    else if (strcmp(item->name, root->name) > 0)
        return parent(root->right, item);

    return NULL; // Catch any other extraneous cases.
}

// Returns a pointer to the node storing the minimum value in the tree
itemNode *minVal(itemNode *root)
{
    if (root->left == NULL)
        return root;

    // The left subtree of the root stores the minimal value.
    else
        return minVal(root->left);
}

// Returns a pointer to the node storing the maximum value in the tree
itemNode *maxVal(itemNode *root)
{
    if (root->right == NULL)
        return root;
    // The right subtree of the root stores the maximal value.
    else
        return maxVal(root->right);
}

itemNode *findItem(itemNode *current_item, char itemName[MAXLEN])
{
    if (current_item != NULL)
    {
        // Found the value at the start
        if (strcmp(current_item->name, itemName) == 0)
            return current_item;

        // Searching to the left
        if (strcmp(itemName, current_item->name) < 0)
            return findItem(current_item->left, itemName);

        // Searching to the right
        else
            return findItem(current_item->right, itemName);
    }
    else
        return NULL; // No node found.
}

itemNode *removeItem(itemNode *root, char delItem[MAXLEN])
{
    itemNode *delnode, *new_del_node, *save_node;
    itemNode *par;
    char save_val[MAXLEN];

    delnode = findItem(root, delItem); // Get a pointer to the node to delete.
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
        if (strcmp(delItem, par->name) > 0)
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
        if (strcmp(delItem, par->name) > 0)
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
        if (strcmp(delItem, par->name) > 0)
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
    removeItem(root, save_val); // Now, delete the proper value.

    // Restore the data to the original node to be deleted.
    strcpy(delnode->name, "");
    delnode->count = 0;

    return root;
}