#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>	/* POSIX flags */
#include <time.h>	/* clock_gettime(), time() */
#include <sys/time.h>	/* gethrtime(), gettimeofday() */

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#else
#error "Unable to define getRealTime( ) for an unknown OS."
#endif

#define MASK -2
#define WSHD  0
#define INIT -1
#define INQE -3

struct Queue* createQueue(unsigned capacity);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
void enqueue(struct Queue* queue, int item);
int dequeue(struct Queue* queue);
int front(struct Queue* queue);
int rear(struct Queue* queue);
int partition (int *arr, int *arrp, int low, int high);
void quickSort(int *arr, int *arrp, int low, int high);
double getRealTime();
void watershed (long nx, long ny, int **imi, int *h, int *sorted_pixels, struct Queue* indices);
