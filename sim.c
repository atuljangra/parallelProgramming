#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common.h"
#include <stdbool.h>

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

void checkGraph ();

typedef struct {
  int parent;
  int id;
  int weight;
}Node;
  
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
    for (j = 0; j < DIMY - 1; j++) {
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
    for (i = 0; i < DIMX - 1; i++) {
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

  // A simple check for the graph.
  checkGraph();

}

void checkGraph() {
  int i, j;         
  int count = 0;
  for (i = 0; i < numOfNodes; i++) {
    count = 0;
    for (j = 0; j < numOfNodes; j++) {
      count += graph[(i * numOfNodes ) + j];
    }
    if (count != 4) {
      printf("ERROR: Problem in building the graph. %d %d\n", i, j);
    }
  }
}

void printMatrix() {
  printf("\n");
  int i, j, sum;
  for (i = 0; i < numOfNodes ; i++) {
    sum = 0;
    for (j = 0; j < numOfNodes; j++) {
      sum +=  graph[ (i * numOfNodes) + j];
      printf("%d  ", graph[ (i * numOfNodes) + j]);
    }
    printf("%d ", sum);
    printf("\n");
  }
}

bool allUsed(bool used[]) {
  int i;
  for (i = 0; i < numOfNodes; i++) {
    if (used[i] == false)
      return false;
  }
  return true;
}

int findMin(Node nodes[], bool used[]) {
  int i, min, minID;
  min = INT_MAX;
  for (i = 0; i < numOfNodes; i++) {
    if(nodes[i].weight < min && used[i] == false) {
      min = nodes[i].weight;
      minID = nodes[i].id;
      printf("findMin: setting min to %d %d\n", min, minID);
    }
  }
  return minID;
}

void printTree(Node tree[]) {
  int i;
  for (i = 0; i < numOfNodes; i++) {
    printf("%d - %d \n", tree[i].parent, tree[i].id);
  }
}

void removeUsedEdges(Node tree[]) {
  int i;
  for (i = 0; i < numOfNodes; i++) {
    printf("%d\t", i);
    if(tree[i].parent == -1)
      continue;
    graph[((tree[i].parent) * numOfNodes) + tree[i].id] = 0;
    graph[tree[i].parent + (tree[i].id)*numOfNodes] = 0;
  }
}

void mst(int src)
{
  Node tree[numOfNodes];
  //bool nodesIncluded[numOfNodes];
  Node nodes[numOfNodes];

  bool used[numOfNodes];

  int i;
  for (i = 0; i < numOfNodes; i++) 
    used[i] = false;
  for (i = 0; i < numOfNodes; i++) {
    tree[i].id = i;
    tree[i].weight = INT_MAX;
    tree[i].parent = -1;
    nodes[i].id = i;
    nodes[i].weight = INT_MAX;
    nodes[i].parent = -1;
  }

 
  // updating src
  nodes[src].weight = 0;
  while(1) {
    if (allUsed(used)) {
      printf("All used breaking\n");
      break;
    }
    int min = findMin(nodes, used);
    
    used[min] = true;
    printf("Adding %d to tree\n", min);
    for (i = 0; i < numOfNodes; i++) {
      if(graph[(i*numOfNodes) + min] && used[i] == false) {
        tree[i].parent = min;
        tree[i].weight = graph[(i * numOfNodes) + min];
        nodes[i].weight = tree[i].weight;
        // Also remove this edge from the graph.
        // Removing edges here is wrong.
        // graph[(i * numOfNodes) + min] = 0;
        // graph[(min * numOfNodes) + i] = 0;
      }
    }
  }
  printf("Removing used edges\n");
  removeUsedEdges(tree);
  printf("printing tree\n");
  printTree(tree);
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
  mst(src); 
  printMatrix();
  mst(src);
  printMatrix();
  mst(src);
  printMatrix();
  int s = p-> size;
	p -> src = src;
	p -> dst = -1;
	p -> hdr1 = 1;
	p -> size = s/2 + ((s%2 > 0)? 1 : 0);
	recv(p, src);
	/*p -> hdr1 = 2;
	p -> size = s/4 +((s%4 > 1) ? 1 : 0);
	recv(p, src);
	p -> size = s/4 + ((s%4 > 2) ? 1: 0);
	p -> hdr1 = 3;
	recv(p, src);
	*/
  p -> size = s/2;
	p -> hdr1 = 2;
	recv(p, src);
	}

