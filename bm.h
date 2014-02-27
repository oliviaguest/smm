#ifndef _bm_h_
#define _bm_h_

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "utils.h"
#include "error.h"
#include "patterns.h"



int bm_iterations_so_far;


// values for states
#define BM_OFF		0
#define BM_ON		1

#define BM_BIAS		9

#define BM_STATE	8


#define    bm_clamped_type_get() bm_get_clamped_type()


// free/clamped states

#define BM_MINUS_NAME		0
#define BM_MINUS_VISUAL		1
#define BM_MINUS_VERBAL		2
#define BM_PLUS			3

#define BM_PLUS_NAME		4
// #define BM_PLUS_VERBAL          5
// #define BM_PLUS_VISUAL		6

#define BM_DELTA		6
// clamping stages     for error

// #define BM_STAGE_NAME	0
// #define BM_STAGE_VISUAL	1
// #define BM_STAGE_VERBAL	2
// #define BM_STAGE_ALL	3

// number of states
#define BM_S		4

// // number of visible units
#define BM_N		(BM_N_NAME + BM_N_VISUAL + BM_N_PERCEPTUAL + BM_N_FUNCTIONAL + BM_N_ENCYCLOPEDIC)

// number of hidden units
#define BM_K		64

// // number of patterns
// #define BM_P_MAX		(N_CATEGORY * N_SUBCATEGORY)
// int BM_P;

// all units
#define BM_UNITS		(BM_N + BM_K)


#define BM_DECAY_RATE 0.999



#define BM_SETTLING_THRESHOLD 0.000001

// temperature
#define BM_T_MAX		100.0
#define BM_T_MIN		0.001
#define BM_T_RATE		0.95

// rate of learning
// #define R_MAX	0.05
// #define R_MIN	0.001
// #define R_MIN	0.01
#define BM_R_RATE	0.99
#define BM_R_MAX	0.05//0.1//2.0
#define BM_R_MIN	0.002
#define BM_R_STEP	0.001
// #define R_RATE	1
double bm_r;// = R_MAX;

// connection weights
#define BM_VH		(BM_N * BM_K) /* number of visible <-> hidden pairs */
#define BM_HH		(BM_K * BM_K - BM_K) / 2 /* number of hidden <-> hidden pairs */
#define BM_PAIRS	(BM_VH + BM_HH) /* all pairs */

#define BM_CLAMPED		-1

#define BM_ERRORS		1

// weight step
#define BM_STEP		0.01

// number of iterations for collecting cooccurence stats
#define BM_C_0		100
int BM_C;





#define BM_I_MAX		ERROR_I_MAX
int BM_I;

#define bm_clamped_stage_get() bm_get_clamped_stage()
#define bm_unlesion_weights() bm_undo_lesion()

/*
double BM_Total_Error[BM_I_MAX];
double BM_Error[3][BM_I_MAX];
double BM_Pattern_Error[3][P_MAX], BM_Total_Pattern_Error[P_MAX], BM_Total_Stage_Error[3];
                                                                                           */
int bm_training; // flag, if non-zero network is being trained
int bm_weights_updated; // flag, if non-zero weights have been updated BUT NOT had their stats calculated
int bm_biases_updated; // same as above but for biases

// indices for random access
int bm_random_index[BM_UNITS];

// clamped flags
int bm_type_clamped;

// array for storing difference between plus and minus states
// double delta_prob[PAIRS + UNITS];


//for stats result, used in stats.c
struct bm_output_stats
{
    double hit;
    double miss;
    // 	int total;
};
struct bm_weight_stats
{
    double pos_average;
    double neg_average;
    double max;
    double min;
};

double bm_decay_rate;


int bm_get_iterations_so_far(void);
void bm_inc_iterations_so_far(void);
// defined in stats.c
int bm_get_miss(int p, int t);
struct bm_weight_stats bm_calculate_weight_average(void);
struct bm_weight_stats bm_calculate_bias_average(void);
double bm_average_delta_prob(void);
double bm_abs_average_delta_prob(void);
double bm_abs_average_delta_prob_step(void);

// defined in bm.c
void bm_train(void);
void bm_init(void);
void bm_anneal(void);
void bm_adjust_weights(int t); // t is type being trained
void bm_adjust_weights_step(int t);
void bm_adjust_weights_step_all(void);
void bm_adjust_weights_step_all_pdptool(void);
void bm_adjust_weights_name(void);
void bm_flip_units(double temperature);//, int t);
double bm_flip_units_delta(double temperature);//, int t)

int bm_valid_count_type(int t);

// defined in utils.c
char* itoa(int value, char* result, int base);

void bm_reset_learning_rate(void);

void bm_start_timing(void);
void bm_stop_timing(void);

void bm_init_states(void);
void bm_init_biases(void);
void bm_init_weights(void);
void bm_init_random(void);

void bm_init_counts(void);
void bm_init_count_plus(void);
void bm_init_count_minus_name(void);
void bm_init_count_minus_visual(void);
void bm_init_count_minus_verbal(void);
void bm_init_count(int t);

void bm_collect_count_plus(void);
void bm_collect_count_minus_name(void);
void bm_collect_count_minus_visual(void);
void bm_collect_count_minus_verbal(void);
void bm_collect_count(int t);

void bm_save_weights(char *filename);
int bm_load_weights(char *filename);


int bm_random_int(int min, int man);
double bm_random_double(double min, double max);

int bm_random_state(void);
double random_weight(void);

void bm_randomise_states(void);
void bm_randomise_indices(void);

int bm_valid_unit_type(int t);
int bm_valid_index(int i, int max_i);
int bm_valid_state(int state);

int bm_pattern_general(int p);

int bm_get_state_type(int i, int t);
int bm_get_state(int i);
int bm_get_visible(int i);
int bm_get_hidden(int i);
int bm_get_pattern(int p, int i);
int bm_get_pattern_type(int p, int t, int i);
double bm_get_count(int i, int t);
double bm_get_delta(int i);
double bm_get_bias(int i);
double bm_get_weight(int i);

void bm_put_state(int i, int s);
void bm_put_count(int i, double c, int t);
void bm_put_delta(int i, double d);
void bm_put_weight(int i, double w);
void bm_put_bias(int i, double w);

void bm_inc_count(int i, int c, int t);
void bm_inc_weight(int i, double w);
void bm_inc_weight_step(int k);
void bm_inc_bias(int i, double w);
void bm_inc_bias_step(int k);

void bm_unclamp(void);
void bm_clamp(int p, int t);
void bm_clamp_visible(int p);
void bm_clamp_name(int p);
void bm_clamp_visual(int p);
void bm_clamp_verbal(int p);

int bm_clamped(int t);
int bm_clamped_index(int i);

int bm_type_of(int i);
int bm_N_of(int t);
int bm_i0_of(int t);

// int is_on(int i);

int bm_map_vh(int i, int j);
int bm_map_vh_type(int i, int j, int t);
int bm_map_hh(int i, int j);

// int pam_vh_j(int k);
// int pam_vh_i(int k);
// int pam_hh_i(int k);
// int pam_hh_j(int k);

void bm_divide_weights(double n);

void bm_error(char file[], char func[], char text[], double i);
void bm_save_weights_backup(int i);

void bm_lesion_random_weight(void);
void bm_disrupt_random_weight(double noise);
int bm_category(int i);
int bm_domain(int i);
void bm_copy_weights(void);
void bm_undo_lesion(void);

int bm_get_i_total(void);

int bm_get_p_clamped(void);

int *bm_get_base_ps(int p);

int bm_error_type(int target_p, int t, int miss);
int bm_network_state_to_unit_type(int s);
int bm_type_to_state(int t);
char *bm_get_name(int p);
char *bm_get_phase(int t);

double bm_calculate_rms_error(int p, int t);

int bm_get_clamped_pattern(void);
double bm_jaccard_distance(int p, int t);
double bm_jaccard_distance_type(int p, int input_t, int error_t);

int bm_get_p_clamped(void);
// double bm_error_measure(int error_type, int p);
int bm_get_clamped_stage(void);
int bm_get_clamped_type(void);
void bm_backup_weights(void);
void bm_lesion_weights(double percent);
void bm_clamp_pdptool_name_plus(int p);
void bm_clamp_pdptool_visual_verbal_plus(int p);
double bm_euclidean_distance_type(int p, int input_t, int error_t);
double bm_sorensen_distance(int p, int t);
char *bm_stage_string_get(int s);
void bm_print_weight_stats(void);
void bm_get_weight_stats(double *array);

void bm_disrupt_weights(double noise);

void bm_anneal_tediously_slowly(void);
double bm_calculate_rms_error_type(int p, int t, int type_for_error);
int bm_compute_output_interpretation_type(int error_type, int t, int type_for_error);
void   bm_decay_learning_rate(void);


void bm_decay_weights(void);

#endif
