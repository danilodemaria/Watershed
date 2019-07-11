#include <limits.h>
#include "watershed.h"


/* QUEUE FUNCTIONS */

// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};

// function to create a queue of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue* queue){
    return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue){
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

// Function to remove an item from queue.
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}


/* SORT FUNCTIONS */

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (int *arr, int *arrp, int low, int high){
    int pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element
    int aux;

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] < pivot)
        {
            i++;    // increment index of smaller element
            aux = arr[i];
            arr[i] = arr[j];
            arr[j] = aux;

            aux = arrp[i];
            arrp[i] = arrp[j];
            arrp[j] = aux;
        }
    }
    aux = arr[i+1];
    arr[i+1] = arr[high];
    arr[high] = aux;

    aux = arrp[i+1];
    arrp[i+1] = arrp[high];
    arrp[high] = aux;
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quickSort(int *arr, int *arrp, int low, int high){
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, arrp, low, high);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, arrp, low, pi - 1);
        quickSort(arr, arrp, pi + 1, high);
    }
}


/* TIME FUNCTION */

double getRealTime( ){
#if defined(_WIN32)
    FILETIME tm;
    ULONGLONG t;
#if defined(NTDDI_WIN8) && NTDDI_VERSION >= NTDDI_WIN8
    /* Windows 8, Windows Server 2012 and later. ---------------- */
    GetSystemTimePreciseAsFileTime( &tm );
#else
    /* Windows 2000 and later. ---------------------------------- */
    GetSystemTimeAsFileTime( &tm );
#endif
    t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
    return (double)t / 1.0;

#elif (defined(__hpux) || defined(hpux)) || ((defined(__sun__) || defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__)))
    /* HP-UX, Solaris. ------------------------------------------ */
    return (double)gethrtime( ) / 1000000000.0;

#elif defined(__MACH__) && defined(__APPLE__)
    /* OSX. ----------------------------------------------------- */
    static double timeConvert = 0.0;
    if ( timeConvert == 0.0 )
    {
        mach_timebase_info_data_t timeBase;
        (void)mach_timebase_info( &timeBase );
        timeConvert = (double)timeBase.numer /
            (double)timeBase.denom /
            1000000000.0;
    }
    return (double)mach_absolute_time( ) * timeConvert;

#elif defined(_POSIX_VERSION)
    /* POSIX. --------------------------------------------------- */
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    {
        struct timespec ts;
#if defined(CLOCK_MONOTONIC_PRECISE)
        /* BSD. --------------------------------------------- */
        const clockid_t id = CLOCK_MONOTONIC_PRECISE;
#elif defined(CLOCK_MONOTONIC_RAW)
        /* Linux. ------------------------------------------- */
        const clockid_t id = CLOCK_MONOTONIC_RAW;
#elif defined(CLOCK_HIGHRES)
        /* Solaris. ----------------------------------------- */
        const clockid_t id = CLOCK_HIGHRES;
#elif defined(CLOCK_MONOTONIC)
        /* AIX, BSD, Linux, POSIX, Solaris. ----------------- */
        const clockid_t id = CLOCK_MONOTONIC;
#elif defined(CLOCK_REALTIME)
        /* AIX, BSD, HP-UX, Linux, POSIX. ------------------- */
        const clockid_t id = CLOCK_REALTIME;
#else
        const clockid_t id = (clockid_t)-1;	/* Unknown. */
#endif /* CLOCK_* */
        if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
            return (double)ts.tv_sec +
                (double)ts.tv_nsec / 1000000000.0;
        /* Fall thru. */
    }
#endif /* _POSIX_TIMERS */

    /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, POSIX, Solaris. ----- */
    struct timeval tm;
    gettimeofday( &tm, NULL );
    return (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
#else
    return -1.0;		/* Failed. */
#endif
}


/*-----------WATERSHED-----------*/

void watershed (long nx, long ny, int **imi, int *h, int *sorted_pixels, struct Queue* indices){
    int i, j, m, n, a, k;
    int **imo;
    int flag = 0;
    int label = 0;
    int pe, p;
    struct Queue* queue = createQueue(nx*ny);
    int stop_index, start_index = 0;


    // allocate storage for imo
    imo = (int **) malloc (nx * sizeof(int *));
    if (imo == NULL){
      printf("not enough storage available\n");
      exit(1);
    }
    for (i=0; i<nx; i++){
      imo[i] = (int *) malloc (ny * sizeof(int));
      if (imo[i] == NULL){
        printf("not enough storage available\n");
        exit(1);
      }
    }


    // value init is assigned to each pixel of imo
    for (i=0; i<nx; i++)
       for (j=0; j<ny; j++)
         imo[i][j] = INIT;


    // --- *** WATERSHED *** --- //
    do{

        stop_index = dequeue(indices);

        // mask all pixels at the current level
        for (k=start_index; k<stop_index; k++){
            pe = sorted_pixels[k];
            i = pe/ny;
            j = pe-(i)*ny;
            imo[i][j] = MASK; // imo(p) = mask
            // initialize queue with neighbours of existing basins at the current level
            for (m=i-1; m<=i+1; m++){ // neighborhood of p
                for (n=j-1; n<=j+1; n++){ // neighborhood of p
                    if ((m < 0) || (n < 0)) ; // tests if it is a border pixel
                    else if ((m >= nx) || (n >= ny)) ; // tests if it is a border pixel
                    else if (imo[m][n] >= WSHD){ // if there exists p' E Ng(p) such that imo(p')>0 or imo(p')=wshed
                        imo[i][j] = INQE; //imo(p) = inqueue
                        enqueue(queue, pe); // fifo_Add(p)  //MUDEI AQUI
                        break;
                    }
                }
            }
        }

        // extend basins
        while(!isEmpty(queue)){ // while fifo_empty = false
            pe = dequeue(queue); // p = fifo_first()
            a = pe/ny;
            // label p by inspecting neighbours
            for (m=(a)-1; m<=(a)+1; m++){ // neighborhood of p
                for (n=(pe-(a)*ny)-1; n<=(pe-(a)*ny)+1; n++){ // neighborhood of p
                    if ((m < 0) || (n < 0)) ; // tests if it is a border pixel
                    else if ((m >= nx) || (n >= ny)) ; // tests if it is a border pixel
                    else if (imo[m][n]>0){ // if imo(p')>0
                        if ((imo[a][pe-(a)*ny] == INQE) || ((imo[a][pe-(a)*ny] == WSHD) && (flag == 1)))//if(imo(p) = inqueue or (imo(p) = wshed and flag = true))
                            imo[pe/ny][pe-(a)*ny] = imo[m][n]; // imo(p)=imo(p')
                        else if ((imo[a][pe-(a)*ny] > 0) && (imo[a][pe-(a)*ny] != imo[m][n])){ //else if (imo(p) > 0 and imo(p) != imo(p'))
                            imo[a][pe-(a)*ny] = WSHD; // imo(p) = whsed
                            flag = 0; // flag=0
                        }
                    }
                    else if (imo[m][n] == WSHD){ // else if (imo(p')=wshed)
                        if (imo[pe/ny][pe-(a)*ny] == INQE){ // if (imo(p)=inqueue)
                            imo[pe/ny][pe-(a)*ny] = WSHD; // imo(p)=wshed
                            flag = 1; // flag = true
                        }
                    }
                    else if (imo[m][n] == MASK){ // else if (imo(p') = mask)
                        imo[m][n] = INQE; // imo(p') = inqueue
                        enqueue(queue, m*ny+n); // fifo_add(p')
                    }
                }
            }
        }

        // detect and process new minima at the current level
        for (k=start_index; k<stop_index; k++){
            pe = sorted_pixels[k];
            i = pe/ny;
            j = pe-(i)*ny;
            // p is inside a new minimum. Create a new label
            if(imo[i][j] == MASK){ // if imo(p) = mask
                label++; // label = label + 1
                enqueue(queue, pe); // fifo_add(p)
                imo[i][j] = label; // imo(p)=label
                while(!isEmpty(queue)){ // while fifo_empty() = false
                    p = dequeue(queue); // p' = fifo_first
                    a = p/ny;
                    for (m=(a)-1; m<=(a)+1; m++){ // neighborhood of p
                        for (n=(p-a*ny)-1; n<=(p-a*ny)+1; n++){ // neighborhood of p
                            if ((m < 0) || (n < 0)) ; // tests if it is a border pixel
                            else if ((m >= nx) || (n >= ny)) ; // tests if it is a border pixel
                            else if (imo[m][n] == MASK){ // if imo(p") = mask
                                enqueue(queue, m*ny+n); // fifo_add(p")
                                imo[m][n] = label; // imo(p'')=label
                            }
                        }
                    }
                }
            }
        }

       start_index = stop_index; //printf ("%d\n", imo[0][0]);

    } while(front(indices) >= 0);


    // writes IMO in IMI
    for (i=0; i<nx; i++)
        for (j=0; j<ny; j++){
            imi[i][j] = imo[i][j]*255/label; // normalizes IMI
            //printf("%d ", imi[i][j]);
        }


    // disallocate storage
    for (i=0; i<nx; i++) free(imo[i]);
    free(imo);
}
