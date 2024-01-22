/*          COP 3502C Assignment 1
This program is written by: Clarence Gomez */

// header files
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "main.h"
#include "leak_detector_c.h"

// constants
#define POP_MAX 1000000
#define NAME_MAX 51
#define trainerVisit trainerList[tc].visits
#define regionMonster regionList[rc]->monsters

int main()
{
    atexit(report_mem_leak);
    int mcount, rcount, tcount;

    monster **Monsters = readMonsters(&mcount);
    region **Regions = readRegions(&rcount, Monsters, mcount);
    trainer *Trainers = loadTrainers(&tcount, Regions, rcount);
    processInputs(Monsters, mcount, Regions, rcount, Trainers, tcount);
    releaseMemory(Monsters, mcount, Regions, rcount, Trainers, tcount);

    return 0;
}

// function to return a dynamically allocated monster filled with the provided parameters
monster *makeMonster(char *name, char *element, int population)
{
    // initalize to NULL first
    monster *monsterInfo = NULL;

    // dynamically allocating memory
    monsterInfo = (monster *)malloc(sizeof(monster));
    monsterInfo->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    monsterInfo->element = (char *)malloc((strlen(name) + 1) * sizeof(char));

    strcpy(monsterInfo->name, name);
    strcpy(monsterInfo->element, element);
    monsterInfo->population = population;

    return monsterInfo;
}

// function to return a returns an array of monster pointers where each monster pointer points to the dynamically allocated monsters
monster **readMonsters(int *monsterCount)
{
    int mc, monPop;
    char monName[NAME_MAX], monElement[NAME_MAX];

    scanf("%d", monsterCount);
    // dynamically allocating memory
    monster **monsterList = (monster **)malloc(*monsterCount * sizeof(monster *));

    // fill the array with monsters using makeMonster function
    for (mc = 0; mc < *monsterCount; mc++)
    {

        scanf("%s %s %d", monName, monElement, &monPop);

        // checking if population entered is valid
        while (monPop > POP_MAX)
        {
            if (monPop <= POP_MAX)
                break;

            scanf("%d", &monPop);
        }

        monsterList[mc] = makeMonster(monName, monElement, monPop);
    }

    return monsterList;
}

// function to returns an array of region pointers where each region pointer points to a dynamically allocated region
region **readRegions(int *countRegions, monster **monsterList, int monsterCount)
{
    int rc, rmcount, mc, mi, totalPop;
    char c, rName[NAME_MAX], mName[NAME_MAX];

    scanf("%d", countRegions);

    // dynamically allocating memory for the region pointer array
    region **regionList = (region **)malloc(*countRegions * sizeof(region *));

    for (rc = 0; rc < *countRegions; rc++)
    {
        totalPop = 0;

        // input for region name
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        scanf("%s", rName);

        // allocating memory for each region struct & region name & monster list
        regionList[rc] = (region *)malloc(sizeof(region));
        regionList[rc]->name = (char *)malloc((strlen(rName) + 1) * sizeof(char));
        regionList[rc]->monsters = (monster **)malloc(monsterCount * sizeof(monster *));

        // getting user info for region name & monster count
        strcpy(regionList[rc]->name, rName);
        scanf("%d", &rmcount);
        regionList[rc]->monster_cnt = rmcount;

        // inputting regional monster information
        for (mc = 0; mc < rmcount; mc++)
        {
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            scanf("%s", mName);

            // loop to go through all the monsters in monsterList to find monster to point to
            for (mi = 0; mi < monsterCount; mi++)
            {
                int sameMon = strcmp(mName, monsterList[mi]->name);
                if (sameMon == 0)
                {
                    regionMonster[mc] = makeMonster(monsterList[mi]->name, monsterList[mi]->element, monsterList[mi]->population);
                    totalPop += regionMonster[mc]->population;
                }
            }
        }
        regionList[rc]->total_population = totalPop;
    }

    return regionList;
}
// This function returns a dynamically allocated array of trainers, filled up with the information from the inputs
trainer *loadTrainers(int *trainerCount, region **regionList, int countRegions)
{
    int tc, rc, ri, visits, captures;
    char c, trcount[NAME_MAX], rName[NAME_MAX];

    scanf("%d", trainerCount);
    // dynamically allocating trainer array

    trainer *trainers = NULL;
    trainers = (trainer *)malloc(*trainerCount * sizeof(trainer));

    for (tc = 0; tc < *trainerCount; tc++)
    {
        scanf("%s", trcount);
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        scanf("%d", &captures);
        scanf("%d", &visits);

        trainers[tc].visits = NULL;

        // dynamically allocating memory for trainer name, visits, and itinerary regions
        trainers[tc].name = (char *)malloc((strlen(trcount) + 1) * sizeof(char));
        trainers[tc].visits = (itinerary *)malloc(sizeof(itinerary));
        trainers[tc].visits->regions = (region **)malloc(visits * sizeof(region *));

        // putting input data
        strcpy(trainers[tc].name, trcount);
        trainers[tc].visits->captures = captures;
        trainers[tc].visits->region_cnt = visits;

        // filling the trainer's visited region's info
        for (rc = 0; rc < visits; rc++)
        {
            scanf("%s", rName);
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            // loop to find the region
            for (ri = 0; ri < countRegions; ri++)
            {
                int regionCmp = strcmp(rName, regionList[ri]->name);

                if (regionCmp == 0)
                {
                    trainers[tc].visits->regions[rc] = (region *)malloc(sizeof(region));
                    trainers[tc].visits->regions[rc]->name = (char *)malloc((strlen(rName) + 1) * sizeof(char));

                    trainers[tc].visits->regions[rc]->monster_cnt = regionList[ri]->monster_cnt;
                    strcpy(trainers[tc].visits->regions[rc]->name, regionList[ri]->name);
                    trainers[tc].visits->regions[rc]->monsters = regionList[ri]->monsters;
                    trainers[tc].visits->regions[rc]->total_population = regionList[ri]->total_population;
                }
            }
        }
    }
    return trainers;
}

// function to calculate a trainer's monster captures
void processInputs(monster **monsterList, int monsterCount, region **regionList,
                   int regionCount, trainer *trainerList, int trainerCount)
{
    int mc, rc, tc;
    float raw, divided, total, coefficient;

    // first go througheach trainer
    for (tc = 0; tc < trainerCount; tc++)
    {
        printf("%s\n", trainerList[tc].name);
        // to multiply by
        coefficient = trainerVisit->captures;
        // now to find the commonality population
        for (rc = 0; rc < trainerVisit->region_cnt; rc++)
        {
            printf("%s\n", trainerVisit->regions[rc]->name);
            // in the trainers specified region, finding all the region's specific monster count
            divided = trainerVisit->regions[rc]->total_population;

            // find each monster population
            for (mc = 0; mc < trainerVisit->regions[rc]->monster_cnt; mc++)
            {
                raw = trainerVisit->regions[rc]->monsters[mc]->population;
                total = round((raw / divided) * coefficient);

                if (total != 0)
                {
                    printf("%.0f-%s\n", total, trainerVisit->regions[rc]->monsters[mc]->name);
                }
            }
        }
        if (tc != (trainerCount - 1))
            printf("\n");
    }
}

void releaseMemory(monster **monsterList, int monsterCount, region **regionList,
                   int regionCount, trainer *trainerList, int trainerCount)
{
    int mc, rc, tc;

    // freeing makeMonster/monsterList functions
    for (mc = 0; mc < monsterCount; mc++)
    {
        free(monsterList[mc]->name);
        free(monsterList[mc]->element);
        free(monsterList[mc]);
        monsterList[mc] = NULL;
    }

    // now free regions/regionList
    for (rc = 0; rc < regionCount; rc++)
    {
        for (mc = 0; mc < regionList[rc]->monster_cnt; mc++)
        {
            free(regionList[rc]->monsters[mc]->name);
            free(regionList[rc]->monsters[mc]->element);
            free(regionList[rc]->monsters[mc]);
        }
        free(regionList[rc]->name);
        free(regionList[rc]->monsters);
        free(regionList[rc]);
        regionList[rc] = NULL;
    }

    // free trainers/trainerList
    for (tc = 0; tc < trainerCount; tc++)
    {

        for (rc = 0; rc < trainerVisit->region_cnt; rc++)
        {
            free(trainerVisit->regions[rc]->name);
            free(trainerVisit->regions[rc]);
        }
        free(trainerList[tc].name);
        free(trainerVisit->regions);
        free(trainerVisit);
        trainerVisit = NULL;
    }

    free(monsterList);
    free(regionList);
    free(trainerList);
}
