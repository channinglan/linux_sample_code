/*
 *  @(#) $Id: doscom_target.h 724 2015-03-11 09:57:33Z ertl-honda $
 */
#ifndef DOSCOM_TARGET_H
#define DOSCOM_TARGET_H

#include <stdbool.h>   /* bool type (for filter function) */
#include <stdint.h>    /* uint32_t */
#include "nt_os-doscom_config.h"

/*
 * TYPES
 */
typedef struct {
	uint32_t num_fifochs;           // number of fifo channels
	uint32_t num_smemch;            // number of smem channels
	uint32_t sregion_pending_events; // offset in shared memory to pending events
} doscom_init_data_t;

typedef bool bool_t;

#endif /* DOSCOM_TYPES */
