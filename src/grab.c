#include <xil_types.h>
#include <xparameters.h>

static volatile u32 *const reg_start = (u32 *)(
	XPAR_MIC_BLOCK_STREAM_GRABBER_0_BASEADDR
);
static volatile u32 *const reg_samples_captured = reg_start;
static volatile u32 *const reg_readout_index = reg_start + 1;
static volatile u32 *const reg_readout_value = reg_start + 2;

void grab_start() {
	*reg_start = 0;
}

u32 grab_count() {
	return *reg_samples_captured;
}

void grab_wait(u32 count) {
	while (*reg_samples_captured < count);
}

u32 grab_read(u32 index) {
	*reg_readout_index = index;
	return *reg_readout_value;
}
