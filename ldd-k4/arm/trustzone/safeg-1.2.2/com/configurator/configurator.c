/*
 *  TOPPERS/SafeG Dual-OS monitor
 *    Toyohashi Open Platform for Embedded Real-Time Systems/
 *    Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2012-2013 by Embedded and Real-Time Systems Laboratory
 *   Graduate School of Information Science, Nagoya University, JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *    権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *    スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *    用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *    者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *    の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *    用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *    と．
 *  (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *      作権表示，この利用条件および下記の無保証規定を掲載すること．
 *  (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *      報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *    害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *    また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *    由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *    免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 * @(#) $Id: configurator.c 794 2015-09-13 15:21:33Z ertl-honda $  
 */
#include <stdio.h>   /* printf */
#include <string.h>  /* strncpy */
#include <assert.h>  /* assert */
#include <stdint.h>  /* uint32_t */
#include <stdbool.h> /* true, false, bool */
#include <stdlib.h>  /* malloc */
#include <stdarg.h>  /* va_list */
#include <ctype.h>   /* toupper */

#include "linux_list.h" /* list_add_tail */

#define MAX_CHARS 256
#define MAX_PARAM_SIZE_WORDS 4

/*
 * T_OS or NT_OS
 */
#define NUM_OS 2

enum os_t {T_OS = 0, NT_OS = 1};

static char *config_filename[] = {"t_os-doscom_config.h", "nt_os-doscom_config.h"};

#define SREGION_CONTROL_SIZE  (4*3)

/*
 * FILTERS
 */
#define CRE_FILTER(FILTER_NAME, filter_function, in_t_os, in_nt_os) \
	assert(add_doscom_filter(#FILTER_NAME, #filter_function, (in_t_os), (in_nt_os)) == 0);

struct filter_t {
	uint32_t			id[NUM_OS];
	char				filter_name[MAX_CHARS];
	char				function_name[MAX_CHARS];
	bool				is_valid[NUM_OS];
	struct list_head	filter_list;
};

static struct filter_t the_filter_list;
static uint32_t the_num_of_filters[NUM_OS] = {0, 0};

int add_doscom_filter(const char *name, const char *func, bool in_t_os, bool in_nt_os)
{
	struct filter_t *filter;

	if ((in_t_os == false) && (in_nt_os == false)) {
		fprintf(stderr, "Error! : CRE_FILTER(%s, %s, %d, %d) : Both of parameter in_t_os and in_nt_os are false.  \n",
				name, func, in_t_os, in_nt_os);
		exit(1);
	}

	filter = (struct filter_t *)malloc(sizeof(struct filter_t));
	assert(filter != NULL);

	strncpy(filter->filter_name, name, MAX_CHARS);
	strncpy(filter->function_name, func, MAX_CHARS);
	filter->is_valid[T_OS] = in_t_os;
	filter->is_valid[NT_OS] = in_nt_os;

	if (filter->is_valid[T_OS]) filter->id[T_OS] = the_num_of_filters[T_OS]++;
	if (filter->is_valid[NT_OS]) filter->id[NT_OS] = the_num_of_filters[NT_OS]++;

	list_add_tail(&filter->filter_list, &the_filter_list.filter_list);

	return 0;
}

/*
 * FIFO CHANNELS
 */
#define CRE_FIFOCH(FIFOCH_NAME, num_blocks, block_size, t_os_default_filter, nt_os_default_filter) \
	assert(add_doscom_fifoch(#FIFOCH_NAME, (num_blocks), (block_size), #t_os_default_filter, #nt_os_default_filter) == 0);

struct fifoch_t {
	uint32_t			id;
	char				fifoch_name[MAX_CHARS];
	uint32_t			num_blocks;
	uint32_t			block_size;
	char				default_filter[NUM_OS][MAX_CHARS];
	uint32_t			memory_size;
	struct list_head	fifoch_list;
};

static struct fifoch_t the_fifoch_list;
static uint32_t the_num_of_fifochs = 0;

int add_doscom_fifoch(const char *name,
						uint32_t num_blocks,
						uint32_t block_size,
						const char *in_t_os_default_filter,
						const char *in_nt_os_default_filter)
{
	struct fifoch_t *fifoch;

	if (block_size == 0) {
		fprintf(stderr, "Error! : CRE_FIFOCH(%s, %d, %d, %s, %s) : Size of block is 0.  \n",
				name, num_blocks, block_size, in_t_os_default_filter, in_nt_os_default_filter);
		exit(1);
	}

	fifoch = (struct fifoch_t *)malloc(sizeof(struct fifoch_t));
	assert(fifoch != NULL);

	fifoch->id		 = the_num_of_fifochs++;
	fifoch->num_blocks = num_blocks;
	fifoch->block_size = ((block_size & 0x3) == 0x00) ? block_size : ((block_size & ~0x03) + 0x04);
	strncpy(fifoch->fifoch_name, name, MAX_CHARS);
	strncpy(fifoch->default_filter[T_OS], in_t_os_default_filter, MAX_CHARS);
	strncpy(fifoch->default_filter[NT_OS], in_nt_os_default_filter, MAX_CHARS);

	// see doscom-shmem.pdf
	fifoch->memory_size =   8  // T2NT FIFO write and read cursor
						   + 4 // T2NT Filter ID
						   + 8 // NT2T FIFO write and read cursor
						   + 4 // NT2T Filter ID
						   + 4 * (fifoch->num_blocks + 1) // NT2T FIFO queue
						   + 4 * (fifoch->num_blocks + 1) // T2NT FIFO queue
						   + fifoch->num_blocks * (fifoch->block_size + 4); // locks per block

	list_add_tail(&fifoch->fifoch_list, &the_fifoch_list.fifoch_list);

	return 0;
}

/*
 * SMEM CHANNELS
 */
#define CRE_SMEMCH(SMEMCH_NAME, size) \
	assert(add_doscom_smemch(#SMEMCH_NAME, (size)) == 0);

struct smemch_t {
	uint32_t			id;
	char				smemch_name[MAX_CHARS];
	uint32_t			buffer_size;
	uint32_t			memory_size;
	struct list_head	smemch_list;
};

static struct smemch_t the_smemch_list;
static uint32_t the_num_of_smemchs = 0;

int add_doscom_smemch(const char *name,
						  uint32_t buffer_size)
{
	struct smemch_t *smemch;

	if (buffer_size == 0) {
		fprintf(stderr, "Error! : CRE_SMEMCH(%s, %d) : Size of shared memory is 0.  \n", name, buffer_size);
		exit(1);
	}

	smemch = (struct smemch_t *)malloc(sizeof(struct smemch_t));
	assert(smemch != NULL);

	smemch->id		  = the_num_of_smemchs++;
	smemch->buffer_size = ((buffer_size & 0x3) == 0x00) ? buffer_size : ((buffer_size & ~0x03) + 0x04);
	strncpy(smemch->smemch_name, name, MAX_CHARS);

	// see doscom-smemch.pdf
	smemch->memory_size = 4 // locks per smemch
						   + smemch->buffer_size;

	list_add_tail(&smemch->smemch_list, &the_smemch_list.smemch_list);

	return 0;
}

/*
 * CONFIG INIT
 */
void config_initialize(void)
{
	INIT_LIST_HEAD(&the_filter_list.filter_list);
	INIT_LIST_HEAD(&the_fifoch_list.fifoch_list);
	INIT_LIST_HEAD(&the_smemch_list.smemch_list);
#include CONFIG_FILE
}

/*
 * CONFIG GENERATE
 */
void config_generate(enum os_t os)
{
	struct filter_t *filter;
	struct fifoch_t *fifoch;
	struct smemch_t *smemch;
	FILE   *fp;
	uint32_t offset;
	bool match;
	enum os_t dst_os;
	int i;

	fp = fopen(config_filename[os], "w");
	assert(fp != NULL);

	fprintf(fp,"#ifndef DOSCOM_CONFIG_H\n");
	fprintf(fp,"#define DOSCOM_CONFIG_H\n");

	fprintf(fp,"\n#include \"doscom_hw.h\"\n");
	fprintf(fp,"#include \"doscom.h\"\n");

	fprintf(fp,"\nextern void doscom_handler(void);\n");
	/*
	 *  For User
	 */
	fprintf(fp,"\n/* Number of FIFO Channel, Smem Channel, Filter */\n");
	// set number of fifochs
	fprintf(fp,"#define DOSCOM_NUM_FIFOCHS %u\n", the_num_of_fifochs);
	// set number of smemchs
	fprintf(fp,"#define DOSCOM_NUM_SMEMCHS %u\n", the_num_of_smemchs);
	// set number of t2nt filters
	fprintf(fp,"#define DOSCOM_NUM_T2NT_FILTERS %u\n", the_num_of_filters[T_OS]);
	// set number of nt2t filters
	fprintf(fp,"#define DOSCOM_NUM_NT2T_FILTERS %u\n", the_num_of_filters[NT_OS]);

	fprintf(fp,"\n/* ID */\n");
	// set the ids of the fifochs
	list_for_each_entry(fifoch, &the_fifoch_list.fifoch_list, fifoch_list) {
		fprintf(fp,"#define %s %u\n", fifoch->fifoch_name, fifoch->id);
	}

	// set the ids of the smemchs
	list_for_each_entry(smemch, &the_smemch_list.smemch_list, smemch_list) {
		fprintf(fp,"#define %s %u\n", smemch->smemch_name, smemch->id);
	}

	// set the filter ids (for the other os)
	for(i = 0;  i < NUM_OS; i++){
		if (the_num_of_filters[(i+1)%NUM_OS] > 0) {
			list_for_each_entry(filter, &the_filter_list.filter_list, filter_list) {
				if (!filter->is_valid[(i+1)%NUM_OS]) continue;
				fprintf(fp,"#define %s %u\n", filter->filter_name, filter->id[(i+1)%NUM_OS]);
			}
		}
	}

	/*
	 * FIFO Channels
	 */
	fprintf(fp,"\n/* FIFO CHANNELS */\n");

	// set the number of blocks
	fprintf(fp,"#define DOSCOM_FIFOCH_NUM_BLOCKS {");
	if (the_num_of_fifochs > 0) {
		list_for_each_entry(fifoch, &the_fifoch_list.fifoch_list, fifoch_list) {
			if (fifoch->id != (the_num_of_fifochs - 1))
				fprintf(fp,"%u, ", fifoch->num_blocks);
			else
				fprintf(fp,"%u}\n", fifoch->num_blocks);
		}
	}
	else {
		fprintf(fp,"}\n");
	}

	// set the block sizes
	fprintf(fp,"#define DOSCOM_FIFOCH_BLOCK_SIZES {");
	if (the_num_of_fifochs > 0) {
		list_for_each_entry(fifoch, &the_fifoch_list.fifoch_list, fifoch_list) {
			if (fifoch->id != (the_num_of_fifochs - 1))
				fprintf(fp,"%u, ", fifoch->block_size);
			else
				fprintf(fp,"%u}\n", fifoch->block_size);
		}
	}
	else {
		fprintf(fp,"}\n");
	}

	// set semaphores
	if (os == T_OS) {
		fprintf(fp,"#define DOSCOM_FIFOCH_SEMAPHORES {");
		if (the_num_of_fifochs > 0) {
			list_for_each_entry(fifoch, &the_fifoch_list.fifoch_list, fifoch_list) {
				if (fifoch->id != (the_num_of_fifochs - 1))
				  fprintf(fp,"DOSCOM_SEM_FIFOCH_%u, ", fifoch->id);
				else
				  fprintf(fp,"DOSCOM_SEM_FIFOCH_%u}\n", fifoch->id);
			}
		}
		else {
			fprintf(fp,"}\n");
		}
	}

	// set default filters
	dst_os = (os == T_OS ? NT_OS : T_OS);
	if (os == T_OS)
	  fprintf(fp,"#define DOSCOM_FIFOCH_DEFAULT_NT2T_FILTERS {");
	else
	  fprintf(fp,"#define DOSCOM_FIFOCH_DEFAULT_T2NT_FILTERS {");

	if (the_num_of_fifochs > 0) {
		list_for_each_entry(fifoch, &the_fifoch_list.fifoch_list, fifoch_list) {
			// make sure that the default filter exists in the OS
			match = false;
			if(strncmp(fifoch->default_filter[dst_os], "NULL_FILTER", MAX_CHARS) == 0) {
				match = true;
			} else {
				list_for_each_entry(filter, &the_filter_list.filter_list, filter_list) {
					if(strncmp(fifoch->default_filter[dst_os], filter->filter_name, MAX_CHARS) == 0) {
						assert(filter->is_valid[dst_os]);
						match = true;
					}
				}
			}
			if (!match) {
				fprintf(stderr, "Error! : CRE_FIFOCH(%s, %d, %d, %s, %s) : Default filter %s does not exist for NT-OS.\n",
						fifoch->fifoch_name, fifoch->num_blocks, fifoch->block_size,
						fifoch->default_filter[0], fifoch->default_filter[1], fifoch->default_filter[1]);
				exit(1);
			}
			if (fifoch->id != (the_num_of_fifochs - 1))
			  fprintf(fp,"%s, ", fifoch->default_filter[dst_os]);
			else
			  fprintf(fp,"%s}\n",  fifoch->default_filter[dst_os]);
		}
	}
	else {
		fprintf(fp,"}\n");
	}


	if (os == T_OS) {
		dst_os = T_OS;
		fprintf(fp,"#define DOSCOM_FIFOCH_DEFAULT_T2NT_FILTERS {");

		if (the_num_of_fifochs > 0) {
			list_for_each_entry(fifoch, &the_fifoch_list.fifoch_list, fifoch_list) {
				// make sure that the default filter exists in the OS
				match = false;
				if(strncmp(fifoch->default_filter[dst_os], "NULL_FILTER", MAX_CHARS) == 0) {
					match = true;
				} else {
					list_for_each_entry(filter, &the_filter_list.filter_list, filter_list) {
						if(strncmp(fifoch->default_filter[dst_os], filter->filter_name, MAX_CHARS) == 0) {
							assert(filter->is_valid[dst_os]);
							match = true;
						}
					}
				}
				if (!match) {
					fprintf(stderr, "Error! : CRE_FIFOCH(%s, %d, %d, %s, %s) : Default filter %s does not exist for T-OS..\n",
							fifoch->fifoch_name, fifoch->num_blocks, fifoch->block_size,
							fifoch->default_filter[0], fifoch->default_filter[1], fifoch->default_filter[0]);
					exit(1);
				}
				if (fifoch->id != (the_num_of_fifochs - 1))
				  fprintf(fp,"%s, ", fifoch->default_filter[dst_os]);
				else
				  fprintf(fp,"%s}\n",  fifoch->default_filter[dst_os]);
			}
		}
		else {
			fprintf(fp,"}\n");
		}
	}

	/*
	 * SMEM CHANNELS
	 */
	fprintf(fp,"\n/* SMEM CHANELS */\n");

	// set the buffer sizes
	fprintf(fp,"#define DOSCOM_SMEMCH_BUFFER_SIZES {");

	if (the_num_of_smemchs > 0) {
		list_for_each_entry(smemch, &the_smemch_list.smemch_list, smemch_list) {
			if (smemch->id != (the_num_of_smemchs - 1))
				fprintf(fp,"%u, ", smemch->buffer_size);
			else
				fprintf(fp,"%u}\n", smemch->buffer_size);
		}
	}
	else{
		fprintf(fp,"}\n");
	}

	/*
	 * Filters
	 */

	fprintf(fp,"\n/* FILTERS */\n");

	if (the_num_of_filters[os] > 0) {
		// define filter function prototypes if they are valid in the current OS
		list_for_each_entry(filter, &the_filter_list.filter_list, filter_list) {
			if (!filter->is_valid[os]) continue;
			fprintf(fp,"extern %s %s(void *buffer, uint32_t size);\n", (os==T_OS)?"bool_t":"bool",filter->function_name);
		}

		// set the names of the filters
		fprintf(fp,"#define DOSCOM_FILTERS {");
		list_for_each_entry(filter, &the_filter_list.filter_list, filter_list) {
			if (!filter->is_valid[os]) continue;
			if (filter->id[os] != (the_num_of_filters[os] - 1))
				fprintf(fp,"%s, ", filter->function_name);
			else
				fprintf(fp,"%s}\n", filter->function_name);
		}
	}

	/* Shared Regin */
	fprintf(fp,"\n/* SHARED REGION */\n");
	offset = SREGION_CONTROL_SIZE;
	fprintf(fp,"#define DOSCOM_SREGION_FIFOCH_OFFSETS {");
	if (the_num_of_fifochs > 0) {
		list_for_each_entry(fifoch, &the_fifoch_list.fifoch_list, fifoch_list) {
			if (fifoch->id != (the_num_of_fifochs - 1))
				fprintf(fp,"%u, ", offset);
			else
				fprintf(fp,"%u", offset);
			offset = offset + fifoch->memory_size;
		}
	}
	fprintf(fp,"}\n");

	fprintf(fp,"#define DOSCOM_SREGION_PENDING_EVENTS %u\n", offset);
	offset += 8 * the_num_of_fifochs;
	fprintf(fp,"#define DOSCOM_SREGION_SMEMCH_OFFSETS {");
	if (the_num_of_smemchs > 0) {
		list_for_each_entry(smemch, &the_smemch_list.smemch_list, smemch_list) {
			if (smemch->id != (the_num_of_smemchs - 1))
				fprintf(fp,"%u, ", offset);
			else
				fprintf(fp,"%u", offset);
			offset = offset + smemch->memory_size;
		}
	}
	fprintf(fp,"}\n");

	fprintf(fp,"#define DOSCOM_SREGION_SIZE %u\n", offset);

	fprintf(fp,"\n#endif /* DOSCOM_CONFIG_H */\n");
	fclose(fp);
}

void config_cfg(int is_fmp)
{
	FILE   *fp;
	struct fifoch_t *fifoch;

	/*
	 * Create ASP cfg file with semaphores etc.
	 */
	fp = fopen("doscom.cfg", "w");
	assert(fp != NULL);

	fprintf(fp,"#include \"t_os-doscom_config.h\"\n\n");

	fprintf(fp,"DEF_INH(DOSCOM_INTHO, { TA_NULL, doscom_handler });\n");
	fprintf(fp,"CFG_INT(DOSCOM_INTNO, { 0, DOSCOM_INT_PRIORITY });\n\n");

	fprintf(fp,"CRE_SEM(DOSCOM_SEM_INIT, { TA_NULL, 0, 1});\n");
	if (the_num_of_fifochs > 0) {
		list_for_each_entry(fifoch, &the_fifoch_list.fifoch_list, fifoch_list) {
			fprintf(fp,"CRE_SEM(DOSCOM_SEM_FIFOCH_%u, { TA_NULL, 0, 1});\n", fifoch->id);
		}
	}

	if (is_fmp == 1) {
		fprintf(fp,"\nCRE_SPN(DOSCOM_SPN, {TA_NULL});\n");
	}

	fclose(fp);
}

/*
 * MAIN
 */
int
main(int argc, char *argv[])
{
	int i;
	int is_fmp = 0;
	
	for(i = 0; i < argc; i++){
		if(strncmp(argv[i], "-fmp", sizeof("-fmp"))) {
			is_fmp = 1;
		}
	}

	config_initialize();
	config_generate(T_OS);
	config_generate(NT_OS);
	config_cfg(is_fmp);
	return 0;
}
