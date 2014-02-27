#include "tyler.h"

// int pattern[P_MAX][N_SUBPATTERN][N_VISUAL];
// int specificity_level[P_MAX];
#define STRING 28
#define dt		(1.0 / (double) TYLER_STEP)
// char name_string[P_MAX][STRING];
// static int  TYLER_N_of(int s);

static int clamped_pattern = -1;
static int clamped_stage = -1;

static void tyler_clamp_now_(int p, int s, int t, int noise, int n);
static double tyler_settle_step_(int t, int noise, int n);

static double tyler_random_state(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return random_double(0.0, 1.0);
}

double tyler_random_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return random_double(-0.001, 0.001);
}

int tyler_stage_to_type(int stage)
{
    switch(stage)
    {
    case TYLER_STAGE_DISTINCTIVE:
        return TYLER_DISTINCTIVE;
        break;
    case TYLER_STAGE_SHARED:
        return TYLER_SHARED;
        break;
    case TYLER_STAGE_FUNCTIONAL:
        return TYLER_FUNCTIONAL;
        break;
    case TYLER_STAGE_ALL:
        return TYLER_VISIBLE;
        break;
    }
    return -1;
}

// States
static double state[TYLER_T][TYLER_UNITS];

double tyler_state_get(int i, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (debug_range(i, TYLER_UNITS)) {
        if (debug_range(t, TYLER_T)) {
            return state[t][i];
        }
        else {
            throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid time index", 1, t);
            return 0.0;
        }
    }
//     else {
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid state index", 1, i);
    return 0.0;
//     }
}

void tyler_randomise_states(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = 0; i < TYLER_UNITS; i++) {
        state[t][i] = tyler_random_state();
    }
}

void tyler_randomise_states_all(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int t;

    for (t = 0; t < TYLER_T; t++) {
        tyler_randomise_states(t);
    }
}

void tyler_zero_states(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int i;

    for (i = 0; i < TYLER_UNITS; i++) {
        state[t][i] = 0.0;
    }
}

// Weights
static double weight_vh[TYLER_N][TYLER_K];
static double weight_hh[TYLER_K][TYLER_K];
static double weight_hv[TYLER_K][TYLER_N];

static double *weight[TYLER_PAIRS];



// Delta Weights
static double delta_weight_vh[TYLER_N][TYLER_K];
static double delta_weight_hh[TYLER_K][TYLER_K];
static double delta_weight_hv[TYLER_K][TYLER_N];


// Clamped flags
static int clamped[TYLER_T][TYLER_UNITS];

static int tyler_stage_index(int s, int upper)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    switch (s) {
    case TYLER_STAGE_ALL: {
        return(upper ? TYLER_N : 0);
    }
    case TYLER_STAGE_DISTINCTIVE: {
        return(upper ? TYLER_N_DISTINCTIVE : 0);
    }
    case TYLER_STAGE_SHARED: {
        return(upper ? TYLER_N_DISTINCTIVE+TYLER_N_SHARED : TYLER_N_SHARED);
    }
    case TYLER_STAGE_FUNCTIONAL: {
        return(upper ? TYLER_N : TYLER_N_DISTINCTIVE+TYLER_N_SHARED);
    }
    default: {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid stage index", 1, s);
        return(0); // If you have OCD you will want a warning message here.
    }
    }
}

void tyler_decay_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;

    for (i = 0; i < TYLER_N; i++) { // Output index
        for (j = 0; j < TYLER_K; j++) { // Hidden index
            weight_hv[j][i] *= tyler_decay_rate;
        }
    }

    for (i = 0; i < TYLER_K; i++) { // Hidden index
        for (j = 0; j < TYLER_K; j++) { // Hidden index
            weight_hh[j][i] *= tyler_decay_rate;
        }
    }

    for (i = 0; i < TYLER_K; i++) { // Hidden index
        for (j = 0; j < TYLER_N; j++) { // Input index
            weight_vh[j][i] *= tyler_decay_rate;
        }
    }
}

void tyler_init_delta_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;

    for (i = 0; i < TYLER_N; i++) { // Output index
        for (j = 0; j < TYLER_K; j++) { // Hidden index
            delta_weight_hv[j][i] = 0.0;
        }
    }

    for (i = 0; i < TYLER_K; i++) { // Hidden index
        for (j = 0; j < TYLER_K; j++) { // Hidden index
            delta_weight_hh[j][i] = 0.0;
        }
    }

    for (i = 0; i < TYLER_K; i++) { // Hidden index
        for (j = 0; j < TYLER_N; j++) { // Input index
            delta_weight_vh[j][i] = 0.0;
        }
    }
}


void tyler_init_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int i, j;

    // Initialise the weights:
    for (i = 0; i < TYLER_N; i++) {
        for (j = 0; j < TYLER_K; j++) {
            weight_vh[i][j] = tyler_random_weight();
            weight_hv[j][i] = tyler_random_weight();
            weight[TYLER_K * i+j] = &(weight_vh[i][j]);
            weight[TYLER_N * TYLER_K + TYLER_K * TYLER_K + TYLER_N * j+i] = &(weight_hv[j][i]);
        }
    }
    for (i = 0; i < TYLER_K; i++) {
        for (j = 0; j < TYLER_K; j++) {
            weight_hh[i][j] = tyler_random_weight();
            weight[TYLER_N*TYLER_K + TYLER_K*i+j] = &(weight_hh[i][j]);
        }
    }
}


// void tyler_print_weights(void)
// {
//     int i, j, s = 0;
//     for (i = 0; i < TYLER_N; i++) {
//         for (j = 0; j < TYLER_K; j++) {
// 	    if ((*weight[TYLER_K]*i+j]) != weight_vh[i][j]) {
// 			s = 1;
// 	    }
// 	    if ((*weight[TYLER_N*TYLER_K+K*K + TYLER_N*j+i]) != weight_hv[j][i]) {
// 			s = 1;
// 	    }
//         }
//     }
//     for (i = 0; i < TYLER_K; i++) {
//         for (j = 0; j < TYLER_K; j++) {
// 	    if ((*weight[TYLER_N*TYLER_K + TYLER_K*i+j]) != weight_hh[i][j]) {
// 			s = 1;
// 	    }
//
// 	}
//     }
//     printf("%i\n", s);
/*
    int i;

    for (i = 0; i < TYLER_PAIRS; i++) {
        printf("%f\n", *weight[i]);
    }
}*/



// Input/Output weights to/from file
void tyler_save_file(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    FILE *fp = NULL;
    int i, j;
    fp = fopen(filename, "wt");
    fprintf(fp, " %i", tyler_iterations_so_far);

    fprintf(fp, "\n");

    fprintf(fp, " %i %i", p_min, p_max);

    fprintf(fp, "\n\n");

    for (i = 0; i < TYLER_N; i++) {
        for (j = 0; j < TYLER_K; j++) {
            fprintf(fp, " %lf", weight_vh[i][j]);
        }
    }

    fprintf(fp, "\n");

    for (i = 0; i < TYLER_K; i++) {
        for (j = 0; j < TYLER_K; j++) {
            fprintf(fp, " %lf", weight_hh[i][j]);
        }
    }

    fprintf(fp, "\n");

    for (j = 0; j < TYLER_K; j++) {
        for (i = 0; i < TYLER_N; i++) {
            fprintf(fp, " %lf", weight_hv[j][i]);
        }
    }

    fprintf(fp, "\n\n");

    for (i = 0; i < tyler_iterations_so_far; i++) {
        fprintf(fp, " %lf", Total_Error[i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < tyler_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[TYLER_STAGE_DISTINCTIVE][i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < tyler_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[TYLER_STAGE_SHARED][i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < tyler_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[TYLER_STAGE_FUNCTIONAL][i]);
    }

    fprintf(fp, "\nEOF\n");



    if (fp) {
        fclose(fp);
    }

}

int tyler_open_file(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    FILE *fp = NULL;
    int i, j;

    fp = fopen(filename, "r");
    if (!fscanf(fp, " %i", &tyler_iterations_so_far)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_iterations_so_far", 0);
        return 0;
    }

    if (!fscanf(fp, " %i %i", &p_min, &p_max)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: p_min and p_max", 0);
        return 0;
    }


// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

    for (i = 0; i < TYLER_N; i++) {
        for (j = 0; j < TYLER_K; j++) {
            if (!fscanf(fp, " %lf", &weight_vh[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_vh[][]", 0);
                return 0;
            }

        }
    }

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

    for (i = 0; i < TYLER_K; i++) {
        for (j = 0; j < TYLER_K; j++) {
            if (!fscanf(fp, " %lf", &weight_hh[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_hh[][]", 0);
                return 0;
            }
        }
    }

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

    for (j = 0; j < TYLER_K; j++) {
        for (i = 0; i < TYLER_N; i++) {
            if (!fscanf(fp, " %lf", &weight_hv[j][i])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_hv[][]", 0);
                return 0;
            }
        }
    }

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));
// 	printf("Total_Error = ");
    for (i = 0; i < tyler_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Total_Error[i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: Total_Error[]", 0);
            return 0;
        }
// 	  printf("%f ", Total_Error[i]);
    }
// 	printf("\n");

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));
// 	printf("Error[TYLER_STAGE_NAME] = ");
    for (i = 0; i < tyler_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[TYLER_STAGE_DISTINCTIVE][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: &Error[TYLER_STAGE_DISTINCTIVE][]", 0);
            return 0;
        }
// 	  printf("%f ", Error[TYLER_STAGE_NAME][i]);
    }
// 	printf("\n");

// 	printf("Error[TYLER_STAGE_VISUAL] = ");
    for (i = 0; i < tyler_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[TYLER_STAGE_SHARED][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: &Error[TYLER_STAGE_SHARED][]", 0);
            return 0;
        }
// 	  printf("%f ", Error[TYLER_STAGE_VISUAL][i]);
    }
// 	printf("\n");

// 	printf("Error[TYLER_STAGE_VERBAL] = ");
    for (i = 0; i < tyler_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[TYLER_STAGE_FUNCTIONAL][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: &Error[TYLER_STAGE_FUNCTIONAL][]", 0);
            return  0;
        }
// 	  printf("%f ", Error[TYLER_STAGE_VERBAL][i]);
    }
// 	printf("\n");

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));




//     char buf[BUFSIZ] = "Garbage";
//     i = 0;
//
//     while (fgets(buf, sizeof(buf), fp) != NULL)
//     {
// //       printf ("Line %4d: %s", i, buf);
//         i++;
//     }
//
    char *buf_2 = malloc(sizeof(char *) * 4);;
    char *compare = "EOF\0";//malloc(sizeof(char *) * 3);;

    if (!fscanf(fp, " %s", buf_2)) {
        printf ("buf_2 = %s\n", buf_2);
        printf ("compare = %s\n", compare);
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not reach end of file", 0);
        return 0;
    }

    if(strncmp(buf_2, compare, 3) != 0) {
        printf ("buf_2 = %s\n", buf_2);
        printf ("compare = %s\n", compare);


        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not reach end of file", 0);
        return 0;
    }

    if (fp) {
        fclose(fp);
    }

//     if (i == 2) {
    return 1;
//     }
//     else {
//         return 0;
//     }
}








/***********************************************************************************************************************************/
/**************************************************** TYLER_Network Functions ************************************************************/
/***********************************************************************************************************************************/

void tyler_unclamp_all(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif


    int t;
    for (t = 0; t < TYLER_T; t++) {
// 	for (i = 0; i < TYLER_UNITS; i++) { // Input index
// 	    clamped[t][i] = 0;
// 	}
        tyler_unclamp_time(t);
    }
}
void tyler_unclamp_time(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = 0; i < TYLER_N; i++) { // Input index
        clamped[t][i] = 0;
    }
    clamped_pattern = -1;
    clamped_stage = -1;
}


void tyler_clamp(int p, int s, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, i0, i1;

    i0 = tyler_stage_index(s, 0);
    i1 = tyler_stage_index(s, 1);
    tyler_unclamp_time(t);
    clamped_pattern = p;
    clamped_stage = s;
    for (i = i0; i < i1; i++) { // Input index
//           state[t][i] = get_pattern_noise(p, i);
        clamped[t][i] = 1;
    }
}

void tyler_set_target(int p, int s, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    //   int i, i0, i1;

    //   i0 = tyler_stage_index(s, 0);
    //   i1 = tyler_stage_index(s, 1);
    //   tyler_unclamp_time(t);
    clamped_pattern = p;
    clamped_stage = s;
    //   for (i = i0; i < i1; i++) { // Input index
    // //           state[t][i] = get_pattern_noise(p, i);
    //       clamped[t][i] = 1;
    //   }
}


void tyler_set_target_noise(int p, int s, int t, int noise, int n)
{
    tyler_set_target(p, s, t);
}

static void tyler_clamp_now_(int p, int s, int t, int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, i0, i1;

    i0 = tyler_stage_index(s, 0);
    i1 = tyler_stage_index(s, 1);
    tyler_unclamp_time(t);
    clamped_pattern = p;
    clamped_stage = s;
    for (i = i0; i < i1; i++) { // Input index
        state[t][i] =  get_pattern_noise(p, i, noise, n);
        clamped[t][i] = 1;
    }
}

void tyler_clamp_now(int p, int s, int t)
{
    return tyler_clamp_now_(p,  s,  t, 0, 0);

}

void tyler_clamp_now_noise(int p, int s, int t, int noise, int n)
{
    return tyler_clamp_now_(p,  s,  t, noise, n);

}




void tyler_clamp_full_pattern(int p, int t)
{
    tyler_clamp(p, TYLER_STAGE_ALL, t);
}

void tyler_clamp_full_pattern_now(int p, int t)
{
    tyler_clamp_now(p, TYLER_STAGE_ALL, t);
}

void tyler_clamp_distinctive(int p, int t)
{
    tyler_clamp(p, TYLER_STAGE_DISTINCTIVE, t);
}

void tyler_clamp_shared(int p, int t)
{
    tyler_clamp(p, TYLER_STAGE_SHARED, t);
}

void tyler_clamp_functional(int p, int t)
{
    tyler_clamp(p, TYLER_STAGE_FUNCTIONAL, t);
}


// void tyler_clamp_all_stages(int p, int t)
// {
//     int i;
//     unclamp_time(t);
//     clamped_pattern = p;
//     for (i = 0; i < TYLER_N; i++) { // Input index
// //           state[t][i] = get_pattern_noise(p, i);
//           clamped[t][i] = 1;
//     }
// }





void tyler_adjust_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;

    for (i = 0; i < TYLER_N; i++) { // Output index
        for (j = 0; j < TYLER_K; j++) { // Hidden index
            weight_hv[j][i] += tyler_learning_rate * delta_weight_hv[j][i];
// 	    printf("delta weight = %f \n", delta_weight_hv[j][i]);
            delta_weight_hv[j][i] = 0.0;
        }
    }

    for (i = 0; i < TYLER_K; i++) { // Hidden index
        for (j = 0; j < TYLER_K; j++) { // Hidden index
            weight_hh[j][i] += tyler_learning_rate * delta_weight_hh[j][i];
// 	    printf("delta weight = %f \n", delta_weight_hh[j][i]);
            delta_weight_hh[j][i] = 0.0;
        }
    }

    for (i = 0; i < TYLER_K; i++) { // Hidden index
//         for (j = stage_index(s, 0); j < stage_index(s, 1); j++) { // Input index
        for (j = 0; j < TYLER_N; j++) { // Input index
            weight_vh[j][i] += tyler_learning_rate * delta_weight_vh[j][i];
// 	    printf("delta weight = %f \n", delta_weight_vh[j][i]);
            delta_weight_vh[j][i] = 0.0;
        }
    }
}

// double tyler_prev_input[TYLER_UNITS];
//
// void tyler_init_prev_input(void)
// {
// 	int i;
//         for (i = 0; i < TYLER_UNITS; i++) {
// 		prev_input[i] = 0.0;
// 	}
// }



static double e[TYLER_T][TYLER_N];
// static double net[TYLER_T][TYLER_UNITS];


double tyler_update_states_(int t, int noise, int n)
// n represents which out of 4 patterns with noise to use
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;
    double tmp_state[TYLER_UNITS];
    double input;

    if (t) {
        for (i = 0; i < TYLER_UNITS; i++) {

            input = TYLER_BIAS;

            if (i < TYLER_N) { // VISIBLE
                if (clamped[t][i]) {
                    state[t][i] = get_pattern_noise(clamped_pattern, i, noise, n);
                }
                for (j = 0; j < TYLER_K; j++) {
                    input += state[t-1][j+N] * weight_hv[j][i];
                }
            }
            else { // HIDDEN
                for (j = 0; j < TYLER_N; j++) {
                    input += state[t-1][j] * weight_vh[j][i-N];
                }
                for (j = 0; j < TYLER_K; j++) {
                    input += state[t-1][j+N] * weight_hh[j][i-N];
                }
            }
// 	      net[t][i] = dt * (input - net[t-1][i]) + net[t-1][i];
            tmp_state[i] = f(input);
        }

        for (i = 0; i < TYLER_UNITS; i++) {
            if (!clamped[t][i]) {
                state[t][i] = tmp_state[i];
            }
            if ((t > (tyler_t1-8)) && (i < TYLER_N)) {
                e[t][i] = cee(get_pattern_noise(clamped_pattern, i, noise, n), state[t][i]);
            } else {
                e[t][i] = 0.0;
            }
        }

    } else { // at the first timestep
        for (i = 0; i < TYLER_UNITS; i++) {
            if (i < TYLER_N) {
                if (clamped[t][i]) {
                    state[t][i] =  get_pattern_noise(clamped_pattern, i, noise, n);
                }
                else  {
                    state[t][i] = 0.0;
                }
            }
            else {
                state[t][i] = tyler_random_state();
            }
            e[t][i] = 0.0;
//  	      net[t][i] = logit(state[t][i]);
        }
    }
    return 0.0;
}


double tyler_update_states(int t)
{
    return tyler_update_states_(t, 0, 0);

}

double tyler_update_states_noise(int t, int noise, int n)
{
    return tyler_update_states_(t, noise, n);

}

// void tyler_propagate(int p, int s)
// {
//     int t;
//     unclamp_all();
//     randomise_states_all();
//     clamp_all(p, s);
//     for (t = 0; t < T; t++) {
// 	propagate_timestep(p, s, t);
//     }
//     unclamp_all();
// }



static double delta_o[TYLER_T][TYLER_N], delta_h[TYLER_T][TYLER_K];
static double epsilon[TYLER_T][TYLER_UNITS];

void tyler_init_deltas(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int t, i;
    for (t = 0; t < TYLER_T; t++) {
        for (i = 0; i < TYLER_N; i++) {
            delta_o[t][i] = 0.0;
        }
        for (i = 0; i < TYLER_K; i++) {
            delta_h[t][i] = 0.0;
        }
    }
}

double tyler_get_delta(int i, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (in_range(i, TYLER_N)) {
        return delta_o[t][i];
    }
    i -= TYLER_N;
    if (in_range(i, TYLER_K)) {
        return delta_h[t][i];
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid index i for unit's delta", 1, i);
    return 0;
}

double tyler_get_epsilon(int i, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (in_range(i, TYLER_UNITS)) {
        return epsilon[t][i];
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid index i for unit's epsilon", 1, i);
    return 0;
}

int tyler_get_clamped_type(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return tyler_stage_to_type(clamped_stage);
}

void tyler_accumulate_deltas_mcclleland_ta(int p)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j, t;

    deltas_updated = 1;
    errors_updated = 1;


    // eq.17 p.447
    // epsilon_k(tyler_t1) = e_k(tyler_t1)
    // eq.18 p.447
    // delta_k(t) = f_prime(s_k(t)) * epsilon_k(t)
    // eq.19 p.447
    // epsilon_k(t-1) = e_k(t-1) + sum(w_kl delta_l(t))

    // eq. 23 p. 449
    // epsilon(i, t) =  delta(i, t+1) * w(i, j)

    // delta(i, t) = error(i, t) + dt * s(i, t) * (1 - s(i, t)) * epsilon(i, t) + (1 - dt) * error(i, t+1)

    // dEdnet_i(t) = dt * newdEdnet_i(t) + (1 - dt) * dEdnet_i(t+1)
    // newdEdnet_i(t) = state_i(t) * (1 - state_i(t)) * sum(w_ki * dEdnet_j(t+1)) + directdEdnet_i(t)

    // 1. Calculate deltas for non-input units. TYLER_Note we must work backwards.
    for (t = tyler_t1; t > 0; t--) {

        // 1.A Output layer deltas
        for (i = 0; i < TYLER_N; i++) {

            if (clamped[t][i]) {
                delta_o[t][i] = 0.0;
            }
            else {
                epsilon[t][i] = 0.0;
                if (t == tyler_t1) {
                    delta_o[t][i] = dt * e[t][i];
                }
                else { // if not at the last time step
                    for (j = 0; j < TYLER_K; j++) {
                        epsilon[t][i] += delta_h[t+1][j] * weight_vh[i][j];
                    }
                    delta_o[t][i] = dt * (state[t][i] * (1.0 - state[t][i]) * epsilon[t][i] + e[t][i]) + (1.0 - dt) * delta_o[t+1][i];
                }
            }
        }

        // 1.B Hidden layer deltas
        for (i = 0; i < TYLER_K; i++) {
            epsilon[t][i+N] = 0.0;
            if (t == tyler_t1) {
                delta_h[t][i] = 0.0;
            }
            else {

                if (t < tyler_t1) { // if not at the last time step
                    for (j = 0; j < TYLER_N; j++) {
                        epsilon[t][i+N] += delta_o[t+1][j] * weight_hv[i][j];
                    }
                    for (j = 0; j < TYLER_K; j++) {
                        epsilon[t][i+N] += delta_h[t+1][j] * weight_hh[i][j];
                    }
                }
                delta_h[t][i] = dt * (state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N]) + (1.0 - dt) * delta_h[t+1][i];
            }



        }
    }

    // eq.16 p.446
    // delta_w_ij(t) = sum(delta_i(t)state_j(t-1))

    // 2. TYLER_Now accumulate weight adjustments
    for (t = tyler_t1; t > 0; t--) {

        // 2.A Hidden to Output
        for (i = 0; i < TYLER_N; i++) { // Output index
            for (j = 0; j < TYLER_K; j++) { // Hidden index
                delta_weight_hv[j][i] += delta_o[t][i] * state[t-1][j+N];
            }
        }

        // 2.B Hidden to Hidden
        for (i = 0; i < TYLER_K; i++) { // Hidden index
            for (j = 0; j < TYLER_K; j++) { // Hidden index
                delta_weight_hh[j][i] += delta_h[t][i] * state[t-1][j+N];
            }
        }

        // 2.C Input to Hidden
        for (i = 0; i < TYLER_K; i++) { // Hidden index
            for (j = 0; j < TYLER_N; j++) { // Input index
                delta_weight_vh[j][i] += delta_h[t][i] * state[t-1][j];
            }
        }
    }
}


void tyler_accumulate_deltas_mcclleland(int p)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j, t;

    deltas_updated = 1;
    errors_updated = 1;


    // eq.17 p.447
    // epsilon_k(tyler_t1) = e_k(tyler_t1)
    // eq.18 p.447
    // delta_k(t) = f_prime(s_k(t)) * epsilon_k(t)
    // eq.19 p.447
    // epsilon_k(t-1) = e_k(t-1) + sum(w_kl delta_l(t))

    // eq. 23 p. 449
    // epsilon(i, t) =  delta(i, t+1) * w(i, j)

    // delta(i, t) = error(i, t) + dt * s(i, t) * (1 - s(i, t)) * epsilon(i, t) + (1 - dt) * error(i, t+1)

    // dEdnet_i(t) = dt * newdEdnet_i(t) + (1 - dt) * dEdnet_i(t+1)
    // newdEdnet_i(t) = state_i(t) * (1 - state_i(t)) * sum(w_ki * dEdnet_j(t+1)) + directdEdnet_i(t)

    // 1. Calculate deltas for non-input units. TYLER_Note we must work backwards.
    for (t = tyler_t1; t > 0; t--) {

        // 1.A Output layer deltas
        for (i = 0; i < TYLER_N; i++) {

            if (clamped[t][i]) {
                delta_o[t][i] = 0.0;
            }
            else {
                epsilon[t][i] = 0.0;
                if (t == tyler_t1) {
                    delta_o[t][i] = dt * e[t][i];
                }
                else { // if not at the last time step
                    for (j = 0; j < TYLER_K; j++) {
                        epsilon[t][i] += delta_h[t+1][j] * weight_vh[i][j];
                    }
                    delta_o[t][i] = (state[t][i] * (1.0 - state[t][i]) * epsilon[t][i] + e[t][i]);//dt * (state[t][i] * (1.0 - state[t][i]) * epsilon[t][i] + e[t][i]) + (1.0 - dt) * delta_o[t+1][i];
                }
            }
        }

        // 1.B Hidden layer deltas
        for (i = 0; i < TYLER_K; i++) {
            epsilon[t][i+N] = 0.0;
            if (t < tyler_t1) { // if not at the last time step
                for (j = 0; j < TYLER_N; j++) {
                    epsilon[t][i+N] += delta_o[t+1][j] * weight_hv[i][j];
                }
                for (j = 0; j < TYLER_K; j++) {
                    epsilon[t][i+N] += delta_h[t+1][j] * weight_hh[i][j];
                }
            }
            delta_h[t][i] = state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N];//dt * (state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N]) + (1.0 - dt) * delta_h[t+1][i];
//


        }
    }

    // eq.16 p.446
    // delta_w_ij(t) = sum(delta_i(t)state_j(t-1))

    // 2. TYLER_Now accumulate weight adjustments
    for (t = tyler_t1; t > 0; t--) {

        // 2.A Hidden to Output
        for (i = 0; i < TYLER_N; i++) { // Output index
            for (j = 0; j < TYLER_K; j++) { // Hidden index
                delta_weight_hv[j][i] += delta_o[t][i] * state[t-1][j+N];
            }
        }

        // 2.B Hidden to Hidden
        for (i = 0; i < TYLER_K; i++) { // Hidden index
            for (j = 0; j < TYLER_K; j++) { // Hidden index
                delta_weight_hh[j][i] += delta_h[t][i] * state[t-1][j+N];
            }
        }

        // 2.C Input to Hidden
        for (i = 0; i < TYLER_K; i++) { // Hidden index
            for (j = 0; j < TYLER_N; j++) { // Input index
                delta_weight_vh[j][i] += delta_h[t][i] * state[t-1][j];
            }
        }
    }
}

// double tyler_error_measure(int error_type, int p)
// {
//     if (error_type == 0) {
//         return tyler_jaccard_distance(p);
//     }
//     else if (error_type == 1) {
//         return bppt_sorensen_distance(p);
//     }
//     else if (error_type == 2) {
//         return bppt_calculate_rms_error(p);
//     }
// }
#define TYLER_ACTIVATION_THRESHOLD 0.5
int tyler_get_state_int(int i)
{
    return (tyler_state_get(i, tyler_t1) >= TYLER_ACTIVATION_THRESHOLD);
}

double tyler_sorensen_distance(int p, int t)
{
    int i;
    double u = 0.0, n = 0.0;
    for (i = 0; i < TYLER_N; i++) {
        u += (double) (tyler_get_state_int(i) || get_pattern_int(p, i));
        n += (double) (tyler_get_state_int(i) && get_pattern_int(p, i));
    }
    if (u) {
        return ((u - 2*fabs(n)) / u) / (double) TYLER_N;
    }
    return 1.0;
}

// int i;
// double u = 0.0, n = 0.0;
// for (i = 0; i < TYLER_N; i++) {
//   u += (double) (tyler_get_state_int(i) || get_pattern_int(p, i));
//   n += (double) (tyler_get_state_int(i) && get_pattern_int(p, i));
// }
//
// if (u) {
//   return ((u - fabs(n)) / u) / (double) TYLER_N;
// }
// return 1.0;

double tyler_jaccard_distance(int p)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif

    int i;
    double total_error = 1.0;
    double u = 0.0, n = 0.0;

    if (p < 0) {
        return 1.0;
    }
    else {
//         if (pattern_general(p) && !pdptool_style_patterns() && (t == 0)) { //this is the only case the alternative targets need to be calculated and used
//             double tmp_error = 1.0;
//             int m, n_patterns = 0, patterns[P_MAX];
//             get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);
//
//             patterns[n_patterns] = p; // add current pattern
//             n_patterns++;
//
//             for (m = 0; m < n_patterns; m++) {
//                 tmp_error = 1.0;
//                 u = 0.0, n = 0.0;
//                 for (i = 0; i < TYLER_N; i++) {
//                     u += (double) (tyler_get_state_int(i) || get_pattern_int(patterns[m], i));
//                     n += (double) (tyler_get_state_int(i) && get_pattern_int(patterns[m], i));
//                 }
//                 if (u) {
//                     tmp_error = ((u - fabs(n)) / u) / (double) TYLER_N;
//                 }
//                 total_error = fmin(total_error, tmp_error);
//
//                 // 	      printf("%s(%i) total_error = %f\n", __FUNCTION__, patterns[m], total_error);
//
//             }
//         } else {
        for (i = 0; i < TYLER_N; i++) {
            u += (double) (tyler_get_state_int(i) || get_pattern_int(p, i));
            n += (double) (tyler_get_state_int(i) && get_pattern_int(p, i));
        }
        if (u) {
            total_error = ((u - fabs(n)) / u) / (double) TYLER_N;
        }
//         }
    }

    return total_error;
}

double tyler_euclidean_distance_noise(int p, int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif

    int i;
    double total_error = 0.0;


    if (p < 0) {
        return 1.0;
    }
    else {

        for (i = 0; i < TYLER_N; i++) {
            total_error += sqr(tyler_get_state_int(i) - get_pattern_noise(p, i, noise, n));


        }
        total_error = sqrt(total_error) / (double) TYLER_N;

    }
    return total_error;
}


double tyler_calculate_rms_error_type_noise(int p, int error_t, int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif
    int n_0 = i0_of(error_t), n_1 = n_0 + N_of(error_t);

    int i;//, m;
    //     int patterns[P_MAX];
    //     int n_patterns;
    double total_error = 0.0;//, pseudo_pattern = 0.0;
    if (p < 0) {
        return(1.0);
    }
    else {

        for (i = n_0; i < n_1; i++) {
            total_error += sqr(get_pattern_noise(p, i, noise, n) - state[tyler_t1][i]);
        }
    }
    total_error /= (double) (n_1 - n_0);
    return sqrt(total_error);
}


double tyler_calculate_rss_error_type_noise(int p, int error_t, int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif
    int n_0 = i0_of(error_t), n_1 = n_0 + N_of(error_t);

    int i;//, m;
    //     int patterns[P_MAX];
    //     int n_patterns;
    double total_error = 0.0;//, pseudo_pattern = 0.0;
    if (p < 0) {
        return(1.0);
    }
    else {

        for (i = n_0; i < n_1; i++) {
            total_error += sqr(get_pattern_noise(p, i, noise, n) - state[tyler_t1][i]);
        }
    }
//   total_error /= (double) (n_1 - n_0);
    return sqrt(total_error);
}
double tyler_calculate_fabs_error_type_noise(int p, int error_t, int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif
    int n_0 = i0_of(error_t), n_1 = n_0 + N_of(error_t);

    int i;

    double total_error = 0.0;
    if (p < 0) {
        return(1.0);
    }
    else {

        for (i = n_0; i < n_1; i++) {
            total_error += fabs(get_pattern_noise(p, i, noise, n) - state[tyler_t1][i]);
        }
    }
    total_error /= (double) (n_1 - n_0);
    return total_error;
}

double tyler_calculate_sse_error_type_noise(int p, int error_t, int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif
    int n_0 = i0_of(error_t), n_1 = n_0 + N_of(error_t);

    int i;//, m;
    //     int patterns[P_MAX];
    //     int n_patterns;
    double total_error = 0.0;//, pseudo_pattern = 0.0;
    if (p < 0) {
        return(1.0);
    }
    else {

        for (i = n_0; i < n_1; i++) {
            total_error += sqr(get_pattern_noise(p, i, noise, n) - state[tyler_t1][i]);
        }
    }
    total_error /= (double) (n_1 - n_0);
    return total_error;
}

double tyler_jaccard_distance_type(int p, int error_t)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif

    int i;
    double total_error = 1.0;
    double u = 0.0, n = 0.0;
    int n_0 = i0_of(error_t), n_1 = n_0 + N_of(error_t);

    if (p < 0) {
        return 1.0;
    }
    else {

        for (i = n_0; i < n_1; i++) {
            u += (double) (tyler_get_state_int(i) || get_pattern_int(p, i));
            n += (double) (tyler_get_state_int(i) && get_pattern_int(p, i));
        }
        if (u) {
            total_error = ((u - fabs(n)) / u) / (double) (n_1 - n_0);
        }
    }

    return total_error;
}




double tyler_calculate_rms_error_noise(int p, int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif

    int i;//, m;
    //     int patterns[P_MAX];
    //     int n_patterns;
    double total_error = 0.0;//, pseudo_pattern = 0.0;
    if (p < 0) {
        return(1.0);
    }
    else {

        for (i = 0; i < TYLER_N; i++) {
            total_error += sqr(get_pattern_noise(p, i, noise, n) - state[tyler_t1][i]);
        }
    }
    total_error /= (double) TYLER_N;
    return sqrt(total_error);
}




int tyler_is_clamped(int i, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    if (t >= TYLER_T) {
        fprintf(stdout, "Error: Overflow of clamped array");
        abort();
    }


    if (i < TYLER_N) {
        return clamped[t][i];
    }
    else if (i < TYLER_UNITS) {
        return 0;
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid index i for units", 1, i);
    return 0;
}

void tyler_accumulate_deltas_williams_ta(int p)
{
    // with time averaging but using Williams & Zipser equations
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    //much slower at learning with this version of code

    int i, j, t;

    deltas_updated = 1;
    errors_updated = 1;


    // eq.17 p.447
    // epsilon_k(tyler_t1) = e_k(tyler_t1)
    // eq.18 p.447
    // delta_k(t) = f_prime(s_k(t)) * epsilon_k(t)
    // eq.19 p.447
    // epsilon_k(t-1) = e_k(t-1) + sum(w_kl delta_l(t))


    // 1. Calculate deltas for non-input units. TYLER_Note we must work backwards.
    for (t = tyler_t1; t > 0; t--) {

        // 1.A Output layer deltas
        for (i = 0; i < TYLER_N; i++) {

            if (clamped[t][i]) {
                delta_o[t][i] = 0.0;
            }
            else {
                epsilon[t][i] = 0.0;
// 		epsilon[t][i] = dt * e[t][i]; won't learn with this
                if (t == tyler_t1) {
                    epsilon[t][i] = dt* e[t][i];
                    delta_o[t][i] =  dt * state[t][i] * (1.0 - state[t][i]) * epsilon[t][i];
                }
                else { // if not at the last time step
                    for (j = 0; j < TYLER_K; j++) {
                        epsilon[t][i] += delta_h[t+1][j] * weight_vh[i][j];
                    }
                    delta_o[t][i] = dt * (state[t][i] * (1.0 - state[t][i]) * epsilon[t][i]) + (1.0 - dt) * delta_o[t+1][i];
                }
            }
        }

        // 1.B Hidden layer deltas
        for (i = 0; i < TYLER_K; i++) {
            epsilon[t][i+N] = 0.0;
            if (t < tyler_t1) { // if not at the last time step
                for (j = 0; j < TYLER_N; j++) {
                    epsilon[t][i+N] += delta_o[t+1][j] * weight_hv[i][j];
                }
                for (j = 0; j < TYLER_K; j++) {
                    epsilon[t][i+N] += delta_h[t+1][j] * weight_hh[i][j];
                }
            }
            delta_h[t][i] = dt * (state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N]) + (1.0 - dt) * delta_h[t+1][i];



        }
    }

    // eq.16 p.446
    // delta_w_ij(t) = sum(delta_i(t)state_j(t-1))

    // 2. TYLER_Now accumulate weight adjustments
    for (t = tyler_t1; t > 0; t--) {

        // 2.A Hidden to Output
        for (i = 0; i < TYLER_N; i++) { // Output index
            for (j = 0; j < TYLER_K; j++) { // Hidden index
                delta_weight_hv[j][i] += delta_o[t][i] * state[t-1][j+N];
            }
        }

        // 2.B Hidden to Hidden
        for (i = 0; i < TYLER_K; i++) { // Hidden index
            for (j = 0; j < TYLER_K; j++) { // Hidden index
                delta_weight_hh[j][i] += delta_h[t][i] * state[t-1][j+N];
            }
        }

        // 2.C Input to Hidden
        for (i = 0; i < TYLER_K; i++) { // Hidden index
            for (j = 0; j < TYLER_N; j++) { // Input index
                delta_weight_vh[j][i] += delta_h[t][i] * state[t-1][j];
            }
        }
    }
}

void tyler_accumulate_deltas_williams(int p)
{
    // using Williams & Zipser equations -  doesn't work
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j, t;

    deltas_updated = 1;
    errors_updated = 1;


    // eq.17 p.447
    // epsilon_k(tyler_t1) = e_k(tyler_t1)
    // eq.18 p.447
    // delta_k(t) = f_prime(s_k(t)) * epsilon_k(t)
    // eq.19 p.447
    // epsilon_k(t-1) = e_k(t-1) + sum(w_kl delta_l(t))


    // 1. Calculate deltas for non-input units. TYLER_Note we must work backwards.
    for (t = tyler_t1; t > 0; t--) {

        // 1.A Output layer deltas
        for (i = 0; i < TYLER_N; i++) {

            if (clamped[t][i]) {
                delta_o[t][i] = 0.0;
            }
            else {
                epsilon[t][i] = e[t][i];
                if (t == tyler_t1) {
                    delta_o[t][i] = state[t][i] * (1.0 - state[t][i]) * epsilon[t][i];
                }
                else { // if not at the last time step
                    for (j = 0; j < TYLER_K; j++) {
                        epsilon[t][i] += delta_h[t+1][j] * weight_vh[i][j];
                    }
                    delta_o[t][i] =(state[t][i] * (1.0 - state[t][i]) * epsilon[t][i]);
                }
            }
        }

        // 1.B Hidden layer deltas
        for (i = 0; i < TYLER_K; i++) {
            epsilon[t][i+N] = 0.0;
            if (t < tyler_t1) { // if not at the last time step
                for (j = 0; j < TYLER_N; j++) {
                    epsilon[t][i+N] += delta_o[t+1][j] * weight_hv[i][j];
                }
                for (j = 0; j < TYLER_K; j++) {
                    epsilon[t][i+N] += delta_h[t+1][j] * weight_hh[i][j];
                }
            }
            delta_h[t][i] = (state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N]);



        }
    }

    // eq.16 p.446
    // delta_w_ij(t) = sum(delta_i(t)state_j(t-1))

    // 2. TYLER_Now accumulate weight adjustments
    for (t = tyler_t1; t > 0; t--) {

        // 2.A Hidden to Output
        for (i = 0; i < TYLER_N; i++) { // Output index
            for (j = 0; j < TYLER_K; j++) { // Hidden index
                delta_weight_hv[j][i] += delta_o[t][i] * state[t-1][j+N];
            }
        }

        // 2.B Hidden to Hidden
        for (i = 0; i < TYLER_K; i++) { // Hidden index
            for (j = 0; j < TYLER_K; j++) { // Hidden index
                delta_weight_hh[j][i] += delta_h[t][i] * state[t-1][j+N];
            }
        }

        // 2.C Input to Hidden
        for (i = 0; i < TYLER_K; i++) { // Hidden index
            for (j = 0; j < TYLER_N; j++) { // Input index
                delta_weight_vh[j][i] += delta_h[t][i] * state[t-1][j];
            }
        }
    }
}

static double tyler_settle_step_(int t, int noise, int n)

// noise is the flag, n is the counter to choose over a set of noisy patterns
{

#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    double tmp_state[TYLER_UNITS];
    double input, tmp, d = 0.0;
    int i, j;

    if (t) {

        for (i = (TYLER_UNITS - 1); i >= 0; i--) {

            input = TYLER_BIAS;
            if (i < TYLER_N) { // VISIBLE
                if (clamped[tyler_t1][i]) {
                    state[tyler_t1][i] =   get_pattern_noise(clamped_pattern, i, noise, n);
                }
                for (j = 0; j < TYLER_K; j++) {
                    input += state[tyler_t1][j+N] * weight_hv[j][i];
                }
            }
            else { // HIDDEN
                for (j = 0; j < TYLER_N; j++) {
                    input += state[tyler_t1][j] * weight_vh[j][i-N];
                }
                for (j = 0; j < TYLER_K; j++) {
                    input += state[tyler_t1][j+N] * weight_hh[j][i-N];
                }
            }

            //net[t][i] = dt * input + (1.0 - dt) * net[t-1][i];
            tmp_state[i] = f(input);

        }

        for (i = (TYLER_UNITS - 1); i >= 0; i--) {
            tmp = state[tyler_t1][i];
            if (!clamped[tyler_t1][i]) {
                state[tyler_t1][i] = tmp_state[i];
            }
            d = fmax(d, fabs(tmp - state[tyler_t1][i]));


        }

    } else
    {
        d = 1.0;
        for (i = (TYLER_UNITS - 1); i >= 0; i--) {
            //  tmp = state[t][i];
            if (i < TYLER_N) {
                if (clamped[tyler_t1][i]) {
                    state[tyler_t1][i] = get_pattern_noise(clamped_pattern, i, noise, n);
                }
                else
                {
                    state[tyler_t1][i] = 0.0;
                }
            } else {
                state[tyler_t1][i] = tyler_random_state();
            }
            //net[t][i] = logit(state[t][i]);
        }
    }

    return d;
}
double tyler_settle_step(int t)
{
    return tyler_settle_step_(t, 0, 0);
}

double tyler_settle_step_noise(int t, int noise, int n)
{
    return tyler_settle_step_(t, noise, n);
}



// void tyler_clamp_all(int p, int s)
// {
//       int t;
//       for (t = 0; t < T; t++) {
// 	clamp(p, s, t);
//     }
// }

int tyler_get_clamped_pattern(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return clamped_pattern;
}



int tyler_get_clamped_stage(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return clamped_stage;
}






// // Input/Output weights to/from file
// void tyler_save_patterns(char *filename)
// {
// 	FILE *fp;
// 	int i, j;
// 	fp = fopen(filename, "wt");
// 	fprintf(fp, " %i", P);
//
// 	fprintf(fp, "\n");
//
// 	fprintf(fp, " %i %i", p_min, p_max);
//
// 	fprintf(fp, "\n\n");
//
// 	for (i = 0; i < TYLER_N; i++) {
// 	  for (j = 0; j < TYLER_K; j++) {
// 	    fprintf(fp, " %lf", weight_vh[i][j]);
// 	  }
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (i = 0; i < TYLER_K; i++) {
// 	  for (j = 0; j < TYLER_K; j++) {
// 	    fprintf(fp, " %lf", weight_hh[i][j]);
// 	  }
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (j = 0; j < TYLER_K; j++) {
// 	  for (i = 0; i < TYLER_N; i++) {
// 	    fprintf(fp, " %lf", weight_hv[j][i]);
// 	  }
// 	}
//
// 	fprintf(fp, "\n\n");
//
// 	for (i = 0; i < tyler_iterations_so_far; i++) {
// 	  fprintf(fp, " %lf", Total_Error[i]);
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (i = 0; i < tyler_iterations_so_far; i++) {
// 	  fprintf(fp, " %lf", Error[TYLER_STAGE_NAME][i]);
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (i = 0; i < tyler_iterations_so_far; i++) {
// 	  fprintf(fp, " %lf", Error[TYLER_STAGE_VISUAL][i]);
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (i = 0; i < tyler_iterations_so_far; i++) {
// 	  fprintf(fp, " %lf", Error[TYLER_STAGE_VERBAL][i]);
// 	}
//
// 	fprintf(fp, "\nEOF\n");
//
//
//
// 	fclose(fp);
// }


// void tyler_reset_errors(void)
// {
//       #if PRINT_FUNCTION
// 	printf("%s\n", __FUNCTION__);
//     #endif
//
//   int c, s, p;
//  for (c = 0; c < C_MAX; c++) {
//       Total_Error[c] = 0.0;
//       for (s = 0; s < 3; s++) {
// 	Error[s][c] = 0.0;
// 	Total_Stage_Error[s] = 0.0;
// 	   for (p = p_min; p < p_max; p++) {
// 	      Pattern_Error[s][p] = 0.0;
// 	      Total_Pattern_Error[p] = 0.0;
// 	   }
//     }
//  }
// }

// void tyler_save_error(int test, int p, int s, int c) // this function gives me a headache... it took me ages to write, but it had to be done even though it worked without doing this, I really wanted it to show error in real time...
// {
//       #if PRINT_FUNCTION
// 	printf("%s\n", __FUNCTION__);
//     #endif
//
//   double tyler_tmp = 0.0;
//   tmp = calculate_rms_error(p);
//   if (test) {
//       Pattern_Error[s][p] = 100 * (Pattern_Error[s][p] / 100.0 * test + tmp) / (double) (test + 1);
//       Error[s][c] = (Error[s][c] * p + Pattern_Error[s][p]) / (double) (p + 1);
//       Total_Stage_Error[s] = (Total_Stage_Error[s] * p + Pattern_Error[s][p]) / (double) (p + 1);
//       Total_Pattern_Error[p] = (Total_Pattern_Error[p] * s + Pattern_Error[s][p]) / (double) (s + 1);
//       Total_Error[c] = (Total_Error[c] * s + Error[s][c]) / (double) (s + 1);
//   } else {
//       Pattern_Error[s][p] = 100 * tmp;
//       if (p) {
// 	  Error[s][c] = (Error[s][c] * p + Pattern_Error[s][p]) / (double) (p + 1);
// 	  Total_Stage_Error[s] = (Total_Stage_Error[s] * p + Pattern_Error[s][p]) / (double) (p + 1);
//
//       } else {
// 	  Error[s][c] = Pattern_Error[s][p];
// 	  Total_Stage_Error[s] = Pattern_Error[s][p];
//
//       }
//
//       if (s) {
// 	  Total_Pattern_Error[p] = (Total_Pattern_Error[p] * s + Pattern_Error[s][p]) / (double) (s + 1);
// 	  Total_Error[c] = (Total_Error[c] * s + Error[s][c]) / (double) (s + 1);
//
//       } else {
// 	  Total_Pattern_Error[p] = Pattern_Error[s][p];
// 	  Total_Error[c] = Error[s][c];
//       }
//   }
// }




//
// double tyler_Category_Error[6];
//
// void tyler_show_category_error(void)
// {
//       #if PRINT_FUNCTION
// 	printf("%s\n", __FUNCTION__);
//     #endif
//
//   int category, p;
//
//   for(category = 0; category < 6; category++) {
//     Category_Error[category] = 0.0;
//     for(p = 0; p < 5; p++) {
//     	  Category_Error[category] += Total_Pattern_Error[5 * category + p];
//     }
//     Category_Error[category] /= 6.0;
//
//     printf("Category_Error[%s] = %f\n", get_category_string(category), Category_Error[category]);
//
//   }
// }

void tyler_lesion_random_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int x = 0, i = random_int(0, TYLER_PAIRS-1);
    // 	printf("%i / %i a\n", i, TYLER_PAIRS);
    while ((!(*weight[i]))) // && (x < (TYLER_PAIRS * 2)))
    {
        // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
        i = random_int(0, TYLER_PAIRS-1);
        x++;
        if (x > TYLER_PAIRS) {
            break;
        }
    }
    // 	printf("%i / %i b\n", i, TYLER_PAIRS);
    // 	printf("weight[%i] = %f 2\n", i, *weight[i]);

    *weight[i] = 0.0;
}


void tyler_lesion_random_ho_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int x = 0, i = random_int(TYLER_N*TYLER_K + TYLER_K*TYLER_K, TYLER_PAIRS-1);
    // 	printf("%i / %i a\n", i, TYLER_PAIRS);
    while ((!(*weight[i]))) // && (x < (TYLER_PAIRS * 2)))
    {
        // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
        i = random_int(TYLER_N*TYLER_K + TYLER_K*TYLER_K, TYLER_PAIRS-1);
        x++;
        if (x > TYLER_N*TYLER_K) {
            break;
        }
    }
    // 	printf("%i / %i b\n", i, TYLER_PAIRS);
    // 	printf("weight[%i] = %f 2\n", i, *weight[i]);

    *weight[i] = 0.0;
}

void tyler_lesion_random_ih_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int x = 0, i = random_int(0, TYLER_N*TYLER_K-1);
    // 	printf("%i / %i a\n", i, TYLER_PAIRS);
    while ((!(*weight[i]))) // && (x < (TYLER_PAIRS * 2)))
    {
        // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
        i = random_int(0, TYLER_N*TYLER_K-1);
        x++;
        if (x > TYLER_N*TYLER_K) {
            break;
        }
    }
    // 	printf("%i / %i b\n", i, TYLER_PAIRS);
    // 	printf("weight[%i] = %f 2\n", i, *weight[i]);

    *weight[i] = 0.0;
}

void tyler_lesion_random_hh_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
// #define TYLER_PAIRS (TYLER_N*TYLER_K + TYLER_K*TYLER_K + TYLER_K*TYLER_N)

    int x = 0, i = random_int(TYLER_N*TYLER_K, TYLER_K*TYLER_K -1);
    // 	printf("%i / %i a\n", i, TYLER_PAIRS);
    while ((!(*weight[i]))) // && (x < (TYLER_PAIRS * 2)))
    {
        // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
        i = random_int(TYLER_N*TYLER_K, TYLER_K*TYLER_K -1);
        x++;
        if (x > TYLER_K*TYLER_K ) {
            break;
        }
    }
    // 	printf("%i / %i b\n", i, TYLER_PAIRS);
    // 	printf("weight[%i] = %f 2\n", i, *weight[i]);

    *weight[i] = 0.0;
}

int weight_has_noise[TYLER_PAIRS];

// void tyler_disrupt_random_weight(double noise)
// {
//   #if PRINT_FUNCTION
//   printf("%s\n", __FUNCTION__);
//   #endif
//
//   int x = 0, i = random_int(0, TYLER_PAIRS-1);
//   // 	printf("%i / %i a\n", i, TYLER_PAIRS);
//   while (!weight_has_noise[i]) // && (x < (TYLER_PAIRS * 2)))
//     {
//       // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
//       i = random_int(0, TYLER_PAIRS-1);
//       x++;
//       if (x > TYLER_PAIRS) {
// 	break;
//       }
//     }
//     // 	printf("%i / %i b\n", i, TYLER_PAIRS);
//   // 	printf("weight[%i] = %f 2\n", i, *weight[i]);
//
//   *weight[i] += random_double(-noise, noise);
// }
//

void tyler_lesion_weights(double percentage)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, n = (int)round(TYLER_PAIRS * percentage/100.0);
    for (i = 0; i < n; i++) {
        tyler_lesion_random_weight();
    }
}



void tyler_lesion_weights_locus(double percentage, int lesion_locus)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, n;
    if(lesion_locus == TYLER_LESION_GLOBAL) {

        n = (int)round(TYLER_PAIRS * percentage/100.0);
        for (i = 0; i < n; i++) {
            tyler_lesion_random_weight();
        }

    }
    else if (lesion_locus == TYLER_LESION_IN_HIDDEN) {
        n = (int)round(TYLER_K * TYLER_N * percentage/100.0);
        for (i = 0; i < n; i++) {
            tyler_lesion_random_ih_weight();
        }

    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_HIDDEN) {
        n = (int)round(TYLER_K * TYLER_K * percentage/100.0);
        for (i = 0; i < n; i++) {
            tyler_lesion_random_hh_weight();
        }

    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_OUT) {
        n = (int)round(TYLER_K * TYLER_N * percentage/100.0);
        for (i = 0; i < n; i++) {
            tyler_lesion_random_ho_weight();
        }

    }


}



double backup_weight[TYLER_PAIRS];

void tyler_backup_weights(void)
{
    int i;
    for (i = 0; i < TYLER_PAIRS; i++) {
        backup_weight[i] = *weight[i];
    }
}


void tyler_disrupt_weights(double noise)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    //   for (i = 0; i < BPTT_PAIRS; i++) {
    //     weight_has_noise[i] = 0;
    //   }

    for (i = 0; i < TYLER_PAIRS; i++) {
        *weight[i] = backup_weight[i] + random_double(-noise, noise);
    }


}

void tyler_undo_lesion(void)
{
    int i;
    for (i = 0; i < TYLER_PAIRS; i++) {
        *weight[i] = backup_weight[i];
    }
}

// int map_vh(int i, int j)
// {
//     return TYLER_K*i+j;
// }
//
// int map_hv(int j, int i)
// {
//     return TYLER_N*TYLER_K+K*K + TYLER_N*j+i;
// }
//
// int map_hh(int i, int j)
// {
//     return TYLER_N*TYLER_K + TYLER_K*i+j;
// }

int tyler_get_iterations_so_far(void)
{
    return tyler_iterations_so_far;
}




void tyler_inc_iterations_so_far(void)
{
    tyler_iterations_so_far++;
}


// double tyler_fabs_average_weight(void)
// {
//
//     double fabs_average_weight = 0.0;
//     int i;
//     for (i = 0; i < TYLER_PAIRS; i++) {
//         fabs_average_weight += fabs(*weight[i]);
//     }
//     fabs_average_weight /= (double) TYLER_PAIRS;
//     return fabs_average_weight;
// }

double tyler_fabs_average_weight(int locus)
{

    double fabs_average_weight = 0.0;
    int i;

    if(locus == TYLER_LESION_GLOBAL) {

        for (i = 0; i < TYLER_PAIRS; i++) {
            fabs_average_weight += fabs(*weight[i]);
        }
        fabs_average_weight /= (double) TYLER_PAIRS;
        return fabs_average_weight;

    }
    else if (locus == TYLER_LESION_IN_HIDDEN) {
        for (i = 0; i < TYLER_K * TYLER_N; i++) {
            fabs_average_weight += fabs(*weight[i]);
        }
        fabs_average_weight /= (double) TYLER_K * TYLER_N;
        return fabs_average_weight;
    }
    else if (locus == TYLER_LESION_HIDDEN_HIDDEN) {
        for (i = TYLER_K * TYLER_N; i < TYLER_K * TYLER_N + TYLER_K * TYLER_K; i++) {
            fabs_average_weight += fabs(*weight[i]);
        }
        fabs_average_weight /= (double) TYLER_K * TYLER_K;
        return fabs_average_weight;

    }
    else if (locus == TYLER_LESION_HIDDEN_OUT) {
        for (i = TYLER_K * TYLER_N + TYLER_K * TYLER_K; i < TYLER_PAIRS; i++) {
            fabs_average_weight += fabs(*weight[i]);
        }
        fabs_average_weight /= (double) TYLER_K * TYLER_N;
        return fabs_average_weight;

    }
    return -100.0;

}

void tyler_noisy_patterns_init(void) {
    const char *filename = "tyler_noisy_patterns.pat";
    if (file_exists(filename)) {
        tyler_load_noisy_patterns(filename);

    } else {
        create_noisy_patterns(0.25);
        tyler_save_noisy_patterns(filename);

    }

    //   print_noisy_patterns();

}


