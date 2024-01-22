typedef struct monster
{
    char *name;
    char *element;
    int population;
} monster;

typedef struct region
{
    char *name;
    int monster_cnt;
    int total_population;
    monster **monsters;
} region;

typedef struct itinerary
{
    int region_cnt;
    region **regions;
    int captures;
} itinerary;

typedef struct trainer
{
    char *name;
    itinerary *visits;
} trainer;

// function prototypes
monster *makeMonster(char *name, char *element, int population);
monster **readMonsters(int *monsterCount);
region **readRegions(int *countRegions, monster **monsterList, int monsterCount);
trainer *loadTrainers(int *trainerCount, region **regionList, int countRegions);

void processInputs(monster **monsterList, int monsterCount, region **regionList,
                   int regionCount, trainer *trainerList, int trainerCount);

void releaseMemory(monster **monsterList, int monsterCount, region **regionList,
                   int regionCount, trainer *trainerList, int trainerCount);
