    /*
 *
 * Author: Andrei Francisco da Rosa
 * Project: Watershed [Based on Soille and Vincent (1990)] & PGM image opener
 * Contact: andreirosa@edu.univali.br
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgmfiles.h"
#include "watershed.h"


int main(int argc, char **argv){
  int  **matrix;
  int *vector, *vectorp, aux = 0;
  int i, j, k=0;
  int result;
  eightBitPGMImage *PGMImage;
  //clock_t t;
  double startTime, endTime;


  /* ---- read image name  ---- */

  PGMImage = (eightBitPGMImage *) malloc(sizeof(eightBitPGMImage));

  if (!argv[1])  {
    printf("name of input PGM image file (with extender): ");
    scanf("%s", PGMImage->fileName);
  }
  else strcpy(PGMImage->fileName, argv[1]);

  result = read8bitPGM(PGMImage);

  if(result < 0) {
    printPGMFileError(result);
    exit(result);
  }


  /* ---- allocate storage for matrixes and vector---- */

  matrix = (int **) malloc (PGMImage->x * sizeof(int *));
  if (matrix == NULL){
    printf("not enough storage available\n");
    exit(1);
  }
  for (i=0; i<PGMImage->x; i++){
    matrix[i] = (int *) malloc (PGMImage->y * sizeof(int));
    if (matrix[i] == NULL){
      printf("not enough storage available\n");
      exit(1);
    }
  }

  vector = (int *) malloc ((PGMImage->x*PGMImage->y)  * sizeof(int));
  if (vector == NULL){
    printf("not enough storage available\n");
    exit(1);
  }

  vectorp = (int *) malloc ((PGMImage->x*PGMImage->y)  * sizeof(int));
  if (vectorp == NULL){
    printf("not enough storage available\n");
    exit(1);
  }


  /* ---- read image data into matrix ---- */

 for (i=0; i<PGMImage->x; i++)
    for (j=0; j<PGMImage->y; j++)
      matrix[i][j] = (int) *(PGMImage->imageData + (i*PGMImage->y) + j);


 /* ---- sort the pixels of matrix in the increasing order of their gray values ---- */

 for (i=0; i<(PGMImage->x*PGMImage->y); i++){
    vector[i] = 0;
    vectorp[i] = i;
 }

 // this vectorP has the position of the vector
 for (i=0; i<PGMImage->x; i++)
    for (j=0; j<PGMImage->y; j++, k++)
        vector[k] = matrix[i][j];

 quickSort (vector, vectorp, 0, (PGMImage->x*PGMImage->y)-1);


 /* ---- levels evenly spaced steps from minimum to maximum ---- */

 int diff = vector[(PGMImage->x*PGMImage->y)-1] - vector[0];
 double levels[256];
 double step = diff/255.0;

 for (i=0; i<256-1; i++) levels[i] = vector[0] + i*step;

 levels[255] = vector[(PGMImage->x*PGMImage->y)-1];


 /* ---- get the indices that deleimit pixels with different values ---- */

 struct Queue* indices = createQueue(257);
 int current_level = 0;

 for (i=0; i<(PGMImage->x*PGMImage->y); i++)
     if (vector[i] > levels[current_level]){
         while(vector[i] > levels[current_level]) current_level++;
        enqueue(indices, i);
     }
 enqueue(indices, (PGMImage->x*PGMImage->y));


 /* ---- watershed operation ---- */

 startTime = getRealTime( ); // starting time to get cpu time

 watershed(PGMImage->x, PGMImage->y, matrix, vector, vectorp, indices);

 endTime = getRealTime( ); // ending time to get cpu time
 fprintf( stderr, "\nTempo funcao = %lf ns\n", (endTime - startTime) );


 /* copy the Result Image to PGM Image/File structure */

   for (i=0; i<PGMImage->x; i++)
     for (j=0; j<PGMImage->y; j++)
       *(PGMImage->imageData + i*PGMImage->y + j) = (char) matrix[i][j];


 /* ---- write image ---- */

 printf("name of output PGM image file (with extender): ");
 scanf("%s", PGMImage->fileName);

 write8bitPGM(PGMImage);


 /* ---- disallocate storage ---- */

 for (i=0; i<PGMImage->x; i++) free(matrix[i]);
 free(matrix);
 free(vector);
 free(vectorp);
 free(PGMImage->imageData);
 free(PGMImage);
}
