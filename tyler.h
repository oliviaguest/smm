#ifndef _tyler_h_
#define _tyler_h_

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


int tyler_iterations_so_far;

#define TYLER_WEIGHT_UPDATE_STAGE 0
#define TYLER_WEIGHT_UPDATE_PATTERN 1
#define TYLER_WEIGHT_UPDATE_BATCH   2


//Comment from rogers.m: Rogers et al report lrate of .005 but this simulator does not converge with that value.

#define TYLER_LEARNING_RATE 0.025
double tyler_learning_rate;

#define TYLER_DECAY_RATE 0.999
double tyler_decay_rate;

#define TYLER_C_MAX		ERROR_I_MAX
int TYLER_C;


int tyler_feedforward_prev_weights_initialised;




#define TYLER_PAIRS (TYLER_N*TYLER_K + TYLER_K*TYLER_K + TYLER_K*TYLER_N)

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
#define TYLER_BIAS		-2.0
#define TYLER_FEEDFORWARD_BIAS		-1.0



// clamping stages

#define TYLER_STAGE_DISTINCTIVE		0
#define TYLER_STAGE_SHARED		1
#define TYLER_STAGE_FUNCTIONAL		2
#define TYLER_STAGE_ALL			3

// number of states
#define TYLER_S		3


// // number of visible units
#define TYLER_N		(TYLER_N_DISTINCTIVE + TYLER_N_SHARED + TYLER_N_FUNCTIONAL)

// number of hidden units
#define TYLER_K		20

// all units
#define TYLER_UNITS		(TYLER_N + TYLER_K)

#define TYLER_FEEDFORWARD_IN TYLER_N
#define TYLER_FEEDFORWARD_HIDDEN TYLER_K
#define TYLER_FEEDFORWARD_OUT TYLER_N


// Training types for Tyler FF
#define Olivia		0
#define Olivia_alt	1
#define Rick 		2
#define Rick_alt	3

#define TYLER_FEEDFORWARD_N (TYLER_FEEDFORWARD_IN+TYLER_FEEDFORWARD_OUT)
#define TYLER_FEEDFORWARD_K TYLER_FEEDFORWARD_HIDDEN
#define TYLER_FEEDFORWARD_UNITS		(TYLER_FEEDFORWARD_IN + TYLER_FEEDFORWARD_HIDDEN + TYLER_FEEDFORWARD_OUT)
#define TYLER_FEEDFORWARD_MOMENTUM_MAX 0.9
#define TYLER_FEEDFORWARD_MOMENTUM TYLER_FEEDFORWARD_MOMENTUM_MIN
#define TYLER_FEEDFORWARD_MOMENTUM_MIN 0.0
#define TYLER_FEEDFORWARD_PAIRS (TYLER_FEEDFORWARD_IN*TYLER_FEEDFORWARD_HIDDEN + TYLER_FEEDFORWARD_HIDDEN*TYLER_FEEDFORWARD_OUT)

int tyler_feedforward_learned_biases;
double tyler_feedforward_momentum;

#define TYLER_FEEDFORWARD_LEARNING_RATE 0.05
double tyler_feedforward_learning_rate;

// layers to unroll through time
#define TYLER_STEP		4
// RPC THINKS IT SHOULD BE CLAMPED FOR THE WHOLE LEARNING PHASE, BUT THE PAPER
// SUGGESTS OTHERWISE...
#define TYLER_T_CLAMPED_MAX		(7 * TYLER_STEP)
#define TYLER_T_CLAMPED_ROGERS		(3 * TYLER_STEP)
// #define TYLER_T_UNCLAMPED	TYLER_T_CLAMPED + (0 * TYLER_STEP)
//#define TYLER_T_CLAMPED		(3 * TYLER_STEP)
//#define TYLER_T_UNCLAMPED	TYLER_T_CLAMPED + (4 * TYLER_STEP)

int TYLER_T_CLAMPED;

#define TYLER_T		(7 * TYLER_STEP + 1)
#define tyler_t1		(TYLER_T-1)

#define tyler_clamped_pattern_get() tyler_get_clamped_pattern()
#define tyler_clamped_stage_get() tyler_get_clamped_stage()

#define tyler_get_state(i, t) tyler_state_get(i, t)
#define tyler_unlesion_weights() tyler_undo_lesion()

#define tyler_feedforward_clamped_pattern_get() tyler_feedforward_get_clamped_pattern()
#define tyler_feedforward_clamped_stage_get() tyler_feedforward_get_clamped_stage()

#define tyler_feedforward_state_get(i) tyler_feedforward_get_state(i)
#define tyler_feedforward_undo_lesion() tyler_feedforward_unlesion_weights()

#define tyler_load_weights(filename) tyler_open_file(filename)
#define tyler_feedforward_load_weights(filename) tyler_feedforward_open_file(filename)

#define tyler_clamped_type_get() tyler_get_clamped_type()
#define tyler_feedforward_clamped_type_get() tyler_feedforward_get_clamped_type()







#define TYLER_SETTLING_THRESHOLD 0.001
// used to be 0.001

#define TYLER_LESION_GLOBAL		0
#define TYLER_LESION_IN_HIDDEN		1
#define TYLER_LESION_HIDDEN_OUT		2
#define TYLER_LESION_HIDDEN_HIDDEN	3


// double tyler_random_state(void);
double tyler_random_weight(void);

// double tyler_get_state(int i);
double tyler_get_state(int i, int t);

void tyler_throw_error(char file[], int line, const char func[], char text[], int num, ...);

void tyler_decay_weights(void);
void tyler_zero_delta_weights(void);

void tyler_randomise_states(int t);
void tyler_randomise_states_all(void);

double tyler_update_states(int t);

// char *get_name_string(int p);


// File Handling
void tyler_save_file(char *filename);
int tyler_open_file(char *filename);

// void tyler_init_random(void);

void tyler_init_deltas(void);

void tyler_lesion_weights_locus(double percentage, int locus);
void tyler_feedforward_lesion_weights_locus(double percentage, int locus);

void tyler_unclamp_time(int t);
void tyler_propagate(int p, int s);
double tyler_propagate_timestep(int p, int s, int t);
// tyler.c
int tyler_get_clamped_pattern(void);
int tyler_get_clamped_stage(void);

void tyler_init(void);
void tyler_adjust_weights(void);
// double tyler_step(void);
double tyler_unclamped_state(int i, int t);
// void tyler_settle(int p, int s);
void tyler_accumulate_deltas(int p);
void tyler_williams_accumulate_deltas(int p);

double tyler_net_error_sse(double tyler_d, double tyler_y);
double tyler_net_error_cross_entropy(double tyler_d, double tyler_y);
void tyler_one_timestep(void);
// double tyler_calculate_rms_error(int p);
void tyler_clamp(int p, int s, int t);
void tyler_init_weights(void);
int tyler_is_clamped(int i, int t);
void tyler_unclamp_all(void);
void tyler_clamp_all(int p, int s);
void tyler_clamp_now(int p, int s, int t);
void tyler_init_delta_weights(void);
double tyler_get_delta(int i, int t);
double tyler_get_epsilon(int i, int t);
/*int tyler_random_int(int min, int max);*/
int errors_updated, deltas_updated;
double tyler_settle_step(int t);
void tyler_accumulate_deltas_williams(int p);
void tyler_accumulate_deltas_williams_ta(int p);
void tyler_accumulate_deltas_mcclleland(int p);
void tyler_accumulate_deltas_mcclleland_ta(int p);
void tyler_open_patterns(char *filename);
// int tyler_clamped_stage_get(void);
void tyler_lesion_weights(double tyler_percentage);

void tyler_clamp_full_pattern(int p, int t);

void tyler_clamp_full_pattern_now(int p, int t);

// patterns.c
// int pattern_general(int p);
// double tyler_get_pattern(int p, int i);
// double tyler_get_pattern_type(int p, int t, int i);

// int i0_of(int type);
// int type_of(int i);
//
//void tyler_constrain_weights(void);
void tyler_randomise_patterns(void);


void tyler_lesion_random_weight(void);
void tyler_init_naming_response(int d, int l, int s);
void tyler_clamp_visual(int p, int t);
void tyler_clamp_verbal(int p, int t);
void tyler_save_naming_response(int p, int sample, int level, int distribution);

void tyler_print_naming_responses(void);

void tyler_init_drawing_and_copying_response(int d, int l, int s);
void tyler_init_sorting_response(int d, int l, int s);
void tyler_save_sorting_given_visual_response(int p, int sample, int level, int distribution);
void tyler_clamp_name(int p, int t);
void tyler_save_sorting_given_name_response(int p, int sample, int level, int distribution);
void tyler_print_sorting_responses(void);
void tyler_save_drawing_and_copying_responses(char *filename);
void tyler_open_drawing_and_copying_responses(char *filename);

void tyler_print_drawing_and_delayed_copying_responses(void);
void tyler_save_copying_response(int p, int sample, int level, int distribution);
void tyler_save_drawing_response(int p, int sample, int level, int distribution);
/*int is_general_name(int p);*/
// int get_domain(int p);


void tyler_get_close_distractors(int p, int *array, int *length);
void tyler_get_distant_distractors(int p, int *array, int *length);
void tyler_get_unrelated_distractors(int p, int *array, int *length);
void tyler_save_target_attractor(int p, int sample, int level, int distribution);

void tyler_save_naming_responses(char *filename);
void tyler_backup_weights(void);
void tyler_undo_lesion(void);

void tyler_disrupt_weights(double noise);

void tyler_open_naming_responses(char *filename);
void tyler_open_sorting_responses(char *filename);

void tyler_init_word_to_picture_response(int d, int l, int s);;
void tyler_open_word_to_picture_responses(char *filename);
void tyler_print_word_to_picture_responses(void);
void tyler_save_word_to_picture_response_target(int p, int sample, int level, int distribution);
void tyler_save_word_to_picture_response_unrelated_distractor(int p, int q, int sample, int level, int distribution);
void tyler_save_word_to_picture_response_distant_distractor(int p, int q, int sample, int level, int distribution);
void tyler_save_word_to_picture_response_close_distractor(int p, int q, int sample, int level, int distribution);
void tyler_save_word_to_picture_responses(char *filename);


// int is_fruit(int p);
// int get_n_fruit(void);

// int get_domain_n_pattern(int domain);
// int excluded_pattern(int p);

void tyler_save_sorting_response_given_name(int p, int sample, int level, int distribution);
void tyler_save_sorting_response_given_visual(int p, int sample, int level, int distribution);
void tyler_save_sorting_responses(char *filename);

// enum category get_category(int p);
// double tyler_error_measure(int error_type, int p);
// void tyler_get_n_close_trials(int *n);
// void tyler_get_n_distant_trials(int *n);
// void tyler_get_n_unrelated_trials(int *n);
double tyler_jaccard_distance(int p);
double tyler_jaccard_distance_type(int p, int error_t);
// double tyler_calculate_rms_error_type(int p, int error_t);
// double tyler_calculate_fabs_error_type(int p, int error_t);
// double tyler_calculate_rss_error_type(int p, int error_t);
double tyler_sorensen_distance(int p, int t);
void tyler_set_target(int p, int s, int t);

int tyler_get_iterations_so_far(void);
void tyler_inc_iterations_so_far(void);


// double tyler_euclidean_distance(int p);
void tyler_feedforward_init(void);
double tyler_feedforward_get_state(int i) ;
void tyler_feedforward_unclamp_all(void);
void tyler_feedforward_unclamp(void);
void tyler_feedforward_clamp(int p, int s);
void tyler_feedforward_set_target(int p, int s);
void tyler_feedforward_clamp_now(int p, int s);
void tyler_feedforward_inc_iterations_so_far(void);
void tyler_feedforward_unclamp_all(void);
void tyler_feedforward_unclamp(void);
double tyler_feedforward_calculate_rms_error_type_noise(int p, int error_t, int noise, int n);
double tyler_feedforward_calculate_rss_error_type_noise(int p, int error_t, int noise, int n) ;
double tyler_feedforward_euclidean_distance_noise(int p, int noise, int n);
double tyler_feedforward_calculate_fabs_error_type_noise(int p, int error_t, int noise, int n);

void tyler_feedforward_clamp(int p, int s);

void tyler_feedforward_set_target(int p, int s);

void tyler_feedforward_clamp_now(int p, int s);
void tyler_feedforward_randomise_states(void);

void tyler_feedforward_clamp_full_pattern(int p);
void tyler_feedforward_clamp_full_pattern_now(int p);

void tyler_feedforward_clamp_distinctive(int p);
void tyler_feedforward_clamp_shared(int p);

void tyler_feedforward_clamp_functional(int p);
// double tyler_feedforward_propagate(void);
void tyler_feedforward_backup_weights(void);

void tyler_feedforward_init_prev_weights(void);

void tyler_feedforward_accumulate_delta_weights(void);
void tyler_feedforward_accumulate_delta_weights_rick(void);


void tyler_feedforward_save_file(char *filename);


int tyler_feedforward_open_file(char *filename);


double tyler_feedforward_sorensen_distance(int p, int t);


double tyler_feedforward_jaccard_distance(int p);

// double tyler_feedforward_calculate_rms_error_type(int p, int error_t);


void tyler_feedforward_init_weights(void);

void tyler_feedforward_init_delta_weights(void);

int tyler_feedforward_is_clamped(int i);


void tyler_feedforward_lesion_random_weight(void);

void tyler_feedforward_lesion_weights(double percentage);

void tyler_feedforward_disrupt_weights(double noise);
void tyler_feedforward_unlesion_weights(void);

int tyler_feedforward_get_clamped_pattern(void);
int tyler_feedforward_get_iterations_so_far(void);
void tyler_feedforward_adjust_weights(void);
int tyler_feedforward_get_clamped_stage(void);
// double tyler_feedforward_calculate_fabs_error_type(int p, int error_t);
double tyler_feedforward_euclidean_distance(int p);
int tyler_get_clamped_type(void);
int tyler_feedforward_get_clamped_type(void);

double tyler_feedforward_propagate(void);
double tyler_feedforward_propagate_type(int t);
void tyler_feedforward_lesion_random_ho_weight(void);
void tyler_feedforward_lesion_random_ih_weight(void);


void tyler_lesion_random_ho_weight(void);
void tyler_lesion_random_hh_weight(void);
void tyler_lesion_random_ih_weight(void);

void tyler_feedforward_adjust_weights_alt(void);

double tyler_feedforward_fabs_average_weight(int locus);
int tyler_stage_to_type(int stage);
double tyler_feedforward_fabs_average_weight(int locus);
void tyler_feedforward_disrupt_weights_locus(double noise, int lesion_locus);
double tyler_fabs_average_weight(int locus);
int tyler_feedforward_open_dat_file(char *filename);
double tyler_feedforward_calculate_rss_error_type(int p, int error_t);

void tyler_feedforward_adjust_weights_type(int training_type);
void tyler_feedforward_adjust_weights(void);
void      tyler_feedforward_adjust_weights_alt(void);
void  tyler_feedforward_adjust_weights_rick(void);
void  tyler_feedforward_adjust_weights_rick_alt(void);
int tyler_feedforward_open_OXweights_file(char *filename);

void tyler_feedforward_accelerate_momentum(void);


void tyler_noisy_patterns_init(void);


void tyler_feedforward_clamp_now_noise(int a, int b, int c, int d);
double tyler_feedforward_propagate_type_noise(int a, int b, int c);
double tyler_feedforward_propagate_noise(int a, int b);
void tyler_clamp_now_noise(int p, int s, int t, int noise, int n);
double tyler_update_states_noise(int t, int noise, int n);
double tyler_settle_step_noise(int t, int noise, int n);
void tyler_feedforward_accumulate_delta_weights_noise(int noise, int n);
#endif
