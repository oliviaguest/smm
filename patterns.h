#ifndef _patterns_h_
#define _patterns_h_

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "utils.h"
// #include "network.h"

#define N_CATEGORY_UNIT 6
#define N_DOMAIN_UNIT	2


//TO DO:

//types of patterns

#define BPTT_BM_PATTERNS	0
#define PDPTOOL_PATTERNS	1
#define TYLER_PATTERNS		2
#define ROGERS_PATTERNS		3


// 2. prefix "bptt_" in front of all related fucntions
// 2.a. sort functions with just wrappers so each file is tidy.
// 2.b. make network_type's address be sent to all "universal" functions, like those for semantic testing

// 3 Figure out what is going on with the error during GUI manipulation.

// lenghts of patterns
#define BM_N_NAME		40
#define BM_N_VISUAL		64
#define BM_N_PERCEPTUAL		61
#define BM_N_FUNCTIONAL		32
#define BM_N_ENCYCLOPEDIC	18
#define BM_N_VERBAL		(BM_N_PERCEPTUAL + BM_N_FUNCTIONAL + BM_N_ENCYCLOPEDIC)

#define BPTT_N_NAME		40
#define BPTT_N_VISUAL		64
#define BPTT_N_PERCEPTUAL	61
#define BPTT_N_FUNCTIONAL	32
#define BPTT_N_ENCYCLOPEDIC	18
#define BPTT_N_VERBAL		(BPTT_N_PERCEPTUAL + BPTT_N_FUNCTIONAL + BPTT_N_ENCYCLOPEDIC)

#define PDPTOOL_N_NAME		40
#define PDPTOOL_N_VISUAL	64
#define PDPTOOL_N_PERCEPTUAL	64
#define PDPTOOL_N_FUNCTIONAL	32
#define PDPTOOL_N_ENCYCLOPEDIC	16
#define PDPTOOL_N_VERBAL	(PDPTOOL_N_PERCEPTUAL + PDPTOOL_N_FUNCTIONAL + PDPTOOL_N_ENCYCLOPEDIC)


#define TYLER_N_SUBPATTERN	3
#define TYLER_N_POOL		8
#define TYLER_N_DISTINCTIVE	8
#define TYLER_N_SHARED		8
#define TYLER_N_FUNCTIONAL	8

#define ROGERS_N_NAME		40
#define ROGERS_N_VISUAL		64
#define ROGERS_N_PERCEPTUAL	64
#define ROGERS_N_FUNCTIONAL	32
#define ROGERS_N_ENCYCLOPEDIC	16
#define ROGERS_N_VERBAL	(ROGERS_N_PERCEPTUAL + ROGERS_N_FUNCTIONAL + ROGERS_N_ENCYCLOPEDIC)
// int N_NAME, N_VISUAL, N_PERCEPTUAL, N_FUNCTIONAL, N_FUNCTIONAL, N_ENCYCLOPEDIC, N_VERBAL;

#define MAX_N_PATTERN	64

#define N_DOMAIN	2

// number of instances per pattern type
#define N_CATEGORY		6 //used to be 5
// #define N_SUBCATEGORY_MIN	6 // should be removed
// #define N_SUBCATEGORY_MAX	8 // should be removed
#define N_SUBCATEGORY		8
#define N_SUBPATTERN		5

#define N_SUPERORDINATE		3
#define N_SPECIFIC_MIN		5
#define N_SPECIFIC_MAX		8

#define N_CATEGORY_UNIT 6
#define N_DOMAIN_UNIT	2


// numbers corresponding to each pattern (sub/super)type like an enumerated type
#define NAME		0
#define VISUAL		1
#define PERCEPTUAL	2
#define FUNCTIONAL	3
#define ENCYCLOPEDIC	4
#define HIDDEN		5
#define VERBAL		6
#define VISIBLE		7


// #define DISTINCTIVE	8
// #define SHARED		9
// #define VERBAL		5
// #define VISIBLE		6
// #define HIDDEN		7

#define TYLER_DISTINCTIVE	0
#define TYLER_SHARED		1
#define TYLER_FUNCTIONAL	2
#define TYLER_VISIBLE		3
#define TYLER_HIDDEN		4


// TYLER-LIKE
#define TYLER_LIKE_A		0
#define TYLER_LIKE_B		1
#define TYLER_LIKE_C		2
// #define TYLER_VISIBLE		3
// #define TYLER_HIDDEN		4

// numbers corresponding to each pattern (sub/super)type like an enumerated type
// #define PDPTOOL_NAME		0
// #define PDPTOOL_VISUAL		1
// #define PDPTOOL_PERCEPTUAL	2
// #define PDPTOOL_FUNCTIONAL	3
// #define PDPTOOL_ENCYCLOPEDIC	4
// #define PDPTOOL_VERBAL		5
// #define PDPTOOL_VISIBLE		6
// #define PDPTOOL_HIDDEN		7


#define N_MAX PDPTOOL_N
#define K_MAX BM_K

// number of visible units
#define BPTT_N		(BPTT_N_NAME + BPTT_N_VISUAL + BPTT_N_PERCEPTUAL + BPTT_N_FUNCTIONAL + BPTT_N_ENCYCLOPEDIC)
#define BPTT_K		64

// number of visible units
#define BM_N		(BM_N_NAME + BM_N_VISUAL + BM_N_PERCEPTUAL + BM_N_FUNCTIONAL + BM_N_ENCYCLOPEDIC)
#define BM_K		64

// number of visible units
#define PDPTOOL_N		(PDPTOOL_N_NAME + PDPTOOL_N_VISUAL + PDPTOOL_N_PERCEPTUAL + PDPTOOL_N_FUNCTIONAL + PDPTOOL_N_ENCYCLOPEDIC)
#define PDPTOOL_K		64

// number of visible units
#define ROGERS_N		(ROGERS_N_NAME + ROGERS_N_VISUAL + ROGERS_N_PERCEPTUAL + ROGERS_N_FUNCTIONAL + ROGERS_N_ENCYCLOPEDIC)
#define ROGERS_K		64


#define EXCLUDE_FRUIT 1
// number of patterns
#define P_MAX		48
#define PDPTOOL_P 	48
#define BPTT_P		48
#define BM_P		48
#define ROGERS_P	48

int P;
int p_min, p_max;
int N;
int K;


#define BIRD	10
#define MAMMAL	11
#define VEHICLE	12
#define HH_OBJ	13
#define TOOL	14
#define FRUIT	15

int specificity_level[P_MAX];
#define P_STRING 28
char name_string[P_MAX][P_STRING];

enum category
{
    bird,
    mammal,
    vehicle,
    hh_object,
    tool,
    fruit
};

#define pattern_get(p, i) get_pattern(p, i)



// int pattern[P_MAX][N_SUBPATTERN][MAX_N_PATTERN];


void init_patterns(int network_type);


void open_patterns(char *filename);
double get_pattern_type(int p, int t, int i);
double get_pattern_type_pdptool(int p, int t, int i);
double get_pattern_pdptool(int p, int i); // special treatment due to names being taught in different way
double get_pattern(int p, int i);
int get_pattern_type_int(int p, int t, int i);
int get_n_general_names(void);
int get_pattern_int(int p, int i);
int pattern_general(int p);
int patterns_that_share_a_name(int p, int ps[]);
int i0_of(int s);
int pdptool_i0_of(int s);
int N_of(int s);
int type_of(int i);
int pdptool_type_of(int i);
char *get_name_string(int p);
int name_to_pattern(int i, int p);
int pattern_to_name_index(int p);
int is_fruit(int p);
int is_general_name(int p);
enum category get_category(int p);
void get_close_distractors(int p, int *array, int *length);
void get_distant_distractors(int p, int *array, int *length);
void get_unrelated_distractors(int p, int *array, int *length);
int excluded_pattern(int p);
int get_domain(int p);
int get_domain_n_pattern(int domain);
int get_aoa_domain(int p);
void get_n_close_trials(int *n);
void get_n_distant_trials(int *n);
void get_n_unrelated_trials(int *n);
void get_patterns_that_share_a_name(int p, int *array, int *length);
int pattern_get_n_visual(void);
int get_domain_n_general_names(int domain);
int get_n_visual_features(int p);
int get_n_fruit(void);
void save_patterns_matlab(char *filename);

double get_noisy_pattern(int n, int p, int i);

void open_patterns_rogers(void);

void pdptool_open_patterns(char *filename);
int pdptool_style_patterns(void);

double get_pattern_noise(int p, int i, int noise, int n);
#endif