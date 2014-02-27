#include "bptt.h"

// int pattern[P_MAX][N_SUBPATTERN][N_VISUAL];
// int specificity_level[P_MAX];
#define STRING 28
#define dt		(1.0 / (double) BPTT_STEP)
// char name_string[P_MAX][STRING];
// static int  BPTT_N_of(int s);

int clamped_pattern;
int clamped_stage;

#define ERROR 1
static double error (double d, double y)
{
//   if (ERROR == 1) {
//     return cee(d, y);
//   }

    return sse(d, y);

}



double bptt_random_state(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return random_double(0.0, 1.0);
}

double bptt_random_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    return random_double(-0.5, 0.5);

//         return random_double(-0.05, 0.05);
    //     return random_double(-0.001, 0.001);
}



// States
static double state[BPTT_T][BPTT_UNITS];

double bptt_state_get(int i, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (debug_range(i, BPTT_UNITS)) {
        if (debug_range(t, BPTT_T)) {
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

void bptt_randomise_states(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = 0; i < BPTT_UNITS; i++) {
        state[t][i] = bptt_random_state();
    }
}

void bptt_randomise_states_all(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int t;

    for (t = 0; t < BPTT_T; t++) {
        bptt_randomise_states(t);
    }
}

void bptt_zero_states(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int i;

    for (i = 0; i < BPTT_UNITS; i++) {
        state[t][i] = 0.0;
    }
}

// Weights
static double weight_vh[BPTT_N][BPTT_K];
static double weight_hh[BPTT_K][BPTT_K];
static double weight_hv[BPTT_K][BPTT_N];

static double *weight[BPTT_PAIRS];



// Delta Weights
static double delta_weight_vh[BPTT_N][BPTT_K];
static double delta_weight_hh[BPTT_K][BPTT_K];
static double delta_weight_hv[BPTT_K][BPTT_N];


// Clamped flags
static int clamped[BPTT_T][BPTT_UNITS];
static int target_clamped[BPTT_T];

static int bptt_stage_index(int s, int upper)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    switch (s) {
    case BPTT_STAGE_ALL: {
        return(upper ? BPTT_N : 0);
    }
    case BPTT_STAGE_NAME: {
        return(upper ? BPTT_N_NAME : 0);
    }
    case BPTT_STAGE_VISUAL: {
        return(upper ? BPTT_N_NAME+BPTT_N_VISUAL : BPTT_N_NAME);
    }
    case BPTT_STAGE_VERBAL: {
        return(upper ? BPTT_N : BPTT_N_NAME+BPTT_N_VISUAL);
    }
    default: {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid stage index", 1, s);
        return(0); // If you have OCD you will want a warning message here.
    }
    }
}

void bptt_decay_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;

    for (i = 0; i < BPTT_N; i++) { // Output index
        for (j = 0; j < BPTT_K; j++) { // Hidden index
            weight_hv[j][i] *= bptt_decay_rate;
        }
    }

    for (i = 0; i < BPTT_K; i++) { // Hidden index
        for (j = 0; j < BPTT_K; j++) { // Hidden index
            weight_hh[j][i] *= bptt_decay_rate;
        }
    }

    for (i = 0; i < BPTT_K; i++) { // Hidden index
        for (j = 0; j < BPTT_N; j++) { // Input index
            weight_vh[j][i] *= bptt_decay_rate;
        }
    }
}

void bptt_init_delta_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;

    for (i = 0; i < BPTT_N; i++) { // Output index
        for (j = 0; j < BPTT_K; j++) { // Hidden index
            delta_weight_hv[j][i] = 0.0;
        }
    }

    for (i = 0; i < BPTT_K; i++) { // Hidden index
        for (j = 0; j < BPTT_K; j++) { // Hidden index
            delta_weight_hh[j][i] = 0.0;
        }
    }

    for (i = 0; i < BPTT_K; i++) { // Hidden index
        for (j = 0; j < BPTT_N; j++) { // Input index
            delta_weight_vh[j][i] = 0.0;
        }
    }
}


void bptt_init_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int i, j;

    // Initialise the weights:
    for (i = 0; i < BPTT_N; i++) {
        for (j = 0; j < BPTT_K; j++) {
            weight_vh[i][j] = bptt_random_weight();
            weight_hv[j][i] = bptt_random_weight();
            weight[BPTT_K * i+j] = &(weight_vh[i][j]);
            weight[BPTT_N * BPTT_K + BPTT_K * BPTT_K + BPTT_N * j+i] = &(weight_hv[j][i]);
        }
    }
    for (i = 0; i < BPTT_K; i++) {
        for (j = 0; j < BPTT_K; j++) {
            weight_hh[i][j] = bptt_random_weight();
            weight[BPTT_N*BPTT_K + BPTT_K*i+j] = &(weight_hh[i][j]);
        }
    }
}


// void bptt_print_weights(void)
// {
//     int i, j, s = 0;
//     for (i = 0; i < BPTT_N; i++) {
//         for (j = 0; j < BPTT_K; j++) {
// 	    if ((*weight[BPTT_K]*i+j]) != weight_vh[i][j]) {
// 			s = 1;
// 	    }
// 	    if ((*weight[BPTT_N*BPTT_K+K*K + BPTT_N*j+i]) != weight_hv[j][i]) {
// 			s = 1;
// 	    }
//         }
//     }
//     for (i = 0; i < BPTT_K; i++) {
//         for (j = 0; j < BPTT_K; j++) {
// 	    if ((*weight[BPTT_N*BPTT_K + BPTT_K*i+j]) != weight_hh[i][j]) {
// 			s = 1;
// 	    }
//
// 	}
//     }
//     printf("%i\n", s);
/*
    int i;

    for (i = 0; i < BPTT_PAIRS; i++) {
        printf("%f\n", *weight[i]);
    }
}*/



// Input/Output weights to/from file
void bptt_save_file(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    FILE *fp = NULL;
    int i, j;
    fp = fopen(filename, "wt");
    fprintf(fp, " %i", bptt_iterations_so_far);

    fprintf(fp, "\n");

    fprintf(fp, " %i %i", p_min, p_max);

    fprintf(fp, "\n\n");

    for (i = 0; i < BPTT_N; i++) {
        for (j = 0; j < BPTT_K; j++) {
            fprintf(fp, " %lf", weight_vh[i][j]);
        }
    }

    fprintf(fp, "\n");

    for (i = 0; i < BPTT_K; i++) {
        for (j = 0; j < BPTT_K; j++) {
            fprintf(fp, " %lf", weight_hh[i][j]);
        }
    }

    fprintf(fp, "\n");

    for (j = 0; j < BPTT_K; j++) {
        for (i = 0; i < BPTT_N; i++) {
            fprintf(fp, " %lf", weight_hv[j][i]);
        }
    }

    fprintf(fp, "\n\n");

    for (i = 0; i < bptt_iterations_so_far; i++) {
        fprintf(fp, " %lf", Total_Error[i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < bptt_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[BPTT_STAGE_NAME][i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < bptt_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[BPTT_STAGE_VISUAL][i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < bptt_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[BPTT_STAGE_VERBAL][i]);
    }

    fprintf(fp, "\nEOF\n");



    if (fp) {
        fclose(fp);
    }

}

int bptt_open_file(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    FILE *fp = NULL;
    int i, j;

    fp = fopen(filename, "r");
    if (!fscanf(fp, " %i", &bptt_iterations_so_far)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bptt_iterations_so_far", 0);
        return 0;
    }

    if (!fscanf(fp, " %i %i", &p_min, &p_max)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: p_min and p_max", 0);
        return 0;
    }


// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

    for (i = 0; i < BPTT_N; i++) {
        for (j = 0; j < BPTT_K; j++) {
            if (!fscanf(fp, " %lf", &weight_vh[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_vh[][]", 0);
                return 0;
            }

        }
    }

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

    for (i = 0; i < BPTT_K; i++) {
        for (j = 0; j < BPTT_K; j++) {
            if (!fscanf(fp, " %lf", &weight_hh[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_hh[][]", 0);
                return 0;
            }
        }
    }

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

    for (j = 0; j < BPTT_K; j++) {
        for (i = 0; i < BPTT_N; i++) {
            if (!fscanf(fp, " %lf", &weight_hv[j][i])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_hv[][]", 0);
                return 0;
            }
        }
    }

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));
// 	printf("Total_Error = ");
    for (i = 0; i < bptt_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Total_Error[i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: Total_Error[]", 0);
            return 0;
        }
// 	  printf("%f ", Total_Error[i]);
    }
// 	printf("\n");

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));
// 	printf("Error[BPTT_STAGE_NAME] = ");
    for (i = 0; i < bptt_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[BPTT_STAGE_NAME][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: &Error[BPTT_STAGE_NAME][]", 0);
            return 0;
        }
// 	  printf("%f ", Error[BPTT_STAGE_NAME][i]);
    }
// 	printf("\n");

// 	printf("Error[BPTT_STAGE_VISUAL] = ");
    for (i = 0; i < bptt_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[BPTT_STAGE_VISUAL][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: &Error[BPTT_STAGE_VISUAL][]", 0);
            return 0;
        }
// 	  printf("%f ", Error[BPTT_STAGE_VISUAL][i]);
    }
// 	printf("\n");

// 	printf("Error[BPTT_STAGE_VERBAL] = ");
    for (i = 0; i < bptt_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[BPTT_STAGE_VERBAL][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: &Error[BPTT_STAGE_VERBAL][]", 0);
            return  0;
        }
// 	  printf("%f ", Error[BPTT_STAGE_VERBAL][i]);
    }
// 	printf("\n");

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

// 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));



    /*
        char buf[BUFSIZ] = "Garbage";
        i = 0;

        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
    //       printf ("Line %4d: %s", i, buf);
            i++;
        }   */

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

    return 1;
    /*
        if (i == 2) {
            return 1;
        }
        else {
            return 0;
        }*/
}








/***********************************************************************************************************************************/
/**************************************************** BPTT_Network Functions ************************************************************/
/***********************************************************************************************************************************/

void bptt_unclamp_all(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif


    int t;
    for (t = 0; t < BPTT_T; t++) {
// 	for (i = 0; i < BPTT_UNITS; i++) { // Input index
// 	    clamped[t][i] = 0;
// 	}
        bptt_unclamp_time(t);
    }
}
void bptt_unclamp_time(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = 0; i < BPTT_N; i++) { // Input index
        clamped[t][i] = 0;
    }
    clamped_pattern = -1;
    clamped_stage = -1;
}

void bptt_init_targets_clamped(void)
{
    //specifies which timesteps/intervals we want to be full clamped but still propagate their unclamped states
    int t;
    for(t = 0; t < BPTT_T_CLAMPED_MAX; t++)
    {
        target_clamped[t] = 0;
    }


    for(t = (3 + 2) * BPTT_STEP; t < BPTT_T_CLAMPED_MAX; t++)
    {
        target_clamped[t] = 1;
    }

}

void bptt_clamp(int p, int s, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, i0, i1;

    i0 = bptt_stage_index(s, 0);
    i1 = bptt_stage_index(s, 1);
    bptt_unclamp_time(t);
    clamped_pattern = p;
    clamped_stage = s;
    for (i = i0; i < i1; i++) { // Input index
//           state[t][i] = get_pattern(p, i);
        clamped[t][i] = 1;
    }
}

void bptt_clamp_now(int p, int s, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, i0, i1;

    i0 = bptt_stage_index(s, 0);
    i1 = bptt_stage_index(s, 1);
    bptt_unclamp_time(t);
    clamped_pattern = p;
    clamped_stage = s;
    for (i = i0; i < i1; i++) { // Input index
        state[t][i] = get_pattern(p, i);
        clamped[t][i] = 1;
    }
}

void bptt_clamp_visual(int p, int t)
{
    bptt_clamp(p, BPTT_STAGE_VISUAL, t);
}

void bptt_clamp_name(int p, int t)
{
    bptt_clamp(p, BPTT_STAGE_NAME, t);
}

void bptt_clamp_verbal(int p, int t)
{
    bptt_clamp(p, BPTT_STAGE_VERBAL, t);
}


void bptt_clamp_all_stages(int p, int t)
{
    int i;
    clamped_pattern = p;
    clamped_stage = BPTT_STAGE_ALL;
    for (i = 0; i < BPTT_N; i++) { // Input index
//           state[t][i] = get_pattern(p, i);
        clamped[t][i] = 1;
    }
}





void bptt_adjust_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;

    for (i = 0; i < BPTT_N; i++) { // Output index
        for (j = 0; j < BPTT_K; j++) { // Hidden index
            weight_hv[j][i] += bptt_learning_rate * delta_weight_hv[j][i];
// 	    printf("delta weight = %f \n", delta_weight_hv[j][i]);
            delta_weight_hv[j][i] = 0.0;
        }
    }

    for (i = 0; i < BPTT_K; i++) { // Hidden index
        for (j = 0; j < BPTT_K; j++) { // Hidden index
            weight_hh[j][i] += bptt_learning_rate * delta_weight_hh[j][i];
// 	    printf("delta weight = %f \n", delta_weight_hh[j][i]);
            delta_weight_hh[j][i] = 0.0;
        }
    }

    for (i = 0; i < BPTT_K; i++) { // Hidden index
//         for (j = stage_index(s, 0); j < stage_index(s, 1); j++) { // Input index
        for (j = 0; j < BPTT_N; j++) { // Input index
            weight_vh[j][i] += bptt_learning_rate * delta_weight_vh[j][i];
// 	    printf("delta weight = %f \n", delta_weight_vh[j][i]);
            delta_weight_vh[j][i] = 0.0;
        }
    }
}

// double bptt_prev_input[BPTT_UNITS];
//
// void bptt_init_prev_input(void)
// {
// 	int i;
//         for (i = 0; i < BPTT_UNITS; i++) {
// 		prev_input[i] = 0.0;
// 	}
// }



static double e[BPTT_T][BPTT_N];
// static double net[BPTT_T][BPTT_UNITS];


double bptt_update_states(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;
    double tmp_state[BPTT_UNITS];
    double input;

    if (t) {
        for (i = 0; i < BPTT_UNITS; i++) {

            input = BPTT_BIAS;

            if (i < BPTT_N) { // VISIBLE
                if (clamped[t][i]) {
                    state[t][i] = get_pattern(clamped_pattern, i);
                }
                for (j = 0; j < BPTT_K; j++) {
                    input += state[t-1][j+N] * weight_hv[j][i];
                }
            }
            else { // HIDDEN
                for (j = 0; j < BPTT_N; j++) {
                    input += state[t-1][j] * weight_vh[j][i-N];
                }
                for (j = 0; j < BPTT_K; j++) {
                    input += state[t-1][j+N] * weight_hh[j][i-N];
                }
            }
            // 	      net[t][i] = dt * (input - net[t-1][i]) + net[t-1][i];
            tmp_state[i] = f(input);
        }

        for (i = 0; i < BPTT_UNITS; i++) {
            if (!clamped[t][i]) {
                state[t][i] = tmp_state[i];
            }
            if ((t > (bptt_t1-8)) && (i < BPTT_N)) {
                e[t][i] = error(get_pattern(clamped_pattern, i), state[t][i]);
            } else {
                e[t][i] = 0.0;
            }
        }

    } else { // at the first timestep
        for (i = 0; i < BPTT_UNITS; i++) {
            if (i < BPTT_N) {
                if (clamped[t][i]) {
                    state[t][i] = get_pattern(clamped_pattern, i);
                }
                else  {
                    state[t][i] = 0.0;
                }
            }
            else {
                state[t][i] = bptt_random_state();
            }
            e[t][i] = 0.0;
            //  	      net[t][i] = logit(state[t][i]);
        }
    }
    return 0.0;
}


double bptt_update_states_gaussian_noise(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;
    double tmp_state[BPTT_UNITS];
    double input;

    if (t) {
        for (i = 0; i < BPTT_UNITS; i++) {

            input = BPTT_BIAS + gaussrand()/2.0;

// 	    printf("input noise = %f\n", gaussrand()/2.0);

            if (i < BPTT_N) { // VISIBLE
                if (clamped[t][i]) {
                    state[t][i] = get_pattern(clamped_pattern, i);
                }
                for (j = 0; j < BPTT_K; j++) {
                    input += state[t-1][j+N] * weight_hv[j][i];
                }
            }
            else { // HIDDEN
                for (j = 0; j < BPTT_N; j++) {
                    input += state[t-1][j] * weight_vh[j][i-N];
                }
                for (j = 0; j < BPTT_K; j++) {
                    input += state[t-1][j+N] * weight_hh[j][i-N];
                }
            }
            // 	      net[t][i] = dt * (input - net[t-1][i]) + net[t-1][i];
            tmp_state[i] = f(input);
        }

        for (i = 0; i < BPTT_UNITS; i++) {
            if (!clamped[t][i]) {
                state[t][i] = tmp_state[i];
            }
            if ((t > (bptt_t1-8)) && (i < BPTT_N)) {
                e[t][i] = error(get_pattern(clamped_pattern, i), state[t][i]);
            } else {
                e[t][i] = 0.0;
            }
        }

    } else { // at the first timestep
        for (i = 0; i < BPTT_UNITS; i++) {
            if (i < BPTT_N) {
                if (clamped[t][i]) {
                    state[t][i] = get_pattern(clamped_pattern, i);
                }
                else  {
                    state[t][i] = 0.0;
                }
            }
            else {
                state[t][i] = bptt_random_state();
            }
            e[t][i] = 0.0;
            //  	      net[t][i] = logit(state[t][i]);
        }
    }
    return 0.0;
}

double bptt_update_states_targets_clamped(int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;
    double tmp_state[BPTT_UNITS];
    double input;

    if (t) {
        for (i = 0; i < BPTT_UNITS; i++) {

            input = BPTT_BIAS;

            if (i < BPTT_N) { // VISIBLE
                if (clamped[t][i]) {
                    state[t][i] = get_pattern(clamped_pattern, i);
                }

                if (target_clamped[t-1]) { // this is because we can clamp them but not change their state

                    for (j = 0; j < BPTT_K; j++) {
                        input += get_pattern(clamped_pattern, i) * weight_hv[j][i];
                    }
                }
                else {
                    for (j = 0; j < BPTT_K; j++) {

                        input += state[t-1][j+N] * weight_hv[j][i];
                    }
                }
            }
            else { // HIDDEN
                if (target_clamped[t-1]) {    // this is because we can clamp them but not change their state
                    for (j = 0; j < BPTT_N; j++) {
                        input += get_pattern(clamped_pattern, j) * weight_vh[j][i-N];

                    }
                }
                else {
                    for (j = 0; j < BPTT_N; j++) {

                        input += state[t-1][j] * weight_vh[j][i-N];
                    }
                }
                for (j = 0; j < BPTT_K; j++) {
                    input += state[t-1][j+N] * weight_hh[j][i-N];
                }
            }
// 	      net[t][i] = dt * (input - net[t-1][i]) + net[t-1][i];
            tmp_state[i] = f(input);
        }

        for (i = 0; i < BPTT_UNITS; i++) {
            if (!clamped[t][i]) {
                state[t][i] = tmp_state[i];
            }
            if ((t > (bptt_t1-8)) && (i < BPTT_N)) {
                e[t][i] = error(get_pattern(clamped_pattern, i), state[t][i]);
            } else {
                e[t][i] = 0.0;
            }
        }

    } else { // at the first timestep
        for (i = 0; i < BPTT_UNITS; i++) {
            if (i < BPTT_N) {
                if (clamped[t][i]) {
                    state[t][i] = get_pattern(clamped_pattern, i);
                }
                else  {
                    state[t][i] = 0.0;
                }
            }
            else {
                state[t][i] = bptt_random_state();
            }
            e[t][i] = 0.0;
//  	      net[t][i] = logit(state[t][i]);
        }
    }
    return 0.0;
}


// void bptt_propagate(int p, int s)
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



static double delta_o[BPTT_T][BPTT_N], delta_h[BPTT_T][BPTT_K];
static double epsilon[BPTT_T][BPTT_UNITS];

void bptt_init_deltas(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int t, i;
    for (t = 0; t < BPTT_T; t++) {
        for (i = 0; i < BPTT_N; i++) {
            delta_o[t][i] = 0.0;
        }
        for (i = 0; i < BPTT_K; i++) {
            delta_h[t][i] = 0.0;
        }
    }
}

double bptt_get_delta(int i, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (in_range(i, BPTT_N)) {
        return delta_o[t][i];
    }
    i -= BPTT_N;
    if (in_range(i, BPTT_K)) {
        return delta_h[t][i];
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid index i for unit's delta", 1, i);
    return 0;
}

double bptt_get_epsilon(int i, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (in_range(i, BPTT_UNITS)) {
        return epsilon[t][i];
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid index i for unit's epsilon", 1, i);
    return 0;
}

void bptt_accumulate_deltas_mcclleland_ta(int p)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j, t;

    deltas_updated = 1;
    errors_updated = 1;


    // eq.17 p.447
    // epsilon_k(bptt_t1) = e_k(bptt_t1)
    // eq.18 p.447
    // delta_k(t) = f_prime(s_k(t)) * epsilon_k(t)
    // eq.19 p.447
    // epsilon_k(t-1) = e_k(t-1) + sum(w_kl delta_l(t))

    // eq. 23 p. 449
    // epsilon(i, t) =  delta(i, t+1) * w(i, j)

    // delta(i, t) = error(i, t) + dt * s(i, t) * (1 - s(i, t)) * epsilon(i, t) + (1 - dt) * error(i, t+1)

    // dEdnet_i(t) = dt * newdEdnet_i(t) + (1 - dt) * dEdnet_i(t+1)
    // newdEdnet_i(t) = state_i(t) * (1 - state_i(t)) * sum(w_ki * dEdnet_j(t+1)) + directdEdnet_i(t)

    // 1. Calculate deltas for non-input units. BPTT_Note we must work backwards.
    for (t = bptt_t1; t > 0; t--) {

        // 1.A Output layer deltas
        for (i = 0; i < BPTT_N; i++) {

            if (clamped[t][i]) {
                delta_o[t][i] = 0.0;
            }
            else {
                epsilon[t][i] = 0.0;
                if (t == bptt_t1) {
                    delta_o[t][i] = dt * e[t][i];
                }
                else { // if not at the last time step
                    for (j = 0; j < BPTT_K; j++) {
                        epsilon[t][i] += delta_h[t+1][j] * weight_vh[i][j];
                    }
                    delta_o[t][i] = dt * (state[t][i] * (1.0 - state[t][i]) * epsilon[t][i] + e[t][i]) + (1.0 - dt) * delta_o[t+1][i];
                }
            }
        }

        // 1.B Hidden layer deltas
        for (i = 0; i < BPTT_K; i++) {
            epsilon[t][i+N] = 0.0;
            if (t == bptt_t1) {
                delta_h[t][i] = 0.0;
            }
            else {

                if (t < bptt_t1) { // if not at the last time step
                    for (j = 0; j < BPTT_N; j++) {
                        epsilon[t][i+N] += delta_o[t+1][j] * weight_hv[i][j];
                    }
                    for (j = 0; j < BPTT_K; j++) {
                        epsilon[t][i+N] += delta_h[t+1][j] * weight_hh[i][j];
                    }
                }
                delta_h[t][i] = dt * (state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N]) + (1.0 - dt) * delta_h[t+1][i];
            }



        }
    }

    // eq.16 p.446
    // delta_w_ij(t) = sum(delta_i(t)state_j(t-1))

    // 2. BPTT_Now accumulate weight adjustments
    for (t = bptt_t1; t > 0; t--) {

        // 2.A Hidden to Output
        for (i = 0; i < BPTT_N; i++) { // Output index
            for (j = 0; j < BPTT_K; j++) { // Hidden index
                delta_weight_hv[j][i] += delta_o[t][i] * state[t-1][j+N];
            }
        }

        // 2.B Hidden to Hidden
        for (i = 0; i < BPTT_K; i++) { // Hidden index
            for (j = 0; j < BPTT_K; j++) { // Hidden index
                delta_weight_hh[j][i] += delta_h[t][i] * state[t-1][j+N];
            }
        }

        // 2.C Input to Hidden
        for (i = 0; i < BPTT_K; i++) { // Hidden index
            for (j = 0; j < BPTT_N; j++) { // Input index
                delta_weight_vh[j][i] += delta_h[t][i] * state[t-1][j];
            }
        }
    }
}


void bptt_accumulate_deltas_mcclleland(int p)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j, t;

    deltas_updated = 1;
    errors_updated = 1;


    // eq.17 p.447
    // epsilon_k(bptt_t1) = e_k(bptt_t1)
    // eq.18 p.447
    // delta_k(t) = f_prime(s_k(t)) * epsilon_k(t)
    // eq.19 p.447
    // epsilon_k(t-1) = e_k(t-1) + sum(w_kl delta_l(t))

    // eq. 23 p. 449
    // epsilon(i, t) =  delta(i, t+1) * w(i, j)

    // delta(i, t) = error(i, t) + dt * s(i, t) * (1 - s(i, t)) * epsilon(i, t) + (1 - dt) * error(i, t+1)

    // dEdnet_i(t) = dt * newdEdnet_i(t) + (1 - dt) * dEdnet_i(t+1)
    // newdEdnet_i(t) = state_i(t) * (1 - state_i(t)) * sum(w_ki * dEdnet_j(t+1)) + directdEdnet_i(t)

    // 1. Calculate deltas for non-input units. BPTT_Note we must work backwards.
    for (t = bptt_t1; t > 0; t--) {

        // 1.A Output layer deltas
        for (i = 0; i < BPTT_N; i++) {

            if (clamped[t][i]) {
                delta_o[t][i] = 0.0;
            }
            else {
                epsilon[t][i] = 0.0;
                if (t == bptt_t1) {
                    delta_o[t][i] = dt * e[t][i];
                }
                else { // if not at the last time step
                    for (j = 0; j < BPTT_K; j++) {
                        epsilon[t][i] += delta_h[t+1][j] * weight_vh[i][j];
                    }
                    delta_o[t][i] = (state[t][i] * (1.0 - state[t][i]) * epsilon[t][i] + e[t][i]);//dt * (state[t][i] * (1.0 - state[t][i]) * epsilon[t][i] + e[t][i]) + (1.0 - dt) * delta_o[t+1][i];
                }
            }
        }

        // 1.B Hidden layer deltas
        for (i = 0; i < BPTT_K; i++) {
            epsilon[t][i+N] = 0.0;
            if (t < bptt_t1) { // if not at the last time step
                for (j = 0; j < BPTT_N; j++) {
                    epsilon[t][i+N] += delta_o[t+1][j] * weight_hv[i][j];
                }
                for (j = 0; j < BPTT_K; j++) {
                    epsilon[t][i+N] += delta_h[t+1][j] * weight_hh[i][j];
                }
            }
            delta_h[t][i] = state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N];//dt * (state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N]) + (1.0 - dt) * delta_h[t+1][i];
//


        }
    }

    // eq.16 p.446
    // delta_w_ij(t) = sum(delta_i(t)state_j(t-1))

    // 2. BPTT_Now accumulate weight adjustments
    for (t = bptt_t1; t > 0; t--) {

        // 2.A Hidden to Output
        for (i = 0; i < BPTT_N; i++) { // Output index
            for (j = 0; j < BPTT_K; j++) { // Hidden index
                delta_weight_hv[j][i] += delta_o[t][i] * state[t-1][j+N];
            }
        }

        // 2.B Hidden to Hidden
        for (i = 0; i < BPTT_K; i++) { // Hidden index
            for (j = 0; j < BPTT_K; j++) { // Hidden index
                delta_weight_hh[j][i] += delta_h[t][i] * state[t-1][j+N];
            }
        }

        // 2.C Input to Hidden
        for (i = 0; i < BPTT_K; i++) { // Hidden index
            for (j = 0; j < BPTT_N; j++) { // Input index
                delta_weight_vh[j][i] += delta_h[t][i] * state[t-1][j];
            }
        }
    }
}

// double bptt_error_measure(int error_type, int p)
// {
//     if (error_type == 0) {
//         return bptt_jaccard_distance(p);
//     }
//     else if (error_type == 1) {
//         return bppt_sorensen_distance(p);
//     }
//     else if (error_type == 2) {
//         return bppt_calculate_rms_error(p);
//     }
// }
#define BPTT_ACTIVATION_THRESHOLD 0.5
int bptt_get_state_int(int i)
{
    return (bptt_state_get(i, bptt_t1) >= BPTT_ACTIVATION_THRESHOLD);
}

double bptt_sorensen_distance(int p, int t)
{
    int i;
    double u = 0.0, n = 0.0;
    for (i = 0; i < BPTT_N; i++) {
        u += (double) (bptt_get_state_int(i) || get_pattern_int(p, i));
        n += (double) (bptt_get_state_int(i) && get_pattern_int(p, i));
    }
    if (u) {
        return ((u - 2*fabs(n)) / u) / (double) BPTT_N;
    }
    return 1.0;
}

// int i;
// double u = 0.0, n = 0.0;
// for (i = 0; i < BPTT_N; i++) {
//   u += (double) (bptt_get_state_int(i) || get_pattern_int(p, i));
//   n += (double) (bptt_get_state_int(i) && get_pattern_int(p, i));
// }
//
// if (u) {
//   return ((u - fabs(n)) / u) / (double) BPTT_N;
// }
// return 1.0;

double bptt_jaccard_distance(int p, int t)
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
        if (pattern_general(p) && !pdptool_style_patterns() && (t == 0)) { //this is the only case the alternative targets need to be calculated and used
            double tmp_error = 1.0;
            int m, n_patterns = 0, patterns[P_MAX];
            get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);

            patterns[n_patterns] = p; // add current pattern
            n_patterns++;

            for (m = 0; m < n_patterns; m++) {
                tmp_error = 1.0;
                u = 0.0, n = 0.0;
                for (i = 0; i < BPTT_N; i++) {
                    u += (double) (bptt_get_state_int(i) || get_pattern_int(patterns[m], i));
                    n += (double) (bptt_get_state_int(i) && get_pattern_int(patterns[m], i));
                }
                if (u) {
                    tmp_error = ((u - fabs(n)) / u) / (double) BPTT_N;
                }
                total_error = fmin(total_error, tmp_error);

                // 	      printf("%s(%i) total_error = %f\n", __FUNCTION__, patterns[m], total_error);

            }
        } else {
            for (i = 0; i < BPTT_N; i++) {
                u += (double) (bptt_get_state_int(i) || get_pattern_int(p, i));
                n += (double) (bptt_get_state_int(i) && get_pattern_int(p, i));
            }
            if (u) {
                total_error = ((u - fabs(n)) / u) / (double) BPTT_N;
            }
        }
    }

    return total_error;
}


double bptt_jaccard_distance_type(int p, int input_t, int error_t)
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
        if (pattern_general(p) && !pdptool_style_patterns() && (input_t == 0)) { //this is the only case the alternative targets need to be calculated and used
            double tmp_error = 1.0;
            int m, n_patterns = 0, patterns[P_MAX];
            get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);

            patterns[n_patterns] = p; // add current pattern
            n_patterns++;

            for (m = 0; m < n_patterns; m++) {
                tmp_error = 1.0;
                u = 0.0, n = 0.0;
                for (i = n_0; i < n_1; i++) {
                    u += (double) (bptt_get_state_int(i) || get_pattern_int(patterns[m], i));
                    n += (double) (bptt_get_state_int(i) && get_pattern_int(patterns[m], i));
                }
                if (u) {
                    tmp_error = ((u - fabs(n)) / u) / (double) (n_1 - n_0);
                }
                total_error = fmin(total_error, tmp_error);

                // 	      printf("%s(%i) total_error = %f\n", __FUNCTION__, patterns[m], total_error);

            }
        } else {
            for (i = n_0; i < n_1; i++) {
                u += (double) (bptt_get_state_int(i) || get_pattern_int(p, i));
                n += (double) (bptt_get_state_int(i) && get_pattern_int(p, i));
            }
            if (u) {
                total_error = ((u - fabs(n)) / u) / (double) (n_1 - n_0);
            }
        }
    }

    return total_error;
}




double bptt_calculate_rms_error(int p, int t)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif

    int i, m;
    int patterns[P_MAX];
    int n_patterns;
    double total_error = 0.0, pseudo_pattern = 0.0;
    if (p < 0) {
        return(1.0);
    }
    else {
        if (pattern_general(p) && !pdptool_style_patterns() && (t == 0)) { //this is the only case the pseduo-pattern targets need to be calculated and used
            n_patterns = 0;
            get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);
            for (i = 0; i < BPTT_N; i++) {
                pseudo_pattern = get_pattern(p, i);
                for (m = 0; m < n_patterns; m++) {
                    pseudo_pattern += get_pattern(patterns[m], i);
                }
                pseudo_pattern /= (double) (n_patterns + 1.0);
// 		printf("%d: total_error = %f, pseudo_pattern = %f, state = %f\n", m, total_error, pseudo_pattern, state[bptt_t1][i]);
                total_error += sqr(pseudo_pattern - state[bptt_t1][i]);
            }
        }
        else                  // else p is either a base-level pattern or a pdptool-style pattern, neither of which need special consideration
        {
            for (i = 0; i < BPTT_N; i++) {
                total_error += sqr(get_pattern(p, i) - state[bptt_t1][i]);
            }
        }
    }
    total_error /= (double) BPTT_N;
    return sqrt(total_error);
}

int bptt_is_clamped(int i, int t)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    if (t >= BPTT_T) {
        fprintf(stdout, "Error: Overflow of clamped array");
        abort();
    }


    if (i < BPTT_N) {
        return clamped[t][i];
    }
    else if (i < BPTT_UNITS) {
        return 0;
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid index i for units", 1, i);
    return 0;
}

void bptt_accumulate_deltas_williams_ta(int p)
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
    // epsilon_k(bptt_t1) = e_k(bptt_t1)
    // eq.18 p.447
    // delta_k(t) = f_prime(s_k(t)) * epsilon_k(t)
    // eq.19 p.447
    // epsilon_k(t-1) = e_k(t-1) + sum(w_kl delta_l(t))


    // 1. Calculate deltas for non-input units. BPTT_Note we must work backwards.
    for (t = bptt_t1; t > 0; t--) {

        // 1.A Output layer deltas
        for (i = 0; i < BPTT_N; i++) {

            if (clamped[t][i]) {
                delta_o[t][i] = 0.0;
            }
            else {
                epsilon[t][i] = 0.0;
// 		epsilon[t][i] = dt * e[t][i]; won't learn with this
                if (t == bptt_t1) {
                    epsilon[t][i] = dt* e[t][i];
                    delta_o[t][i] =  dt * state[t][i] * (1.0 - state[t][i]) * epsilon[t][i];
                }
                else { // if not at the last time step
                    for (j = 0; j < BPTT_K; j++) {
                        epsilon[t][i] += delta_h[t+1][j] * weight_vh[i][j];
                    }
                    delta_o[t][i] = dt * (state[t][i] * (1.0 - state[t][i]) * epsilon[t][i]) + (1.0 - dt) * delta_o[t+1][i];
                }
            }
        }

        // 1.B Hidden layer deltas
        for (i = 0; i < BPTT_K; i++) {
            epsilon[t][i+N] = 0.0;
            if (t < bptt_t1) { // if not at the last time step
                for (j = 0; j < BPTT_N; j++) {
                    epsilon[t][i+N] += delta_o[t+1][j] * weight_hv[i][j];
                }
                for (j = 0; j < BPTT_K; j++) {
                    epsilon[t][i+N] += delta_h[t+1][j] * weight_hh[i][j];
                }
            }
            delta_h[t][i] = dt * (state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N]) + (1.0 - dt) * delta_h[t+1][i];



        }
    }

    // eq.16 p.446
    // delta_w_ij(t) = sum(delta_i(t)state_j(t-1))

    // 2. BPTT_Now accumulate weight adjustments
    for (t = bptt_t1; t > 0; t--) {

        // 2.A Hidden to Output
        for (i = 0; i < BPTT_N; i++) { // Output index
            for (j = 0; j < BPTT_K; j++) { // Hidden index
                delta_weight_hv[j][i] += delta_o[t][i] * state[t-1][j+N];
            }
        }

        // 2.B Hidden to Hidden
        for (i = 0; i < BPTT_K; i++) { // Hidden index
            for (j = 0; j < BPTT_K; j++) { // Hidden index
                delta_weight_hh[j][i] += delta_h[t][i] * state[t-1][j+N];
            }
        }

        // 2.C Input to Hidden
        for (i = 0; i < BPTT_K; i++) { // Hidden index
            for (j = 0; j < BPTT_N; j++) { // Input index
                delta_weight_vh[j][i] += delta_h[t][i] * state[t-1][j];
            }
        }
    }
}

void bptt_accumulate_deltas_williams(int p)
{
    // using Williams & Zipser equations -  doesn't work
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j, t;

    deltas_updated = 1;
    errors_updated = 1;


    // eq.17 p.447
    // epsilon_k(bptt_t1) = e_k(bptt_t1)
    // eq.18 p.447
    // delta_k(t) = f_prime(s_k(t)) * epsilon_k(t)
    // eq.19 p.447
    // epsilon_k(t-1) = e_k(t-1) + sum(w_kl delta_l(t))


    // 1. Calculate deltas for non-input units. BPTT_Note we must work backwards.
    for (t = bptt_t1; t > 0; t--) {

        // 1.A Output layer deltas
        for (i = 0; i < BPTT_N; i++) {

            if (clamped[t][i]) {
                delta_o[t][i] = 0.0;
            }
            else {
                epsilon[t][i] = e[t][i];
                if (t == bptt_t1) {
                    delta_o[t][i] = state[t][i] * (1.0 - state[t][i]) * epsilon[t][i];
                }
                else { // if not at the last time step
                    for (j = 0; j < BPTT_K; j++) {
                        epsilon[t][i] += delta_h[t+1][j] * weight_vh[i][j];
                    }
                    delta_o[t][i] =(state[t][i] * (1.0 - state[t][i]) * epsilon[t][i]);
                }
            }
        }

        // 1.B Hidden layer deltas
        for (i = 0; i < BPTT_K; i++) {
            epsilon[t][i+N] = 0.0;
            if (t < bptt_t1) { // if not at the last time step
                for (j = 0; j < BPTT_N; j++) {
                    epsilon[t][i+N] += delta_o[t+1][j] * weight_hv[i][j];
                }
                for (j = 0; j < BPTT_K; j++) {
                    epsilon[t][i+N] += delta_h[t+1][j] * weight_hh[i][j];
                }
            }
            delta_h[t][i] = (state[t][i+N] * (1.0 - state[t][i+N]) * epsilon[t][i+N]);



        }
    }

    // eq.16 p.446
    // delta_w_ij(t) = sum(delta_i(t)state_j(t-1))

    // 2. BPTT_Now accumulate weight adjustments
    for (t = bptt_t1; t > 0; t--) {

        // 2.A Hidden to Output
        for (i = 0; i < BPTT_N; i++) { // Output index
            for (j = 0; j < BPTT_K; j++) { // Hidden index
                delta_weight_hv[j][i] += delta_o[t][i] * state[t-1][j+N];
            }
        }

        // 2.B Hidden to Hidden
        for (i = 0; i < BPTT_K; i++) { // Hidden index
            for (j = 0; j < BPTT_K; j++) { // Hidden index
                delta_weight_hh[j][i] += delta_h[t][i] * state[t-1][j+N];
            }
        }

        // 2.C Input to Hidden
        for (i = 0; i < BPTT_K; i++) { // Hidden index
            for (j = 0; j < BPTT_N; j++) { // Input index
                delta_weight_vh[j][i] += delta_h[t][i] * state[t-1][j];
            }
        }
    }
}

double bptt_settle_step(int t)
{

#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    double tmp_state[BPTT_UNITS];
    double input, tmp, d = 0.0;
    int i, j;

    if (t) {

        for (i = (BPTT_UNITS - 1); i >= 0; i--) {

            input = BPTT_BIAS;
            if (i < BPTT_N) { // VISIBLE
                if (clamped[bptt_t1][i]) {
                    state[bptt_t1][i] = get_pattern(clamped_pattern, i);
                }
                for (j = 0; j < BPTT_K; j++) {
                    input += state[bptt_t1][j+N] * weight_hv[j][i];
                }
            }
            else { // HIDDEN
                for (j = 0; j < BPTT_N; j++) {
                    input += state[bptt_t1][j] * weight_vh[j][i-N];
                }
                for (j = 0; j < BPTT_K; j++) {
                    input += state[bptt_t1][j+N] * weight_hh[j][i-N];
                }
            }

            //net[t][i] = dt * input + (1.0 - dt) * net[t-1][i];
            tmp_state[i] = f(input);

        }

        for (i = (BPTT_UNITS - 1); i >= 0; i--) {
            tmp = state[bptt_t1][i];
            if (!clamped[bptt_t1][i]) {
                state[bptt_t1][i] = tmp_state[i];
            }
            d = fmax(d, fabs(tmp - state[bptt_t1][i]));


        }

    } else
    {
        d = 1.0;
        for (i = (BPTT_UNITS - 1); i >= 0; i--) {
            //  tmp = state[t][i];
            if (i < BPTT_N) {
                if (clamped[bptt_t1][i]) {
                    state[bptt_t1][i] = get_pattern(clamped_pattern, i);
                }
                else
                {
                    state[bptt_t1][i] = 0.0;
                }
            } else {
                state[bptt_t1][i] = bptt_random_state();
            }
            //net[t][i] = logit(state[t][i]);
        }
    }

    return d;
}

// void bptt_clamp_all(int p, int s)
// {
//       int t;
//       for (t = 0; t < T; t++) {
// 	clamp(p, s, t);
//     }
// }

int bptt_get_clamped_pattern(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return clamped_pattern;
}

int bptt_get_clamped_stage(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return clamped_stage;
}



// // Input/Output weights to/from file
// void bptt_save_patterns(char *filename)
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
// 	for (i = 0; i < BPTT_N; i++) {
// 	  for (j = 0; j < BPTT_K; j++) {
// 	    fprintf(fp, " %lf", weight_vh[i][j]);
// 	  }
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (i = 0; i < BPTT_K; i++) {
// 	  for (j = 0; j < BPTT_K; j++) {
// 	    fprintf(fp, " %lf", weight_hh[i][j]);
// 	  }
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (j = 0; j < BPTT_K; j++) {
// 	  for (i = 0; i < BPTT_N; i++) {
// 	    fprintf(fp, " %lf", weight_hv[j][i]);
// 	  }
// 	}
//
// 	fprintf(fp, "\n\n");
//
// 	for (i = 0; i < bptt_iterations_so_far; i++) {
// 	  fprintf(fp, " %lf", Total_Error[i]);
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (i = 0; i < bptt_iterations_so_far; i++) {
// 	  fprintf(fp, " %lf", Error[BPTT_STAGE_NAME][i]);
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (i = 0; i < bptt_iterations_so_far; i++) {
// 	  fprintf(fp, " %lf", Error[BPTT_STAGE_VISUAL][i]);
// 	}
//
// 	fprintf(fp, "\n");
//
// 	for (i = 0; i < bptt_iterations_so_far; i++) {
// 	  fprintf(fp, " %lf", Error[BPTT_STAGE_VERBAL][i]);
// 	}
//
// 	fprintf(fp, "\nEOF\n");
//
//
//
// 	fclose(fp);
// }


// void bptt_reset_errors(void)
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

// void bptt_save_error(int test, int p, int s, int c) // this function gives me a headache... it took me ages to write, but it had to be done even though it worked without doing this, I really wanted it to show error in real time...
// {
//       #if PRINT_FUNCTION
// 	printf("%s\n", __FUNCTION__);
//     #endif
//
//   double bptt_tmp = 0.0;
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
// double bptt_Category_Error[6];
//
// void bptt_show_category_error(void)
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

void bptt_lesion_random_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int x = 0, i = random_int(0, BPTT_PAIRS-1);
// 	printf("%i / %i a\n", i, BPTT_PAIRS);
    while ((!(*weight[i]))) // && (x < (BPTT_PAIRS * 2)))
    {
// 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
        i = random_int(0, BPTT_PAIRS-1);
        x++;
        if (x > BPTT_PAIRS) {
            break;
        }
    }
// 	printf("%i / %i b\n", i, BPTT_PAIRS);
// 	printf("weight[%i] = %f 2\n", i, *weight[i]);

    *weight[i] = 0.0;
}

int weight_has_noise[BPTT_PAIRS];

// void bptt_disrupt_random_weight(double noise)
// {
//   #if PRINT_FUNCTION
//   printf("%s\n", __FUNCTION__);
//   #endif
//
//   int x = 0, i = random_int(0, BPTT_PAIRS-1);
//   // 	printf("%i / %i a\n", i, BPTT_PAIRS);
//   while (!weight_has_noise[i]) // && (x < (BPTT_PAIRS * 2)))
//     {
//       // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
//       i = random_int(0, BPTT_PAIRS-1);
//       x++;
//       if (x > BPTT_PAIRS) {
// 	break;
//       }
//     }
//     // 	printf("%i / %i b\n", i, BPTT_PAIRS);
//   // 	printf("weight[%i] = %f 2\n", i, *weight[i]);
//
//   *weight[i] += random_double(-noise, noise);
// }
//

void bptt_lesion_weights(double percentage)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, n = (int)round(BPTT_PAIRS * percentage/100.0);
    for (i = 0; i < n; i++) {
        bptt_lesion_random_weight();
    }
}
double backup_weight[BPTT_PAIRS];

void bptt_disrupt_weights(double noise)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

//   for (i = 0; i < BPTT_PAIRS; i++) {
//     weight_has_noise[i] = 0;
//   }

    for (i = 0; i < BPTT_PAIRS; i++) {
        *weight[i] = backup_weight[i] + random_double(-noise, noise);
    }
}


void bptt_backup_weights(void)
{
    int i;
    for (i = 0; i < BPTT_PAIRS; i++) {
        backup_weight[i] = *weight[i];
    }
}

void bptt_undo_lesion(void)
{
    int i;
    for (i = 0; i < BPTT_PAIRS; i++) {
        *weight[i] = backup_weight[i];
    }
}

// static int map_vh(int i, int j)
// {
//     return BPTT_K*i+j;
// }
//
// static int map_hv(int j, int i)
// {
//     return BPTT_N*BPTT_K+K*K + BPTT_N*j+i;
// }
//
// static int map_hh(int i, int j)
// {
//     return BPTT_N*BPTT_K + BPTT_K*i+j;
// }

int bptt_get_iterations_so_far(void)
{
    return bptt_iterations_so_far;
}


void bptt_inc_iterations_so_far(void)
{
    bptt_iterations_so_far++;
}

int bptt_stage_to_type(int stage)
{
    switch(stage)
    {
    case BPTT_STAGE_NAME:
        return NAME;
        break;
    case BPTT_STAGE_VISUAL:
        return VISUAL;
        break;
    case BPTT_STAGE_VERBAL:
        return VERBAL;
        break;
    }
    return -1;
}
int bptt_clamped_type_get(void)
{
    return bptt_stage_to_type(clamped_stage);
}

void bptt_print_weight_stats(void)
{

    double average_weight = 0.0, average_weight_hh = 0.0, average_weight_hv = 0.0, average_weight_vh = 0.0;
    double fabs_average_weight = 0.0, fabs_average_weight_hh = 0.0, fabs_average_weight_hv = 0.0, fabs_average_weight_vh = 0.0;
    int i;
    for (i = 0; i < BPTT_PAIRS; i++) {
        average_weight += *weight[i];
        fabs_average_weight += fabs(*weight[i]);
    }
    average_weight /= (double) BPTT_PAIRS;
    fabs_average_weight /= (double) BPTT_PAIRS;

    for (i = 0; i < BPTT_N*BPTT_K; i++) {
        average_weight_vh += *weight[i];
        fabs_average_weight_vh += fabs(*weight[i]);
    }
    average_weight_vh /= (double) (BPTT_N*BPTT_K);
    fabs_average_weight_vh /= (double) (BPTT_N*BPTT_K);

    for (i = BPTT_N*BPTT_K; i < BPTT_N*BPTT_K + BPTT_K*BPTT_K; i++) {
        average_weight_hh += *weight[i];
        fabs_average_weight_hh += fabs(*weight[i]);
    }
    average_weight_hh /= (double) (BPTT_K*BPTT_K);
    fabs_average_weight_hh /= (double) (BPTT_K*BPTT_K);

    for (i = BPTT_N*BPTT_K + BPTT_K*BPTT_K; i < BPTT_PAIRS; i++) {
        average_weight_hv += *weight[i];
        fabs_average_weight_hv += fabs(*weight[i]);
    }
    average_weight_hv /= (double) (BPTT_K*BPTT_N);
    fabs_average_weight_hv /= (double) (BPTT_K*BPTT_N);
    //   ( + BPTT_K*BPTT_K + BPTT_K*BPTT_N)


    printf("Total Average = %f\nInput Average = %f\nHidden Average = %f\nOutput Average = %f\n", average_weight, average_weight_vh, average_weight_hh, average_weight_hv);
    printf("Total Absolute Average = %f\nInput Absolute Average = %f\nHidden Absolute Average = %f\nOutput Absolute Average = %f\n", fabs_average_weight, fabs_average_weight_vh, fabs_average_weight_hh, fabs_average_weight_hv);
}


void bptt_get_weight_stats(double *array)
{

    double average_weight = 0.0, average_weight_hh = 0.0, average_weight_hv = 0.0, average_weight_vh = 0.0;
    double fabs_average_weight = 0.0, fabs_average_weight_hh = 0.0, fabs_average_weight_hv = 0.0, fabs_average_weight_vh = 0.0;
    int i;
    for (i = 0; i < BPTT_PAIRS; i++) {
        average_weight += *weight[i];
        fabs_average_weight += fabs(*weight[i]);
    }
    average_weight /= (double) BPTT_PAIRS;
    fabs_average_weight /= (double) BPTT_PAIRS;

    for (i = 0; i < BPTT_N*BPTT_K; i++) {
        average_weight_vh += *weight[i];
        fabs_average_weight_vh += fabs(*weight[i]);
    }
    average_weight_vh /= (double) (BPTT_N*BPTT_K);
    fabs_average_weight_vh /= (double) (BPTT_N*BPTT_K);

    for (i = BPTT_N*BPTT_K; i < BPTT_N*BPTT_K + BPTT_K*BPTT_K; i++) {
        average_weight_hh += *weight[i];
        fabs_average_weight_hh += fabs(*weight[i]);
    }
    average_weight_hh /= (double) (BPTT_K*BPTT_K);
    fabs_average_weight_hh /= (double) (BPTT_K*BPTT_K);

    for (i = BPTT_N*BPTT_K + BPTT_K*BPTT_K; i < BPTT_PAIRS; i++) {
        average_weight_hv += *weight[i];
        fabs_average_weight_hv += fabs(*weight[i]);
    }
    average_weight_hv /= (double) (BPTT_K*BPTT_N);
    fabs_average_weight_hv /= (double) (BPTT_K*BPTT_N);
    //   ( + BPTT_K*BPTT_K + BPTT_K*BPTT_N)


//   printf("Total Average = %f\nInput Average = %f\nHidden Average = %f\nOutput Average = %f\n", average_weight, average_weight_vh, average_weight_hh, average_weight_hv);
//   printf("Total Absolute Average = %f\nInput Absolute Average = %f\nHidden Absolute Average = %f\nOutput Absolute Average = %f\n", fabs_average_weight, fabs_average_weight_vh, fabs_average_weight_hh, fabs_average_weight_hv);

    array[0] =     average_weight;
    array[1] =        average_weight_vh;
    array[2] =          average_weight_hh;
    array[3] =           average_weight_hv;
    array[4] =           fabs_average_weight;
    array[5] =        fabs_average_weight_vh;
    array[6] =          fabs_average_weight_hh;
    array[7] =            fabs_average_weight_hv;


}
