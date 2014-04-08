#include <stdio.h>
#include "common.h"

#define MAXRANKS 1024
#define MYDBG
int DIMX, DIMY ;
int numRanks ;
int stats[MAXRANKS][3] ;
int estats[MAXRANKS][MAXRANKS][3] ;

void initStats()
{
    int i, j ;
    for( i = 0 ; i < numRanks; i++ )
    {
        stats[i][0] = 0 ;
        for( j = 0 ; j < numRanks ; j++ )
          estats[i][j][0] = 0 ;
    }
}

void printStats()
{
    int i, j, max=0 ;
    for( i = 0 ; i < numRanks ; i++ )
      printf( " Recvd[%d]=%d\n", i, stats[i][0] ) ;

    for( i = 0 ; i < numRanks ; i++ )
    {
      for( j = 0 ; j < numRanks ; j++ )
        if ( estats[i][j][0] > 0 )
        {
#ifdef MYDBG
          printf( "   pass[%d][%d]=%d, ", i, j, estats[i][j][0] ) ;
#endif
          if ( estats[i][j][0] > max )
            max = estats[i][j][0] ;
        }
#ifdef MYDBG
      printf( "\n" ) ;
#endif
    }

    printf( "MAX CONGESTION=%d\n", max ) ;
}

void consume( pkt *p, int myRank )
{
  // Update any stats
  stats[myRank][0]++ ;
}

void send( pkt *p, int myRank, int to )
{
    extern void recv( pkt *, int ) ;

#ifdef MYDBG
    printf( "Debug: %d sending to %d\n", myRank, to ) ;
#endif
    estats[myRank][to][0]++ ;
    recv( p, to ) ;
}

void setup( int type, int param1, int param2 )
{
    if ( type == TORUS )
    {
       DIMX = param1 ;
       DIMY = param2 ;
       numRanks = DIMX * DIMY ;
    }
}

void create_packet( pkt *p, int src, int dst, int size, int hdr1, int hdr2 )
{
   p->src = src ;
   p->dst = dst ;
   p->size = size ;
   p->hdr1 = hdr1 ;
   p->hdr2 = hdr2 ;
}

