#include "bm.h"

// int pattern[P_MAX][N_SUBPATTERN][BM_N_VISUAL];
// // int bm_specificity_level[P_MAX];
// #define BM_STRING 28
// char bm_name_string[P_MAX][BM_STRING];

//timing
time_t  t_0, t_1; /* time_t is defined on <time.h> and <sys/types.h> as long */
clock_t c_0, c_1; /* clock_t is defined on <time.h> and <sys/types.h> as int */

void bm_start_timing(void)
{
    t_0 = time(NULL);
    c_0 = clock();
}

void bm_stop_timing(void)
{
    t_1 = time(NULL);
    c_1 = clock();
    printf ("\telapsed wall clock time: %ld\n", (long) (t_1 - t_0));
    printf ("\telapsed CPU time:        %f\n", (float) (c_1 - c_0) / CLOCKS_PER_SEC);
}

void bm_init_random(void)
{
    srand(time(NULL));
}

int bm_valid_index(int i, int max_i)
{
    return ((i >= 0) && (i < max_i));
}


// int bm_valid_clamping_stage(int s)
// {
//   switch (s)
//   {
//     case BM_MINUS_NAME:
//         return 1;
//     case BM_MINUS_NAME_PDPTOOL:
//         return 1;
//     case BM_MINUS_VISUAL:
//         return 1;
//     case BM_MINUS_VERBAL:
//         return 1;
//     case BM_PLUS:
// 	return 1;
//     default:
//         return 0;
//   }
// }

int bm_valid_count_type(int t)
{
    switch (t)
    {
    case BM_MINUS_NAME:
        return 1;
//     case BM_MINUS_NAME_PDPTOOL:
//         return 1;
    case BM_MINUS_VISUAL:
        return 1;
    case BM_MINUS_VERBAL:
        return 1;
    case BM_PLUS:
        return 1;
    case BM_PLUS_NAME:
        return 1;
    case BM_DELTA:
        return 1;
    default:
        return 0;
    }
//     if ((t > -1) && (t < 8)) {
//         return 1;
//     }
    return 0;

}


int bm_valid_unit_type(int t)
{
    switch (t)
    {
    case NAME:
        return 1;
    case VISUAL:
        return 1;
//     case PERCEPTUAL:
//         return 1;
//     case FUNCTIONAL:
//         return 1;
//     case ENCYCLOPEDIC:
//         return 1;
    case VERBAL:
        return 1;
    case VISIBLE:
        return 1;
    case HIDDEN:
        return 1;
    case BM_BIAS:
        return 1;
    default:
        return 0;
    }
}

// all units
int state[BM_UNITS]; // do not access directly


// // visible units
// int *bm_visible;
// int *bm_name;
// int *bm_visual;
// int *bm_verbal;
// int *bm_perceptual;
// int *bm_functional;
// int *bm_encyclopedic;
// // hidden units
// int *bm_hidden;
// int *bm_semantic;

// connection weights + biases
double weight[BM_PAIRS + BM_UNITS]; //BM_PAIRS for the units and BM_UNITS for the biases

// state
void bm_randomise_states(void)
{
    int i;
    for (i = 0; i < BM_UNITS; i++)
    {
// 	if (!bm_clamped_index(i)) {
// 	  state[i] = bm_random_state();
// 	}
        bm_put_state(i, bm_random_state());
    }
}

void bm_init_states(void)
{
    bm_randomise_states();

//     bm_visible = &state[0];
//     bm_name = &state[0];
//     bm_visual = &state[BM_N_NAME];
//     bm_verbal = &state[BM_N_NAME + BM_N_VISUAL];
//     bm_perceptual = &state[BM_N_NAME + BM_N_VISUAL];
//     bm_functional = &state[BM_N_NAME + BM_N_VISUAL + BM_N_PERCEPTUAL];
//     bm_encyclopedic = &state[BM_N_NAME + BM_N_VISUAL + BM_N_PERCEPTUAL + BM_N_FUNCTIONAL];
//     bm_hidden = &state[N];
//     bm_semantic = &state[N];
}

int bm_random_state(void)
{
    if (rand()%2)
    {
        return BM_ON;
    }
    else
    {
        return BM_OFF;
    }
}



int bm_valid_state(int s)
{
    if ((s == BM_ON) || (s == BM_OFF))
    {
        return 1;

    }
    else
    {
        return 0;
    }
}

// the reason put_s is not called put_state_type is because it is a wholly internal function and should not really be called from outside utils.c and bm.c
// i is an array index, s is a state value, t is the array/pointer type
// void bm_put_s(int i, int s, int t)
// {
//
//
//     if (bm_clamped(t)) {
//         printf("trying to modify clamped!\n");
//     }
//
//     if (bm_valid_state(s))
//     {
//         switch(t)
//         {
//         case VISIBLE:
//             if (bm_valid_index(i , BM_N))
//             {
//                 if (bm_valid_index(i, BM_N_NAME))
//                 {
//                     if (!bm_clamped(NAME))
//                     {
//                         state[i] = s;
//                         break;
//                     }
//                     else
//                     {
//                         bm_error("utils.c", "put_s", "Trying to modify clamped name", i);
//                         break;
//                     }
//
//                 }
//
//                 i -= BM_N_NAME;
//
//                 if (bm_valid_index(i, BM_N_VISUAL))
//                 {
//                     if (!bm_clamped(VISUAL))
//                     {
//                         state[i] = s;
//                     }
//                     else
//                     {
//                         bm_error("utils.c", "put_s", "Trying to modify clamped visual", i);
//                     }
//                     break;
//                 }
//
//                 i -= BM_N_VISUAL;
//
//                 if (bm_valid_index(i, BM_N_VERBAL))
//                 {
//                     if (!bm_clamped(VERBAL))
//                     {
//                         state[i] = s;
//                     }
//                     else
//                     {
//                         bm_error("utils.c", "put_s", "Trying to modify clamped verbal", i);
//                     }
//                     break;
//                 }
//
// //                 if (bm_valid_index(i, BM_N_PERCEPTUAL))
// //                 {
// //                     if (!bm_clamped(PERCEPTUAL))
// //                     {
// //                         bm_perceptual[i] = s;
// //                     }
// //                     else
// //                     {
// //                         bm_error("utils.c", "put_s", "Trying to modify clamped perceptual", i);
// //                     }
// //                     break;
// //                 }
// //
// //                 i -= BM_N_PERCEPTUAL;
// //
// //                 if (bm_valid_index(i, BM_N_FUNCTIONAL))
// //                 {
// //                     if (!bm_clamped(FUNCTIONAL))
// //                     {
// //                         bm_functional[i] = s;
// //                     }
// //                     else
// //                     {
// //                         bm_error("utils.c", "put_s", "Trying to modify clamped functional", i);
// //                     }
// //                     break;
// //                 }
// //
// //                 i -= BM_N_FUNCTIONAL;
// //
// //                 if (bm_valid_index(i, BM_N_ENCYCLOPEDIC))
// //                 {
// //                     if (!bm_clamped(ENCYCLOPEDIC))
// //                     {
// //                         bm_encyclopedic[i] = s;
// //                     }
// //                     else
// //                     {
// //                         bm_error("utils.c", "put_s", "Trying to modify clamped encyclopedic", i);
// //                     }
// //                     break;
// //                 }
//             }
//             else
//             {
//                 bm_error("utils.c", "put_s", "Invalid index for visible", i);
//             }
//             break;
//
//         case HIDDEN:
//             if (bm_valid_index(i, BM_K))
//             {
//                 state[BM_N+i] = s;
//             }
//             else
//             {
//                 bm_error("utils.c", "put_s", "Invalid index for hidden", i);
//             }
//             break;
//
//         case BM_STATE:
//             if (bm_valid_index(i, BM_UNITS))
//             {
//                 if (bm_valid_index(i, BM_N))
//                 {
//                     bm_put_s(i, s, VISIBLE);
//                     break;
//                 }
//
//                 i -= BM_N;
//
//                 if (bm_valid_index(i, BM_K))
//                 {
//                     bm_put_s(i, s, HIDDEN);
//                     break;
//                 }
//             }
//             else
//             {
//                 bm_error("utils.c", "put_s", "Invalid index for state", i);
//             }
//             break;
//
//         default:
//             bm_error("utils.c", "put_s", "Invalid type of unit", t);
//         }
//     }
//     else
//     {
//         bm_error("utils.c", "put_s", "Invalid state value", s);
//     }
// }


// returns the state of a unit i from array of type t
// int bm_get_state_type(int i, int t)
// {
//     switch(t)
//     {
//     case VISIBLE:
//         if (bm_valid_index(i , BM_N))
//         {
//             if (bm_valid_index(i, BM_N_NAME))
//             {
//                 return state[i];
//             }
//
//             i -= BM_N_NAME;
//
//             if (bm_valid_index(i, BM_N_VISUAL))
//             {
//                 return state[i];
//             }
//
//             i -= BM_N_VISUAL;
//
//             if (bm_valid_index(i, BM_N_VERBAL))
//             {
//                 return state[i];
//             }
//
// //             if (bm_valid_index(i, BM_N_PERCEPTUAL))
// //             {
// //                 return bm_perceptual[i];
// //             }
// //
// //             i -= BM_N_PERCEPTUAL;
// //
// //             if (bm_valid_index(i, BM_N_FUNCTIONAL))
// //             {
// //                 return bm_functional[i];
// //             }
// //
// //             i -= BM_N_FUNCTIONAL;
// //
// //             if (bm_valid_index(i, BM_N_ENCYCLOPEDIC))
// //             {
// //                 return bm_encyclopedic[i];
// //             }
//         }
//         else
//         {
//             bm_error("utils.c", "get_state_type", " Invalid index for visible", i);
//             return 0;
//         }
//
//     case HIDDEN:
//         if (bm_valid_index(i, BM_K))
//         {
//             return state[BM_N+i];
//         }
//         else
//         {
//             bm_error("utils.c", "get_state_type", " Invalid index for hidden", i);
//             return 0;
//         }
//
//     case BM_STATE:
//         if (bm_valid_index(i, BM_UNITS))
//         {
//             if (bm_valid_index(i, BM_N))
//             {
//                 return bm_get_state_type(i, VISIBLE);
//             }
//
//             i -= BM_N;
//
//             if (bm_valid_index(i, BM_K))
//             {
//                 return bm_get_state_type(i, HIDDEN);
//             }
//         }
//         else
//         {
//             bm_error("utils.c", "get_state_type", " Invalid index for state", i);
//             return 0;
//         }
//
//     case NAME:
//         if (bm_valid_index(i, BM_N_NAME))
//         {
//             return state[i];
//         }
//         else
//         {
//             bm_error("utils.c", "get_state_type", " Invalid index for name", i);
//             return 0;
//         }
//     case VISUAL:
//         if (bm_valid_index(i, BM_N_VISUAL))
//         {
//             return state[BM_N_NAME+i];
//         }
//         else
//         {
//             bm_error("utils.c", "get_state_type", " Invalid index for visual", i);
//             return 0;
//         }
// //     case PERCEPTUAL:
// //         if (bm_valid_index(i, BM_N_PERCEPTUAL))
// //         {
// //             return bm_perceptual[i];
// //         }
// //         else
// //         {
// //             bm_error("utils.c", "get_state_type", " Invalid index for perceptual", i);
// //             return 0;
// //         }
// //     case FUNCTIONAL:
// //         if (bm_valid_index(i, BM_N_FUNCTIONAL))
// //         {
// //             return bm_functional[i];
// //         }
// //         else
// //         {
// //             bm_error("utils.c", "get_state_type", " Invalid index for functional", i);
// //             return 0;
// //         }
// //     case ENCYCLOPEDIC:
// //         if (bm_valid_index(i, BM_N_ENCYCLOPEDIC))
// //         {
// //             return bm_encyclopedic[i];
// //         }
// //         else
//     case VERBAL:
//         if (bm_valid_index(i, BM_N_VERBAL))
//         {
//             return state[BM_N_NAME+BM_N_VISUAL+i];
//         }
//         else
//         {
//             bm_error("utils.c", "get_state_type", " Invalid index for encyclopedic", i);
//             return 0;
//         }
//
//
//     default:
//         bm_error("utils.c", "get_state_type", " Invalid type of unit", t);
//         return 0;
//     }
// }

int bm_get_state(int i)
{
//     return bm_get_state_type(i, BM_STATE);
    return state[i];
}

int bm_get_state_type(int i, int t)
{
    return bm_get_state(i-bm_i0_of(t));
}

int bm_get_visible(int i)
{
//     return bm_get_state_type(i, VISIBLE);
    return state[i];
}

int bm_get_hidden(int i)
{
//     return bm_get_state_type(i, HIDDEN);
    return state[i+BM_N];
}


void bm_put_state(int i, int s)
{
//     bm_put_s(i, s, BM_STATE);




    if (!bm_clamped_index(i)) {
        state[i] = s;

    }
}

void bm_put_visible(int i, int s)
{
    //     bm_put_s(i, s, VISIBLE);#

    if (!bm_clamped_index(i)) {
        state[i] = s;
    }
}

void bm_put_hidden(int i, int s)
{
//     bm_put_s(i, s, HIDDEN);


    state[i+BM_N] = s;
}

// weights and biases

double bm_random_weight(void)
{
    double max = 0.2, min = -0.2;
    return (rand() * ((max - min) / (double) RAND_MAX)) + min;
}

void bm_init_weights(void)
{
    int i;
    for (i = 0; i < BM_PAIRS; i++)
    {
        weight[i] = bm_random_weight();
        // 		weight[i] = 0.01;
    }
}

void bm_init_biases(void)
{
    int i;
    for (i = BM_PAIRS; i < BM_UNITS + BM_PAIRS; i++)
    {
        weight[i] = bm_random_weight();
        // 		weight[i] = 0.0;
    }
}

void bm_put_weight(int i, double w)
{
    if (bm_valid_index(i, BM_PAIRS))
    {
        weight[i] = w;
    }
    else
    {
        bm_error("utils.c", "put_weight", "Invalid index for weight", i);
    }
}

double bm_get_weight(int i)
{
    if (bm_valid_index(i, BM_PAIRS))
    {
        return weight[i];
    }
    else
    {
        bm_error("utils.c", "bm_get_weight", "Invalid index for weight", i);
        return -1;
    }
}

double bm_get_bias(int i)
{
    if (bm_valid_index(i, BM_UNITS))
    {
        return weight[i + BM_PAIRS];
    }

    bm_error("utils.c", "bm_get_bias", " Invalid index for bias", i);
    return -1;

}

void bm_put_bias(int i, double b)
{
    if (bm_valid_index(i, BM_UNITS))
    {
        weight[i + BM_PAIRS] = b;
    }
    else {
        bm_error("utils.c", "bm_put_bias", " Invalid index for bias", i);
    }

}


// increments weight by value w
void bm_inc_weight(int i, double w)
{
    double temp = bm_get_weight(i);
    temp += w;
    bm_put_weight(i, temp);
}

// increments bias by value b
void bm_inc_bias(int i, double b)
{
    double temp = bm_get_bias(i);
    temp += b;
    bm_put_bias(i, temp);
}

// given visible unit i and hidden unit j returns their connection weight index
int bm_map_vh(int i, int j)
{
    if (bm_valid_index(i, BM_N) && bm_valid_index(j, BM_K))
    {
        return BM_N * j + i;
    }

    bm_error("utils.c", "map_vh", "Invalid indices", exp(1));
    return -1;

}

// given visible unit i and hidden unit j returns their connection weight index
int bm_map_vh_type(int i, int j, int t)
{
    return bm_map_vh(i+ + bm_i0_of(t), j);
}

// given units hidden(i) <-> hidden(j) returns their connection weight index
int bm_map_hh(int i, int j)
{

    if (i > j)
    {
        // i must always be 1 or more less than j
        int temp;
        temp = i;
        i = j;
        j = temp;
    }
    if (bm_valid_index(i, BM_K) && bm_valid_index(j, BM_K) && (i != j))
    {
        return BM_VH + j - 1 + i * (BM_K - i/2.0 - 1.5);
    }

    bm_error("utils.c", "map_hh", "Invalid indices", exp(1));
    return -1;

}

// k must be pre-mapped to v-h or h-h
void bm_inc_weight_step(int k)
{
    if (bm_get_delta(k) > 0.0)
    {
        bm_inc_weight(k, BM_STEP*bm_r);
    }
    else if (bm_get_delta(k) < 0.0)
    {
        bm_inc_weight(k, BM_STEP*(-1.0)*bm_r);
    }
}

void bm_inc_bias_step(int k)
{
    if (bm_get_delta(BM_PAIRS + k) > 0.0)
    {
        bm_inc_bias(k, BM_STEP*bm_r);
    }
    else if (bm_get_delta(BM_PAIRS + k) < 0.0)
    {
        bm_inc_bias(k, BM_STEP*(-1.0)*bm_r);
    }
}

// // given connection weight index (visible(i) <-> hidden(j) returns the corresponding hidden j
// int pam_vh_j(int k)
// {
// 	if (bm_valid_index(k, VH))
// 		return k%K;
// 	else
// 	{
// // 		bm_error("utils.c", "pam_vh_j", "Invalid index", k);
// 		return -1;
// 	}
// }
//
// // given connection weight index (visible(i) <-> hidden(j) returns the corresponding visible i
// int pam_vh_i(int k)
// {
// 	if (bm_valid_index(k, VH))
// 	{
// 		return (k - pam_vh_j(k)) / K;
// 	}
// 	else
// 	{
// 		bm_error("utils.c", "pam_vh_i", "Invalid index", k);
// 		return -1;
// 	}
// }
//
// // given connection weight index k (hidden(i) <-> hidden(j)) returns the corresponding hidden i
// int pam_hh_i(int k)
// {
// 	if (bm_valid_index(k, HH))
// 	{
// 		return K - 0.5 - sqrt(K*K - K + 0.25 - 2*k);
// 	}
// 	else
// 	{
// 		bm_error("utils.c", "pam_hh_i", "Invalid index", k);
// 		return -1;
// 	}
// }
//
// // given connection weight index k (hidden(i) <-> hidden(j)) returns the corresponding hidden j
// int pam_hh_j(int k)
// {
// 	if (bm_valid_index(k, HH))
// 		return k - map_hh(pam_hh_i(k), 0);
// 	else
// 	{
// 		bm_error("utils.c", "pam_hh_j", "Invalid index", k);
// 		return -1;
// 	}
// }

void bm_error(char file[], char func[], char text[], double i) // I should be an array of ints/doubles to output, should outout contents of that array, now it's a silly hack to output just an int, but it's not important, so leave it.
{
    if (i != exp(1))
    {
        fprintf(stderr, "ERROR: %s: %s: %s: %i (press \"q\" to quit, return to continue)\n", file, func, text, (int)i);
    }
    else
    {
        fprintf(stderr, "ERROR: %s: %s: %s (press \"q\" to quit, return to continue)\n", file, func, text);
    }

    if (getchar() == 'q')
    {
        exit(0);
    }
}







// Input/Output weights to/from file
void bm_save_weights(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    FILE *fp = NULL;
    int i;
    fp = fopen(filename, "wt");
    fprintf(fp, " %i", bm_iterations_so_far);

    fprintf(fp, "\n");

    fprintf(fp, " %i %i", p_min, p_max);

    fprintf(fp, "\n\n");
//   fprintf(fp, "double weight[%i] = {\n", BM_PAIRS + BM_UNITS);
    for (i = 0; i < BM_PAIRS + BM_UNITS; i++) {
        fprintf(fp, " %lf", weight[i]);
    }

    fprintf(fp, "\n\n");

    for (i = 0; i < bm_iterations_so_far; i++) {
        fprintf(fp, " %lf", Total_Error[i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < bm_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[NAME_ERROR][i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < bm_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[VISUAL_ERROR][i]);
    }

    fprintf(fp, "\n");

    for (i = 0; i < bm_iterations_so_far; i++) {
        fprintf(fp, " %lf", Error[VERBAL_ERROR][i]);
    }

    fprintf(fp, "\nEOF\n");



    if (fp) {
        fclose(fp);
    }

}



// Input/Output weights to/from file
int bm_load_weights(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    FILE *fp = NULL;
    int i;

    fp = fopen(filename, "r");
    if (!fscanf(fp, " %i", &bm_iterations_so_far)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bm_iterations_so_far", 0);
        return 0;
    }
    if (!fscanf(fp, " %i %i", &p_min, &p_max)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: p_min and p_max", 0);
        return 0;
    }


    // 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));

    for (i = 0; i < (BM_PAIRS + BM_UNITS); i++) {
        if (!fscanf(fp, " %lf", &weight[i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight[]", 0);
            return 0;
        }

    }


    // 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));
    // 	printf("BM_Total_Error = ");
    for (i = 0; i < bm_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Total_Error[i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: Total_Error[]", 0);
            return 0;
        }
        // 	  printf("%f ", BM_Total_Error[i]);
    }
    // 	printf("\n");

    // 	(fscanf(fp, "\n")) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid file format", 0));
    // 	printf("BM_Error[BM_STAGE_NAME] = ");
    for (i = 0; i < bm_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[NAME_ERROR][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: Error[NAME_ERROR][]", 0);
            return 0;
        }
        // 	  printf("%f ", BM_Error[BM_STAGE_NAME][i]);
    }
    // 	printf("\n");

    // 	printf("BM_Error[BM_STAGE_VISUAL] = ");
    for (i = 0; i < bm_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[VISUAL_ERROR][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: Error[VISUAL_ERROR][]", 0);
            return 0;
        }
        // 	  printf("%f ", BM_Error[BM_STAGE_VISUAL][i]);
    }
    // 	printf("\n");

    // 	printf("BM_Error[BM_STAGE_VERBAL] = ");
    for (i = 0; i < bm_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Error[VERBAL_ERROR][i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: Error[VERBAL_ERROR][]", 0);
            return 0;
        }
        // 	  printf("%f ", BM_Error[BM_STAGE_VERBAL][i]);
    }
    /*
        char buf[BUFSIZ] = "Garbage";
        i = 0;

        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
    //             printf ("Line %4d: %s", i, buf);
            i++;
        }
              */


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




int bm_i0_of(int t)
{
    switch (t)
    {
    case NAME:
        return 0;
        break;
    case VISUAL:
        return BM_N_NAME;
        break;
//     case PERCEPTUAL:
//         return BM_N_NAME + BM_N_VISUAL;
//         break;
//     case FUNCTIONAL:
//         return BM_N_NAME + BM_N_VISUAL + BM_N_PERCEPTUAL;
//         break;
//     case ENCYCLOPEDIC:
//         return BM_N_NAME + BM_N_VISUAL + BM_N_PERCEPTUAL + BM_N_FUNCTIONAL;
//         break;
    case VERBAL:
        return BM_N_NAME + BM_N_VISUAL;
        break;
    case VISIBLE:
        return 0;
        break;
    case HIDDEN:
        return BM_N;
        break;
    default:
        bm_error("utils.c", "i0_of", "Invalid type", t);
        return -1;
    }
    return -1;
}

int bm_N_of(int t)
{
    switch (t)
    {
    case NAME:
        return BM_N_NAME;
    case VISUAL:
        return BM_N_VISUAL;
//     case PERCEPTUAL:
//         return BM_N_PERCEPTUAL;
//     case FUNCTIONAL:
//         return BM_N_FUNCTIONAL;
//     case ENCYCLOPEDIC:
//         return BM_N_ENCYCLOPEDIC;
    case VERBAL:
        return BM_N_VERBAL;
    case VISIBLE:
        return BM_N;
    case HIDDEN:
        return BM_K;
    case BM_BIAS:
        return BM_UNITS;
    default:
        bm_error("utils.c", "N_of", "Invalid type", t);
        return -1;
    }
    return -1;
}

//input: state array index i (0 to BM_UNITS)
//output: type of i (NAME, VISUAL, PERCEPTUAL, ..., HIDDEN)
int bm_type_of(int i)
{
    if (bm_valid_index(i, BM_N))
    {
        if (bm_valid_index(i, BM_N_NAME))
        {
            return NAME;
        }

        i -= BM_N_NAME;

        if (bm_valid_index(i, BM_N_VISUAL))
        {
            return VISUAL;
        }

        i -= BM_N_VISUAL;

        if (bm_valid_index(i, BM_N_VERBAL))
        {
            return VERBAL;
        }

// 	i -= BM_N_VISUAL;

//         if (bm_valid_index(i, BM_N_PERCEPTUAL))
//         {
//             return PERCEPTUAL;
//         }
//
//         i -= BM_N_PERCEPTUAL;
//
//         if (bm_valid_index(i, BM_N_FUNCTIONAL))
//         {
//             return FUNCTIONAL;
//         }
//
//         i -= BM_N_FUNCTIONAL;
//
//         if (bm_valid_index(i, BM_N_ENCYCLOPEDIC))
//         {
//             return ENCYCLOPEDIC;
//         }
    }

    i -= BM_N;

    if (bm_valid_index(i, BM_K))
    {
        return HIDDEN;
    }

    bm_error("utils.c", "type_of", "Invalid index for state", i);
    return -1;
}

// counts

// cooccurence statistics
// t is the type
static double count[7][BM_PAIRS + BM_UNITS];
// int count_minus_name[BM_PAIRS + BM_UNITS];
// int count_minus_visual[BM_PAIRS + BM_UNITS];
// int count_minus_verbal[BM_PAIRS + BM_UNITS];
// int count_minus_perceptual[BM_PAIRS + BM_UNITS];
// int count_minus_functional[BM_PAIRS + BM_UNITS];
// int count_minus_encyclopedic[BM_PAIRS + BM_UNITS];

void bm_put_delta(int i, double d)
{
    bm_put_count(i, d, BM_DELTA);
}

double bm_get_delta(int i)
{
    return bm_get_count(i, BM_DELTA);
}

void bm_put_count(int i, double c, int t)
{


    if (bm_valid_index(i, BM_PAIRS+BM_UNITS))
    {
        if (bm_valid_count_type(t))
        {
            if (((c <= (BM_C*P)) && (c >= 0.0)) || (t == BM_DELTA))
            {
                count[t][i] = c;
            }
            else
            {
                bm_error("utils.c", "put_count", "Invalid value for cooccurence count", c);
                return;
            }
        }
        else
        {
            bm_error("utils.c", "put_count", "Invalid type of cooccurence count", t);
            return;
        }
    }
    else
    {
        bm_error("utils.c", "put_count", "Invalid index of cooccurence count", i);
        return;
    }
}

double bm_get_count(int i, int t)
{
    if (bm_valid_index(i, BM_PAIRS+BM_UNITS))
    {
        if (bm_valid_count_type(t))
        {
            return count[t][i];
        }
        else
        {
            bm_error("utils.c", "get_count", "Invalid type of cooccurence count", t);
            return -1;
        }
    }
    else
    {
        bm_error("utils.c", "get_count", "Invalid index of cooccurence count", i);
        return -1;
    }
    return -1;
}

void bm_inc_count(int i, int c, int t)
{
    int temp = bm_get_count(i, t);
    temp += c;
    bm_put_count(i, temp, t);
}
void bm_init_count(int t)
{
    int i;
    for (i = 0; i < BM_PAIRS + BM_UNITS; i++)
    {
        bm_put_count(i, 0, t);
    }
}

void bm_init_count_plus(void)
{
    bm_init_count(BM_PLUS);
}

void bm_init_count_plus_name(void)
{
    bm_init_count(BM_PLUS_NAME);
}

// void bm_init_count_minus_name_pdptool(void)
// {
//     bm_init_count(BM_MINUS_NAME_PDPTOOL);
// }

void bm_init_count_minus_name(void)
{
    bm_init_count(BM_MINUS_NAME);
}

void bm_init_count_minus_visual(void)
{
    bm_init_count(BM_MINUS_VISUAL);
}

void bm_init_count_minus_verbal(void)
{
    bm_init_count(BM_MINUS_VERBAL);
}

void bm_init_count_delta(void)
{
    bm_init_count(BM_DELTA);
}

// initialise coocurrence statistics
void bm_init_counts(void)
{
    bm_init_count_plus();
    bm_init_count_plus_name();

    bm_init_count_minus_name();
    bm_init_count_minus_visual();
    bm_init_count_minus_verbal();
    bm_init_count_delta();
    // 	init_count_minus_perceptual();
    // 	init_count_minus_functional();
    // 	init_count_minus_encyclopedic();
}




// clamping units' states

// check if type t is clamped
int bm_clamped(int t)
{
    if ((bm_type_clamped == VISIBLE) && (t != HIDDEN)) {
        return 1;
    }
    return (bm_type_clamped == t);

}

//checks if a given state index (range: 0 -BM_UNITS) is in a clamped pattern
int bm_clamped_index(int i)
{
    if (bm_valid_index(i, BM_UNITS))
    {
        return bm_clamped(bm_type_of(i));
    }
    else
    {
        bm_error("utils.c", "clamped_index", "Invalid index for state", i);
        return -1;
    }

}
int bm_pattern_general(int p)
{
    return pattern_general(p);
}

int bm_p_clamped = -1;

int bm_get_clamped_pattern(void)
{
    return bm_p_clamped;

}

int bm_get_p_clamped(void)
{
    return bm_p_clamped;
}
int bm_get_clamped_stage(void)
{
    if (bm_type_clamped == NAME) {
        return BM_MINUS_NAME;
    }
    if (bm_type_clamped == VISUAL) {
        return BM_MINUS_VISUAL;
    }
    if (bm_type_clamped == VERBAL) {
        return BM_MINUS_VERBAL;
    }
    if (bm_type_clamped == VISIBLE) {
        return BM_PLUS;
    }

    return -1;
// return bm_type_clamped;
}
int bm_get_clamped_type(void)
{

    return bm_type_clamped;
}

// clamps units to a given pattern p of type t
void bm_clamp(int p, int t)
{
    int i;
    if (bm_valid_index(p, P_MAX))
    {
        bm_p_clamped = p;
        switch (t)
        {
        case NAME:
            bm_type_clamped = NAME;
            for (i = 0; i < BM_N_NAME; i++) {
                state[i] = get_pattern_type_int(p, NAME, i);
            }
            break;
        case VISUAL:
            bm_type_clamped = VISUAL;
            for (i = 0; i < BM_N_VISUAL; i++) {
                state[BM_N_NAME+i] = get_pattern_type_int(p, VISUAL, i);
            }
            break;
        case VERBAL:
            bm_type_clamped = VERBAL;
            for (i = 0; i < BM_N_VERBAL; i++) {
                state[BM_N_NAME+BM_N_VISUAL+i] = get_pattern_int(p, BM_N_NAME+BM_N_VISUAL+i);
            }
            break;
        case VISIBLE:
            bm_type_clamped = VISIBLE;
            for (i = 0; i < BM_N; i++) {
                state[i] = get_pattern_int(p, i);
            }
            break;
        default:
            bm_error("utils.c", "clamp", "Invalid type", t);
        }
    }
    else
    {
        bm_error("utils.c", "clamp", "Invalid pattern index", p);
    }
}
void bm_clamp_visible(p)
{
    bm_clamp(p, VISIBLE);
}

void bm_clamp_name(p)
{
    bm_clamp(p, NAME);
}

void bm_clamp_visual(p)
{
    bm_clamp(p, VISUAL);
}

void bm_clamp_verbal(p)
{
    bm_clamp(p, VERBAL);
}
void bm_unclamp(void)
{
    // reset clamped flags
    bm_type_clamped = -1;
    bm_p_clamped = -1;

}

void bm_clamp_pdptool_name_plus(int p)
{
    int i;
    bm_type_clamped = VISIBLE;
    for (i = 0; i < BM_N; i++) {
        state[i] = get_pattern_int(p, i);
    }
}

void bm_clamp_pdptool_visual_verbal_plus(int p)
{
    int i;
    bm_type_clamped = VISIBLE;
    for (i = BM_N-1; i >= 0; i--) {
        state[i] = get_pattern_int(p, i);
    }
}
// create an array with indices for accessing every unclamped units' states in a random order
void bm_randomise_indices(void)
{
    int r, i, temp;

//     for (i = 0; i < BM_UNITS; i++)
//     {
//         if (bm_clamped_index(i))
//         {
//             bm_random_index[i] = BM_CLAMPED;
// 	    if (bm_type_clamped == VISUAL) {
//
// 	    }
//
//         }
//         else
//         {

//             bm_random_index[i] = i;
//         }
//     }
    for (i = 0; i < BM_UNITS; i++)
    {
        bm_random_index[i] = i;
        r = (i + random_int(0, BM_UNITS-1)) % BM_UNITS;
        temp = bm_random_index[i];
        bm_random_index[i] = bm_random_index[r];
        bm_random_index[r] = temp;
    }
}

void bm_reset_learning_rate(void)
{
    bm_r = BM_R_MAX;
}

void bm_divide_weights(double n)
{
    int i;
    for (i = 0; i < BM_PAIRS; i++)
    {
        bm_put_weight(i, bm_get_weight(i) / (double) n);
    }
    for (i = 0; i < BM_UNITS; i++)
    {
        bm_put_bias(i, bm_get_bias(i) / (double) n);
    }
    printf("Divided weights by: %f\n", n);
}


void bm_lesion_random_weight(void)
{
    int x = 0, i = random_int(0, BM_PAIRS-1);

    while (!bm_get_weight(i))
    {
        i = random_int(0, BM_PAIRS-1);
        x++;
        if (x > PAIRS) {
            break;
        }
    }
    bm_put_weight(i, 0.0);
}




int bm_category(int i) //ALERT: this needs to change if the patterns change!!
{
    if (bm_valid_index(i, BM_N_NAME))
    {
        if (bm_valid_index(i, 6)) //this is the lazy late at night programming style, not recommended
        {
            return BIRD;
        }
        i -= 6;
        if (bm_valid_index(i, 6))
        {
            return MAMMAL;
        }
        i -= 6;
        if (bm_valid_index(i, 6))
        {
            return VEHICLE;
        }
        i -= 6;
        if (bm_valid_index(i, 8))
        {
            return HH_OBJ;
        }
        i -= 8;
        if (bm_valid_index(i, 6))
        {
            return TOOL;
        }
        i -= 6;
        if (bm_valid_index(i, 8))
        {
            return FRUIT;
        }
    }
    else
    {
        bm_error("utils.c", "category", "Invalid name pattern index", i);
    }
    return -1;
}

#define BM_LIVING 0
#define BM_INANIMATE 1

int bm_domain(int i)
{
    if (bm_valid_index(i, BM_N_NAME))
    {
        if ((bm_category(i) == BIRD) || (bm_category(i) == MAMMAL))
        {
            return BM_LIVING;
        }
        else
        {
            return BM_INANIMATE;
        }
    }
    else
    {
        bm_error("utils.c", "domain", "Invalid name pattern index", i);
    }
    return -1;
}


double tmp_weight[BM_PAIRS];

void bm_backup_weights(void)
{
    int i;
    for (i = 0; i < BM_PAIRS; i++)
    {
        tmp_weight[i] = weight[i];
//         printf("tmp_weight[%i] = %f, weight[%i] = %f\n", i, tmp_weight[i], i, weight[i]);
    }
}

void bm_undo_lesion(void)
{
    int i;
    for (i = 0; i < BM_PAIRS; i++)
    {
        weight[i] = tmp_weight[i];
//         printf("tmp_weight[%i] = %f, weight[%i] = %f\n", i, tmp_weight[i], i, weight[i]);

    }
}

void bm_disrupt_weights(double noise)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    //   for (i = 0; i < BPTT_PAIRS; i++) {
    //     weight_has_noise[i] = 0;
    //   }

    for (i = 0; i < BM_PAIRS; i++) {
        weight[i] = tmp_weight[i] + random_double(-noise, noise);
    }


}


void bm_lesion_weights(double percentage)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, n = (int)round(BM_PAIRS * percentage/100.0);
    for (i = 0; i < n; i++) {
        bm_lesion_random_weight();
    }
}

void bm_disrupt_random_weight(double noise)
{
    int i = random_int(0, BM_PAIRS-1);

    while (!bm_get_weight(i))
    {
        i = random_int(0, BM_PAIRS-1);
    }
    bm_inc_weight(i, random_double(noise*(-1), noise));
}


int bm_get_i_total(void)
{
    return bm_iterations_so_far;
//   return -1;
}

// get patterns that are base named
int *bm_get_base_ps(int p)
{
    if (pattern_general(p))
    {
        static int ps[3];

        if (p == 0 || p == 1 || p == 2)
        {
            ps[0] = 0;
            ps[1] = 1;
            ps[2] = 2;
        }

        if (p == 8 || p == 9 || p == 10)
        {
            ps[0] = 8;
            ps[1] = 9;
            ps[2] = 10;
        }

        if (p == 16 || p == 17 || p == 18)
        {
            ps[0] = 16;
            ps[1] = 17;
            ps[2] = 18;
        }

        if (p == 24 || p == 25 || p == 26)
        {
            ps[0] = 24;
            ps[1] = 25;
            ps[2] = 26;
        }
        return ps; //ALERT very wrong but just need to see if this works
    }
    else
    {
        bm_error("utils.c", "get_base_ps", "Invalid general name", p);
        return NULL;
    }
}

// return state number, for use in array to make neat code
int bm_type_to_state(int t)
{
    switch (t)
    {
    case NAME:
        return BM_MINUS_NAME;
        break;
    case VISUAL:
        return BM_MINUS_VISUAL;
        break;
    case VERBAL:
        return BM_MINUS_VERBAL;
        break;
    case VISIBLE:
        return BM_PLUS;
        break;
    default:
        bm_error("utils.c", "type_to_state", "Invalid type for free/clamped state", t);
        return -1;
    }
    return -1;
}

// return state number, for use in array to make neat code
int bm_network_state_to_unit_type(int s)
{
    switch (s)
    {
    case BM_MINUS_NAME:
        return NAME;
        break;
//     case BM_MINUS_NAME_PDPTOOL:
//         return NAME;
//         break;
    case BM_MINUS_VISUAL:
        return VISUAL;
        break;
    case BM_MINUS_VERBAL:
        return VERBAL;
        break;
    case BM_PLUS:
        return VISIBLE;
        break;
    case BM_PLUS_NAME:
        return VISIBLE;
        break;
    default:
        bm_error("utils.c", "bm_network_state_to_unit_type", "Invalid state for type", s);
        return -1;
    }
}


char *bm_get_name(int p)
{
    return get_name_string(p);
}

char *bm_get_phase(int t)
{
    switch (t)
    {
    case NAME:
        return "Minus Name";
    case VISUAL:
        return "Minus Visual";
    case VERBAL:
        return "Minus Verbal";
    case VISIBLE:
        return "Plus";
    default:
        return NULL;
    }
}


double bm_calculate_rms_error(int p, int t)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif

    int i;
    double total_error = 0.0;
    if (p < 0) {
        return(1.0);
    }
    else {
        if (pattern_general(p) && !pdptool_style_patterns() && (t == 0)) { //this is the only case the pseduo-pattern targets need to be calculated and used
            double tmp_error;
            total_error = (double) BM_N;
            int m, n_patterns, patterns[P_MAX];
            n_patterns = 0;
            get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);
            for (m = 0; m < n_patterns; m++) {
                tmp_error = 0.0;
                for (i = 0; i < BM_N; i++) {
                    tmp_error += sqr(get_pattern(patterns[m], i) - (double)bm_get_state(i));
                }
//                 printf("%d total_error = %f, tmp_error = %f\n", m, total_error, tmp_error);
                total_error = fmin(total_error, tmp_error);
            }
        } else {
            for (i = 0; i < BM_N; i++) {
                total_error += sqr(get_pattern(p, i) - (double)bm_get_state(i));
            }
        }
    }
    total_error /= (double) BM_N;
    return sqrt(total_error);
}


double bm_calculate_rms_error_type(int p, int t, int type_for_error)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif

    int i;
    int n_0 = i0_of(type_for_error), n_1 = n_0 + N_of(type_for_error);
    double total_error = 0.0;
    if (p < 0) {
        return(1.0);
    }
    else {
        if (pattern_general(p) && !pdptool_style_patterns() && (t == 0)) { //this is the only case the pseduo-pattern targets need to be calculated and used
            double tmp_error;
            total_error = (double) BM_N;
            int m, n_patterns, patterns[P_MAX];
            n_patterns = 0;
            get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);
            for (m = 0; m < n_patterns; m++) {
                tmp_error = 0.0;
                for (i = n_0; i < n_1; i++) {
                    tmp_error += sqr(get_pattern(patterns[m], i) - (double)bm_get_state(i));
                }
                //                 printf("%d total_error = %f, tmp_error = %f\n", m, total_error, tmp_error);
                total_error = fmin(total_error, tmp_error);
            }
        } else {
            for (i = n_0; i < n_1; i++) {
                total_error += sqr(get_pattern(p, i) - (double)bm_get_state(i));
            }
        }
    }
    total_error /= (double)(n_1 - n_0);
    return sqrt(total_error);
}
/*
double bm_calculate_rms_error(int p, int t)
{
  #if PRINT_FUNCTION
  printf("%s(%i)\n", __FUNCTION__, p);
  #endif

  int i, m;
  int patterns[P_MAX];
  int n_patterns;
  double total_error = 0.0;
  if (p < 0) {
    return(1.0);
  }
  else {
    if (pattern_general(p) && !pdptool_style_patterns() && (t == 0)) { //this is the only case the pseduo-pattern targets need to be calculated and used
	    double tmp_error;
            n_patterns = 0;
	    get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);
	    for (m = 0; m < n_patterns; m++) {
	      tmp_error = 0.0;
              for (i = 0; i < BM_N; i++) {
		tmp_error += sqr(get_pattern(patterns[m], i) - (double)bm_get_state(i));
	      }
	      total_error = fmin(total_error, tmp_error);
	    }
    }
    else                  // else p is either a base-level pattern or a pdptool-style pattern, neither of which need special consideration
        {
	  for (i = 0; i < BM_N; i++) {
	    total_error += sqr(get_pattern(p, i) - (double)bm_get_state(i));
	  }
	}
  }
  total_error /= (double) BM_N;
  return sqrt(total_error);
}   */

double bm_jaccard_distance(int p, int t)
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
//                         printf("%s(%i)\n", __FUNCTION__, p);

            double tmp_error = 1.0;
            total_error = (double) BM_N;
            int m, n_patterns = 0, patterns[P_MAX];
            get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);

            patterns[n_patterns] = p; // add current pattern
            n_patterns++;

            for (m = 0; m < n_patterns; m++) {
                tmp_error = 1.0;
                u = 0.0, n = 0.0;
                for (i = 0; i < BM_N; i++) {
                    u += (double) (bm_get_state(i) || get_pattern_int(patterns[m], i));
                    n += (double) (bm_get_state(i) && get_pattern_int(patterns[m], i));
                }


                if (u) {
                    tmp_error = ((u - fabs(n)) / u) / (double) BM_N;
                }
                total_error = fmin(total_error, tmp_error);


            }
        } else {
            for (i = 0; i < BM_N; i++) {
                u += (double) (bm_get_state(i) || get_pattern_int(p, i));
                n += (double) (bm_get_state(i) && get_pattern_int(p, i));
            }
            if (u) {
                total_error = ((u - fabs(n)) / u) / (double) BM_N;
            }
        }
    }

    return total_error;
}

double bm_jaccard_distance_type(int p, int input_t, int error_t)
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
//                         printf("%s(%i)\n", __FUNCTION__, p);

            double tmp_error = 1.0;
            total_error = (double) (n_1 - n_0);
            int m, n_patterns = 0, patterns[P_MAX];
            get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);

            patterns[n_patterns] = p; // add current pattern
            n_patterns++;

            for (m = 0; m < n_patterns; m++) {
                tmp_error = 1.0;
                u = 0.0, n = 0.0;
                for (i = n_0; i < n_1; i++) {
                    u += (double) (bm_get_state(i) || get_pattern_int(patterns[m], i));
                    n += (double) (bm_get_state(i) && get_pattern_int(patterns[m], i));
                }


                if (u) {
                    tmp_error = ((u - fabs(n)) / u) / (double) (n_1 - n_0);
                }
                total_error = fmin(total_error, tmp_error);


            }
        } else {
            for (i = n_0; i < n_1; i++) {
                u += (double) (bm_get_state(i) || get_pattern_int(p, i));
                n += (double) (bm_get_state(i) && get_pattern_int(p, i));
            }
            if (u) {
                total_error = ((u - fabs(n)) / u) / (double) (n_1 - n_0);
            }
        }
    }

    return total_error;
}

double bm_euclidean_distance_type(int p, int input_t, int error_t)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif

    int i;
    double total_error = 0.0;
    int n_0 = i0_of(error_t), n_1 = n_0 + N_of(error_t);

    if (p < 0) {
        return 1.0;
    }
    else {
        if (pattern_general(p) && !pdptool_style_patterns() && (input_t == 0)) { //this is the only case the alternative targets need to be calculated and used
//                         printf("%s(%i)\n", __FUNCTION__, p);

            total_error = 0.0;
            int m, n_patterns = 0, patterns[P_MAX];
            get_patterns_that_share_a_name(p, &patterns[0], &n_patterns);

            patterns[n_patterns] = p; // add current pattern
            n_patterns++;

            for (m = 0; m < n_patterns; m++) {
//                 tmp_error = 1.0;
//                 u = 0.0, n = 0.0;
                for (i = n_0; i < n_1; i++) {
//                     u += (double) (bm_get_state(i) || get_pattern_int(patterns[m], i));
//                     n += (double) (bm_get_state(i) && get_pattern_int(patterns[m], i));
                    total_error += sqr(bm_get_state(i)-get_pattern_int(patterns[m], i));
                }


//                 if (u) {
//                     tmp_error = ((u - fabs(n)) / u) / (double) (n_1 - n_0);
//                 }
                total_error = sqrt(total_error);


            }
        } else {
            for (i = n_0; i < n_1; i++) {
//                 u += (double) (bm_get_state(i) || get_pattern_int(p, i));
//                 n += (double) ;

                total_error += sqr(bm_get_state(i)-get_pattern_int(p, i));

            }
            total_error = sqrt(total_error);

        }
    }

    return total_error;
}

double bm_sorensen_distance(int p, int t)
{
    int i;
    double u = 0.0, n = 0.0;
    for (i = 0; i < BM_N; i++) {
        u += bm_get_state(i) || get_pattern_int(p, i);
        n += bm_get_state(i) && get_pattern_int(p, i);
    }

    if (u) {
        return ((u - 2*fabs(n)) / u) / (double) BM_N;
    }

    return 1.0;
}
int bm_get_iterations_so_far(void)
{
    return bm_iterations_so_far;
}


void bm_decay_weights(void)
{

    int i;
    for (i = 0; i < BM_PAIRS; i++) {
        weight[i] *= bm_decay_rate;

    }

}

void bm_inc_iterations_so_far(void)
{
    bm_iterations_so_far++;
}

char *bm_stage_string_get(int s)
{
    switch (s)
    {
    case BM_MINUS_NAME:
        return "Minus: Name";
//     case BM_MINUS_NAME_PDPTOOL:
//         return "Minus: Name";
    case BM_MINUS_VISUAL:
        return "Minus: Visual";
    case BM_MINUS_VERBAL:
        return "Minus: Verbal";
    case BM_PLUS:
        return "Plus";
    case BM_PLUS_NAME:
        return "Plus";
    default:
        return '\0';
    }
}



void bm_print_weight_stats(void)
{

    double average_weight = 0.0, average_weight_hh = 0.0, average_weight_hv = 0.0, average_weight_vh = 0.0, fabs_average_weight = 0.0, fabs_average_weight_hh = 0.0, fabs_average_weight_hv = 0.0, fabs_average_weight_vh = 0.0;;
    int i;

    //   #define BM_PAIRS	(BM_VH + BM_HH) /* all pairs */

    for (i = 0; i < BM_PAIRS; i++) {
        average_weight += weight[i];
        fabs_average_weight += fabs(weight[i]);
    }
    average_weight /= (double) BM_PAIRS;
    fabs_average_weight /= (double) BM_PAIRS;

    for (i = 0; i < BM_VH; i++) {
        average_weight_vh += weight[i];
        fabs_average_weight_vh += fabs(weight[i]);
    }
    average_weight_vh /= (double) (BM_VH);
    fabs_average_weight_vh /= (double) (BM_VH);

    for (i = BM_VH; i < BM_PAIRS; i++) {
        average_weight_hh += weight[i];
        fabs_average_weight_hh += fabs(weight[i]);
    }
    average_weight_hh /= (double) (BM_HH);
    fabs_average_weight_hh /= (double) (BM_HH);


    average_weight_hv = average_weight_vh;
    fabs_average_weight_hv = fabs_average_weight_vh;
    //   ( + BM_K*BM_K + BM_K*BM_N)


    printf("Total Average = %f\nInput Average = %f\nHidden Average = %f\nOutput Average = %f\n", average_weight, average_weight_vh, average_weight_hh, average_weight_hv);
    printf("Total Absolute Average = %f\nInput Absolute Average = %f\nHidden Absolute Average = %f\nOutput Absolute Average = %f\n", fabs_average_weight, fabs_average_weight_vh, fabs_average_weight_hh, fabs_average_weight_hv);

}


void bm_get_weight_stats(double *array)
{

    double average_weight = 0.0, average_weight_hh = 0.0, average_weight_hv = 0.0, average_weight_vh = 0.0, fabs_average_weight = 0.0, fabs_average_weight_hh = 0.0, fabs_average_weight_hv = 0.0, fabs_average_weight_vh = 0.0;;
    int i;

    //   #define BM_PAIRS	(BM_VH + BM_HH) /* all pairs */

    for (i = 0; i < BM_PAIRS; i++) {
        average_weight += weight[i];
        fabs_average_weight += fabs(weight[i]);
    }
    average_weight /= (double) BM_PAIRS;
    fabs_average_weight /= (double) BM_PAIRS;

    for (i = 0; i < BM_VH; i++) {
        average_weight_vh += weight[i];
        fabs_average_weight_vh += fabs(weight[i]);
    }
    average_weight_vh /= (double) (BM_VH);
    fabs_average_weight_vh /= (double) (BM_VH);

    for (i = BM_VH; i < BM_PAIRS; i++) {
        average_weight_hh += weight[i];
        fabs_average_weight_hh += fabs(weight[i]);
    }
    average_weight_hh /= (double) (BM_HH);
    fabs_average_weight_hh /= (double) (BM_HH);


    average_weight_hv = average_weight_vh;
    fabs_average_weight_hv = fabs_average_weight_vh;
    //   ( + BM_K*BM_K + BM_K*BM_N)


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