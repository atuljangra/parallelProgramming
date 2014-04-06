#include <stdio.h>

#define TORUS 1

typedef struct pkt
{
  int src ;
  int dst ;
  int size ;
  int hdr1 ;   /* User defined field */
  int hdr2 ;   /* User defined field */
} pkt ;

void setup( int type, int param1, int param2 ) ;
void create_packet( pkt *p, int src, int dst, int size, int hdr1, int hdr2 ) ;
void broadcast( pkt *p, int src ) ;
void initStats() ;
void printStats() ;
