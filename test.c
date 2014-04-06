#include <stdio.h>
#include "common.h"

int main( int argc, char ** argv )
{
    pkt p ;

    setup( TORUS, 3, 5 ) ;
    create_packet( &p, 5, -1, 1, 1, 1 ) ;
    initStats() ;
    broadcast( &p, 5 ) ;
    printStats() ;

    return 0 ;
}
