//
// Generic dot product
//

#include <stdlib.h>
#include <string.h>

#include "dotprod_internal.h"

// basic dot product

T X(_run)(T *_x, T *_y, unsigned int _n)
{
    T r=0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _x[i] * _y[i];
    return r;
}

T X(_run4)(T *_x, T *_y, unsigned int _n)
{
    T r=0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += _x[i]   * _y[i];
        r += _x[i+1] * _y[i+1];
        r += _x[i+2] * _y[i+2];
        r += _x[i+3] * _y[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += _x[i] * _y[i];

    return r;
}

//
// structured dot product
//

X() X(_create)(T * _v, unsigned int _n)
{
    X() q = (X()) malloc(sizeof(struct X(_s)));
    q->n = _n;
    q->v = (T*) malloc((q->n)*sizeof(T));
    memmove(q->v, _v, (q->n)*sizeof(T));
    return q;
}

void X(_destroy)(X() _q)
{
    free(_q->v);
    free(_q);
}

T X(_execute)(X() _q, T * _v)
{
    return X(_run)(_q->v, _v, _q->n);
}

