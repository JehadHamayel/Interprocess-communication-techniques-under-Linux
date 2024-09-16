//Musab Masalmah 1200078
//Jehad Hamayel 1200348
//Abdalkarim Eiss 1200015


#include "local.h"

// Containers Struct for carry the information of the containers
struct container
{
    int StartTimeOfDrop;
    float capacity;
    bool isBoomed;
    int area;
    bool sentToArea;
};

// struct for message of Message Queue
struct MESSAGEcontainer
{
    long msg_to; /*Type very important */ /* Placed in the queue for */
    struct container msg_container;
};
// Planes Struct For Carry the information of the planes
struct Planes
{
    struct container *containers;
    int timeOfCrash;
};

// Families Struct for carry the information of the families
struct families
{
    float starvationRate;
    int numberOfFlourBags;
    bool isMartyred;
    bool thereIsWorkerInTheWay;
};

enum
{
    collecting,
    distributing,
    splitting
};

// Workers Struct for carry the information of the workers
struct workers
{
    int numberOfFlourBags;
    bool isMartyred;
    double energy;
};

// Relief Committee Struct for carry the information of the cholliction committees
struct reliefCommittee
{
    struct container container;
    bool busy;
    bool isMartyred;
    int timeToFinish;
    int startTime;
    bool isSentToArea;
    struct workers *workers;
    int numberOfWorkers;
    int numberOfWorkersMartyred;
    double energy;
};

// Distribution Committee Struct for carry the information of the distribution committees
struct distributionCommittee
{
    bool busy;
    int timeToFinish;
    int indexOfMaxStarvationRateCamp;
    int indexOfMaxStarvationRateFamily;
    struct workers workers;
    int startTime;
    bool isSentToArea;
};

// Structs For connection to the opengl
struct ToOpenJl
{
    float Families_Starvation_Rate[4][4];
    int Fly_Plane_Flag[10];
    int store1;
    int store2;
    int timeToStop;
};
struct ContainerToOpenJl
{
    int Container_Move_Flag[10][10];
    int Boombed_Container_Move_Flag[10];
    int Collecting_Move_Flag[10][5];
    int Distibuter_Move_Flag[4][10];
    int Boombed_plan_Move_Flag[10];
    int Receved_Boombed_plan_Move_Flag[10];
};


// node struct for queue
struct node
{
    struct container data;
    struct node *next;
};

// queue struct
struct queue
{
    struct node *front;
    struct node *rear;
};
void enQueue(struct queue *q, struct container data);
struct container deQueue(struct queue *q);
struct container frontOfQueue(struct queue *q);
void initQueue(struct queue *q);
int isEmpty(struct queue *q);
// Global Variables for the containers in where the containers are landed
int fromArea[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
typedef void (*sighandler_t)(int);
sighandler_t sigset(int sig, sighandler_t disp);
union semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

// Structs for the shared memory in the collecting and splitting workers
struct kilosFromContainer
{
    int kilos;
};

// Structs for the shared memory in the splitting and distributing workers
struct kilosFromSplitting
{
    int kilos;
};

// Global Variables for the shared memory
int Shmid;
int ContainerShmid;

/////////////////////////////////////////////////
__pid_t *ElementsPids;
int totalElementsNum;
bool exitFlag = false;
/////////////////////////////////////////////////

int main()
{
    
    // number of the martyred workers
    numberOfCollctingWorkersMartyred = 0;
    numberOfDistributingWorkersMartyred = 0;
    numberOfFamiliesPassedAway = 0;

    // Shared Memory for the opengl
    struct ToOpenJl *Info;
    if ((Shmid = shmget(123123, sizeof(struct ToOpenJl), IPC_CREAT | 0666)) < 0)
    {
        printf("in code\n");
        perror("shmget fail");
        exit(1);
    }

    // Shared Memory for the containersinformation in the opengl
    struct ContainerToOpenJl *ContainerInfo;
    if ((ContainerShmid = shmget(99999, sizeof(struct ContainerToOpenJl), IPC_CREAT | 0666)) < 0)
    {
        printf("in container code\n");
        perror("shmget fail");
        exit(1);
    }

    // malloc for the number of the collecting and distributing workers
    numberOfCollictingWorkers = (int *)malloc(sizeof(int));
    numberofDistributingWorkers = (int *)malloc(sizeof(int));

    // semaphores for the shared memory
    static struct sembuf acquire = {0, -1, SEM_UNDO}, // atomic transaction
        release = {0, 1, SEM_UNDO};
    // start values for the semaphores
    static ushort start_val[2] = {1, 0};
    // variables for the semaphores
    int semid1, semid2, semid3;
    union semun arg;
    static ushort vals[2];
    enum
    {
        PRODUCER,
        CONSUMER
    };
    enum
    {
        PRODUCER1,
        CONSUMER1,
        PRODUCER2
    };

    key_t key;
    char u_char = 'a';
    // create 10 message queues
    
    int messageQueuePid[10] = {0};
    for (int i = 0; i < 10; i++)
    {
        key = ftok(".", ++u_char);
        messageQueuePid[i] = msgget(key, IPC_CREAT | 0660);
        if (messageQueuePid[i] == -1)
        {
            perror("msgget failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 10; i++)
    {
        messageQueuePid[i] = msgctl(messageQueuePid[i], IPC_RMID, NULL);
    }

    u_char = 'a';
    for (int i = 0; i < 10; i++)
    {
        key = ftok(".", ++u_char);
        messageQueuePid[i] = msgget(key, IPC_CREAT | 0660);
        if (messageQueuePid[i] == -1)
        {
            perror("msgget failed");
            exit(EXIT_FAILURE);
        }
    }
    // shared memory
    int shmidForDistribution, shmidForSplitters, shmidSplittingWorkers, shmidForChrasheContainers, shmidForChrashePlanes;
    // create shared memory for the collecting and splitting workers and Chrashe Containers and Chrashe Planes
    key = ftok(".", ++u_char);
    if ((shmidForSplitters = shmget(key, sizeof(struct kilosFromContainer), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget fail");
        exit(1);
    }
    key = ftok(".", ++u_char);
    if ((shmidForDistribution = shmget(key, sizeof(struct kilosFromSplitting), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget fail");
        exit(1);
    }
    key = ftok(".", ++u_char);
    if ((shmidForChrasheContainers = shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget fail");
        exit(1);
    }
    key = ftok(".", ++u_char);
    if ((shmidForChrashePlanes = shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget fail");
        exit(1);
    }
    key = ftok(".", ++u_char);
    if ((shmidSplittingWorkers = shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget fail");
        exit(1);
    }
    // semaphores for the shared memory
    key = ftok(".", ++u_char);
    if ((semid1 = semget(key, 2, IPC_CREAT | 0660)) != -1)
    {
        arg.array = start_val;

        if (semctl(semid1, 0, SETALL, arg) == -1)
        {
            perror("semctl -- producer -- initialization");
            exit(1);
        }
    }
    key = ftok(".", ++u_char);
    if ((semid2 = semget(key, 2, IPC_CREAT | 0660)) != -1)
    {

        arg.array = start_val;

        if (semctl(semid2, 0, SETALL, arg) == -1)
        {
            perror("semctl -- producer -- initialization");
            exit(1);
        }
    }
    key = ftok(".", ++u_char);
    if ((semid3 = semget(key, 2, IPC_CREAT | 0660)) != -1)
    {

        arg.array = start_val;

        if (semctl(semid3, 0, SETALL, arg) == -1)
        {
            perror("semctl -- producer -- initialization");
            exit(1);
        }
    }
    // check if the semaphores were created successfully
    printf("semid1 %d\n", semid1);
    printf("semid2 %d\n", semid2);
    printf("semid3 %d\n", semid3);

    // Open the file for reading
    FILE *file = fopen("settings.txt", "r");

    // Check if the file was opened successfully
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return 1; // Exit with error
    }

    int numberOfCollictingWorkersPerCommittee;
    // Read the values from the file
    fscanf(file, "AppTime: %f\n", &AppTime);
    fscanf(file, "Planes: %d\n", &cargoPlanes);
    fscanf(file, "min number of wheat flour containers: %d\n", &rangNumberOfContainers[0]);
    fscanf(file, "max number of wheat flour containers: %d\n", &rangNumberOfContainers[1]);
    fscanf(file, "Drop Period: %d\n", &DropPeriod);
    fscanf(file, "Re-fill Period: %d\n", &periodOfReFilling);
    fscanf(file, "collecting relief committees: %d\n", &collectingReliefCommittees);
    fscanf(file, "collecting relief worker per committees: %d\n", &numberOfCollictingWorkersPerCommittee);
    fscanf(file, "spliting relief workers: %d\n", &numberOfSplittingWorkers);
    fscanf(file, "number of bags per distribution trip: %d\n", &numberOfKiloesThatWorkersCanCarry);
    fscanf(file, "distributing workers: %d\n", numberofDistributingWorkers);

    int crashedCargoPlanesThreshold;
    int wheatFlourContainersBeenShotThreshold;
    int collectingReliefCommitteeWorkersBeenMartyredThreshold;
    int distributingReliefCommitteeWorkersBeenMartyredThreshold;
    int numberOfFamiliesWhoPassedAwayThreshold;
    float familyStarvationRate = 1; // 100% threshold to die
    // Read the values from the file
    fscanf(file, "crashed cargo planes threshold: %d\n", &crashedCargoPlanesThreshold);
    fscanf(file, "wheat flour containers been shot threshold: %d\n", &wheatFlourContainersBeenShotThreshold);
    fscanf(file, "collecting relief committee workers been martyred threshold: %d\n", &collectingReliefCommitteeWorkersBeenMartyredThreshold);
    fscanf(file, "distributing relief committee workers been martyred threshold: %d\n", &distributingReliefCommitteeWorkersBeenMartyredThreshold);
    fscanf(file, "number of families who passed away threshold: %d\n", &numberOfFamiliesWhoPassedAwayThreshold);
    

    *numberOfCollictingWorkers = numberOfCollictingWorkersPerCommittee * collectingReliefCommittees;
    // create the collecting and distributing committees and splitting workers
    struct reliefCommittee collectingCommittees[collectingReliefCommittees];
    for (int i = 0; i < collectingReliefCommittees; i++)
    {
        struct workers workersPerCommittee[numberOfCollictingWorkersPerCommittee];
        for (int j = 0; j < numberOfCollictingWorkersPerCommittee; j++)
        {

            workersPerCommittee[j].numberOfFlourBags = 0;
            workersPerCommittee[j].isMartyred = false;

            workersPerCommittee[j].energy = 100;
        }
        collectingCommittees[i].busy = false;
        collectingCommittees[i].isSentToArea = false;
        collectingCommittees[i].isMartyred = false;
        collectingCommittees[i].numberOfWorkersMartyred = 0;
        collectingCommittees[i].startTime = 0;
        collectingCommittees[i].timeToFinish = 0;
        collectingCommittees[i].container.area = 0;
        collectingCommittees[i].container.capacity = 0;
        collectingCommittees[i].container.isBoomed = false;
        collectingCommittees[i].container.sentToArea = false;
        collectingCommittees[i].workers = workersPerCommittee;
        collectingCommittees[i].numberOfWorkers = numberOfCollictingWorkersPerCommittee;
        collectingCommittees[i].energy = 100;
    }
    struct workers splittingWorkers[numberOfSplittingWorkers];
    for (int i = 0; i < numberOfSplittingWorkers; i++)
    {

        splittingWorkers[i].numberOfFlourBags = 0;
        splittingWorkers[i].isMartyred = false;
        splittingWorkers[i].energy = 100;
    }
    struct distributionCommittee distributingCommittees[*numberofDistributingWorkers];
    for (int i = 0; i < *numberofDistributingWorkers; i++)
    {
        distributingCommittees[i].busy = false;
        distributingCommittees[i].timeToFinish = 0;

        distributingCommittees[i].workers.numberOfFlourBags = 0;
        distributingCommittees[i].workers.isMartyred = false;
        distributingCommittees[i].workers.energy = 100;
        distributingCommittees[i].indexOfMaxStarvationRateCamp = -1;
        distributingCommittees[i].indexOfMaxStarvationRateFamily = -1;
        distributingCommittees[i].startTime = 0;
        distributingCommittees[i].isSentToArea = false;
    }

    // initialize the collecting and distributing committees workers numbers
    int numberOfCollectingCommitteeAvailable = collectingReliefCommittees;
    int numberOfDistributingCommitteeAvailable = *numberofDistributingWorkers;
    /////////////////////////////////////////////////
    totalElementsNum = cargoPlanes + 3; // Planes + collectingReliefCommittees + distributingReliefCommittees + splittingReliefCommittees
    __pid_t elementsPids[totalElementsNum];
    /////////////////////////////////////////////////
    __pid_t ppid = getpid();

    int randomNumberOfContainers, re_fillingTime, randomDropPeriod;
    // pointer to the shared memory 
    struct kilosFromContainer *kilosFromContainer;
    struct kilosFromSplitting *kilosFromSplitting;
    int *NumberOfSplittingWorkers;
    int *chrashedContainers;
    int *chrashedPlanes;

    int numberKeloesFromContainer = 0;

    // initialize the value in the  shared memories
    if ((kilosFromContainer = (struct kilosFromContainer *)shmat(shmidForSplitters, 0, 0)) == (struct kilosFromContainer *)-1)
    {
        perror("shmat: parent");
        exit(2);
    }
    kilosFromContainer[0].kilos = 0;
    shmdt(kilosFromContainer);

    if ((kilosFromSplitting = (struct kilosFromSplitting *)shmat(shmidForDistribution, 0, 0)) == (struct kilosFromSplitting *)-1)
    {
        perror("shmat: parent");
        exit(2);
    }

    kilosFromSplitting[0].kilos = 0;

    shmdt(kilosFromSplitting);

    if ((NumberOfSplittingWorkers = (int *)shmat(shmidSplittingWorkers, 0, 0)) == (int *)-1)
    {
        perror("shmat: parent");
        exit(2);
    }
    NumberOfSplittingWorkers[0] = numberOfSplittingWorkers;
    shmdt(NumberOfSplittingWorkers);

    if ((chrashedContainers = (int *)shmat(shmidForChrasheContainers, 0, 0)) == (int *)-1)
    {
        perror("shmat: parent");
        exit(2);
    }
    chrashedContainers[0] = 0;
    shmdt(chrashedContainers);
    if ((chrashedPlanes = (int *)shmat(shmidForChrashePlanes, 0, 0)) == (int *)-1)
    {
        perror("shmat: parent");
        exit(2);
    }
    chrashedPlanes[0] = 0;
    shmdt(chrashedPlanes);

    // variables for energy and delay of the workers
    double energy = 100;
    double delay;

    // fork for the collecting committee
    elementsPids[0] = fork();
    

    int revecivedMessage;
    int StartSemaphores = 0;

    if (elementsPids[0] == 0)
    {
        // queue for the containers
        struct queue q;
        initQueue(&q);
        // signal handlers for the collecting committees
        sigset(SIGHUP, sigset_catcherFromArea1);
        sigset(SIGQUIT, sigset_catcherFromArea2);
        sigset(SIGILL, sigset_catcherFromArea3);
        sigset(SIGTRAP, sigset_catcherFromArea4);
        sigset(SIGABRT, sigset_catcherFromArea5);
        sigset(SIGBUS, sigset_catcherFromArea6);
        sigset(SIGFPE, sigset_catcherFromArea7);
        sigset(SIGUSR1, sigset_catcherFromArea8);
        sigset(SIGSEGV, sigset_catcherFromArea9);
        sigset(SIGUSR2, sigset_catcherFromArea10);


        int i = 0;
        struct MESSAGEcontainer containerMessage = {0, {0, 0, 0, 0}};
        int thereIsWorkerCollicting = 0;
        srand(getpid());
        int prop;
        // the collecting committee life cycle
        while (1)
        {

            if (i == 10)
            {
                i = 0;
            }
            // check if there is a container in the message queue to start the collecting committee
            if (fromArea[i] >= 1)
            {
                if ((revecivedMessage = msgrcv(messageQueuePid[i], &containerMessage, sizeof(containerMessage), 0, 0)) == -1)
                {
                    perror("Server: msgrcv");
                    return 2;
                }
                enQueue(&q, containerMessage.msg_container);
                fromArea[i]--;
            }
            i++;

            // check if there is a container in the queue to start the collecting committee
            if (!isEmpty(&q))
            {
                struct container container = frontOfQueue(&q);
                // extract the container from the queue when the time of the drop is reached
                if (container.StartTimeOfDrop + 10 <= time(NULL) && numberOfCollectingCommitteeAvailable > 0)
                {
                    for (int j = 0; j < collectingReliefCommittees; j++)
                    {
                        if (collectingCommittees[j].busy == false && collectingCommittees[j].isMartyred == false)
                        {
                            for (int k = 0; k < numberOfCollictingWorkersPerCommittee; k++)
                            {
                                // propability of the workers to be martyred
                                prop = (rand() % 101);
                                if (prop <= 20)
                                {
                                    collectingCommittees[j].numberOfWorkersMartyred++;
                                    numberOfCollctingWorkersMartyred++;
                                    if (numberOfCollctingWorkersMartyred >= collectingReliefCommitteeWorkersBeenMartyredThreshold)
                                    {
                                        kill(getppid(), SIGILL);
                                    }
                                    collectingCommittees[j].isMartyred = true;
                                    collectingCommittees[j].workers[k].isMartyred = true;
                                }
                            }
                            // check if the workers are martyred and put the time of the finish
                            if (collectingCommittees[j].numberOfWorkersMartyred <= numberOfCollictingWorkersPerCommittee && collectingCommittees[j].numberOfWorkersMartyred > 0)
                            {
                                // the workers are martyred and put the time of the finish and the time of the start of the collecting committee
                                printf("Container from Area %d but kill some of the committee's workers in the way they run back\n", container.area);

                                collectingCommittees[j].busy = true;

                                if (container.area == 10)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 2;
                                }
                                else if (container.area == 9)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 3;
                                }
                                else if (container.area == 8)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 4;
                                }
                                else if (container.area == 7)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 5;
                                }
                                else if (container.area == 6)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 6;
                                }
                                else if (container.area == 5)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 7;
                                }
                                else if (container.area == 4)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 8;
                                }
                                else if (container.area == 3)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 9;
                                }
                                else if (container.area == 2)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 10;
                                }
                                else if (container.area == 1)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 10;
                                }
                                // update the energy of the workers
                                double energyOfCommittee = 0;
                                for (int u = 0; u < numberOfCollictingWorkersPerCommittee; u++)
                                {
                                    energyOfCommittee += collectingCommittees[j].workers[u].energy;
                                }
                                energy = energyOfCommittee / numberOfCollictingWorkersPerCommittee;
                                delay = 5 - (energy / 100) * 5;
                                collectingCommittees[j].timeToFinish = collectingCommittees[j].timeToFinish + delay;
                                collectingCommittees[j].startTime = delay;
                                numberOfCollectingCommitteeAvailable--;
                                thereIsWorkerCollicting++;
                                break;
                            }
                            else
                            {
                                // check if the workers are not martyred and put the time of the finish
                                collectingCommittees[j].busy = true;
                                if (container.area == 10)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 2;
                                }
                                else if (container.area == 9)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 3;
                                }
                                else if (container.area == 8)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 4;
                                }
                                else if (container.area == 7)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 5;
                                }
                                else if (container.area == 6)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 6;
                                }
                                else if (container.area == 5)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 7;
                                }
                                else if (container.area == 4)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 8;
                                }
                                else if (container.area == 3)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 9;
                                }
                                else if (container.area == 2)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 10;
                                }
                                else if (container.area == 1)
                                {
                                    collectingCommittees[j].timeToFinish = time(NULL) + 10;
                                }

                                double energyOfCommittee = 0;
                                for (int u = 0; u < numberOfCollictingWorkersPerCommittee; u++)
                                {
                                    energyOfCommittee += collectingCommittees[j].workers[u].energy;
                                }
                                energy = energyOfCommittee / numberOfCollictingWorkersPerCommittee;
                                delay = 5 - (energy / 100) * 5;
                                collectingCommittees[j].timeToFinish = collectingCommittees[j].timeToFinish + delay;
                                collectingCommittees[j].startTime = delay;

                                collectingCommittees[j].container = deQueue(&q);
                                numberOfCollectingCommitteeAvailable--;
                                thereIsWorkerCollicting++;
                                break;
                            }
                        }
                    }
                }
            }
            // finish the collecting committee and update the shared memory
            if (thereIsWorkerCollicting > 0)
            {
                for (int j = 0; j < collectingReliefCommittees; j++)
                {
                    // start the collecting committee on the way to the area
                    if (collectingCommittees[j].startTime <= time(NULL) && collectingCommittees[j].isSentToArea == false && collectingCommittees[j].busy == true)
                    {
                        // send to the opengl the information of the collecting committee
                        if (collectingCommittees[j].isMartyred == true)
                        {
                            int colNum = (collectingCommittees[j].container.area - 10) * -1;
                            ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid, 0, 0);
                            ContainerInfo[0].Collecting_Move_Flag[colNum][j] = 2;
                        }
                        else
                        {
                            int colNum = (collectingCommittees[j].container.area - 10) * -1;
                            ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid, 0, 0);
                            ContainerInfo[0].Collecting_Move_Flag[colNum][j] = 1;
                        }

                        collectingCommittees[j].isSentToArea = true;
                    }
                    // finish the collecting committee and update the shared memory
                    if (collectingCommittees[j].timeToFinish <= time(NULL) && collectingCommittees[j].busy == true && collectingCommittees[j].isMartyred == false)
                    {
                        numberOfCollectingCommitteeAvailable++;
                        thereIsWorkerCollicting--;
                        collectingCommittees[j].busy = false;
                        collectingCommittees[j].isSentToArea = false;
                        // update the energy of the workers
                        for (int k = 0; k < numberOfCollictingWorkersPerCommittee; k++)
                        {
                            if (collectingCommittees[j].workers[k].energy >= 30)
                            {
                                collectingCommittees[j].workers[k].energy -= 10;
                            }
                        }
                        // update the number of the kiloes from the container to send it to the shared memory
                        numberKeloesFromContainer += collectingCommittees[j].container.capacity;
                        Info = (struct ToOpenJl *)shmat(Shmid, 0, 0);
                        Info[0].store1 = numberKeloesFromContainer;
                    }
                    else if (collectingCommittees[j].timeToFinish <= time(NULL) && collectingCommittees[j].busy == true && collectingCommittees[j].isMartyred == true)
                    {

                        if (numberOfSplittingWorkers > collectingCommittees[j].numberOfWorkersMartyred)
                        {
                            arg.array = vals;
                            if (semctl(semid3, 0, GETALL, arg) == -1)
                            {
                                perror("semctl");
                                exit(1);
                            }
                            // if the collecting committee is martyred and the number of the splitting workers is enough to give workers to the collecting committee
                            if (vals[0] != 0)
                            {
                                acquire.sem_num = PRODUCER;
                                StartSemaphores = time(NULL) + 1;
                                if (semop(semid3, &acquire, 1) == -1)
                                {
                                    perror("semop -- producer -- waiting for consumer to read number");
                                    exit(3);
                                }
                                if ((NumberOfSplittingWorkers = (int *)shmat(shmidSplittingWorkers, 0, 0)) == (int *)-1)
                                {
                                    perror("shmat: parent");
                                    exit(2);
                                }

                                NumberOfSplittingWorkers[0] = NumberOfSplittingWorkers[0] - collectingCommittees[j].numberOfWorkersMartyred;
                                numberOfSplittingWorkers = NumberOfSplittingWorkers[0];
                                collectingCommittees[j].isMartyred = false;
                                for (int k = 0; k < collectingCommittees[j].numberOfWorkersMartyred; k++)
                                {
                                    collectingCommittees[j].workers[k].isMartyred = false;

                                    collectingCommittees[j].workers[k].energy = 100;
                                }

                                collectingCommittees[j].numberOfWorkersMartyred = 0;
                                numberOfCollectingCommitteeAvailable++;
                                thereIsWorkerCollicting--;
                                collectingCommittees[j].busy = false;
                                collectingCommittees[j].isSentToArea = false;
                                shmdt(NumberOfSplittingWorkers);

                                release.sem_num = CONSUMER;
                                if (semop(semid3, &release, 1) == -1)
                                {
                                    perror("semop -- consumer -- indicating number has been read");
                                    exit(9);
                                }
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    }
                }
            }
            // write the number of the kiloes from the container to the shared memory to split it
            arg.array = vals;
            if (semctl(semid1, 0, GETALL, arg) == -1)
            {
                perror("semctl");
                exit(1);
            }
            if (vals[0] != 0 && numberKeloesFromContainer > 0)
            {
                printf("Collecting Workers write the number of kiloes from containers to the sherad memory at %ld\n", time(NULL));
                acquire.sem_num = PRODUCER;
                if (semop(semid1, &acquire, 1) == -1)
                {
                    perror("semop -- producer -- waiting for consumer to read number");
                    exit(3);
                }
                if ((kilosFromContainer = (struct kilosFromContainer *)shmat(shmidForSplitters, 0, 0)) == (struct kilosFromContainer *)-1)
                {
                    perror("shmat: parent");
                    exit(2);
                }

                kilosFromContainer[0].kilos = numberKeloesFromContainer;

                numberKeloesFromContainer = 0;

                shmdt(kilosFromContainer);
                release.sem_num = CONSUMER;

                if (semop(semid1, &release, 1) == -1)
                {
                    perror("semop -- consumer -- indicating number has been read");
                    exit(9);
                }
            }
            // give the semaphores to the distributing workers if worker is martyred and the number of the splitting workers is enough to give workers to the distributing workers
            arg.array = vals;
            if (semctl(semid3, 0, GETALL, arg) == -1)
            {
                perror("semctl");
                exit(1);
            }
            if (vals[0] != 0 && time(NULL) >= StartSemaphores)
            {
                StartSemaphores = time(NULL) + 1;
                acquire.sem_num = PRODUCER;
                if (semop(semid3, &acquire, 1) == -1)
                {
                    perror("semop -- producer -- waiting for consumer to read number");
                    exit(3);
                }
                release.sem_num = CONSUMER;
                if (semop(semid3, &release, 1) == -1)
                {
                    perror("semop -- consumer -- indicating number has been read");
                    exit(9);
                }
            }
        }
    }
    // variables for the splitting workers
    int timeOfSplitting;
    bool startSplitting = false;
    int storage = 0;
    // Commitee Splitting
    numberKeloesFromContainer = 0;
    // fork for the splitting workers
    elementsPids[1] = fork();
    if (elementsPids[1] == 0)
    {
        srand(getpid());
        // the loop for the splitting workers life cycle
        while (1)
        {
            // check if the number of the kiloes from the container is enough to start the splitting workers
            if ((NumberOfSplittingWorkers = (int *)shmat(shmidSplittingWorkers, 0, 0)) == (int *)-1)
            {
                perror("shmat: parent");
                exit(2);
            }
            numberOfSplittingWorkers = NumberOfSplittingWorkers[0];
            shmdt(NumberOfSplittingWorkers);

            arg.array = vals;
            if (semctl(semid1, 0, GETALL, arg) == -1)
            {
                perror("semctl");
                exit(1);
            }
            if (vals[1] != 0)
            {
                startSplitting = true;
                timeOfSplitting = time(NULL) + 3;

                acquire.sem_num = CONSUMER;
                if (semop(semid1, &acquire, 1) == -1)
                {
                    perror("semop -- producer -- waiting for consumer to read number");
                    exit(3);
                }
                if ((kilosFromContainer = (struct kilosFromContainer *)shmat(shmidForSplitters, 0, 0)) == (struct kilosFromContainer *)-1)
                {
                    perror("shmat: parent");
                    exit(2);
                }

                numberKeloesFromContainer += kilosFromContainer[0].kilos;

                kilosFromContainer[0].kilos = 0;
                shmdt(kilosFromContainer);
                release.sem_num = PRODUCER;

                if (semop(semid1, &release, 1) == -1)
                {
                    perror("semop -- consumer -- indicating number has been read");
                    exit(9);
                }
            }
            // start the splitting workers process
            if (numberKeloesFromContainer > 0 && numberOfSplittingWorkers > 0 && timeOfSplitting <= time(NULL) && startSplitting == true)
            {
                if (numberKeloesFromContainer >= (numberOfSplittingWorkers * 10))
                {
                    numberKeloesFromContainer = numberKeloesFromContainer - numberOfSplittingWorkers * 10;
                    storage += numberOfSplittingWorkers * 10;
                }
                else
                {
                    storage += numberKeloesFromContainer;
                    numberKeloesFromContainer = 0;
                    startSplitting = false;
                }
                timeOfSplitting = time(NULL) + 1;
            }

            arg.array = vals;
            if (semctl(semid2, 0, GETALL, arg) == -1)
            {
                perror("semctl");
                exit(1);
            }
            // check if the number of the kiloes from the container is enough to start the distributing workers
            if (vals[0] != 0 && storage > 0)
            {
                acquire.sem_num = PRODUCER;
                if (semop(semid2, &acquire, 1) == -1)
                {
                    perror("semop -- producer -- waiting for consumer to read number");
                    exit(3);
                }
                if ((kilosFromSplitting = (struct kilosFromSplitting *)shmat(shmidForDistribution, 0, 0)) == (struct kilosFromSplitting *)-1)
                {
                    perror("shmat: parent");
                    exit(2);
                }
                Info = (struct ToOpenJl *)shmat(Shmid, 0, 0);
                Info[0].store1 = 0;
                kilosFromSplitting[0].kilos = kilosFromSplitting[0].kilos + storage;
                Info[0].store2 = kilosFromSplitting[0].kilos;
                storage = 0;
                shmdt(kilosFromSplitting);
                release.sem_num = CONSUMER;

                if (semop(semid2, &release, 1) == -1)
                {
                    perror("semop -- consumer -- indicating number has been read");
                    exit(9);
                }
            }
        }
    }
    // variables for the distributing workers and the families
    int numOfCampFamilies = 4;
    int numOfFamiliesInCamp = 4;
    // initialize the families
    struct families families[numOfCampFamilies][numOfFamiliesInCamp];
    for (int i = 0; i < numOfCampFamilies; i++)
    {
        for (int j = 0; j < numOfFamiliesInCamp; j++)
        {
            families[i][j].starvationRate = 0;
            families[i][j].numberOfFlourBags = 0;
            families[i][j].isMartyred = false;
            families[i][j].thereIsWorkerInTheWay = false;
        }
    }
    // 
    int startTime = time(NULL) + 6;
    int timeOfDistributing;
    int thereIsWorkerDistributing = 0;
    float maxStarvationRate = 0;
    int indexOfMaxStarvationRate[2] = {0, 0};
    numberKeloesFromContainer = 0;
    // fork for the distributing workers
    int prop;
    elementsPids[2] = fork();

    if (elementsPids[2] == 0)
    {
        srand(getpid());
        // the loop for the distributing workers life cycle
        while (1)
        {
            // check if the number of the kiloes from the container is enough to start the distributing workers
            arg.array = vals;
            if (semctl(semid2, 0, GETALL, arg) == -1)
            {
                perror("semctl");
                exit(1);
            }
            if (vals[1] != 0)
            {
                acquire.sem_num = CONSUMER;
                if (semop(semid2, &acquire, 1) == -1)
                {
                    perror("semop -- producer -- waiting for consumer to read number");
                    exit(3);
                }
                if ((kilosFromSplitting = (struct kilosFromSplitting *)shmat(shmidForDistribution, 0, 0)) == (struct kilosFromSplitting *)-1)
                {
                    perror("shmat: parent");
                    exit(2);
                }

                numberKeloesFromContainer += kilosFromSplitting[0].kilos;

                kilosFromSplitting[0].kilos = 0;
                shmdt(kilosFromSplitting);
                release.sem_num = PRODUCER;

                if (semop(semid2, &release, 1) == -1)
                {
                    perror("semop -- consumer -- indicating number has been read");
                    exit(9);
                }
            }
            // update the starvation rate of the families 
            if (time(NULL) >= startTime)
            {

                for (int i = 0; i < numOfCampFamilies; i++)
                {
                    for (int j = 0; j < numOfFamiliesInCamp; j++)
                    {

                        if (families[i][j].starvationRate < familyStarvationRate && families[i][j].isMartyred == false)
                        {
                            if (families[i][j].numberOfFlourBags > 0 && families[i][j].starvationRate >= (float)0.02)
                            {

                                families[i][j].starvationRate -= (float)0.02;
                                families[i][j].numberOfFlourBags--;
                            }
                            else if (families[i][j].numberOfFlourBags == 0)
                            {

                                families[i][j].starvationRate += (float)0.02;
                            }
                        }
                        else if (families[i][j].starvationRate >= familyStarvationRate && families[i][j].isMartyred == false)
                        {

                            families[i][j].isMartyred = true;
                            numberOfFamiliesPassedAway++;
                            printf("Family %d at Camp %d passed away\n", j, i);
                            if (numberOfFamiliesPassedAway >= numberOfFamiliesWhoPassedAwayThreshold)
                            {
                                kill(getppid(), SIGABRT);
                            }

                            printf("Family %d at Camp %d passed away\n", j, i);
                        }
                        if (families[i][j].starvationRate <= (float)1.0)
                        {
                            Info = (struct ToOpenJl *)shmat(Shmid, 0, 0);
                            Info[0].Families_Starvation_Rate[i][j] = families[i][j].starvationRate;
                        }
                    }
                }
                startTime = time(NULL) + 2;
            }
            // check if the number of the kiloes from the container is enough to start the distributing workers 
            // and the number of the distributing workers is enough to start the distributing workers
            // and get the maximum starvation rate of the families
            if (numberKeloesFromContainer > 0 && numberOfDistributingCommitteeAvailable > 0)
            {

                for (int j = 0; j < *numberofDistributingWorkers; j++)
                {
                    if (distributingCommittees[j].busy == false && distributingCommittees[j].workers.isMartyred == false)
                    {
                        distributingCommittees[j].busy = true;
                        maxStarvationRate = 0;

                        indexOfMaxStarvationRate[0] = 0;
                        indexOfMaxStarvationRate[1] = 0;

                        for (int k = 0; k < numOfCampFamilies; k++)
                        {
                            for (int l = 0; l < numOfFamiliesInCamp; l++)
                            {

                                if (families[k][l].starvationRate >= maxStarvationRate && families[k][l].isMartyred == false && families[k][l].thereIsWorkerInTheWay == false)
                                {
                                    maxStarvationRate = families[k][l].starvationRate;
                                    indexOfMaxStarvationRate[0] = k;
                                    indexOfMaxStarvationRate[1] = l;
                                }
                            }
                        }
                        distributingCommittees[j].indexOfMaxStarvationRateCamp = indexOfMaxStarvationRate[0];
                        distributingCommittees[j].indexOfMaxStarvationRateFamily = indexOfMaxStarvationRate[1];
                        if (indexOfMaxStarvationRate[0] == 1)
                        {
                            distributingCommittees[j].timeToFinish = time(NULL) + 4;
                        }
                        else if (indexOfMaxStarvationRate[0] == 2)
                        {
                            distributingCommittees[j].timeToFinish = time(NULL) + 7;
                        }
                        else if (indexOfMaxStarvationRate[0] == 3)
                        {
                            distributingCommittees[j].timeToFinish = time(NULL) + 10;
                        }
                        else if (indexOfMaxStarvationRate[0] == 4)
                        {
                            distributingCommittees[j].timeToFinish = time(NULL) + 13;
                        }
                        // 
                        delay = 5 - (energy / 100) * 5;
                        distributingCommittees[j].timeToFinish = distributingCommittees[j].timeToFinish + delay;
                        distributingCommittees[j].startTime = delay;

                        numberOfDistributingCommitteeAvailable--;
                        thereIsWorkerDistributing++;
                        if (numberKeloesFromContainer >= numberOfKiloesThatWorkersCanCarry)
                        {
                            numberKeloesFromContainer = numberKeloesFromContainer - numberOfKiloesThatWorkersCanCarry;
                            distributingCommittees[j].workers.numberOfFlourBags = numberOfKiloesThatWorkersCanCarry;
                        }
                        else
                        {
                            distributingCommittees[j].workers.numberOfFlourBags = numberKeloesFromContainer;
                            numberKeloesFromContainer = 0;
                        }
                        Info = (struct ToOpenJl *)shmat(Shmid, 0, 0);
                        Info[0].store2 = numberKeloesFromContainer;

                        families[distributingCommittees[j].indexOfMaxStarvationRateCamp][distributingCommittees[j].indexOfMaxStarvationRateFamily].thereIsWorkerInTheWay = true;

                        prop = (rand() % 101);
                        if (prop <= 20)
                        {
                            distributingCommittees[j].workers.isMartyred = true;
                            numberOfDistributingWorkersMartyred++;
                            if (numberOfDistributingWorkersMartyred >= distributingReliefCommitteeWorkersBeenMartyredThreshold)
                            {
                                kill(getppid(), SIGTRAP);
                            }
                        }

                        if (distributingCommittees[j].workers.isMartyred == true)
                        {                                                            // kill some of the committee's workers or all of them in the return way
                            distributingCommittees[j].workers.numberOfFlourBags = 0; // the container is not going to be used it missed at the way back
                            printf("Distribution Workers killed in the way To Family Camp\n");
                            break;
                        }
                        else
                        {
                            printf("Sending the Distribution Workers to the Family Camp at %ld\n", time(NULL));
                            break;
                        }
                    }
                }
            }
            if (thereIsWorkerDistributing > 0)
            {
                for (int j = 0; j < *numberofDistributingWorkers; j++)
                {
                    if (distributingCommittees[j].startTime <= time(NULL) && distributingCommittees[j].isSentToArea == false && distributingCommittees[j].busy == true)
                    {
                        if (distributingCommittees[j].workers.isMartyred == true)
                        {
                            ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid, 0, 0);
                            ContainerInfo[0].Distibuter_Move_Flag[distributingCommittees[j].indexOfMaxStarvationRateCamp][j] = 2;
                        }
                        else
                        {
                            ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid, 0, 0);
                            ContainerInfo[0].Distibuter_Move_Flag[distributingCommittees[j].indexOfMaxStarvationRateCamp][j] = 1;
                        }
                        distributingCommittees[j].isSentToArea = true;
                    }
                    if (distributingCommittees[j].timeToFinish <= time(NULL) && distributingCommittees[j].busy == true && distributingCommittees[j].workers.isMartyred == false && distributingCommittees[j].workers.numberOfFlourBags > 0)
                    {
                        numberOfDistributingCommitteeAvailable++;
                        thereIsWorkerDistributing--;
                        distributingCommittees[j].busy = false;
                        distributingCommittees[j].isSentToArea = false;

                        if (distributingCommittees[j].workers.energy >= 30)
                        {
                            distributingCommittees[j].workers.energy -= 10;
                        }

                        printf("iam the worker %d\n", j);
                        printf("the most Family Camp %d with starvation rate %f number of the family %d\n", distributingCommittees[j].indexOfMaxStarvationRateCamp, families[distributingCommittees[j].indexOfMaxStarvationRateCamp][distributingCommittees[j].indexOfMaxStarvationRateFamily].starvationRate, distributingCommittees[j].indexOfMaxStarvationRateFamily);
                        families[distributingCommittees[j].indexOfMaxStarvationRateCamp][distributingCommittees[j].indexOfMaxStarvationRateFamily].numberOfFlourBags += distributingCommittees[j].workers.numberOfFlourBags;

                        families[distributingCommittees[j].indexOfMaxStarvationRateCamp][distributingCommittees[j].indexOfMaxStarvationRateFamily].thereIsWorkerInTheWay = false;
                    }
                    else if (distributingCommittees[j].timeToFinish <= time(NULL) && distributingCommittees[j].busy == true && distributingCommittees[j].workers.isMartyred == true)
                    {

                        if (numberOfSplittingWorkers > 0)
                        {
                            arg.array = vals;
                            if (semctl(semid3, 0, GETALL, arg) == -1)
                            {
                                perror("semctl");
                                exit(1);
                            }
                            if (vals[1] != 0)
                            {
                                printf("Distribution Worker is dead \n");

                                acquire.sem_num = CONSUMER;
                                StartSemaphores = time(NULL) + 1;
                                if (semop(semid3, &acquire, 1) == -1)
                                {
                                    perror("semop -- producer -- waiting for consumer to read number");
                                    exit(3);
                                }
                                if ((NumberOfSplittingWorkers = (int *)shmat(shmidSplittingWorkers, 0, 0)) == (int *)-1)
                                {
                                    perror("shmat: parent");
                                    exit(2);
                                }
                                families[distributingCommittees[j].indexOfMaxStarvationRateCamp][distributingCommittees[j].indexOfMaxStarvationRateFamily].thereIsWorkerInTheWay = false;
                                NumberOfSplittingWorkers[0] = NumberOfSplittingWorkers[0] - 1;

                                numberOfSplittingWorkers = NumberOfSplittingWorkers[0];

                                distributingCommittees[j].workers.isMartyred = false;
                                numberOfDistributingCommitteeAvailable++;
                                thereIsWorkerDistributing--;
                                distributingCommittees[j].busy = false;
                                distributingCommittees[j].isSentToArea = false;
                                distributingCommittees[j].workers.energy = 100;

                                shmdt(NumberOfSplittingWorkers);

                                release.sem_num = PRODUCER;
                                if (semop(semid3, &release, 1) == -1)
                                {
                                    perror("semop -- consumer -- indicating number has been read");
                                    exit(9);
                                }
                            }
                        }
                        else
                        {
                            return 0;
                        }
                    }
                }
            }
            arg.array = vals;
            if (semctl(semid3, 0, GETALL, arg) == -1)
            {
                perror("semctl");
                exit(1);
            }
            if (vals[1] != 0 && time(NULL) >= StartSemaphores)
            {
                StartSemaphores = time(NULL) + 1;
                acquire.sem_num = CONSUMER;
                if (semop(semid3, &acquire, 1) == -1)
                {
                    perror("semop -- producer -- waiting for consumer to read number");
                    exit(3);
                }

                release.sem_num = PRODUCER;
                if (semop(semid3, &release, 1) == -1)
                {
                    perror("semop -- consumer -- indicating number has been read");
                    exit(9);
                }
            }
        }
    }

    sleep(1);
    int returnTime;
    srand(getpid());
    int caseOfCrash;

    if (cargoPlanes <= 7)
    {
        caseOfCrash = 0;
    }
    else if (cargoPlanes == 8)
    {
        prop = rand() % 101;
        if (prop <= 50)
        {
            caseOfCrash = 0;
        }
        else
        {
            caseOfCrash = 1;
        }
    }
    else if (cargoPlanes == 9)
    {
        prop = rand() % 101;
        if (prop <= 25)
        {
            caseOfCrash = 0;
        }
        else if (prop > 25 && prop <= 50)
        {
            caseOfCrash = 1;
        }
        else if (prop > 50 && prop <= 75)
        {
            caseOfCrash = 3;
        }
        else
        {
            caseOfCrash = 5;
        }
    }
    else
    {
        prop = rand() % 101;

        if (prop <= 12.5)
        {
            caseOfCrash = 1;
        }
        else if (prop > 12.5 && prop <= 25)
        {
            caseOfCrash = 2;
        }
        else if (prop > 25 && prop <= 37.5)
        {
            caseOfCrash = 3;
        }
        else if (prop > 37.5 && prop <= 50)
        {
            caseOfCrash = 4;
        }
        else if (prop > 50 && prop <= 62.5)
        {
            caseOfCrash = 5;
        }
        else if (prop > 62.5 && prop <= 75)
        {
            caseOfCrash = 6;
        }
        else if (prop > 75 && prop <= 87.5)
        {
            caseOfCrash = 7;
        }
        else if (prop > 87.5 && prop <= 100)
        {
            caseOfCrash = 8;
        }
    }
    printf("Case of Crash %d\n", caseOfCrash);
    int startPlaneTime;
    for (int i = 3; i < totalElementsNum; i++)
    {

        elementsPids[i] = fork();
        if (elementsPids[i] == 0)
        {
            int planNumber = i - 2;

            if (planNumber == 10)
            {
                re_fillingTime = periodOfReFilling + (i - 3) + 4;
            }
            else if (planNumber == 9)
            {
                re_fillingTime = periodOfReFilling + (i - 3) + 2;
            }
            else
            {
                re_fillingTime = periodOfReFilling + (i - 3);
            }
            // randomNumberOfContainers = (int)(rangNumberOfContainers[0] + (rand() % (rangNumberOfContainers[1] - rangNumberOfContainers[0])));
            randomNumberOfContainers = 1;
            struct Planes plane = {NULL, 0};
            
            // printf("Random re-filling time %d for Plane: %d\n", re_fillingTime, planNumber);
            startTime = time(NULL);
            // printf("startTime Of the plane(%d): %d\n", planNumber, startTime);
            startPlaneTime = startTime + re_fillingTime;
            returnTime = startTime + re_fillingTime + 20;

            caseOfCrash = 1;

            if (caseOfCrash > 1)
            {
                if (caseOfCrash == 2)
                {
                    if (planNumber == 1)
                    {
                        plane.timeOfCrash = startPlaneTime + 13;
                    }
                    else if (planNumber == 8)
                    {
                        plane.timeOfCrash = startPlaneTime + 6;
                    }
                    else
                    {
                        plane.timeOfCrash = -1;
                    }
                }
                else if (caseOfCrash == 3)
                {
                    if (planNumber == 2)
                    {
                        plane.timeOfCrash = startPlaneTime + 15;
                    }
                    else if (planNumber == 9)
                    {
                        plane.timeOfCrash = startPlaneTime + 4;
                    }
                    else
                    {
                        plane.timeOfCrash = -1;
                    }
                }
                else if (caseOfCrash == 4)
                {
                    if (planNumber == 3)
                    {
                        plane.timeOfCrash = startPlaneTime + 17;
                    }
                    else if (planNumber == 10)
                    {
                        plane.timeOfCrash = startPlaneTime + 2;
                    }
                    else
                    {
                        plane.timeOfCrash = -1;
                    }
                }
                else if (caseOfCrash == 5)
                {
                    if (planNumber == 1)
                    {
                        plane.timeOfCrash = startPlaneTime + 13;
                    }
                    else if (planNumber == 8)
                    {
                        plane.timeOfCrash = startPlaneTime + 6;
                    }
                    else if (planNumber == 2)
                    {
                        plane.timeOfCrash = startPlaneTime + 15;
                    }
                    else if (planNumber == 9)
                    {
                        plane.timeOfCrash = startPlaneTime + 4;
                    }
                    else
                    {
                        plane.timeOfCrash = -1;
                    }
                }
                else if (caseOfCrash == 6)
                {
                    if (planNumber == 1)
                    {
                        plane.timeOfCrash = startPlaneTime + 13;
                    }
                    else if (planNumber == 8)
                    {
                        plane.timeOfCrash = startPlaneTime + 6;
                    }
                    else if (planNumber == 3)
                    {
                        plane.timeOfCrash = startPlaneTime + 17;
                    }
                    else if (planNumber == 10)
                    {
                        plane.timeOfCrash = startPlaneTime + 2;
                    }
                    else
                    {
                        plane.timeOfCrash = -1;
                    }
                }
                else if (caseOfCrash == 7)
                {
                    if (planNumber == 2)
                    {
                        plane.timeOfCrash = startPlaneTime + 15;
                    }
                    else if (planNumber == 9)
                    {
                        plane.timeOfCrash = startPlaneTime + 4;
                    }
                    else if (planNumber == 3)
                    {
                        plane.timeOfCrash = startPlaneTime + 17;
                    }
                    else if (planNumber == 10)
                    {
                        plane.timeOfCrash = startPlaneTime + 2;
                    }
                    else
                    {
                        plane.timeOfCrash = -1;
                    }
                }
                else if (caseOfCrash == 8)
                {
                    if (planNumber == 1)
                    {
                        plane.timeOfCrash = startPlaneTime + 13;
                    }
                    else if (planNumber == 8)
                    {
                        plane.timeOfCrash = startPlaneTime + 6;
                    }
                    else if (planNumber == 2)
                    {
                        plane.timeOfCrash = startPlaneTime + 15;
                    }
                    else if (planNumber == 9)
                    {
                        plane.timeOfCrash = startPlaneTime + 4;
                    }
                    else if (planNumber == 3)
                    {
                        plane.timeOfCrash = startPlaneTime + 17;
                    }
                    else if (planNumber == 10)
                    {
                        plane.timeOfCrash = startPlaneTime + 2;
                    }
                    else
                    {
                        plane.timeOfCrash = -1;
                    }
                }
            }
            else
            {
                plane.timeOfCrash = -1;
            }
            printf("Time of crash for Plane(%d): %d\n", planNumber, plane.timeOfCrash);

            srand(getpid());
            while (1)
            {

                plane.containers = malloc(randomNumberOfContainers * sizeof(struct container));
                for (int j = 0; j < randomNumberOfContainers; j++)
                {

                    randomDropPeriod = (int)(1 + (rand() % (DropPeriod - 1))) % 10 + 1;

                    plane.containers[j].area = randomDropPeriod;
                    // printf("Random drop period %d for Plane: %d\n", randomDropPeriod, i-2);
                    plane.containers[j].StartTimeOfDrop = re_fillingTime + randomDropPeriod + startTime;
                    plane.containers[j].capacity = FULL;
                    plane.containers[j].isBoomed = false;
                    plane.containers[j].sentToArea = false;

                    if (randomDropPeriod == 1 || randomDropPeriod == 2 || randomDropPeriod == 9 || randomDropPeriod == 10)
                    {
                        int Prop = (rand() % 101);
                        if (Prop <= 10)
                        {
                            plane.containers[j].isBoomed = true;
                            if ((chrashedContainers = (int *)shmat(shmidForChrasheContainers, 0, 0)) == (int *)-1)
                            {
                                perror("shmat: parent");
                                exit(2);
                            }
                            chrashedContainers[0] = chrashedContainers[0] + 1;
                            if (chrashedContainers[0] >= wheatFlourContainersBeenShotThreshold)
                            {
                                kill(getppid(), SIGQUIT);
                            }
                            shmdt(chrashedContainers);
                        }
                        else if (Prop >= 10 && Prop <= 40)
                        {
                            // printf("The container is shouted at Area %d\n", randomDropPeriod);
                            plane.containers[j].capacity = FULL - 40;
                        }
                    }
                    else if (randomDropPeriod == 3 || randomDropPeriod == 4 || randomDropPeriod == 7 || randomDropPeriod == 8)
                    {
                        int Prop = (rand() % 101);
                        if (Prop <= 25)
                        {
                            plane.containers[j].isBoomed = true;
                            if ((chrashedContainers = (int *)shmat(shmidForChrasheContainers, 0, 0)) == (int *)-1)
                            {
                                perror("shmat: parent");
                                exit(2);
                            }
                            chrashedContainers[0] = chrashedContainers[0] + 1;
                            if (chrashedContainers[0] >= wheatFlourContainersBeenShotThreshold)
                            {
                                kill(getppid(), SIGQUIT);
                            }
                            shmdt(chrashedContainers);
                        }
                        else if (Prop >= 25 && Prop <= 60)
                        {
                            // printf("The container is shouted at Area %d\n", randomDropPeriod);
                            plane.containers[j].capacity = FULL - 40;
                        }
                    }

                    else if (randomDropPeriod == 5 || randomDropPeriod == 6)
                    {
                        int Prop = (rand() % 101);
                        if (Prop <= 40)
                        {
                            plane.containers[j].isBoomed = true;
                            if ((chrashedContainers = (int *)shmat(shmidForChrasheContainers, 0, 0)) == (int *)-1)
                            {
                                perror("shmat: parent");
                                exit(2);
                            }
                            chrashedContainers[0] = chrashedContainers[0] + 1;
                            if (chrashedContainers[0] >= wheatFlourContainersBeenShotThreshold)
                            {
                                kill(getppid(), SIGQUIT);
                            }
                            shmdt(chrashedContainers);
                        }
                        else if (Prop >= 40 && Prop <= 80)
                        {
                            // printf("The container is shouted at Area %d\n", randomDropPeriod);
                            plane.containers[j].capacity = FULL - 40;
                        }
                    }
                }

                int x = 0;
                int nextType = 0;
                printf("re_fillingTime Now\n");
                while (time(NULL) != startPlaneTime)
                {
                }
                Info = (struct ToOpenJl *)shmat(Shmid, 0, 0);

                ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid, 0, 0);

                if (caseOfCrash > 1)
                {
                    if (caseOfCrash == 2)
                    {
                        if (planNumber == 1)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[7] = 1;
                        }
                        else if (planNumber == 8)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[6] = 1;
                        }
                        else
                        {
                            Info[0].Fly_Plane_Flag[planNumber - 1] = 1;
                        }
                    }
                    else if (caseOfCrash == 3)
                    {
                        if (planNumber == 2)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[5] = 1;
                        }
                        else if (planNumber == 9)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[4] = 1;
                        }
                        else
                        {
                            Info[0].Fly_Plane_Flag[planNumber - 1] = 1;
                        }
                    }
                    else if (caseOfCrash == 4)
                    {
                        if (planNumber == 3)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[3] = 1;
                        }
                        else if (planNumber == 10)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[2] = 1;
                        }
                        else
                        {
                            Info[0].Fly_Plane_Flag[planNumber - 1] = 1;
                        }
                    }
                    else if (caseOfCrash == 5)
                    {
                        if (planNumber == 1)
                        {
                            plane.timeOfCrash = startPlaneTime + 13;
                        }
                        else if (planNumber == 8)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[6] = 1;
                        }
                        else if (planNumber == 2)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[5] = 1;
                        }
                        else if (planNumber == 9)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[4] = 1;
                        }
                        else
                        {
                            Info[0].Fly_Plane_Flag[planNumber - 1] = 1;
                        }
                    }
                    else if (caseOfCrash == 6)
                    {
                        if (planNumber == 1)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[7] = 1;
                        }
                        else if (planNumber == 8)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[6] = 1;
                        }
                        else if (planNumber == 3)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[3] = 1;
                        }
                        else if (planNumber == 10)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[2] = 1;
                        }
                        else
                        {
                            Info[0].Fly_Plane_Flag[planNumber - 1] = 1;
                        }
                    }
                    else if (caseOfCrash == 7)
                    {
                        if (planNumber == 2)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[5] = 1;
                        }
                        else if (planNumber == 9)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[4] = 1;
                        }
                        else if (planNumber == 3)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[3] = 1;
                        }
                        else if (planNumber == 10)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[2] = 1;
                        }
                        else
                        {
                            Info[0].Fly_Plane_Flag[planNumber - 1] = 1;
                        }
                    }
                    else if (caseOfCrash == 8)
                    {
                        if (planNumber == 1)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[7] = 1;
                        }
                        else if (planNumber == 8)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[6] = 1;
                        }
                        else if (planNumber == 2)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[5] = 1;
                        }
                        else if (planNumber == 9)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[4] = 1;
                        }
                        else if (planNumber == 3)
                        {
                            ContainerInfo[0].Receved_Boombed_plan_Move_Flag[3] = 1;
                        }
                        else if (planNumber == 10)
                        {
                            ContainerInfo[0].Boombed_plan_Move_Flag[2] = 1;
                        }
                        else
                        {
                            Info[0].Fly_Plane_Flag[planNumber - 1] = 1;
                        }
                    }
                }
                else
                {
                    Info[0].Fly_Plane_Flag[planNumber - 1] = 1;
                }

                // printf("Time for start plane to drop containers: %ld\n", time(NULL));
                int Timearea = time(NULL) + 1;
                int area = 1;
                bool increase = true;
                // printf("End Time of Plane(%d) to drop containers: %d\n", i-2, returnTime);

                while (time(NULL) != returnTime)
                {
                    sleep(0.1);
                    if (plane.timeOfCrash <= time(NULL) && plane.timeOfCrash != -1)
                    {
                        if ((chrashedPlanes = (int *)shmat(shmidForChrashePlanes, 0, 0)) == (int *)-1)
                        {
                            perror("shmat: parent");
                            exit(2);
                        }
                        chrashedPlanes[0] = chrashedPlanes[0] + 1;

                        if (chrashedPlanes[0] >= crashedCargoPlanesThreshold)
                        {
                            kill(getppid(), SIGHUP);
                        }
                        shmdt(chrashedPlanes);
                        printf("Plane(%d) is crashed at area %d\n", planNumber, area);
                        exit(2);
                    }
                    if (Timearea == time(NULL))
                    {
                        // printf("Plane(%d) is at Area %d at Time %ld\n", planNumber, area, time(NULL));
                        if (increase == true)
                        {
                            area++;
                            if (area == 11)
                            {
                                increase = false;
                                area = 9;
                            }
                        }
                        else
                        {
                            area--;
                            if (area == 1)
                            {
                                increase = true;
                            }
                        }

                        Timearea = time(NULL) + 1;
                    }

                    if (x == randomNumberOfContainers)
                    {
                        x = 0;
                    }

                    if (plane.containers[x].isBoomed != true && plane.containers[x].StartTimeOfDrop == time(NULL) && plane.containers[x].sentToArea == false)
                    {
                        int conNum = 0;
                        ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid, 0, 0);
                        while (ContainerInfo[0].Container_Move_Flag[plane.containers[x].area - 1][conNum] == 1)
                        {
                            conNum++;
                            if (conNum == 10)
                            {
                                conNum = 0;
                            }
                        }
                        ContainerInfo[0].Container_Move_Flag[plane.containers[x].area - 1][conNum] = 1;

                        plane.containers[x].sentToArea = true;

                        struct MESSAGEcontainer containerMessage = {++nextType, plane.containers[x]};
                        if (msgsnd(messageQueuePid[plane.containers[x].area - 1], &containerMessage, sizeof(containerMessage), IPC_NOWAIT) == -1)
                        {
                            perror("msgsnd failed");
                            exit(EXIT_FAILURE);
                        }

                        printf("Time for start contaners to fall to area:%d >> Time: %ld\n", plane.containers[x].area, time(NULL));
                        if (plane.containers[x].area == 1)
                        {
                            kill(elementsPids[0], SIGHUP);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 2)
                        {
                            kill(elementsPids[0], SIGQUIT);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 3)
                        {
                            kill(elementsPids[0], SIGILL);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 4)
                        {
                            kill(elementsPids[0], SIGTRAP);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 5)
                        {
                            kill(elementsPids[0], SIGABRT);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 6)
                        {
                            kill(elementsPids[0], SIGBUS);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 7)
                        {
                            kill(elementsPids[0], SIGFPE);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 8)
                        {
                            kill(elementsPids[0], SIGUSR1);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 9)
                        {
                            kill(elementsPids[0], SIGSEGV);
                            sleep(0.01);
                        }
                        else if (plane.containers[x].area == 10)
                        {
                            kill(elementsPids[0], SIGUSR2);
                            sleep(0.01);
                        }
                    }
                    else if (plane.containers[x].isBoomed == true && plane.containers[x].StartTimeOfDrop == time(NULL) && plane.containers[x].sentToArea == false)
                    {
                        ContainerInfo = (struct ContainerToOpenJl *)shmat(ContainerShmid, 0, 0);
                        ContainerInfo[0].Boombed_Container_Move_Flag[plane.containers[x].area - 1] = 1;
                        plane.containers[x].sentToArea = true;
                    }

                    x++;
                }
                // printf("Time for end plane(%d) to drop containers: %ld\n", i-2,time(NULL));
                /* Info[0].Fly_Plane_Flag[planNumber - 1] = 0;*/
                re_fillingTime = periodOfReFilling;
                struct Planes plane = {NULL, 0};

                startTime = time(NULL);
                // printf("startTime Of the plane(%d): %d\n", planNumber, startTime);
                startPlaneTime = startTime + re_fillingTime;
                returnTime = startTime + re_fillingTime + 20;

                // printf("Time Start Of the plane(%d): %d\n", planNumber, startPlaneTime);
                // printf("Time End Of the plane(%d): %d\n", planNumber, returnTime);
            }

            pause();
        }
    }
    /////////////////////////////////////////////////
    signal(SIGHUP, threshold1);
    signal(SIGQUIT, threshold2);
    signal(SIGILL, threshold3);
    signal(SIGTRAP, threshold4);
    signal(SIGABRT, threshold5);
    ElementsPids = elementsPids;

    AppTime = time(NULL) + AppTime * 60;
    while (time(NULL) <= AppTime)
    {
        if (exitFlag == true)
        {
            break;
        }
    }
    if (exitFlag == false)
    {
        for (int i = 0; i < totalElementsNum; i++)
        {
            kill(elementsPids[i], SIGKILL);
        }
        printf(">>>>>>>>>>>>>>>>The Simulation is finished>>>>>>>>>>>>>>>>\n");
    }
    /////////////////////////////////////////////////

    // Close the file
    fclose(file);
    for (int i = 0; i < 10; i++)
    {
        messageQueuePid[i] = msgctl(messageQueuePid[i], IPC_RMID, NULL);
    }

    shmidForDistribution = shmctl(shmidForDistribution, IPC_RMID, NULL);
    if (shmidForDistribution == -1)
    {
        perror("shmctl");
        exit(1);
    }

    return 0;
}
/////////////////////////////////////////////////
void threshold1(int signo)
{
    exitFlag = true;
    for (int i = 0; i < totalElementsNum; i++)
    {
        kill(ElementsPids[i], SIGKILL);
    }
    printf(">>>>>>>>>>>>>>>>The Simulation is finished>>>>>>>>>>>>>>>>\n");
    printf(" The number of cargo planes that crashed by collision in northern Gaza airspace exceeds a user-defined threshold.");
}
void threshold2(int signo)
{
    exitFlag = true;
    for (int i = 0; i < totalElementsNum; i++)
    {
        kill(ElementsPids[i], SIGKILL);
    }
    printf(">>>>>>>>>>>>>>>>The Simulation is finished>>>>>>>>>>>>>>>>\n");
    printf(" The number of wheat flour containers who have been shot down exceeds a user-defined threshold");
}
void threshold3(int signo)
{
    exitFlag = true;
    for (int i = 0; i < totalElementsNum; i++)
    {
        kill(ElementsPids[i], SIGKILL);
    }
    printf(">>>>>>>>>>>>>>>>The Simulation is finished>>>>>>>>>>>>>>>>\n");
    printf(" The number of collecting relief committee workers who have been martyred is above a user-defined threshold");
}
void threshold4(int signo)
{
    exitFlag = true;
    for (int i = 0; i < totalElementsNum; i++)
    {
        kill(ElementsPids[i], SIGKILL);
    }
    printf(">>>>>>>>>>>>>>>>The Simulation is finished>>>>>>>>>>>>>>>>\n");
    printf(" The number of distributing relief committee workers who have been martyred is above a user-defined threshold");
}
void threshold5(int signo)
{
    exitFlag = true;
    for (int i = 0; i < totalElementsNum; i++)
    {
        kill(ElementsPids[i], SIGKILL);
    }
    printf(">>>>>>>>>>>>>>>>The Simulation is finished>>>>>>>>>>>>>>>>\n");
    printf(" The number of families who passed away due to starvation is above a user defined threshold");
}
/////////////////////////////////////////////////

void DoNothing(int signo)
{
}
void sigset_catcherFromArea1(int signo)
{
    fromArea[0]++;
}
void sigset_catcherFromArea2(int signo)
{
    fromArea[1]++;
}
void sigset_catcherFromArea3(int signo)
{
    fromArea[2]++;
}
void sigset_catcherFromArea4(int signo)
{
    fromArea[3]++;
}
void sigset_catcherFromArea5(int signo)
{
    fromArea[4]++;
}
void sigset_catcherFromArea6(int signo)
{
    fromArea[5]++;
}
void sigset_catcherFromArea7(int signo)
{
    fromArea[6]++;
}
void sigset_catcherFromArea8(int signo)
{
    fromArea[7]++;
}
void sigset_catcherFromArea9(int signo)
{
    fromArea[8]++;
}
void sigset_catcherFromArea10(int signo)
{
    fromArea[9]++;
}

struct node *createNode(struct container data)
{
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    if (newNode == NULL)
    {
        printf("Error creating a new node.\n");
        exit(0);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void initQueue(struct queue *q)
{
    q->front = q->rear = NULL;
}

int isEmpty(struct queue *q)
{
    return q->front == NULL;
}

void enQueue(struct queue *q, struct container data)
{
    struct node *temp = createNode(data);
    if (isEmpty(q))
    {
        q->front = q->rear = temp;
    }
    else
    {
        q->rear->next = temp;
        q->rear = temp;
    }
}

struct container deQueue(struct queue *q)
{
    struct container data = {0, 0, 0, 0};
    if (isEmpty(q))
    {
        printf("Queue is empty.\n");
    }
    else
    {
        struct node *temp = q->front;
        data = temp->data;
        q->front = q->front->next;
        free(temp);
    }
    return data;
}

struct container frontOfQueue(struct queue *q)
{
    return q->front->data;
}