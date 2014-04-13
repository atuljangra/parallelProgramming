#include <stdio.h>
#include "common.h"

int main( int argc, char ** argv )
{
    pkt p ;

    int src;
    int i, j;
    scanf(" %d %d %d", &i, &j, &src);
    setup( TORUS, i, j) ;
    create_packet( &p, src, -1, 1, 1, 1 ) ;
    initStats() ;
    broadcast( &p, src) ;
    printStats() ;

    return 0 ;
}
