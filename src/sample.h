#ifndef SAMPLE_H
#define SAMPLE_H

#include <complex.h>
#include <xil_types.h>

#define FREQ_CLOCK 100000000
#define FREQ_SAMPLE (FREQ_CLOCK/2048.0f)

#define GRAB_BITS 12
#define SAMP_FACTOR 2
#define FFT_SIZE (1 << (GRAB_BITS-SAMP_FACTOR))
#define FFT_RES (FREQ_SAMPLE / (1 << GRAB_BITS))

#define PI 3.14159265f

void grab_start();
void grab_wait(u32 count);
u32 grab_count();
u32 grab_read(u32 index);

void fft_init();
void rufft(float complex *a, int n);
int fft_max(float complex *a, int n);

float tsin(float x);
float tcos(float x);

#endif
