#ifndef DMPRUNTIME_CONFIG_H
#define DMPRUNTIME_CONFIG_H


#define GALOIS_CHANGE
#define DMP_MOT_GRANULARITY 6
#define DMP_ENABLE_TINY_SERIAL_MODE

#if 0
#define DMP_ENABLE_MODEL_B_S
#define DMP_ENABLE_WB_PARALLEL_COMMIT
#define DMP_ENABLE_WB_HBSYNC_UNLOCKOPT
#else
#define DMP_ENABLE_MODEL_O_S
#define DMP_ENABLE_FAST_HANDOFF
#define DMP_ENABLE_FAST_HANDOFF_QUANTUM_OPT
#endif

#endif