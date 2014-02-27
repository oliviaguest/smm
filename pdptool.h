#ifndef _pdptool_h_
#define _pdptool_h_


#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "utils.h"
#include "patterns.h"






#define PDPTOOL_BIAS		-2.0

/*
#define PDPTOOL_K 64
#define PDPTOOL_N (PDPTOOL_N_NAME + PDPTOOL_N_VISUAL + PDPTOOL_N_VERBAL) */

#define PDPTOOL_PAIRS (PDPTOOL_N*PDPTOOL_K + PDPTOOL_K*PDPTOOL_K + PDPTOOL_K*PDPTOOL_N)

#define PDPTOOL_UNITS		(PDPTOOL_N + PDPTOOL_K)

// clamping stages

#define PDPTOOL_STAGE_NAME	0
#define PDPTOOL_STAGE_VISUAL	1
#define PDPTOOL_STAGE_VERBAL	2
#define PDPTOOL_STAGE_ALL	3

#define PDPTOOL_SETTLING_THRESHOLD 0.001

// layers to unroll through time
#define PDPTOOL_STEP		4
#define PDPTOOL_T_CLAMPED		(3 * PDPTOOL_STEP)
#define PDPTOOL_T_UNCLAMPED	PDPTOOL_T_CLAMPED + (4 * PDPTOOL_STEP)

#define PDPTOOL_T		(1 + PDPTOOL_T_UNCLAMPED)
#define pdptool_t1		(PDPTOOL_T-1)

#define pdptool_unlesion_weights() pdptool_undo_lesion()
#define pdptool_clamped_pattern_get() pdptool_get_clamped_pattern()
#define pdptool_clamped_stage_get(t) pdptool_get_clamped_stage(t)
void pdptool_init(void);

double pdptool_get_state(int i);
void pdptool_randomise_states(void);


double pdptool_calculate_rms_error(int p);
void pdptool_weights_init(void);

double pdptool_settle_step(int t);

int pdptool_get_clamped_pattern(void);
int pdptool_get_clamped_stage(void);

double pdptool_state_get(int i);
void pdptool_lesion_weights(double percentage);

void pdptool_lesion_random_weight(void);

int pdptool_is_clamped(int i);
void pdptool_unclamp(void);
void pdptool_clamp(int p, int s);
void pdptool_clamp_now(int p, int s);

void pdptool_clamp_name(int p);
void pdptool_clamp_verbal(int p);
void pdptool_clamp_visual(int p);

void pdptool_backup_weights(void);
void pdptool_undo_lesion(void);
int pdptool_stage_to_type(int stage);

int pdptool_load_weights(char *filename);

// double pdptool_error_measure(int error_type, int p);
double pdptool_jaccard_distance(int p);
double pdptool_jaccard_distance_type(int p, int error_t);
double pdptool_jaccard_distance_type_alt(int p, int error_t);
int pdptool_clamped_type_get(void);

void pdptool_print_weight_stats(void);
void pdptool_disrupt_weights(double noise);
#endif
