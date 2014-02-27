#ifndef _bptt_h_
#define _bptt_h_

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"
#include "patterns.h"
#include "error.h"


int bptt_iterations_so_far;

#define BPTT_WEIGHT_UPDATE_STAGE 0
#define BPTT_WEIGHT_UPDATE_PATTERN 1
#define BPTT_WEIGHT_UPDATE_BATCH   2


//Comment from rogers.m in PDPTOOL: Rogers et al report lrate of .005 but this simulator does not converge with that value.
// Email from Tim Rogers' "I used a learning rate of 0.005 with a decay rate of 0.001."
#define BPTT_LEARNING_RATE 0.005
double bptt_learning_rate;

#define BPTT_DECAY_RATE (1.0-0.0002)
//was 0.999
double bptt_decay_rate;

#define BPTT_C_MAX		ERROR_I_MAX
int BPTT_C;






#define BPTT_PAIRS (BPTT_N*BPTT_K + BPTT_K*BPTT_K + BPTT_K*BPTT_N)

// number of training iterations (in addition to c_min)
// settrainopts ('lrate',0.001,'momentum',0,'nepochs',500,'trainmode','ptrain');
// Rogers et al report lrate of .005 but this simulator does not converge with that value.
// As in Rogers et al, wdecay is set to x/npatterns (npatterns = 144) x = .001
// settrainopts ('wdecay',.0000069444,'wrange',0.25,'lgrain','pattern','errmargin',0.05);
// Wrange and errmarin values were not specified in Rogers et al.
// That paper did not specifically indicate that cross-entropy error was used, but this runs best using cross entropy error.
/*
int step_number;

int pattern_number;*/

// int clamped_pattern;
// int clamped_stage;

// int stage;


// input bias
#define BPTT_BIAS		-2.0



// clamping stages

#define BPTT_STAGE_NAME		0
#define BPTT_STAGE_VISUAL	1
#define BPTT_STAGE_VERBAL	2
#define BPTT_STAGE_ALL		3

// number of states
#define BPTT_S		3



// // number of visible units
#define BPTT_N		(BPTT_N_NAME + BPTT_N_VISUAL + BPTT_N_PERCEPTUAL + BPTT_N_FUNCTIONAL + BPTT_N_ENCYCLOPEDIC)

// number of hidden units
#define BPTT_K		64

// all units
#define BPTT_UNITS		(BPTT_N + BPTT_K)

// layers to unroll through time
#define BPTT_STEP		4
// RPC THINKS IT SHOULD BE CLAMPED FOR THE WHOLE LEARNING PHASE, BUT THE PAPER
// SUGGESTS OTHERWISE...
#define BPTT_T_CLAMPED_MAX		(7 * BPTT_STEP)
#define BPTT_T_CLAMPED_ROGERS		(3 * BPTT_STEP)
// #define BPTT_T_UNCLAMPED	BPTT_T_CLAMPED + (0 * BPTT_STEP)
//#define BPTT_T_CLAMPED		(3 * BPTT_STEP)
//#define BPTT_T_UNCLAMPED	BPTT_T_CLAMPED + (4 * BPTT_STEP)

int BPTT_T_CLAMPED;
#define BPTT_TARGETS_CLAMPED (2 * BPTT_STEP)
#define BPTT_T		(7 * BPTT_STEP + 1)
#define bptt_t1		(BPTT_T-1)

#define bptt_clamped_pattern_get() bptt_get_clamped_pattern()
#define bptt_clamped_stage_get() bptt_get_clamped_stage()

#define bptt_get_state(i, t) bptt_state_get(i, t)
#define bptt_unlesion_weights() bptt_undo_lesion()



#define BPTT_SETTLING_THRESHOLD 0.001
// used to be 0.001




double bptt_random_state(void);
double bptt_random_weight(void);

// double bptt_get_state(int i);
double bptt_get_state(int i, int t);

void bptt_throw_error(char file[], int line, const char func[], char text[], int num, ...);

void bptt_decay_weights(void);
void bptt_zero_delta_weights(void);

void bptt_randomise_states(int t);
void bptt_randomise_states_all(void);

double bptt_update_states(int t);

// char *get_name_string(int p);


// File Handling
void bptt_save_file(char *filename);
int bptt_open_file(char *filename);

// void bptt_init_random(void);

void bptt_init_deltas(void);

void bptt_init_targets_clamped(void);

void bptt_unclamp_time(int t);
void bptt_propagate(int p, int s);
double bptt_propagate_timestep(int p, int s, int t);
// bptt.c
int bptt_get_clamped_pattern(void);
int bptt_get_clamped_stage(void);

void bptt_init(void);
void bptt_adjust_weights(void);
// double bptt_step(void);
double bptt_unclamped_state(int i, int t);
// void bptt_settle(int p, int s);
void bptt_accumulate_deltas(int p);
void bptt_williams_accumulate_deltas(int p);

double bptt_net_error_sse(double bptt_d, double bptt_y);
double bptt_net_error_cross_entropy(double bptt_d, double bptt_y);
void bptt_one_timestep(void);
double bptt_calculate_rms_error(int p, int t);
void bptt_clamp(int p, int s, int t);
void bptt_init_weights(void);
int bptt_is_clamped(int i, int t);
void bptt_unclamp_all(void);
void bptt_clamp_all(int p, int s);
void bptt_clamp_all_stages(int p, int t);
void bptt_clamp_now(int p, int s, int t);
void bptt_init_delta_weights(void);
double bptt_get_delta(int i, int t);
double bptt_get_epsilon(int i, int t);
/*int bptt_random_int(int min, int max);*/
int errors_updated, deltas_updated;
double bptt_settle_step(int t);
void bptt_accumulate_deltas_williams(int p);
void bptt_accumulate_deltas_williams_ta(int p);
void bptt_accumulate_deltas_mcclleland(int p);
void bptt_accumulate_deltas_mcclleland_ta(int p);
void bptt_open_patterns(char *filename);
// int bptt_clamped_stage_get(void);
void bptt_lesion_weights(double bptt_percentage);

// patterns.c
// int pattern_general(int p);
// double bptt_get_pattern(int p, int i);
// double bptt_get_pattern_type(int p, int t, int i);

// int i0_of(int type);
// int type_of(int i);
//
//void bptt_constrain_weights(void);
void bptt_randomise_patterns(void);



void bptt_lesion_random_weight(void);
void bptt_init_naming_response(int d, int l, int s);
void bptt_clamp_visual(int p, int t);
void bptt_clamp_verbal(int p, int t);
void bptt_save_naming_response(int p, int sample, int level, int distribution);

void bptt_print_naming_responses(void);

void bptt_init_drawing_and_copying_response(int d, int l, int s);
void bptt_init_sorting_response(int d, int l, int s);
void bptt_save_sorting_given_visual_response(int p, int sample, int level, int distribution);
void bptt_clamp_name(int p, int t);
void bptt_save_sorting_given_name_response(int p, int sample, int level, int distribution);
void bptt_print_sorting_responses(void);
void bptt_save_drawing_and_copying_responses(char *filename);
void bptt_open_drawing_and_copying_responses(char *filename);
// void bptt_save_attractor(int p, double bptt_array[]);
// void bptt_distance_between_current_and_target_attractor(int p, double bptt_array[], double bptt_*distance);
// void bptt_get_close_distractors(int p, int array[], int *length);
// void bptt_get_distant_distractors(int p, int array[], int *length);
// void bptt_get_unrelated_distractors(int p, int array[], int *length);
// void bptt_init_drawing_response(void);
void bptt_print_drawing_and_delayed_copying_responses(void);
void bptt_save_copying_response(int p, int sample, int level, int distribution);
void bptt_save_drawing_response(int p, int sample, int level, int distribution);
/*int is_general_name(int p);*/
// int get_domain(int p);


void bptt_get_close_distractors(int p, int *array, int *length);
void bptt_get_distant_distractors(int p, int *array, int *length);
void bptt_get_unrelated_distractors(int p, int *array, int *length);
void bptt_save_target_attractor(int p, int sample, int level, int distribution);

void bptt_save_naming_responses(char *filename);
void bptt_backup_weights(void);
void bptt_undo_lesion(void);

void bptt_disrupt_weights(double noise);

void bptt_open_naming_responses(char *filename);
void bptt_open_sorting_responses(char *filename);

void bptt_init_word_to_picture_response(int d, int l, int s);;
void bptt_open_word_to_picture_responses(char *filename);
void bptt_print_word_to_picture_responses(void);
void bptt_save_word_to_picture_response_target(int p, int sample, int level, int distribution);
void bptt_save_word_to_picture_response_unrelated_distractor(int p, int q, int sample, int level, int distribution);
void bptt_save_word_to_picture_response_distant_distractor(int p, int q, int sample, int level, int distribution);
void bptt_save_word_to_picture_response_close_distractor(int p, int q, int sample, int level, int distribution);
void bptt_save_word_to_picture_responses(char *filename);


// int is_fruit(int p);
// int get_n_fruit(void);

// int get_domain_n_pattern(int domain);
// int excluded_pattern(int p);

void bptt_save_sorting_response_given_name(int p, int sample, int level, int distribution);
void bptt_save_sorting_response_given_visual(int p, int sample, int level, int distribution);
void bptt_save_sorting_responses(char *filename);

// enum category get_category(int p);
// double bptt_error_measure(int error_type, int p);
// void bptt_get_n_close_trials(int *n);
// void bptt_get_n_distant_trials(int *n);
// void bptt_get_n_unrelated_trials(int *n);
double bptt_jaccard_distance(int p, int t);
double bptt_jaccard_distance_type(int p, int input_t, int error_t);

double bptt_sorensen_distance(int p, int t);

int bptt_get_iterations_so_far(void);
void bptt_inc_iterations_so_far(void);

int bptt_stage_to_type(int stage);
int bptt_clamped_type_get(void);
void bptt_print_weight_stats(void);
double bptt_update_states_targets_clamped(int t);
void bptt_get_weight_stats(double *array);
double bptt_update_states_gaussian_noise(int t);
#endif
