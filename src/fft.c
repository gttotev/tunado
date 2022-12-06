#include "sample.h"

static int reorderLUT[FFT_SIZE];
static float complex trigLUT[GRAB_BITS];

void fft_init() {
    int i, j, bit;
    float ang;
    for (i = 0; i < GRAB_BITS; ++i) {
        ang = 2*PI / (1 << (i+1));
        trigLUT[i] = tcos(ang) + tsin(ang)*I;
    }

	for (i = 1, j = 0; i < FFT_SIZE; i++) {
        bit = FFT_SIZE >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        reorderLUT[i] = j;
    }
}

void rufft(float complex *a, int n) {
	int i, j, k, bit, len;
	float complex wlen, w, u, v;

	for (i = 1, j = 0; i < n; i++) {
        j = reorderLUT[i];
        if (i < j) {
			u = a[i];
			a[i] = a[j];
			a[j] = u;
		}
    }

    for (len = 2, k = 0; len <= n; len <<= 1, ++k) {
        wlen = trigLUT[k];
        for (i = 0; i < n; i += len) {
			w = 1;
			bit = len >> 1;
            for (j = 0; j < bit; j++) {
                u = a[i+j];
				v = a[i+j+bit] * w;

                a[i+j] = u + v;
                a[i+j+bit] = u - v;

                w *= wlen;
            }
        }
    }
}

int fft_max(float complex *a, int n, float *mag) {
    int i, m, lim = n >> 1;
    float max, re, im;

    for (max = 0, i = 1; i < lim; ++i) {
        re = crealf(a[i]);
        im = cimagf(a[i]);
        mag[i] = re*re + im*im;
        if (mag[i] > max) {
            max = mag[i];
            m = i;
        }
    }
    // TODO: parabolic curve fitting?
    return m;
}
