#ifndef _network_h_
#define _network_h_

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
#include "bptt.h"
#include "bm.h"
#include "pdptool.h"
#include "tyler.h"



#define network_N_get(network_type) network_get_N(network_type)
#define network_UNITS_get(network_type) network_get_UNITS(network_type)

// #define network_settle_step_noise() network_settle_step_()
// #define network_clamp_now_noise(a, b, c, d, e) network_clamp_now_(a, b, c, d, e)


// double get_state(int i);
double network_state_get(int network_type, int i);
double network_state_timestep_get(int network_type, int i, int t);
void network_decay_weights(int network_type);

void network_randomise_states(int network_type);

int network_get_iterations_so_far(int network_type);
// File Handling
void network_file_save(int network_type, char *filename);
int network_file_open(int network_type, char *filename);

char *network_type_get_string(int net_type);
int network_input_units_n_get(int network_type);
void network_unclamp(int network_type);
// void propagate(int p, int s);
// double propagate_timestep(int p, int s, int t);
// bptt.c

void network_init(int network_type);
void network_adjust_weights(int network_type);
int network_get_default_iterations(int net_type);
double network_error_measure(int network_type, int error_type, int p, int t);
/*void network_clamp(int network_type, int p, int s, int t);*/
void network_weights_init(int network_type);
int network_is_state_clamped(int network_type, int i);
void network_unclamp(int network_type);
void network_clamp_now(int network_type, int p, int s);
// void network_clamp_now_gaussian(int network_type, int p, int s, int n);
void network_clamp(int network_type, int p, int s);
double network_settle_step(int network_type, int t);
int network_clamped_pattern_get(int network_type);
void network_patterns_open(int network_type, char *filename);
int network_clamped_stage_get(int network_type);
void network_lesion_weights(int network_type, double percentage);

void network_lesion_random_weight(int network_type);
// void network_clamp_visual(int network_type, int p, int t);
void network_clamp_name(int network_type, int p);
int network_n_timesteps_get(int network_type);
void network_backup_weights(int network_type);
void network_unlesion_weights(int network_type);
// double network_pattern_get(int network_type, int p, int i);
int network_N_get(int network_type);
int network_UNITS_get(int network_type);
int network_clamped_type_get(int network_type);

void network_print_weight_stats(int network_type);

void network_save_current_attractor_for_clustering(int network_type);
void init_attractor_matrix_for_clustering(void);
void network_save_attractors_matlab(int network_type, char *filename);
void network_save_current_attractor_for_clustering(int network_type);
int network_compute_output_interpretation(int network_type, int error_type, int t);
void network_save_current_settling_attractor_for_clustering(int network_type, int p);
double network_error_measure_noise(int network_type, int error_type, int p, int t, int noise, int n);
void network_clamp_now_noise(int network_type, int p, int s, int noise, int n);
void network_get_weight_stats(int network_type, double *array);
#endif
