/*
 * Ridged multifractal noise
 * -same as perlin or simplex noise except one thing
 * -each octave of noise is calculated as 1.0 - abs(noise(x,...))
 */

#include "ridged_mf.h"


float rmf_perlin1(float x, int repeat, int base)
{
    if (_oct == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(pnoise1(x, repeat, base));
    } else if (_oct > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < _oct; i++) {
            total += 1.0f - fabs(pnoise1(x * _freq, (const int)(repeat * _freq), base) * _amp);
            max += _amp;
            _freq *= _lac;
            _amp *= _per;
        }
        return (total / max);
    } 
    return 0.0f;
}

float rmf_perlin2(float x, float y, int repeatx, int repeaty, int base)
{
    if (_oct == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(pnoise2(x,y, repeatx, repeaty, base));
    } else if (_oct > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < _oct; i++) {
            total += 1.0f - fabs(pnoise2(x * _freq, y * _freq, repeatx * _freq, repeaty * _freq, base) * _amp);
            max += _amp;
            _freq *= _lac;
            _amp *= _per;
        }
        return (total / max);
    }
    return 0.0f;
}

float rmf_perlin3(float x, float y, float z, int repeatx, int repeaty, int repeatz, int base)
{
    if (_oct == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(pnoise3(x, y, z, repeatx, repeaty, repeatz, base));
    } else if (_oct > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < _oct; i++) {
            total += 1.0f - fabs(pnoise3(x * _freq, y * _freq, z * _freq, 
                (const int)(repeatx*_freq), (const int)(repeaty*_freq), (const int)(repeatz*_freq), base) * _amp);
            max += _amp;
            _freq *= _lac;
            _amp *= _per;
        }
        return (total / max);
    }
    return 0.0f;
}


float rmf_simplex2(float x, float y)
{
    if (_oct == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(snoise2(x,y));
    } else if (_oct > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < _oct; i++) {
            total += 1.0f - fabs(snoise2(x * _freq, y * _freq) * _amp);
            max += _amp;
            _freq *= _lac;
            _amp *= _per;
        }
        return (total/max); // why max
    }
    return 0.0f;
}

float rmf_simplex3(float x, float y, float z)
{
    if (_oct == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(snoise3(x,y,z));
    } else if (_oct > 1) {
        int i;
        float total = 0.0f;
        float max = 0.0f;

        for (i = 0; i < _oct; i++) {
            total += 1.0f - fabs(snoise3(x * _freq, y * _freq, z * _freq) * _amp);
            max += _amp;
            _freq *= _lac;
            _amp *= _per;
        }
        return (total/max);
    }
    return 0.0f;
}

/* fill methods */
void rmf_perlin1_fill(int x, int repeat, int base) {
    float fx = (float)x + 2.0f;
    int i;
    float h;
    for (i=0; i<x; i++) {
        h = rmf_perlin1((i+1)/fx, repeat, base);
        noisemap[i] = h;
    }
}

void rmf_perlin2_fill(int x, int y, int repeatx, int repeaty, int base) {
    printf("RMF PERLIN 2 FILL\n");
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    int i,j;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            h = rmf_perlin2((i+1)/fx,(j+1)/fy, repeatx, repeaty, base);
            noisemap[i + x*j] = h;
        }
    }
}

void rmf_perlin3_fill(int x, int y, int z, int repeatx, int repeaty, int repeatz, int base) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    int i,j,k;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                h = rmf_perlin3((i+1)/fx,(j+1)/fy,(k+1)/fz, repeatx, repeaty, repeatz, base);
                noisemap[i + x*j + x*y*k] = h;
                printf("%0.2f\n",h);
            }
        }
    }
}

void rmf_simplex2_fill(int x, int y) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    int i,j;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            h = rmf_simplex2((i+1)/fx,(j+1)/fy);
            noisemap[i + x*j] = h;
        }
    }
}

void rmf_simplex3_fill(int x, int y, int z) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    int i,j,k;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                h = rmf_simplex3((i+1)/fx,(j+1)/fy,(k+1)/fz);
                noisemap[i + x*j + x*y*k] = h;
            }
        }
    }
}
