#ifndef _semantic_tasks_h_
#define _semantic_tasks_h_

#include "patterns.h"
// #include "network.h"


#define NAMING_SEMANTIC		0
#define NAMING_SUPERORDINATE	1
#define NAMING_CROSSDOMAIN	2
#define NAMING_OMISSION		3
#define NAMING_CORRECT		4



#define init_tyler_response(d, l, s) init_tyler_feedforward_response(d, l, s)
#define open_tyler_responses(filename) open_tyler_feedforward_responses(filename)
#define save_tyler_responses(filename) save_tyler_feedforward_responses(filename)

#define init_aoa_response(d, l, s) init_aoa_feedforward_response(d, l, s)
#define open_aoa_responses(filename) open_aoa_feedforward_responses(filename)
#define save_aoa_responses(filename) save_aoa_feedforward_responses(filename)

#define NAMING_LESION_DISTRIBUTION	50
// Rogers et al. 2004 p.217
#define NAMING_LESION_LEVEL		20
#define NAMING_SAMPLE			10
double naming_response[NAMING_LESION_DISTRIBUTION][NAMING_LESION_LEVEL][NAMING_SAMPLE][P_MAX][2];
double naming_response_1[NAMING_LESION_DISTRIBUTION][NAMING_LESION_LEVEL][NAMING_SAMPLE][P_MAX][2];
double naming_response_alt[NAMING_LESION_DISTRIBUTION][NAMING_LESION_LEVEL][NAMING_SAMPLE][P_MAX][2];
int naming_lesion_distribution;
int naming_lesion_level;
int naming_sample;

// 0 is pattern given
// 1 is the activation value


#define SORTING_LESION_DISTRIBUTION	50
// Rogers et al. 2004 p.217
#define SORTING_LESION_LEVEL		20
#define SORTING_SAMPLE			10
int sorting_response_given_name[SORTING_LESION_DISTRIBUTION][SORTING_LESION_LEVEL][SORTING_SAMPLE][P_MAX][2];
int sorting_response_given_visual[SORTING_LESION_DISTRIBUTION][SORTING_LESION_LEVEL][SORTING_SAMPLE][P_MAX][2];
int sorting_lesion_distribution;
int sorting_lesion_level;
int sorting_sample;

// 0 is general domain
// 1 is specific category

#define WORD_TO_PICTURE_LESION_DISTRIBUTION	50
// Rogers et al. 2004 p.217
#define WORD_TO_PICTURE_LESION_LEVEL		20
#define WORD_TO_PICTURE_SAMPLE			10
int word_to_picture_close[WORD_TO_PICTURE_LESION_DISTRIBUTION][WORD_TO_PICTURE_LESION_LEVEL][WORD_TO_PICTURE_SAMPLE][P_MAX];
// int word_to_picture_correct[WORD_TO_PICTURE_LESION_DISTRIBUTION][WORD_TO_PICTURE_LESION_LEVEL][WORD_TO_PICTURE_SAMPLE][P_MAX];
int word_to_picture_distant[WORD_TO_PICTURE_LESION_DISTRIBUTION][WORD_TO_PICTURE_LESION_LEVEL][WORD_TO_PICTURE_SAMPLE][P_MAX];
int word_to_picture_unrelated[WORD_TO_PICTURE_LESION_DISTRIBUTION][WORD_TO_PICTURE_LESION_LEVEL][WORD_TO_PICTURE_SAMPLE][P_MAX];
int word_to_picture_lesion_distribution;
int word_to_picture_lesion_level;
int word_to_picture_sample;


#define DRAWING_AND_COPYING_LESION_DISTRIBUTION		50
// Rogers et al. 2004 p.217
#define DRAWING_AND_COPYING_LESION_LEVEL		20
#define DRAWING_AND_COPYING_SAMPLE			10

// int drawing_correct[DRAWING_AND_COPYING_LESION_DISTRIBUTION][DRAWING_AND_COPYING_LESION_LEVEL][DRAWING_AND_COPYING_SAMPLE][P_MAX];
int drawing_omission[DRAWING_AND_COPYING_LESION_DISTRIBUTION][DRAWING_AND_COPYING_LESION_LEVEL][DRAWING_AND_COPYING_SAMPLE][P_MAX];
int drawing_intrusion[DRAWING_AND_COPYING_LESION_DISTRIBUTION][DRAWING_AND_COPYING_LESION_LEVEL][DRAWING_AND_COPYING_SAMPLE][P_MAX];
// int copying_correct[DRAWING_AND_COPfactivationYING_LESION_DISTRIBUTION][DRAWING_AND_COPYING_LESION_LEVEL][DRAWING_AND_COPYING_SAMPLE][P_MAX];
int copying_omission[DRAWING_AND_COPYING_LESION_DISTRIBUTION][DRAWING_AND_COPYING_LESION_LEVEL][DRAWING_AND_COPYING_SAMPLE][P_MAX];
int copying_intrusion[DRAWING_AND_COPYING_LESION_DISTRIBUTION][DRAWING_AND_COPYING_LESION_LEVEL][DRAWING_AND_COPYING_SAMPLE][P_MAX];
int drawing_and_copying_lesion_distribution;
int drawing_and_copying_lesion_level;
int drawing_and_copying_sample;


#define TYLER_LESION_DISTRIBUTION	300
// Rogers et al. 2004 p.217
#define TYLER_LESION_LEVEL		20
#define TYLER_SAMPLE			100
#define TYLER_NETWORK			300
double tyler_distinctive_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][TYLER_SAMPLE][P_MAX];
double tyler_shared_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][TYLER_SAMPLE][P_MAX];
double tyler_functional_error[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][TYLER_SAMPLE][P_MAX];
int tyler_response[TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][TYLER_SAMPLE][P_MAX];



// double tyler_distinctive_error_network[TYLER_NETWORK][TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][TYLER_SAMPLE][P_MAX];
// double tyler_shared_error_network[TYLER_NETWORK][TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][TYLER_SAMPLE][P_MAX];
// double tyler_functional_error_network[TYLER_NETWORK][TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][TYLER_SAMPLE][P_MAX];
// int tyler_response_network[TYLER_NETWORK][TYLER_LESION_DISTRIBUTION][TYLER_LESION_LEVEL][TYLER_SAMPLE][P_MAX];
int tyler_lesion_distribution;
int tyler_lesion_level;
int tyler_sample;
// int tyler_network;

// 0 is pattern given
// 1 is the activation value


#define AOA_LESION_DISTRIBUTION		TYLER_LESION_DISTRIBUTION
// Rogers et al. 2004 p.217
#define AOA_LESION_LEVEL		TYLER_LESION_LEVEL
#define AOA_SAMPLE			TYLER_SAMPLE
#define AOA_NETWORK			TYLER_NETWORK
double aoa_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];
// double aoa_distinctive_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];
// double aoa_shared_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];
// double aoa_functional_error[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];
int aoa_response[AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];



// double aoa_distinctive_error_network[AOA_NETWORK][AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];
// double aoa_shared_error_network[AOA_NETWORK][AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];
// double aoa_functional_error_network[AOA_NETWORK][AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];
// int aoa_response_network[AOA_NETWORK][AOA_LESION_DISTRIBUTION][AOA_LESION_LEVEL][AOA_SAMPLE][P_MAX];
int aoa_lesion_distribution;
int aoa_lesion_level;
int aoa_sample;
//
double target_attractor[WORD_TO_PICTURE_LESION_DISTRIBUTION][WORD_TO_PICTURE_LESION_LEVEL][WORD_TO_PICTURE_SAMPLE][P_MAX][K_MAX];
double distance_target[WORD_TO_PICTURE_LESION_DISTRIBUTION][WORD_TO_PICTURE_LESION_LEVEL][WORD_TO_PICTURE_SAMPLE][P_MAX];


void init_naming_response(int d, int l, int s);
void bptt_save_naming_response(int p, int sample, int level, int distribution);
void bm_save_naming_response(int p, int sample, int level, int distribution);
void pdptool_save_naming_response(int p, int sample, int level, int distribution);

void init_sorting_response(int d, int l, int s);
void bptt_save_sorting_response_given_name(int p, int sample, int level, int distribution);
void bm_save_sorting_response_given_name(int p, int sample, int level, int distribution);
void pdptool_save_sorting_response_given_name(int p, int sample, int level, int distribution);
void bptt_save_sorting_response_given_visual(int p, int sample, int level, int distribution);
void bm_save_sorting_response_given_visual(int p, int sample, int level, int distribution);
void pdptool_save_sorting_response_given_visual(int p, int sample, int level, int distribution);
void bptt_save_target_attractor(int p, int sample, int level, int distribution);
void bm_save_target_attractor(int p, int sample, int level, int distribution);
void pdptool_save_target_attractor(int p, int sample, int level, int distribution);
double bptt_distance_between_current_and_target_attractor(int p, int sample, int level, int distribution);
double bm_distance_between_current_and_target_attractor(int p, int sample, int level, int distribution);
double pdptool_distance_between_current_and_target_attractor(int p, int sample, int level, int distribution);


void bptt_save_naming_response_alt(int p, int sample, int level, int distribution);
void pdptool_save_naming_response_alt(int p, int sample, int level, int distribution);
void bm_save_naming_response_alt(int p, int sample, int level, int distribution);


void save_naming_responses_1(char *filename);
void open_naming_responses_1(char *filename);
void init_naming_response_1(int d, int l, int s);

void save_naming_responses_alt(char *filename);
void open_naming_responses_alt(char *filename);
void init_naming_response_alt(int d, int l, int s);
int bptt_determine_naming_response(int target_pattern);
void bptt_save_naming_response_1(int p, int sample, int level, int distribution);


void init_word_to_picture_response(int d, int l, int s);
void bptt_save_word_to_picture_response_close_distractor(int p, int q, int sample, int level, int distribution);
void bm_save_word_to_picture_response_close_distractor(int p, int q, int sample, int level, int distribution);
void pdptool_save_word_to_picture_response_close_distractor(int p, int q, int sample, int level, int distribution);
void bptt_save_word_to_picture_response_target(int p, int sample, int level, int distribution);
void bm_save_word_to_picture_response_target(int p, int sample, int level, int distribution);
void pdptool_save_word_to_picture_response_target(int p, int sample, int level, int distribution);
void bptt_save_word_to_picture_response_distant_distractor(int p, int q, int sample, int level, int distribution);
void bm_save_word_to_picture_response_distant_distractor(int p, int q, int sample, int level, int distribution);
void pdptool_save_word_to_picture_response_distant_distractor(int p, int q, int sample, int level, int distribution);
void bptt_save_word_to_picture_response_unrelated_distractor(int p, int q, int sample, int level, int distribution);
void bm_save_word_to_picture_response_unrelated_distractor(int p, int q, int sample, int level, int distribution);
void pdptool_save_word_to_picture_response_unrelated_distractor(int p, int q, int sample, int level, int distribution);

void init_drawing_and_copying_response(int d, int l, int s);
void bptt_save_drawing_response(int d, int l, int s, int p);
void bm_save_drawing_response(int d, int l, int s, int p);
void pdptool_save_drawing_response(int d, int l, int s, int p);
void bptt_save_copying_response(int d, int l, int s, int p);
void bm_save_copying_response(int d, int l, int s, int p);
void pdptool_save_copying_response(int d, int l, int s, int p);

void save_naming_responses(char *filename);
void open_naming_responses(char *filename);
void save_sorting_responses(char *filename);
void open_sorting_responses(char *filename);
void save_word_to_picture_responses(char *filename);
void open_word_to_picture_responses(char *filename);
void save_drawing_and_copying_responses(char *filename);
void open_drawing_and_copying_responses(char *filename);

void init_tyler_response(int d, int l, int s);
void open_tyler_responses(char *filename);
void save_tyler_responses(char *filename);


void tyler_save_response(int p, int sample, int level, int distribution, int noise, int n);
void tyler_feedforward_save_response(int p, int sample, int level, int distribution, int noise, int n);
// void tyler_feedforward_save_response_network(int n, int p, int sample, int level, int distribution, int noise, int n_p);
void init_tyler_response_network(int n, int d, int l, int s);
void save_tyler_responses_network(char *filename);
void open_tyler_responses_network(char *filename);



void init_aoa_response(int d, int l, int s);
void open_aoa_responses(char *filename);
void save_aoa_responses(char *filename);
void aoa_save_response(int p, int sample, int level, int distribution, int noise, int n);
void aoa_feedforward_save_response(int p, int sample, int level, int distribution, int noise, int n);
// void aoa_feedforward_save_response_network(int n, int p, int sample, int level, int distribution, int noise, int n);
void init_aoa_response_network(int n, int d, int l, int s);
void save_aoa_responses_network(char *filename);
void open_aoa_responses_network(char *filename);
#endif