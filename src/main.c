#include <xil_cache.h>

#include "ui.h"

#define EVENTQ_SIZE 64
static QEvent eventq[EVENTQ_SIZE];

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
	{ NULL, NULL, 0 },
	{ (QActive *)&tuner_ao, eventq, EVENTQ_SIZE },
};
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

/* Do not touch Q_onAssert */
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
	(void)file;
	(void)line;
	QF_INT_LOCK();
	while (1);
}

int main() {
	Xil_ICacheInvalidate();
	Xil_ICacheEnable();
	Xil_DCacheInvalidate();
	Xil_DCacheEnable();

	bsp_init();
	hsm_init();

	QF_run();
	return 0;
}
