/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2012-2015 by Embedded and Real-Time Systems Laboratory
 *     Graduate School of Information Science, Nagoya University, JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 *  @(#) $Id: doscom_user.c 806 2015-09-24 13:42:11Z ertl-honda $  
 */
#include "doscom.h"
#include "doscom_kernel.h"

#if (DOSCOM_NUM_FIFOCHS == 0) && (DOSCOM_NUM_SMEMCHS == 0)
#error "No channel defined. You should at least define 1 fifo channel or smem channel."
#endif /* (DOSCOM_NUM_FIFOCHS == 0) && (DOSCOM_NUM_SMEMCHS == 0) */

//#define DOSCOM_DEBUG
#define DEBUG_OUT false

/*
 * INTERNAL TYPES
 */
typedef struct {
	uint32_t	*p_write;
	uint32_t	*p_read;
	uint32_t	*p_buffer;
	uint32_t	num_elements;
} doscom_fifo_t;

typedef struct {
	bool_t			assigned;
	uint32_t		block_size;
	uint32_t		num_blocks;
	uint32_t		*p_blocks_buffer;
	doscom_fifo_t	t2nt_fifo;
	doscom_fifo_t	nt2t_fifo;
	doscom_id_t		*p_t2nt_filter_id;
	doscom_id_t		*p_nt2t_filter_id;
} doscom_fifoch_t;

typedef struct {
	uint32_t	*p_lock;
	uint32_t	buffer_size;
	uint32_t	*p_buffer;
} doscom_smemch_t;

#ifdef DOSCOM_T

#define DOSCOM_NUM_SRC2DEST_FILTERS DOSCOM_NUM_T2NT_FILTERS
#define DOSCOM_NUM_DEST2SRC_FILTERS DOSCOM_NUM_NT2T_FILTERS

#define GET_SRC2DEST_FIFO(p_fifoch)				(p_fifoch->t2nt_fifo)
#define GET_DEST2SRC_FIFO(p_fifoch)				(p_fifoch->nt2t_fifo)
#define GET_SRC2DEST_FILTER_ID(p_fifoch)		(*(p_fifoch->p_t2nt_filter_id))
#define SET_SRC2DEST_FILTER_ID(p_fifoch, id)	(*(p_fifoch->p_t2nt_filter_id) = id)
#define GET_DEST2SRC_FILTER_ID(p_fifoch)		(*(p_fifoch->p_nt2t_filter_id))
#define SET_DEST2SRC_FILTER_ID(p_fifoch, id)	(*(p_fifoch->p_nt2t_filter_id) = id)

#define DOSCOM_FIFOCH_DEFAULT_DEST2SRC_FILTERS DOSCOM_FIFOCH_DEFAULT_NT2T_FILTERS
#define DOSCOM_FIFOCH_DEFAULT_SRC2DEST_FILTERS DOSCOM_FIFOCH_DEFAULT_T2NT_FILTERS

#else /* DOSCOM_NT */

#define DOSCOM_NUM_SRC2DEST_FILTERS DOSCOM_NUM_NT2T_FILTERS
#define DOSCOM_NUM_DEST2SRC_FILTERS DOSCOM_NUM_T2NT_FILTERS

#define GET_SRC2DEST_FIFO(p_fifoch)				(p_fifoch->nt2t_fifo)
#define GET_DEST2SRC_FIFO(p_fifoch)				(p_fifoch->t2nt_fifo)
#define GET_SRC2DEST_FILTER_ID(p_fifoch)		(*(p_fifoch->p_nt2t_filter_id))
#define SET_SRC2DEST_FILTER_ID(p_fifoch, id)	(*(p_fifoch->p_nt2t_filter_id) = id)
#define GET_DEST2SRC_FILTER_ID(p_fifoch)		(*(p_fifoch->p_t2nt_filter_id))
#define SET_DEST2SRC_FILTER_ID(p_fifoch, id)	(*(p_fifoch->p_t2nt_filter_id) = id)

#define DOSCOM_FIFOCH_DEFAULT_DEST2SRC_FILTERS DOSCOM_FIFOCH_DEFAULT_T2NT_FILTERS
#define DOSCOM_FIFOCH_DEFAULT_SRC2DEST_FILTERS DOSCOM_FIFOCH_DEFAULT_NT2T_FILTERS

#endif /* DOSCOM_T */

typedef struct {
	bool_t				initialized;
	doscom_fifoch_t		fifochs[DOSCOM_NUM_FIFOCHS];
	doscom_smemch_t		smemchs[DOSCOM_NUM_SMEMCHS];
	doscom_filter_t		filters[DOSCOM_NUM_SRC2DEST_FILTERS];
} doscom_user_t;

/*
 * MODULE VARIABLES
 */
static doscom_user_t the_doscom_user = {
	.initialized = false,
#ifdef DOSCOM_FILTERS
	.filters     = DOSCOM_FILTERS,
#endif /* DOSCOM_FILTERS */
};

/*
 *  GET control block
 */
#define get_fifoch(fifoch_id)		(&(the_doscom_user.fifochs[fifoch_id]))
#define get_smemch(smemch_id)		(&(the_doscom_user.smemchs[smemch_id]))

/*
 * FIFO
 */
static inline int
__doscom_fifo_empty(doscom_fifo_t *p_fifo)
{
	return (*p_fifo->p_write == *p_fifo->p_read);
}

#ifdef COPY_UPDATE_ALGORITHM_ENABLE

static inline int
__doscom_fifo_full(doscom_fifo_t *p_fifo)
{
	uint32_t fifo_write;
	uint32_t fifo_read;

	fifo_write = *p_fifo->p_write;
	assert(fifo_write >= 0);
	assert(fifo_write < p_fifo->num_elements);

	fifo_read = *p_fifo->p_read;
	assert(fifo_read >= 0);
	assert(fifo_read < p_fifo->num_elements);

	return ((fifo_write + 1) % p_fifo->num_elements) == fifo_read;
}

static inline int
__doscom_fifo_enqueue(doscom_fifo_t *p_fifo, uint32_t value)
{
	uint32_t fifo_write;

	if (__doscom_fifo_full(p_fifo)) return -1;

	fifo_write = *p_fifo->p_write;
	assert(fifo_write >= 0);
	assert(fifo_write < p_fifo->num_elements);

	p_fifo->p_buffer[fifo_write] = value;
	*p_fifo->p_write = (fifo_write + 1) % p_fifo->num_elements;

	return 0;
}

static inline int
__doscom_fifo_dequeue(doscom_fifo_t *p_fifo, uint32_t *p_value)
{
	uint32_t fifo_read;

	if (__doscom_fifo_empty(p_fifo)) return -1;

	fifo_read = *p_fifo->p_read;
	assert(fifo_read >= 0);
	assert(fifo_read < p_fifo->num_elements);

	*p_value = p_fifo->p_buffer[*p_fifo->p_read];
	*p_fifo->p_read = (*p_fifo->p_read + 1) % p_fifo->num_elements;

	return 0;
}

#else /* !COPY_UPDATE_ALGORITHM_ENABLE */

static inline int
__doscom_fifo_full(doscom_fifo_t *p_fifo)
{
	return ((*p_fifo->p_write + 1) % p_fifo->num_elements) == *p_fifo->p_read;
}

static inline int
__doscom_fifo_enqueue(doscom_fifo_t *p_fifo, uint32_t value)
{
	if (__doscom_fifo_full(p_fifo)) return -1;

	p_fifo->p_buffer[*p_fifo->p_write] = value;
	*p_fifo->p_write = (*p_fifo->p_write + 1) % p_fifo->num_elements;

	return 0;
}

static inline int
__doscom_fifo_dequeue(doscom_fifo_t *p_fifo, uint32_t *p_value)
{
	if (__doscom_fifo_empty(p_fifo)) return -1;

	*p_value = p_fifo->p_buffer[*p_fifo->p_read];
	*p_fifo->p_read = (*p_fifo->p_read + 1) % p_fifo->num_elements;

	return 0;
}

#endif /* COPY_UPDATE_ALGORITHM_ENABLE */

extern void *memset(void *str, int c, size_t len);

#ifdef DOSCOM_T

static inline void
__doscom_fifo_init(doscom_fifo_t *p_fifo, uint32_t num_blocks)
{
	*(p_fifo->p_read) = 0;
	*(p_fifo->p_write) = 0;
	memset(p_fifo->p_buffer, 0, sizeof(uint32_t)*(num_blocks + 1));
}

void
__doscom_fifoch_init(doscom_id_t fifoch_id)
{
	uint32_t *p_lock;
	uint32_t loop;
	doscom_fifoch_t *p_fifoch;
	uint32_t dest2src_def_filter[DOSCOM_NUM_FIFOCHS] = DOSCOM_FIFOCH_DEFAULT_DEST2SRC_FILTERS;
	uint32_t src2dest_def_filter[DOSCOM_NUM_FIFOCHS] = DOSCOM_FIFOCH_DEFAULT_SRC2DEST_FILTERS;

	p_fifoch = get_fifoch(fifoch_id);

	__doscom_fifo_init(&GET_SRC2DEST_FIFO(p_fifoch), p_fifoch->num_blocks);
	__doscom_fifo_init(&GET_DEST2SRC_FIFO(p_fifoch), p_fifoch->num_blocks);

	for (loop=0; loop < p_fifoch->num_blocks; loop++){
		p_lock = p_fifoch->p_blocks_buffer + loop*(1 + (p_fifoch->block_size / sizeof(uint32_t)));
		*p_lock = 0;
	}

	// set default filter
	SET_DEST2SRC_FILTER_ID(p_fifoch, dest2src_def_filter[fifoch_id]);
	SET_SRC2DEST_FILTER_ID(p_fifoch, src2dest_def_filter[fifoch_id]);
}

#endif /* DOSCOM_T */

/*
 *  CHECK
 */
#define CHECK_NOINIT() do {						\
	if (the_doscom_user.initialized == false) {	\
		return DOSCOM_NOINIT;					\
	}											\
} while(false)

#define CHECK_FIFOCH_ID(id) do {	\
	if (id >= DOSCOM_NUM_FIFOCHS) {	\
		return DOSCOM_EPARAM;		\
	}								\
} while(false)

#define CHECK_SMEMCH_ID(id) do {	\
	if (id >= DOSCOM_NUM_SMEMCHS) {	\
		return DOSCOM_EPARAM;		\
	}								\
} while(false)

#define CHECK_BLOCK_ID(id, p_fifo) do {	\
	if (id >= p_fifo->num_blocks) {		\
		return DOSCOM_EPARAM;			\
	}									\
} while(false)

#define CHECK_FILTER_ID(id) do {										\
	if ((id >= DOSCOM_NUM_DEST2SRC_FILTERS) && (id != NULL_FILTER)		\
										&& (id != DEFAULT_FILTER)) {	\
		return DOSCOM_EPARAM;											\
	}																	\
} while(false)

#ifdef DOSCOM_T
#define CHECK_ASSIGNED(p_fifoch)
#else  /* DOSCOM_NT  */
#define CHECK_ASSIGNED(p_fifoch) do {	\
	if (!(p_fifoch->assigned)) {		\
		return DOSCOM_EASSIGN;			\
	}									\
} while(false)
#endif /* DOSCOM_T */

#define CHECK_NULL(ptr) do {	\
	if (ptr == NULL) {			\
		return DOSCOM_EPARAM;	\
	}							\
} while(false)

#define CHECK_CTX() do {					\
	if (!__doscom_kernel_check_tskctx()) {	\
		return DOSCOM_ECTX;					\
	}										\
} while(false)

#ifdef DOSCOM_T
#define CHECK_FIFOCHMASK(fifoch_mask)
#else /* DOSCOM_NT */
#if DOSCOM_NUM_FIFOCHS <= 32
#define CHECK_FIFOCHMASK(fifoch_mask) do {					\
	if ((fifoch_mask & (0xffffffff << DOSCOM_NUM_FIFOCHS)) != 0U) {	\
		return DOSCOM_EPARAM;								\
	}														\
} while(false)
#else  /* DOSCOM_NUM_FIFOCHS */
#error Number of FIFO Channel must be 32 or less. 
#endif /* DOSCOM_NUM_FIFOCHS */
#endif /* DOSCOM_T */


/*
 * DEBUG
 */
#ifdef DOSCOM_DEBUG

#define DOSCOM_DEBUG_INIT
#define DOSCOM_DEBUG_EALLOC
#define DOSCOM_DEBUG_GETBUFFER
#define DOSCOM_DEBUG_ENQUEUE
#define DOSCOM_DEBUG_DEQUEUE

static void
___doscom_debug_print_locks(doscom_fifoch_t *fifoch)
{
	int i;
	uint32_t *tmp;

	PRINT("LOCKS");
	for(i=0; i < fifoch->num_blocks; i++) {
		tmp = fifoch->p_blocks_buffer + i*(1 + fifoch->block_size);
		PRINT("*(0x%x): 0x%x", (uint32_t)tmp, *((uint32_t *)tmp));
	}
}

static void
___doscom_debug_print_fifoch(doscom_fifoch_t *fifoch)
{
	PRINT("block_size: %u bytes", fifoch->block_size);
	PRINT("num_blocks: %u", fifoch->num_blocks);
	PRINT("p_blocks_buffer: 0x%x", (uint32_t)fifoch->p_blocks_buffer);
	PRINT("p_t2nt_filter_id: %u", *fifoch->p_t2nt_filter_id);
	PRINT("p_nt2t_filter_id: %u", *fifoch->p_nt2t_filter_id);
}

static void
___doscom_debug_print_fifos(doscom_fifoch_t *fifoch)
{
	int i;

	PRINT("t2nt_fifo.p_write: %u", *fifoch->t2nt_fifo.p_write);
	PRINT("t2nt_fifo.p_read: %u", *fifoch->t2nt_fifo.p_read);
	PRINT("t2nt_fifo.p_buffer: 0x%x", (uint32_t)fifoch->t2nt_fifo.p_buffer);
	PRINT("t2nt_fifo.num_elements: %u", (uint32_t)fifoch->t2nt_fifo.num_elements);
	PRINT("t2nt_fifo:");
	for(i=0; i < fifoch->num_blocks+1; i++) {
		PRINT("*(0x%x): 0x%x", fifoch->t2nt_fifo.p_buffer + i, *(fifoch->t2nt_fifo.p_buffer + i));
	}

	PRINT("nt2t_fifo.p_write: %u", *fifoch->nt2t_fifo.p_write);
	PRINT("nt2t_fifo.p_read: %u", *fifoch->nt2t_fifo.p_read);
	PRINT("nt2t_fifo.p_buffer: 0x%x", (uint32_t)fifoch->nt2t_fifo.p_buffer);
	PRINT("nt2t_fifo.num_elements: %u", (uint32_t)fifoch->nt2t_fifo.num_elements);
	PRINT("nt2t_fifo:");
	for(i=0; i < fifoch->num_blocks+1; i++) {
		PRINT("*(0x%x): 0x%x", fifoch->nt2t_fifo.p_buffer + i, *(fifoch->nt2t_fifo.p_buffer + i));
	}
}

static void
___doscom_debug_print_blocks(doscom_fifoch_t *fifoch)
{
	int i, j;
	uint32_t *tmp, *tmp2;

	PRINT("BLOCKS");
	for(i=0; i < fifoch->num_blocks; i++) {
		tmp = fifoch->p_blocks_buffer + i*(1 + fifoch->block_size) + 1;
		PRINT("block(%d): 0x%x:", i, (uint32_t)tmp);
		for(j=0; j < fifoch->block_size; j++) {
			tmp2 = tmp + j;
			PRINT("*(0x%X): 0x%x", (uint32_t)(tmp2), *(tmp2));
		}
	}
}

static void
___doscom_debug_print_all()
{
	int i;
	doscom_fifoch_t *p_fifoch;

	PRINT("initialized: %s", (the_doscom_user.initialized)?"true":"false");

	for(i=0; i<DOSCOM_NUM_SRC2DEST_FILTERS; i++)
		PRINT("filter(%d): 0x%x", i, (uint32_t)the_doscom_user.filters[i]);

	for(i=0; i<DOSCOM_NUM_FIFOCHS; i++) {
		p_fifoch = &the_doscom_user.fifochs[i];
		PRINT("CHANNEL %d", i);
		___doscom_debug_print_fifoch(p_fifoch);
		___doscom_debug_print_fifos(p_fifoch);
		___doscom_debug_print_locks(p_fifoch);
		___doscom_debug_print_blocks(p_fifoch);
	}
}
#endif /* DOSCOM_DEBUG */

#ifdef DOSCOM_DEBUG_ALLOC
#define __doscom_debug_print_locks(fifoch)	___doscom_debug_print_locks(fifoch)
#else /* !DOSCOM_DEBUG_ALLOC */
#define __doscom_debug_print_locks(fifoch)
#endif /* DOSCOM_DEBUG_ALLOC */

#ifdef DOSCOM_DEBUG_GETBUFFER
#define __doscom_debug_print_fifoch(fifoch)	___doscom_debug_print_fifoch(fifoch)
#else /* !DOSCOM_DEBUG_GETBUFFER */
#define __doscom_debug_print_fifoch(fifoch)
#endif /* DOSCOM_DEBUG_GETBUFFER */

#ifdef DOSCOM_DEBUG_ENQUEUE
#define __doscom_debug_print_fifos(fifoch)	___doscom_debug_print_fifos(fifoch)
#else  /* !DOSCOM_DEBUG_ENQUEUE */
#define __doscom_debug_print_fifos(fifoch)
#endif /* DOSCOM_DEBUG_ENQUEUE */

#ifdef DOSCOM_DEBUG_INIT
#define __doscom_debug_print_all()	___doscom_debug_print_all()
#else  /* !DOSCOM_DEBUG_INIT */
#define __doscom_debug_print_all()
#endif /* DOSCOM_DEBUG_INIT */

/*
 * doscom_init
 */
int
doscom_init(doscom_mask_t fifoch_mask, doscom_time_t timeout)
{
	int ret, i;
	void *p_sregion;
	doscom_fifoch_t *p_fifoch;
	doscom_smemch_t *p_smemch;

	uint32_t num_blocks[DOSCOM_NUM_FIFOCHS] = DOSCOM_FIFOCH_NUM_BLOCKS;
	uint32_t block_size[DOSCOM_NUM_FIFOCHS] = DOSCOM_FIFOCH_BLOCK_SIZES;
	uint32_t fifoch_offsets[DOSCOM_NUM_FIFOCHS] = DOSCOM_SREGION_FIFOCH_OFFSETS;
	uint32_t buffer_size[DOSCOM_NUM_SMEMCHS] = DOSCOM_SMEMCH_BUFFER_SIZES;
	uint32_t smemch_offsets[DOSCOM_NUM_SMEMCHS]  = DOSCOM_SREGION_SMEMCH_OFFSETS;

	CHECK_CTX();
	CHECK_FIFOCHMASK(fifoch_mask);

	if(the_doscom_user.initialized == true)  return DOSCOM_SUCCESS;

	// 1.- create shared memory object and map it and 0 clean.
	ret = __doscom_kernel_sregion_init(&p_sregion);
	if (ret != 0) return ret;

	// 2.- initialize all fifo channels
	for(i=0; i < DOSCOM_NUM_FIFOCHS; i++) {
		p_fifoch = get_fifoch(i);

#ifdef DOSCOM_NT
		//skip initialize in case of a fifo channel is not assignd
		if ((fifoch_mask & (1 << i)) == 0) {
			p_fifoch->assigned = false;
			continue;
		}
#endif /* DOSCOM_NT */

		// local variables
		p_fifoch->assigned = true;
		p_fifoch->block_size = block_size[i];
		p_fifoch->num_blocks = num_blocks[i];
		p_fifoch->t2nt_fifo.num_elements = p_fifoch->num_blocks + 1;
		p_fifoch->nt2t_fifo.num_elements = p_fifoch->num_blocks + 1;
		// initialize pointers to shared memory
		p_fifoch->t2nt_fifo.p_write  = p_sregion + fifoch_offsets[i];
		p_fifoch->t2nt_fifo.p_read   = p_fifoch->t2nt_fifo.p_write  + 1;
		p_fifoch->t2nt_fifo.p_buffer = p_fifoch->t2nt_fifo.p_read   + 1;
		p_fifoch->nt2t_fifo.p_write  = p_fifoch->t2nt_fifo.p_buffer + p_fifoch->t2nt_fifo.num_elements;
		p_fifoch->nt2t_fifo.p_read   = p_fifoch->nt2t_fifo.p_write  + 1;
		p_fifoch->nt2t_fifo.p_buffer = p_fifoch->nt2t_fifo.p_read   + 1;
		p_fifoch->p_t2nt_filter_id   = (doscom_id_t*)(p_fifoch->nt2t_fifo.p_buffer + p_fifoch->nt2t_fifo.num_elements);
		p_fifoch->p_nt2t_filter_id   = (doscom_id_t*)(p_fifoch->p_t2nt_filter_id   + 1);
		p_fifoch->p_blocks_buffer    = (uint32_t*)(p_fifoch->p_nt2t_filter_id   + 1); 

#ifdef DOSCOM_T
		// initialize 
		__doscom_fifoch_init(i);
#endif /* DOSCOM_T */
	}

	// 2.- initialize all smemchs
	for(i=0; i<DOSCOM_NUM_SMEMCHS; i++) {
		p_smemch = get_smemch(i);
		p_smemch->buffer_size = buffer_size[i];
		p_smemch->p_lock = p_sregion + smemch_offsets[i];
		p_smemch->p_buffer = p_smemch->p_lock + 1;
	}

	__doscom_debug_print_all();

	// 4.- init events and wait for the Destinataion OS to initialize (with a timeout)
	ret = __doscom_kernel_init(fifoch_mask, timeout, p_sregion);
	if (ret != 0) return ret;

	the_doscom_user.initialized = true;

	return DOSCOM_SUCCESS;
}

/*
 * doscom_term
 */
int
doscom_term(void)
{
	CHECK_NOINIT();

	the_doscom_user.initialized = false;
	return __doscom_kernel_term();
}

/*
 * doscom_sense_process
 */
int
doscom_sense_process(uint32_t *p_num_process) {
	CHECK_NOINIT();

	return __doscom_kernel_get_num_process(p_num_process);
}

/*
 * doscom_sense_fifoch
 */
int
doscom_sense_fifoch(doscom_mask_t *p_fifoch_mask) {
	CHECK_NOINIT();

	return __doscom_kernel_get_fifoch_mask(p_fifoch_mask);
}

/*
 * doscom_fifoch_alloc
 */
int
doscom_fifoch_alloc(const doscom_id_t fifoch_id,
								doscom_id_t *p_block_id)
{
	int i, ret;
	doscom_fifoch_t *p_fifoch;
	uint32_t *p_lock;

	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_NULL(p_block_id);
	p_fifoch = get_fifoch(fifoch_id);
	CHECK_ASSIGNED(p_fifoch);

	for(i=0; i < p_fifoch->num_blocks; i++) {
		p_lock = p_fifoch->p_blocks_buffer + i*(1 + (p_fifoch->block_size / sizeof(uint32_t)));
		// atomic: if (*p_lock == 0) *p_lock = 1, ret = true
		ret = __sync_bool_compare_and_swap (p_lock, 0, 1);
		if (ret == false) continue;
		*p_block_id = (doscom_id_t)i;

		__doscom_debug_print_locks(p_fifoch);

		return DOSCOM_SUCCESS;
	}

	__doscom_debug_print_locks(p_fifoch);

	return DOSCOM_EALLOC;
}

/*
 * doscom_fifoch_free
 */
int
doscom_fifoch_free(const doscom_id_t fifoch_id, const doscom_id_t block_id)
{
	doscom_fifoch_t *p_fifoch;
	uint32_t *p_lock;

	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	p_fifoch = get_fifoch(fifoch_id);
	CHECK_ASSIGNED(p_fifoch);
	CHECK_BLOCK_ID(block_id, p_fifoch);

	p_lock = p_fifoch->p_blocks_buffer + block_id*(1 + (p_fifoch->block_size / sizeof(uint32_t)));
	if (*p_lock == 0) return DOSCOM_EALLOC;

	*p_lock = 0;

	__doscom_debug_print_locks(p_fifoch);

	return DOSCOM_SUCCESS;
}

/*
 * doscom_fifoch_get
 */
int
doscom_fifoch_get(const doscom_id_t fifoch_id, const doscom_id_t block_id,
					void **p_block)
{
	doscom_fifoch_t *p_fifoch;
	uint32_t *p_lock;

	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	p_fifoch = get_fifoch(fifoch_id);
	CHECK_ASSIGNED(p_fifoch);
	CHECK_BLOCK_ID(block_id, p_fifoch);

	p_lock = p_fifoch->p_blocks_buffer + block_id*(1 + (p_fifoch->block_size/ sizeof(uint32_t)));
	if (*p_lock == 0) return DOSCOM_EALLOC;

	*p_block = (void *)(p_lock + 1);

	__doscom_debug_print_fifoch(p_fifoch);

	return DOSCOM_SUCCESS;
}

/*
 * doscom_fifoch_enqueue
 */
int
doscom_fifoch_enqueue(const doscom_id_t fifoch_id,
							const doscom_id_t block_id)
{
	doscom_fifoch_t *p_fifoch;
	uint32_t *p_lock;
	int ret;
	doscom_id_t filter_id;
	doscom_filter_t filter;
	void *buffer;

	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	p_fifoch = get_fifoch(fifoch_id);
	CHECK_ASSIGNED(p_fifoch);
	CHECK_BLOCK_ID(block_id, p_fifoch);

	// filtering
	filter_id = GET_SRC2DEST_FILTER_ID(p_fifoch);
	if (filter_id != NULL_FILTER) {
		assert(filter_id < DOSCOM_NUM_SRC2DEST_FILTERS);
		filter = the_doscom_user.filters[filter_id];
		// TODO: check this address
		buffer = p_fifoch->p_blocks_buffer + block_id*(1 + (p_fifoch->block_size/sizeof(uint32_t))) + 1;
		if (filter(buffer, p_fifoch->block_size) == false) return DOSCOM_FILTER;
	}
	p_lock = p_fifoch->p_blocks_buffer + block_id*(1 + (p_fifoch->block_size/ sizeof(uint32_t)));
	if (*p_lock == 0) return DOSCOM_EALLOC;

	__doscom_kernel_lock();
	ret = __doscom_fifo_enqueue(&GET_SRC2DEST_FIFO(p_fifoch), block_id);
	__doscom_kernel_unlock();
	if(ret == -1) return DOSCOM_FULL;

	__doscom_debug_print_fifos(p_fifoch);

	return 0;
}

/*
 * doscom_fifoch_dequeue
 */
int
doscom_fifoch_dequeue(const doscom_id_t fifoch_id,
							doscom_id_t *p_block_id)
{
	doscom_fifoch_t *p_fifoch;
	int ret;
	uint32_t *p_lock;

	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_NULL(p_block_id);
	p_fifoch = get_fifoch(fifoch_id);
	CHECK_ASSIGNED(p_fifoch);

	__doscom_kernel_lock();
	ret = __doscom_fifo_dequeue(&GET_DEST2SRC_FIFO(p_fifoch), (uint32_t*)p_block_id);
	__doscom_kernel_unlock();
	if (ret == -1) return DOSCOM_EMPTY;

	assert(*p_block_id < p_fifoch->num_blocks);

	p_lock = p_fifoch->p_blocks_buffer + *p_block_id*(1 + (p_fifoch->block_size / sizeof(uint32_t)));
	if (*p_lock == 0) return DOSCOM_EALLOC;

	__doscom_debug_print_fifos(p_fifoch);

	return 0;
}

/*
 * doscom_fifoch_notify
 */
int
doscom_fifoch_notify(const doscom_id_t fifoch_id)
{
	int ret;

	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_ASSIGNED(get_fifoch(fifoch_id));
	ret = __doscom_kernel_notify(fifoch_id);
	assert(ret == 0);

	return 0;
}

/*
 * doscom_fifoch_wait
 */
int
doscom_fifoch_wait(const doscom_id_t fifoch_id,
						 const doscom_time_t timeout)
{
	int ret;

	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_ASSIGNED(get_fifoch(fifoch_id));

	if (!__doscom_kernel_check_tskctx()) return DOSCOM_ECTX;

	ret = __doscom_kernel_wait(fifoch_id, timeout);

	return ret;
}

/*
 * doscom_filter_set
 */
int doscom_filter_set(const doscom_id_t fifoch_id,
								const doscom_id_t filter_id)
{
	doscom_fifoch_t *p_fifoch;
	uint32_t def_filter[DOSCOM_NUM_FIFOCHS] = DOSCOM_FIFOCH_DEFAULT_DEST2SRC_FILTERS;

	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_FILTER_ID(filter_id);
	p_fifoch = get_fifoch(fifoch_id);
	CHECK_ASSIGNED(p_fifoch);

	if (filter_id == DEFAULT_FILTER) {
		SET_DEST2SRC_FILTER_ID(p_fifoch, def_filter[fifoch_id]);
	}
	else {
		SET_DEST2SRC_FILTER_ID(p_fifoch, filter_id);
	}

	return DOSCOM_SUCCESS;
}

/*
 * doscom_fifoch_init
 */
int
doscom_fifoch_init(doscom_id_t fifoch_id) {
	CHECK_NOINIT();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_ASSIGNED(get_fifoch(fifoch_id));

	__doscom_kernel_fifoch_init(fifoch_id);

	return DOSCOM_SUCCESS;
}

/*
 * doscom_smemch_get
 */
int
doscom_smemch_get(const doscom_id_t smemch_id, void **p_smem)
{
	doscom_smemch_t *p_smemch;

	CHECK_NOINIT();
	CHECK_SMEMCH_ID(smemch_id);
	p_smemch = get_smemch(smemch_id);

	*p_smem = (void *)(p_smemch->p_buffer);

	return DOSCOM_SUCCESS;
}

/*
 * doscom_smemch_trylock
 */
int
doscom_smemch_trylock(const doscom_id_t smemch_id)
{
	doscom_smemch_t *p_smemch;
	uint32_t *p_lock;
	int ret;

	CHECK_NOINIT();
	CHECK_SMEMCH_ID(smemch_id);
	p_smemch = get_smemch(smemch_id);

	p_lock = p_smemch->p_lock;
	for(;;){
		if (*p_lock == 1) return DOSCOM_LOCKED;
		ret = __sync_bool_compare_and_swap (p_lock, 0, 1);
		if (ret == true) break;
	}

	return DOSCOM_SUCCESS;
}

/*
 * doscom_smemch_unlock
 */
int
doscom_smemch_unlock(const doscom_id_t smemch_id)
{
	doscom_smemch_t *p_smemch;
	uint32_t *p_lock;

	CHECK_NOINIT();
	CHECK_SMEMCH_ID(smemch_id);
	p_smemch = get_smemch(smemch_id);

	p_lock = p_smemch->p_lock;
	if (*p_lock == 0) return DOSCOM_UNLOCKED;
	*p_lock = 0;

	return DOSCOM_SUCCESS;
}

/*
 * Initialization for the doscom system retart.
 */
void
_doscom_restart(void) {
    the_doscom_user.initialized = false;
}
