#include <xspi.h>
#include <xintc.h>
#include <xgpio.h>
#include <xtmrctr.h>
#include <xparameters.h>

#include "ui.h"
#include "sample.h"

#define POST(sig) QActive_postISR((QActive *)&tuner_ao, sig)

#define GPIO_CH 1
#define ENC_INTR \
	XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENC_IP2INTC_IRPT_INTR
#define BTN_INTR \
	XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR

#define TMR_BLANK_NUM 0
#define TMR_BLANK_RST -200000000	// 2 s
#define TMR_DEBTN_NUM 1
#define TMR_DEBTN_RST -2000000		// 20 ms
#define TMR_INTR \
	XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR

XSpi spi;
XIntc intc;
XGpio enc, btn, led, dc;
XTmrCtr tmr;

static int enc_state = 3, enc_dir = 0;
static int btn_enabled = 1, btn_state = 0, ecl_state = 0;

void bsp_timerArm() {
	XTmrCtr_Start(&tmr, TMR_BLANK_NUM);
}

void bsp_timerDisarm() {
	// XTmrCtr_Stop(&tmr, TMR_BLANK_NUM);
	XGpio_DiscreteSet(&led, GPIO_CH, 1<<14);
}

void interrupt_tmr(void *_, u8 tmr_num) {
	switch (tmr_num) {
	case TMR_BLANK_NUM:
		POST(Q_TIMEOUT_SIG);
		break;
	case TMR_DEBTN_NUM:
		btn_enabled = 1;
		break;
	}
}

static void interrupt_enc() {
	u32 pins;
	int new_state;

	XGpio_InterruptClear(&enc, XGPIO_IR_CH1_MASK);
	pins = XGpio_DiscreteRead(&enc, GPIO_CH);

	new_state = pins & 4;
	if (new_state != ecl_state && btn_enabled) {
		btn_enabled = 0;
		XTmrCtr_Start(&tmr, TMR_DEBTN_NUM);
		if (new_state) POST(SIG_ENC_CL);
	}
	ecl_state = new_state;

	new_state = pins & 3;
	switch (new_state) {
	case 3:
		if (enc_state == 2 && enc_dir == 1) POST(SIG_ENC_UP);
		else if (enc_state == 1 && enc_dir == -1) POST(SIG_ENC_DN);
		enc_dir = 0;
		break;
	case 1:
		if (enc_state == 3) enc_dir = 1;
		break;
	case 2:
		if (enc_state == 3) enc_dir = -1;
		break;
	}
	enc_state = new_state;
}

static void interrupt_btn() {
	int state;
	XGpio_InterruptClear(&btn, XGPIO_IR_CH1_MASK);

	state = XGpio_DiscreteRead(&btn, GPIO_CH);
	if (state == btn_state) return;
	btn_state = state;
	if (!btn_enabled) return;

	btn_enabled = 0;
	XTmrCtr_Start(&tmr, TMR_DEBTN_NUM);
	switch (state) {
	case 1:
		POST(SIG_BTN_UP);
		break;
	case 2:
		POST(SIG_BTN_LF);
		break;
	case 4:
		POST(SIG_BTN_RG);
		break;
	case 8:
		POST(SIG_BTN_DN);
		break;
	case 16:
		POST(SIG_BTN_CT);
		break;
	}
}

void bsp_init(void) {
	fft_init();

	// Intc
	XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	microblaze_register_handler(
		XIntc_DeviceInterruptHandler, XPAR_INTC_0_DEVICE_ID
	);

	// Timer
	XTmrCtr_Initialize(&tmr, XPAR_TMRCTR_0_DEVICE_ID);
	XTmrCtr_SetOptions(&tmr, TMR_BLANK_NUM, XTC_INT_MODE_OPTION);
	XTmrCtr_SetResetValue(&tmr, TMR_BLANK_NUM, TMR_BLANK_RST);
	XTmrCtr_SetOptions(&tmr, TMR_DEBTN_NUM, XTC_INT_MODE_OPTION);
	XTmrCtr_SetResetValue(&tmr, TMR_DEBTN_NUM, TMR_DEBTN_RST);

	XTmrCtr_SetHandler(&tmr, interrupt_tmr, NULL);
	XIntc_Connect(&intc, TMR_INTR, XTmrCtr_InterruptHandler, &tmr);
	XIntc_Enable(&intc, TMR_INTR);

	// Encoder
	XGpio_Initialize(&enc, XPAR_AXI_GPIO_ENC_DEVICE_ID);
	XGpio_InterruptEnable(&enc, XGPIO_IR_CH1_MASK);
	XGpio_InterruptGlobalEnable(&enc);
	XIntc_Connect(&intc, ENC_INTR, interrupt_enc, NULL);
	XIntc_Enable(&intc, ENC_INTR);

	// Buttons
	XGpio_Initialize(&btn, XPAR_AXI_GPIO_BTN_DEVICE_ID);
	XGpio_InterruptEnable(&btn, XGPIO_IR_CH1_MASK);
	XGpio_InterruptGlobalEnable(&btn);
	XIntc_Connect(&intc, BTN_INTR, interrupt_btn, NULL);
	XIntc_Enable(&intc, BTN_INTR);

	// LCD
	XGpio_Initialize(&dc, XPAR_SPI_DC_DEVICE_ID);
	XSpi_Initialize(&spi, XPAR_SPI_DEVICE_ID);
	XSpi_Reset(&spi);
	XSpi_SetControlReg(&spi, (
		XSpi_GetControlReg(&spi)
		| XSP_CR_ENABLE_MASK
		| XSP_CR_MASTER_MODE_MASK
	) & (~XSP_CR_TRANS_INHIBIT_MASK));
	XSpi_SetSlaveSelectReg(&spi, ~0x1);
	lcd_init();

	// Green LED means all is well
	XGpio_Initialize(&led, XPAR_AXI_GPIO_LED_DEVICE_ID);
	XGpio_DiscreteWrite(&led, GPIO_CH, 1);
}

void QF_onStartup(void) {
	XIntc_Start(&intc, XIN_REAL_MODE);
	microblaze_enable_interrupts();
	grab_start();
}

static void sample(float complex *a) {
	int i, lim;
	for (i = 0; i < FFT_SIZE; ++i) a[i] = 0;

	i = 0;
	while (i < GRAB_SIZE) {
		lim = grab_count();
		for (; i < lim; ++i) a[i>>SAMP_FACTOR] += grab_read(i);
	}
}

// TODO: make adjustable on octave range
#define FFTAVG_SIZE 2
static int fai;
static float complex fft_avg[FFTAVG_SIZE][FFT_SIZE];

void QF_onIdle(void) {        /* entered with interrupts locked */
	int i;
	QF_INT_UNLOCK();                       /* unlock interrupts */
	// TODO: test nested interrupts, posting from here, stopping?
	for (i = 0; i < FFT_SIZE; ++i)
		tuner_ao.fft_a[i] -= fft_avg[fai][i];

	sample(fft_avg[fai]);
	grab_start();
	rufft(fft_avg[fai], FFT_SIZE);

	for (i = 0; i < FFT_SIZE; ++i)
		tuner_ao.fft_a[i] += fft_avg[fai][i];
	fai = (fai + 1) % FFTAVG_SIZE;

	QActive_post((QActive *)&tuner_ao, SIG_FFT_DONE);
}
