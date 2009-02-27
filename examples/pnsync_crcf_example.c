//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

int main() {
    // options
    unsigned int n=63;
    float phi = 0.3f;       // channel phase offset
    float snr_db = 10.0f;

    // create objects
    float snr = powf(10.0f, snr_db/10.0f); // TODO: scale snr for complex signal?
    pnsync_crcf fs = pnsync_crcf_create_msequence(LIQUID_MSEQUENCE_N63);
    msequence ms = msequence_create(6);

    unsigned int i;
    float complex s, rxy;

    for (i=0; i<n; i++) {
        // advance
        s = msequence_advance(ms) ? 1.0f : -1.0f;
        s *= cexpf(_Complex_I*phi); // phasor rotation
        s += (randnf() + _Complex_I*randnf()) / snr;

        // correlate
        pnsync_crcf_correlate(fs,s,&rxy);

        printf("%4u: mag: %12.8f, angle: %5.2f\n",i,cabsf(rxy),cargf(rxy));
    }

    // clean it up
    pnsync_crcf_destroy(fs);
    msequence_destroy(ms);

    printf("done.\n");
    return 0;
}

