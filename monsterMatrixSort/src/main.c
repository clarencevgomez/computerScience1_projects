/* COP 3502C Programming Assignment 3
    - Monster City Tour -
This program is written by: Clarence Gomez */

// header files
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "leak_detector_c.h"

// constants
#define LOC_MAX 10000
#define MON_MAX 1000000
#define POINT_MAX 200000
#define THRESHOLD 30
#define NOT_FOUND -999

// struct to store x & y coordinates
typedef struct Location
{
    int x;
    int y;
    int monDistance;
} location;

// global variables/users x and y coordinates with file ptr
FILE *monsterFile;
int x, y;

//  - Functions -

// function to calculate distance
int distance(int x2, int y2)
{
    int n1, n2;
    n1 = (x2 - x) * (x2 - x);
    n2 = (y2 - y) * (y2 - y);
    return sqrt(n1 + n2);
}

// function to initalize struct array of monster locations
location *readData(int *n, int *s, int *t)
{
    int i;
    scanf("%d %d %d %d %d", n, s, &x, &y, t);
    // allocate memory for struct array which store coordinates
    location *l = malloc(*n * sizeof(location));

    // monster coordinate information
    for (i = 0; i < *n; i++)
        scanf("%d %d", &l[i].x, &l[i].y);
    // calculating their distance from you
    for (i = 0; i < *n; i++)
        l[i].monDistance = distance(l[i].x, l[i].y);

    return l;
}

// creating another array for locations to search for
location *findValues(int s)
{
    // array to find the values to search for
    location *search = malloc(s * sizeof(location));
    int i;
    for (i = 0; i < s; i++)
        scanf("%d %d", &search[i].x, &search[i].y);

    // setting the distance from you for binary search later
    for (i = 0; i < s; i++)
        search[i].monDistance = distance(search[i].x, search[i].y);

    return search;
}

// comparing two monster locations
// -1 if ptr 1 is closer, 0 if theyre identical, 1 if ptr2 is closer
int compareTo(location *ptrPt1, location *ptrPt2)
{
    if (ptrPt1->monDistance < ptrPt2->monDistance)
        return -1;
    else if (ptrPt1->x == ptrPt2->x && ptrPt1->y == ptrPt2->y)
        return 0;
    else if (ptrPt1->monDistance > ptrPt2->monDistance)
        return 1;
    // exceptions if same distance but distinct points
    else if ((ptrPt1->monDistance == ptrPt2->monDistance) && (ptrPt1->x != ptrPt2->x || ptrPt1->y != ptrPt2->y))
    {
        if (ptrPt1->x < ptrPt2->x)
            return -1;
        else if (ptrPt1->x == ptrPt2->x)
        {
            if (ptrPt1->y < ptrPt2->y)
                return -1;
            else
                return 1;
        }
        else
            return 1;
    }
    return -1;
}

// insertion sort function for a certain subsection of the array
void insertionSort(location *list, int start, int end)
{
    for (int i = start + 1; i < end; i++)
    {
        location hand = list[i];
        int j = i - 1;
        // checking the numbers before it
        while (j >= start && (compareTo(&hand, &list[j]) < 0))
        {
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = hand;
    }
}

// merge function
void merge(location *list, int start, int mid, int end)
{
    int size = end - start + 1;
    int i = start, j = mid + 1, k = 0;

    // helper array
    location *temp = malloc(size * sizeof(location));
    while (i <= mid && j <= end)
    {
        if (compareTo(&list[i], &list[j]) < 0)
            temp[k++] = list[i++];
        else
            temp[k++] = list[j++];
    }
    // for out of bounds - only one of them will run
    while (i <= mid)
        temp[k++] = list[i++];

    while (j <= end)
        temp[k++] = list[j++];
    // copying to the original struct array
    for (i = 0; i < size; i++)
        list[start + i] = temp[i];

    // free after using it
    free(temp);
}

void mergeSort(location *list, int start, int end, int t)
{
    // to sort, use the distance formula to sort which monster distance is the shortest from you
    if (start < end)
    {
        if ((end - start <= t)) // if arr size <=t to use insertionSort
            insertionSort(list, start, end + 1);
        else
        {
            int mid = (start + end) / 2;
            // breaking the arrays down into smaller sub arrays
            mergeSort(list, start, mid, t);   // left part of the array, left to mid.
            mergeSort(list, mid + 1, end, t); // right part of the array, mid+1 to right.
            // and then merge
            merge(list, start, mid, end);
        }
    }
}

// wrapper function to decide which to use
void sort(location *list, int start, int end, int t)
{
    if (end - start <= 1)
        printf("\n\t - Array is already sorted.");
    else
        mergeSort(list, start, end, t);
}

// recursive binary search
int binSearch(location *list, location find, int start, int end)
{
    int mid;
    if (start <= end)
    {
        mid = (start + end) / 2;
        if (compareTo(&find, &list[mid]) == 0)
            return mid;
        else if (compareTo(&find, &list[mid]) == -1)
            return binSearch(list, find, start, mid - 1);
        else if (compareTo(&find, &list[mid]) == 1)
            return binSearch(list, find, mid + 1, end);
    }
    return NOT_FOUND;
}

// driver function
int main()
{
    atexit(report_mem_leak);
    // initalizing local variables
    int n, s, t;
    location *monLocations = readData(&n, &s, &t);
    location *search = findValues(s);
    int i;
    sort(monLocations, 0, n - 1, t);

    monsterFile = fopen("out.txt", "w");
    // after sort
    for (int l = 0; l < n; l++)
    {
        printf("%d %d\n", monLocations[l].x, monLocations[l].y);
        fprintf(monsterFile, "%d %d\n", monLocations[l].x, monLocations[l].y);
    }

    // after we conduct binary search
    if (s > 0)
    {
        for (int j = 0; j < s; j++)
        {

            int idx = binSearch(monLocations, search[j], 0, n - 1);
            if (idx != NOT_FOUND)
            {
                printf("%d %d found at position %d\n", search[j].x, search[j].y, idx + 1);
                fprintf(monsterFile, "%d %d found at position %d\n", search[j].x, search[j].y, idx + 1);
            }
            else
            {
                printf("%d %d not found\n", search[j].x, search[j].y);
                fprintf(monsterFile, "%d %d not found\n", search[j].x, search[j].y);
            }
        }
    }

    // closing the file
    fclose(monsterFile);
    // freeing the struct arrays I created
    free(monLocations);
    free(search);

    return 0;
}
