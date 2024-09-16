///Header Files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <GL/glut.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/sem.h>

///For Opengl
#define maxPlanNum 10
#define maxContainerNum 10

//For backend (Project2.c)
// constants
#define FULL 100.0
#define DISTANCE 10000

#define SHM_SIZE 128

// Global Variables
float AppTime;
int cargoPlanes;
int periodOfReFilling;
int DropPeriod;
int rangNumberOfContainers[2];
int collectingReliefCommittees;
int *numberOfCollictingWorkers;
int numberOfSplittingWorkers;
int *numberofDistributingWorkers;
int numberOfKiloesThatWorkersCanCarry;
int numberOfCollctingWorkersMartyred;
int numberOfDistributingWorkersMartyred;
int numberOfFamiliesPassedAway;

// prototypes of Functions
void sigset_catcherFromArea1(int signo);
void sigset_catcherFromArea2(int signo);
void sigset_catcherFromArea3(int signo);
void sigset_catcherFromArea4(int signo);
void sigset_catcherFromArea5(int signo);
void sigset_catcherFromArea6(int signo);
void sigset_catcherFromArea7(int signo);
void sigset_catcherFromArea8(int signo);
void sigset_catcherFromArea9(int signo);
void sigset_catcherFromArea10(int signo);

void DoNothing(int signo);
void threshold1(int signo);
void threshold2(int signo);
void threshold3(int signo);
void threshold4(int signo);
void threshold5(int signo);
