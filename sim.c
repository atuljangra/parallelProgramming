#include <stdio.h>
#include <stdlib.h>
#include "common.h"

/*
// Example of node numbering
// of a 3x3 2D-Torus
//
// 0 - 1 - 2
// |   |   |
// 3 - 4 - 5
// |   |   |
// 6 - 7 - 8
//
*/

extern int DIMY, numRanks ;
extern int DIMX;
#define ROW( p )   ( p / DIMY )
#define COL( p )   ( p % DIMY )
#define DOWN( p )  ( (p + DIMY) % numRanks )
#define UP( p )    ( (p + numRanks - DIMY) % numRanks )
#define RIGHT( p ) ( p + ( ( (p % DIMY) == DIMY-1 ) ? ( 1 - DIMY ) : 1 ) )
#define LEFT( p )  ( p + ( ( (p % DIMY) == 0 ) ? ( DIMY - 1 ) : - 1 ) )

int numOfNodes = 0;
int *graph;
void createAdjacencyMatrix () {
  numOfNodes = DIMY * DIMX;
  // Allocate the graph
  graph = malloc(numOfNodes * numOfNodes * sizeof(int));

  printf("%d * %d \n", DIMX, DIMY);
  // Initialize the links
  int i, j;
  for (i = 0; i < numOfNodes; i++) {
    for (j = 0; j < numOfNodes ; j++) {
      graph[(i*numOfNodes ) + j] = 0;
    }
  }
 
  int iter, jter;
  // Create row-wise links.
  for (i = 0; i < DIMX; i++) {
    for (j = 0; j < DIMY; j++) {
      iter = (i*DIMY) + j;
      jter = (i*DIMY) + j + 1;
      graph[(iter*numOfNodes) + jter] = 1;
      graph[(jter*numOfNodes) + iter] = 1;
                          
    }
    // Terminal links
    iter = (i*DIMY) + 0;
    jter = (i*DIMY) + DIMY - 1;
    graph[(iter*numOfNodes) + jter] = 1;
    graph[(jter*numOfNodes) + iter] = 1;
  }

  // Creating columnwise links.
  for (j = 0; j < DIMY; j++) {
    for (i = 0; i < DIMX; i++) {
      iter = (i * DIMY) + j;
      jter = ((i + 1) * DIMY) + j;
      graph[(iter*numOfNodes) + jter] = 1;
      graph[(jter*numOfNodes) + iter] = 1;
    }
    // Terminal links.
    iter = (0*DIMY) + j;
    jter = ((DIMX - 1) * DIMY) + j;
    graph[(iter*numOfNodes) + jter] = 1;
    graph[(jter*numOfNodes) + iter] = 1;
  }
}

void printMatrix() {
  printf("\n");
  int i, j;
  for (i = 0; i < numOfNodes ; i++) {
    for (j = 0; j < numOfNodes; j++) {
      printf("%d  ", graph[ (i * numOfNodes) + j]);
    }
    printf("\n");
  }
}

void recv( pkt *p, int myRank )
{
    /* Algorithm:
     * 
     * 1. Check if src is along my x dimension.
     * 2. If so,
     *    a. send along y dimension.
     *    b. Also forward along x dimension if not last node (to left of src).
     * 3. If not,
     *    a. forward along y dimension if not last node (above line of src).
     * 4. consume.
     */

    if ( ROW( p->src ) == ROW( myRank ) )
    {
        send( p, myRank, DOWN(myRank) ) ;
        if ( LEFT(p->src) != myRank )
            send( p, myRank, RIGHT(myRank) ) ;
    }
    else if ( ROW(UP(p->src)) != ROW(myRank) )
            send( p, myRank, DOWN(myRank) ) ;

    consume( p, myRank ) ;
}

void broadcast( pkt *p, int src )
{
  createAdjacencyMatrix();
  printMatrix();
  int s = p-> size;
	p -> src = src;
	p -> dst = -1;
	p -> hdr1 = 1;
	p -> size = s/4 + ((s%4 > 0)? 1 : 0);
	recv(p, src);
	p -> hdr1 = 2;
	p -> size = s/4 +((s%4 > 1) ? 1 : 0);
	recv(p, src);
	p -> size = s/4 + ((s%4 > 2) ? 1: 0);
	p -> hdr1 = 3;
	recv(p, src);
	p -> size = s/4;
	p -> hdr1 = 4;
	recv(p, src);
	}

