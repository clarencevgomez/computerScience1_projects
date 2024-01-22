
/* COP 3502C Programming Assignment 5
This program is written by: Clarence Gomez */

// header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "leak_detector_c.h"

// constants
#define MAX_LENGTH 2000001
#define MAX_CHILD 26

// Trie/Dictionary Node
typedef struct Dictionary
{
    int freq;
    int prefixSum;
    int maxChildFreq;
    struct Dictionary *children[MAX_CHILD];
} Dictionary;

// node creation function
Dictionary *create_node()
{
    Dictionary *node = (Dictionary *)malloc(sizeof(Dictionary));
    node->freq = 0;
    node->prefixSum = 0;
    node->maxChildFreq = 0;

    // initializing each of the children
    for (int i = 0; i < MAX_CHILD; i++)
        node->children[i] = NULL;

    return node;
}

// insert into dictionary function
void insert(Dictionary *root, char *word, int f)
{
    Dictionary *current = root;
    while (*word)
    {
        int idx = *word - 'a';
        if (current->children[idx] == NULL)
            current->children[idx] = create_node();

        current = current->children[idx];
        current->prefixSum += f;
        word++;
    }
    current->freq += f;
}

// function to handle queries
void query(Dictionary *root, char *prefix)
{
    Dictionary *current = root;
    while (*prefix)
    {
        int idx = *prefix - 'a';
        // printf("prefix is: %c \n", *prefix);
        if (current->children[idx] == NULL)
        {
            printf("unknown word\n");
            return;
        }
        current = current->children[idx];
        prefix++;
    }

    for (int i = 0; i < MAX_CHILD; i++)
    {
        if (current->children[i] != NULL && current->children[i]->prefixSum == current->maxChildFreq)
            printf("%c", i + 'a');
    }
    printf("\n");
}

void maxChildUpdate(Dictionary *node)
{
    node->maxChildFreq = 0;
    for (int i = 0; i < MAX_CHILD; i++)
    {
        if (node->children[i] != NULL)
            if (node->children[i]->prefixSum > node->maxChildFreq)
                node->maxChildFreq = node->children[i]->prefixSum;
    }
}
// Function to free the memory used by the trie
void freeDictionary(Dictionary *node)
{
    if (node != NULL)
    {
        for (int i = 0; i < MAX_CHILD; i++)
            freeDictionary(node->children[i]);
        free(node);
    }
}

// driver function
int main()
{
    atexit(report_mem_leak);
    int n;
    scanf("%d", &n);

    Dictionary *root = create_node(); // create the trie dictionary

    for (int i = 0; i < n; i++)
    {
        int command, frequency;
        char word[MAX_LENGTH];
        scanf("%d", &command);

        if (command == 1) // add to dictionary
        {
            scanf("%s %d", word, &frequency);
            insert(root, word, frequency);
            Dictionary *current = root;
            for (int j = 0; word[j]; j++)
            {
                maxChildUpdate(current);
                current = current->children[word[j] - 'a'];
            }
        }
        else if (command == 2) // complete the query
        {
            scanf("%s", word);
            query(root, word);
        }
    }
    // free the dictionary
    freeDictionary(root);

    return 0;
}