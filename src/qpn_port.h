#ifndef qpn_port_h
#define qpn_port_h

#include <stdint.h>
#include <mb_interface.h>

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE           1
#define QF_TIMEEVT_CTR_SIZE     1

#define QF_INT_LOCK()           microblaze_disable_interrupts()
#define QF_INT_UNLOCK()         microblaze_enable_interrupts()

#define Q_ROM
#define Q_ROM_VAR
#define Q_ROM_PTR(rom_var_)     rom_var_
#define Q_ROM_BYTE(rom_var_)    rom_var_

#define Q_REENTRANT
#define Q_PARAM_SIZE            0

#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */

#endif                                                        /* qpn_port_h */
