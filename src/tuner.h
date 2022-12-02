#ifndef TUNER_H
#define TUNER_H

#include <complex.h>
#include <xil_types.h>

#define FREQ_CLOCK 100000000
#define FREQ_SAMPLE (FREQ_CLOCK/2048.0f)

#define MAX_GRAB_BITS 12
#define N_FFT 4096

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
