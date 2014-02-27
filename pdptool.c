#include "pdptool.h"
// Clamped flags
static int clamped[PDPTOOL_UNITS];
int clamped_pattern;
int clamped_stage;
static double weight_vh[PDPTOOL_N][PDPTOOL_K];
static double weight_hh[PDPTOOL_K][PDPTOOL_K];
static double weight_hv[PDPTOOL_K][PDPTOOL_N];
static double *weight[PDPTOOL_PAIRS];

int pdptool_i0_of(int t)
{

#if PRINT_FUNCTION
    printf("%s (s=%d)\n", __FUNCTION__, t);
#endif

    switch (t)
    {
    case NAME:
        return 0;
        break;
    case VISUAL:
        return PDPTOOL_N_NAME;
        break;
//     case PERCEPTUAL:
//         return PDPTOOL_N_NAME + PDPTOOL_N_VISUAL;
//         break;
//     case FUNCTIONAL:
//         return PDPTOOL_N_NAME + PDPTOOL_N_VISUAL + PDPTOOL_N_PERCEPTUAL;
//         break;
//     case ENCYCLOPEDIC:
//         return PDPTOOL_N_NAME + PDPTOOL_N_VISUAL + PDPTOOL_N_PERCEPTUAL + PDPTOOL_N_FUNCTIONAL;
//         break;
    case VERBAL:
        return PDPTOOL_N_NAME + PDPTOOL_N_VISUAL;
        break;
    case VISIBLE:
        return 0;
        break;
    case HIDDEN:
        return PDPTOOL_N;
        break;
    default:
        throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, t);
        return -1;
    }
}





int pdptool_N_of(int t)
{
#if PRINT_FUNCTION
    printf("%s (s=%d)\n", __FUNCTION__, t);
#endif

    switch (t)
    {
    case NAME:
        return PDPTOOL_N_NAME;
    case VISUAL:
        return PDPTOOL_N_VISUAL;
//     case PERCEPTUAL:
//         return PDPTOOL_N_PERCEPTUAL;
//     case FUNCTIONAL:
//         return PDPTOOL_N_FUNCTIONAL;
//     case ENCYCLOPEDIC:
//         return PDPTOOL_N_ENCYCLOPEDIC;
    case VERBAL:
        return PDPTOOL_N_VERBAL;
    case VISIBLE:
        return PDPTOOL_N;
    case HIDDEN:
        return PDPTOOL_K;
    default:
        throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, t);
        return -1;
    }
}

static int pdptool_stage_index(int s, int upper)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    switch (s) {
    case PDPTOOL_STAGE_ALL: {
        return(upper ? PDPTOOL_N : 0);
    }
    case PDPTOOL_STAGE_NAME: {
        return(upper ? PDPTOOL_N_NAME : 0);
    }
    case PDPTOOL_STAGE_VISUAL: {
        return(upper ? PDPTOOL_N_NAME+PDPTOOL_N_VISUAL : PDPTOOL_N_NAME);
    }
    case PDPTOOL_STAGE_VERBAL: {
        return(upper ? PDPTOOL_N : PDPTOOL_N_NAME+PDPTOOL_N_VISUAL);
    }
    default: {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid stage index", 1, s);
        return(0); // If you have OCD you will want a warning message here.
    }
    }
}

double pdptool_random_state(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return random_double(0.0, 1.0);
}



double pdptool_random_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return random_double(-0.2, 0.2);
}


void pdptool_weights_init(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int i, j;

    // Initialise the weights:
    for (i = 0; i < PDPTOOL_N; i++) {
        for (j = 0; j < PDPTOOL_K; j++) {
            weight_vh[i][j] = pdptool_random_weight();
            weight_hv[j][i] = pdptool_random_weight();
            weight[PDPTOOL_K*i+j] = &(weight_vh[i][j]);
            weight[PDPTOOL_N*PDPTOOL_K+PDPTOOL_K*PDPTOOL_K + PDPTOOL_N*j+i] = &(weight_hv[j][i]);
        }
    }
    for (i = 0; i < PDPTOOL_K; i++) {
        for (j = 0; j < PDPTOOL_K; j++) {
            weight_hh[i][j] = pdptool_random_weight();
            weight[PDPTOOL_N*PDPTOOL_K + PDPTOOL_K*i+j] = &(weight_hh[i][j]);
        }
    }
}




int pdptool_load_weights(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    FILE *fp;
    int i, j;
//     char *line = NULL;
//     size_t len = 0;

    fp = fopen(filename, "r");


//     i = strlen(filename);
//     printf("string length: %i %s\n", i, filename);

    // If file is not saved as *.mat then this will need to be run to remove the header
//     if ((filename[i-1] == 't') && (filename[i-2] == 'w')) {
//       for (i = 0; i < 2150; i++) {
// 	getline(&line, &len, fp);
// 	printf("%c \n", line[0]);
//       }
//     }


    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;

    }

    if (i != 2) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 1) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    for (i = 0; i < PDPTOOL_N_NAME; i++) {
        if (!fscanf(fp, "%d", &j)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
            return 0;
        }
        if (j != -2) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
            return 0;
        }
    }

    if (!fscanf(fp, "%d", &i))
    {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 2) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 2) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    /// ********************* ///
    ///  FROM HIDDEN TO NAME  ///
    for (i = 0; i < PDPTOOL_N_NAME; i++) {
        for (j = 0; j < PDPTOOL_K; j++) {
            if (!fscanf(fp, "%lf", &weight_hv[j][i])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0);
                return 0;
            }
//             printf("%f ", weight_hv[j][i]);
        }
// printf("\n ");
    }


    /// ********************* ///
    /// ********************* ///


    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 3) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 1) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
    }

    for (i = 0; i < PDPTOOL_N_VERBAL; i++) {
        if (!fscanf(fp, "%d", &j)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
            return 0;
        }
        if (j != -2) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
            return 0;
        }
    }

    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 3) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 2) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
    }

    /// ********************* ///
    /// FROM HIDDEN TO VERBAL ///
    for (i = PDPTOOL_N_NAME+PDPTOOL_N_VISUAL; i < PDPTOOL_N; i++) {
        for (j = 0; j < PDPTOOL_K; j++) {
            if (!fscanf(fp, "%lf", &weight_hv[j][i])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0);
                return 0;
            }
// 	    printf("%f ", weight_hv[j][i]);
        }
//


    }


//     printf("weight: hidden->verbal = %f\n", weight_hv[0][PDPTOOL_N_NAME+PDPTOOL_N_VISUAL]);

    /// ********************* ///
    /// ********************* ///


    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 4) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 1) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    for (i = 0; i < PDPTOOL_N_VISUAL; i++) {
        if (!fscanf(fp, "%d", &j)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
            return 0;
        }
        if (j != -2) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
            return 0;
        }
    }

    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 4) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 2) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    /// ********************* ///
    /// FROM HIDDEN TO VISUAL ///
    for (i = PDPTOOL_N_NAME; i < PDPTOOL_N_NAME+PDPTOOL_N_VISUAL; i++) {
        for (j = 0; j < PDPTOOL_K; j++) {
            if (!fscanf(fp, "%lf", &weight_hv[j][i])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0);
                return 0;
            }
// 	    printf("%f ", weight_hv[j][i]);
        }
//

    }


//     printf("weight: hidden->visual = %f\n", weight_hv[0][PDPTOOL_N_NAME]);

    /// ********************* ///
    /// ********************* ///


    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 5) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 1) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    for (i = 0; i < PDPTOOL_K; i++) {
        if (!fscanf(fp, "%d", &j)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
            return 0;
        }
        if (j != -2) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
            return 0;
        }
    }

    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 5) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 2) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    /// ********************* ///
    ///  FROM NAME TO HIDDEN  ///
    for (j = 0; j < PDPTOOL_K; j++) {
        for (i = 0; i < PDPTOOL_N_NAME; i++) {
            if (!fscanf(fp, "%lf", &weight_vh[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0);
                return 0;
            }
// 	    if (i < 3) printf("weight_vh[%i][%i] = %f\n", i, j, weight_vh[i][j]);

// 	    printf("%f ", weight_vh[i][j]);
        }
//

    }


//     printf("weight: name->hidden = %f\n", weight_vh[0][0]);

    /// ********************* ///
    /// ********************* ///


    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 5) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;

    }
    if (i != 3) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    /// ********************* ///
    /// FROM VERBAL TO HIDDEN ///
    for (j = 0; j < PDPTOOL_K; j++) {
        for (i = PDPTOOL_N_NAME+PDPTOOL_N_VISUAL; i < PDPTOOL_N; i++) {
            if (!fscanf(fp, "%lf", &weight_vh[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0);
                return 0;
            }
// 	    printf("%f ", weight_vh[i][j]);
        }
//

    }

//     printf("weight: verbal->hidden = %f\n", weight_vh[PDPTOOL_N_NAME+PDPTOOL_N_VISUAL][0]);

    /// ********************* ///
    /// ********************* ///


    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 5) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 4) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    /// ********************* ///
    /// FROM VISUAL TO HIDDEN ///
    for (j = 0; j < PDPTOOL_K; j++) {
        for (i = PDPTOOL_N_NAME; i < PDPTOOL_N_NAME+PDPTOOL_N_VISUAL; i++) {
//       printf("%i, %i\n", i, j);
            if (!fscanf(fp, "%lf", &weight_vh[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0);
                return 0;
            }

// 	    printf("%f ", weight_vh[i][j]);
        }
//

    }
//     printf("weight: visual->hidden = %f\n", weight_vh[PDPTOOL_N_NAME][0]);

    /// ********************* ///
    /// ********************* ///


    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 5) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (!fscanf(fp, "%d", &i)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }
    if (i != 5) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__, "If file is saved as \"binary\", then the file must be opened from within Matlab using pdptool, and then saved as non-binary.", 0);
        return 0;
    }

    /// ********************* ///
    /// FROM HIDDEN TO HIDDEN ///
    for (j = 0; j < PDPTOOL_K; j++) {
        for (i = 0; i < PDPTOOL_K; i++) {
            if (!fscanf(fp, "%lf", &weight_hh[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0);
                return 0;
            }
        }


    }

//     printf("weight: hidden->hidden = %f\n", weight_hh[0][0]);

    /// ********************* ///
    /// ********************* ///



    fclose(fp);

//     printf("hidden-hidden %f name hidden %f hidden name %f\n", weight_hh[0][0], weight_vh[0][0], weight_hv[0][0]);
//     printf("hidden-hidden %f visual hidden %f hidden visual %f\n", weight_hh[0][0], weight_vh[PDPTOOL_N_NAME][0], weight_hv[0][PDPTOOL_N_NAME]);
    return 1;
}


// States
static double state[PDPTOOL_UNITS];


double pdptool_get_state(int i)
{
    return state[i];
}

// double pdptool_error_measure(int error_type, int p)
// {
//     if (error_type == 0) {
//         return pdptool_jaccard_distance(p);
//     }
//     else if (error_type == 1) {
//         return bppt_sorensen_distance(p);
//     }
//     else if (error_type == 2) {
//         return bppt_calculate_rms_error(p);
//     }
// }

#define PDPTOOL_ACTIVATION_THRESHOLD 0.5
int pdptool_get_state_int(int i)
{
    return (pdptool_get_state(i) >= PDPTOOL_ACTIVATION_THRESHOLD);
}

double pdptool_jaccard_distance(int p)
{
    int i;
    double u = 0.0, n = 0.0;
    for (i = 0; i < PDPTOOL_N; i++) {
        u += (double) pdptool_get_state_int(i) || get_pattern_int(p, i);
        n += (double) pdptool_get_state_int(i) && get_pattern_int(p, i);

    }
    if (u) {
        return ((u - fabs(n)) / u) / (double) PDPTOOL_N;
    }
    return 1.0;

}

double pdptool_jaccard_distance_type(int p, int error_t)
{
    int i;
    double u = 0.0, n = 0.0;
    int n_0 = i0_of(error_t), n_1 = n_0 + N_of(error_t);
    for (i = n_0; i < n_1; i++) {
        u += (double) pdptool_get_state_int(i) || get_pattern_int(p, i);
        n += (double) pdptool_get_state_int(i) && get_pattern_int(p, i);

    }
    if (u) {
        return ((u - fabs(n)) / u) / (double) (n_1 - n_0);
    }
    return 1.0;
}

double pdptool_jaccard_distance_type_alt(int p, int error_t)
{
    int i;
    double u = 0.0, n = 0.0;
    int n_0 = i0_of(error_t), n_1 = n_0 + N_of(error_t);
    for (i = n_0; i < n_1; i++) {
//     u += fmax(pdptool_get_state(i), get_pattern(p, i));
//     n += fmin(pdptool_get_state(i), get_pattern(p, i));

        u += pdptool_get_state(i) + get_pattern(p, i) - pdptool_get_state(i) * get_pattern(p, i) ;
        n += pdptool_get_state(i) * get_pattern(p, i) ;

    }
    if (u) {
        return ((u - fabs(n)) / u) / (double) (n_1 - n_0);
    }
    return 1.0;
}

// double pdptool_sorensen_distance(int p)
// {
//     int i;
//     double u = 0.0, n = 0.0;
//     for (i = 0; i < PDPTOOL_N; i++) {
//         u += pdptool_get_state_int(i) && get_pattern_int(p, i);
//         n -= pdptool_get_state_int(i) || get_pattern_int(p, i);
//     }
//     return ((u - 2.0*n) / u) / (double) PDPTOOL_N;
// }

double pdptool_calculate_rms_error(int p)
{
#if PRINT_FUNCTION
    printf("%s(%i)\n", __FUNCTION__, p);
#endif
    int i;
    double error[PDPTOOL_N], total_error = 0.0;

    if (p < 0) {
        return(1.0);
    }
    else {
        for (i = 0; i < PDPTOOL_N; i++) {
            error[i] = sqr(get_pattern(p, i) - state[i]);
            total_error += error[i];
        }
    }
    total_error /= (double) PDPTOOL_N;
    return sqrt(total_error);
}



double old_pdptool_settle_step(int t)
{

#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    double tmp_state[PDPTOOL_UNITS];
    double input, tmp, d = 0.0;
    int i, j;

    if (t) {

        for (i = (PDPTOOL_UNITS - 1); i >= 0; i--) {

            input = PDPTOOL_BIAS;
            if (i < PDPTOOL_N) { // VISIBLE
                if (clamped[i]) {
                    state[i] = get_pattern(clamped_pattern, i);

                }
                for (j = 0; j < PDPTOOL_K; j++) {
                    input += state[j+PDPTOOL_N] * weight_hv[j][i];
                }
            }
            else { // HIDDEN
                for (j = 0; j < PDPTOOL_N; j++) {
                    input += state[j] * weight_vh[j][i-PDPTOOL_N];
                }
                for (j = 0; j < PDPTOOL_K; j++) {
                    input += state[j+PDPTOOL_N] * weight_hh[j][i-PDPTOOL_N];
                }
            }

            //             net[t][i] = dt * input + (1.0 - dt) * net[t-1][i];
            tmp_state[i] = f(input);

        }

        for (i = (PDPTOOL_UNITS - 1); i >= 0; i--) {
            tmp = state[i];
            if (!clamped[i]) {
                state[i] = tmp_state[i];
            }
            d = fmax(d, fabs(tmp - state[i]));


        }

    }
    else {
        d = 1.0;
        for (i = (PDPTOOL_UNITS - 1); i >= 0; i--) {
            //  tmp = state[t][i];
            if (i < PDPTOOL_N) {
                if (clamped[i]) {
                    state[i] = get_pattern(clamped_pattern, i);
                }
                else
                {
                    state[i] = 0.0;
                }
            } else {
                state[i] = pdptool_random_state();
            }
            //net[t][i] = logit(state[t][i]);
        }
    }

    return d;
}

static double netinput[PDPTOOL_UNITS];
#define dt		(1.0 / (double) PDPTOOL_STEP)


/*
 * void pdptool_netinput_init(void)
 * {
 *  #if PRINT_FUNCTION
 *  printf("%s\n", __FUNCTION__);
 *  #endif
 *  int i;
 *
 *  // Initialise the weights:
 *  for (i = 0; i < PDPTOOL_UNITS; i++) {
 *    netinput[i] = PDPTOOL_BIAS;
 *
 }

 }  */

double pdptool_settle_step(int t)
{

#if PRINT_FUNCTION
    printf("%s(t = %d)\n", __FUNCTION__, t);
#endif

    int i, j;
    double prev_state[PDPTOOL_UNITS];
    double prev_netinput[PDPTOOL_UNITS];
    double input;  // aka newnet
    double d = 0.0;
//     double hidden_input = 0.0, visual_input = 0.0, verbal_input = 0.0, name_input = 0.0;

    if (t) {  // if not first timestep, i.e., prev_state and prev_netinput values are of relevance to this timestep, otherwise go to else of this if-statement

        for (i = (PDPTOOL_UNITS - 1); i >= 0; i--) {
            prev_state[i] = state[i]; //save t-1 state
            prev_netinput[i] = netinput[i]; //save t-1 netinput
        }

        // check all the prev states, activation history, at t==1 in matlab version.

        for (i = 0; i < PDPTOOL_UNITS; i++) {


// 	  hidden_input = 0.0;
// 	  visual_input = 0.0;
// 	  verbal_input = 0.0;
// 	  name_input = 0.0;


            if (t == 1) {
                input = PDPTOOL_BIAS; // initialise input for unit to be the value of the bias

            }
            else {
                input = 0.5 * PDPTOOL_BIAS; // initialise input for unit to be the value of the bias

            }


            if (i < PDPTOOL_N) { // VISIBLE



                if (clamped[i]) {  // if clamped, then ignore all inputs and apply pattern over unit's state and over unit's netinput (= time-averaged input).

                    state[i] = get_pattern(clamped_pattern, i);
                    netinput[i] = inverse_logistic(state[i]);


// 		    if ((i == 0) && (t == 1)) {
// 		      printf("at %d first name unit is clamped to %f\n which gives a netinput of %f\n", t, state[i], netinput[i]);
// 		    }
// 		    else if ((i == PDPTOOL_N_NAME + PDPTOOL_N_VISUAL) && (t == 1)) {
// 		      printf("at %d first verbal unit is clamped to %f\n which gives a netinput of %f\n", t, state[i], netinput[i]);
// 		    }
// 		    else if ((i == PDPTOOL_N_NAME) && (t == 1)) {
// 		      printf("at %d first visual unit is clamped to %f\n which gives a netinput of %f\n", t, state[i], netinput[i]);
// 		    }

                }
                else { // the unit is free and therefore has input from hidden units

// 		    if ((i == 0) && (t == 1)) {
// 		      printf("at %d input from all bias units to first unit of name is: %f\n", t, input);
// 		      printf("which is turned into: %f\n", dt * (input - prev_netinput[i]) + prev_netinput[i]);
// 		    }
// 		    else if ((i == PDPTOOL_N_NAME + PDPTOOL_N_VISUAL) && (t == 1)) {
// 		      printf("at %d input from all bias units to first unit of verbal is: %f\n", t, input);
// 		      printf("which is turned into: %f\n", dt * (input - prev_netinput[i]) + prev_netinput[i]);
// 		    }
// 		    else if ((i == PDPTOOL_N_NAME) && (t == 1)) {
// 		      printf("at %d input from all bias units to first unit of visual is: %f\n", t, input);
// 		      printf("which is turned into: %f\n", dt * (input - prev_netinput[i]) + prev_netinput[i]);
// 		    }

                    for (j = 0; j < PDPTOOL_K; j++) {
                        input += prev_state[j+PDPTOOL_N] * weight_hv[j][i]; // collecting inputs from hidden units to this visible unit
// 		      hidden_input += prev_state[j+PDPTOOL_N] * weight_hv[j][i];
                    }



                    // Time-average the states of units.
                    netinput[i] = dt * (input - prev_netinput[i]) + prev_netinput[i]; // dt is 0.25
                    state[i] = logistic(netinput[i]); // f is the inverse squashing function

// 		    if ((i == 0) && (t == 1)) {
// 		      printf("at %d input from all hidden units to first unit of name is: %f\n", t, hidden_input);
// 		      printf("which is turned into: %f\n", dt * (hidden_input - prev_netinput[i]) + prev_netinput[i]);
// 		      printf("name state: %f\n", state[i]);
// 		    }
// 		    else if ((i == PDPTOOL_N_NAME + PDPTOOL_N_VISUAL) && (t == 1)) {
// 		      printf("at %d input from all hidden units to first unit of verbal is: %f\n", t, hidden_input);
// 		      printf("which is turned into: %f\n", dt * (hidden_input - prev_netinput[i]) + prev_netinput[i]);
// 		      printf("verbal state: %f\n", state[i]);
// 		    }
// 		    else if ((i == PDPTOOL_N_NAME) && (t == 1)) {
// 		      printf("at %d input from all hidden units to first unit of visual is: %f\n", t, hidden_input);
// 		      printf("which is turned into: %f\n", dt * (hidden_input - prev_netinput[i]) + prev_netinput[i]);
// 		      printf("visual state: %f\n", state[i]);
// 		    }
                }







            }
            else { // HIDDEN

//                 if (i == PDPTOOL_N) {
// //                     printf("state = %f t = %d start netinput = %f\n", state[PDPTOOL_N], t+1, netinput[PDPTOOL_N]);
//                 }


// 		for (j = 0; j < PDPTOOL_N; j++) {
// 		  input += prev_state[j] * weight_vh[j][i-PDPTOOL_N];
// 		}

//                 if (i == PDPTOOL_N) {
//                     printf("state = %f t = %d loop:1 ninput = %f\n", state[PDPTOOL_N], t+1, input);
//                 }
// 		if ((i == PDPTOOL_N) && (t == 1)) {
// 		  printf("at %d input from all bias units to first unit of hidden is: %f\n", t, input);
// 		  printf("which is turned into: %f\n", dt * (input - prev_netinput[i]) + prev_netinput[i]);
// 		}




                for (j = 0; j < PDPTOOL_N_NAME; j++) {  //name units' input the hidden unit i
                    input += prev_state[j] * weight_vh[j][i-PDPTOOL_N];
//                     name_input +=  prev_state[j] * weight_vh[j][i-PDPTOOL_N];
                }
//                 if ((i == PDPTOOL_N && (t == 1))) {
// 		  printf("at %d input from all name units to first unit of hidden is: %f\n", t, name_input);
// 			printf("which is turned into: %f\n", dt * (name_input - prev_netinput[i]) + prev_netinput[i]);
// 		}



//                 if (i == PDPTOOL_N) {
//                     printf("state = %f t = %d loop:2 ninput = %f\n", state[PDPTOOL_N], t+1, input);
//                 }

                for (j = PDPTOOL_N_NAME + PDPTOOL_N_VISUAL; j < PDPTOOL_N; j++) {  // verbal units' inout to i
// 		                    if ((i == PDPTOOL_N && (t == 1))) {
// 				      printf("%1.1f ", prev_state[j]);
// 				    }
                    input += prev_state[j] * weight_vh[j][i-PDPTOOL_N];
//                     verbal_input += prev_state[j] * weight_vh[j][i-PDPTOOL_N];
                }
                /*                if ((i == PDPTOOL_N && (t == 1))) {
                		  printf("\nat %d input from all verbal units to first unit of hidden is: %f\n", t, verbal_input);
                		  printf("which is turned into: %f\n", dt * (verbal_input - prev_netinput[i]) + prev_netinput[i]);

                		}*/

//                 if (i == PDPTOOL_N) {
//                     printf("state = %f t = %d loop:3 ninput = %f\n", state[PDPTOOL_N], t+1, input);
//                 }

                for (j = PDPTOOL_N_NAME; j < PDPTOOL_N_NAME + PDPTOOL_N_VISUAL; j++) { // visual units' input to i
                    input += prev_state[j] * weight_vh[j][i-PDPTOOL_N];
//                     visual_input += prev_state[j] * weight_vh[j][i-PDPTOOL_N];
                }

//                 if ((i == PDPTOOL_N && (t == 1))) {
// 		  printf("at %d input from all visual units to first unit of hidden is: %f\n", t, visual_input);
// 		  printf("which is turned into: %f\n", dt * (visual_input - prev_netinput[i]) + prev_netinput[i]);
//
// 		}

                for (j = 0; j < PDPTOOL_K; j++) { // hidden units' inout to i
                    input += prev_state[j+PDPTOOL_N] * weight_hh[j][i-PDPTOOL_N];
// 				    hidden_input += prev_state[j+PDPTOOL_N] * weight_hh[j][i-PDPTOOL_N];

                }
// 		if ((i == PDPTOOL_N) && (t == 1)) {
// 		  printf("at %d input from all hidden units to first unit of hidden is: %f\n", t, hidden_input);
// 		  printf("which is turned into: %f\n", dt * (hidden_input - prev_netinput[i]) + prev_netinput[i]);
//
// 		}


//                 if (i == PDPTOOL_N) {

//                     printf("state = %f t = %d loop:5 ninput = %f\n", state[PDPTOOL_N], t+1, input);
//                     printf("state = %f t = %d loop:5 name input = %f\n", state[PDPTOOL_N], t+1, name_input);
// //                     printf("state = %f t = %d loop:5 verbal input = %f\n", state[PDPTOOL_N], t+1, verbal_input);
//                     printf("state = %f t = %d loop:5 visual input = %f\n", state[PDPTOOL_N], t+1, visual_input);
//                     printf("state = %f t = %d loop:5 hidden input = %f\n", state[PDPTOOL_N], t+1, hidden_input);
//                    for (j=0; j < 64; j++) {
                //                        printf("%d weight_hh: %f; prev_state: %f\n", j+1, weight_hh[j][i-PDPTOOL_N], prev_state[j+PDPTOOL_N]);
                //                    }
//                 }


                netinput[i] = dt * (input - prev_netinput[i]) + prev_netinput[i];


                state[i] = logistic(netinput[i]);

// 		if ((i == PDPTOOL_N) && (t == 1)) {
// 		  printf("hidden state: %f\n", state[i]);
//
// 		}

//                 if (i == PDPTOOL_N) {
//                     printf("state = %f t = %d end netinput = %f\n", state[PDPTOOL_N], t+1, netinput[PDPTOOL_N]);
//                 }

            }



            // Keep track of max difference in state values caused by this update step.
            d = fmax(d, fabs(prev_state[i] - state[i]));

            //             if ((t == 2) && (i == N)) {
            //                 for (j = 0; j < PDPTOOL_K; j++) {
            //                     printf("prevstate[%d] = %f; state = %f t = %d loop ninput = %f\n", j, prev_state[j+PDPTOOL_N], state[PDPTOOL_N], t, input);
            //                 }
            //             }

        }

    } else { // at the first timestep

        d = 1.0;
        for (i = (PDPTOOL_UNITS - 1); i >= 0; i--) {
            if (i < PDPTOOL_N) {
                if (clamped[i]) {
                    //                     get_pattern(clamped_pattern, i) ? (state[i] = 0.99999988) : (state[i] = 0.00000012);
                    state[i] = get_pattern(clamped_pattern, i);
                    netinput[i] = inverse_logistic(state[i]);
                    // 		    printf("netinput[%i] = %1.1f\tstate[%i] = %1.1f\n", i, netinput[i], i, state[i]);


                }
                else  {

                    netinput[i] = PDPTOOL_BIAS;
                    state[i] =  logistic(netinput[i]);
                    //                     state[i] = 0.0;
                    // 		  printf("netinput[%i] = %1.1f\tstate[%i] = %1.1f\n", i, netinput[i], i, state[i]);


                }
            }
            else {
                netinput[i] = PDPTOOL_BIAS;
                state[i] =  logistic(netinput[i]);
                //                 state[i] = 0.0;
                // 		    netinput[i] = log(state[i] / (1.0 - state[i]));
                // 		printf("netinput[%i] = %1.1f\tstate[%i] = %1.1f\n", i, netinput[i], i, state[i]);


            }
            //             netinput[i] = inverse_logistic(state[i]);

            //             printf("netinput[%i] = %1.4f\tstate[%i] = %1.4f\n", i, netinput[i], i, state[i]);
        }
    }
    //     printf("d = %f\t%s(t = %d)\n", d, __FUNCTION__, t);
    return d;
}



int pdptool_get_clamped_pattern(void) {
    return clamped_pattern;
}
int pdptool_get_clamped_stage(void) {
    return clamped_stage;
}

void pdptool_lesion_random_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int x = 0, i = random_int(0, PDPTOOL_PAIRS-1);

    while ((!(*weight[i])))
    {
        i = random_int(0, PDPTOOL_PAIRS-1);
        x++;
        if (x > PDPTOOL_PAIRS) {
            break;
        }
    }

    *weight[i] = 0.0;
}

void pdptool_print_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;


    //     /// ********************* ///
    //     /// FROM HIDDEN TO HIDDEN ///
    //
    //     for (i = 0; i < PDPTOOL_K; i++) {
    //       for (j = 0; j < PDPTOOL_K; j++) {
    // 	printf("%f\n", weight_hh[i][j]);
    //
    //       }
    //     }
    //     /// ********************* ///
    //     /// ********************* ///

    //     /// ********************* ///
    //     /// FROM VERBAL TO HIDDEN ///
    //     for (j = 0; j < PDPTOOL_K; j++) {
    //       for (i = PDPTOOL_N_NAME+PDPTOOL_N_VISUAL; i < PDPTOOL_N; i++) {
    // 	(fscanf(fp, "%lf", &weight_vh[i][j])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0));
    //       }
    //     }
    //     /// ********************* ///
    //     /// ********************* ///
    //
    //
    //     /// ********************* ///
    //     ///  FROM NAME TO HIDDEN  ///
    //     for (j = 0; j < PDPTOOL_K; j++) {
    //       for (i = 0; i < PDPTOOL_N_NAME; i++) {
    // 	(fscanf(fp, "%lf", &weight_vh[i][j])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0));
    //       }
    //     }
    //     /// ********************* ///
    //     /// ********************* ///
    //
    //
    //     /// ********************* ///
    //     /// FROM HIDDEN TO VISUAL ///
    //     for (i = PDPTOOL_N_NAME; i < PDPTOOL_N_NAME+PDPTOOL_N_VISUAL; i++) {
    //       for (j = 0; j < PDPTOOL_K; j++) {
    // 	(fscanf(fp, "%lf", &weight_hv[j][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0));
    //       }
    //     }
    //     /// ********************* ///
    //     /// ********************* ///
    //
    //     /// ********************* ///
    //     /// FROM HIDDEN TO VERBAL ///
    //     for (i = PDPTOOL_N_NAME+PDPTOOL_N_VISUAL; i < PDPTOOL_N; i++) {
    //       for (j = 0; j < PDPTOOL_K; j++) {
    // 	(fscanf(fp, "%lf", &weight_hv[j][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__, "Could not read variable: weight_vh[][]", 0));
    //       }
    //     }
    //     /// ********************* ///
    //     /// ********************* ///
    //
    //
    /// ********************* ///
    ///  FROM HIDDEN TO NAME  ///
    for (i = 0; i < PDPTOOL_N_NAME; i++) {
        for (j = 0; j < PDPTOOL_K; j++) {
            printf("%f\n", weight_hv[j][i]);
        }
    }
    /// ********************* ///
    /// ********************* ///





}
//
void pdptool_print_states(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = PDPTOOL_N; i < PDPTOOL_UNITS; i++) {
        //       if (state[i] == 0.0) {
        printf("hidden[%i] = %f\n", i - PDPTOOL_N, state[i]);
        //       }
    }


}

double pdptool_state_get(int i) {
    return state[i];
}
//
// void pdptool_print_patterns(void)
// {
// #if PRINT_FUNCTION
//     printf("%s\n", __FUNCTION__);
// #endif
//
//     int p, i;
//     for (p = 0; p < PDPTOOL_P; p++) {
//         for (i = 0; i < PDPTOOL_N; i++) {
// //       if (state[i] == 0.0) {
//             printf("state[%i] = %f\n", i, pdptool_get_pattern(p, i));
// //       }
//         }
//     }
//
//
// }

void pdptool_lesion_weights(double percentage)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, n = (int)round(PDPTOOL_PAIRS * percentage/100.0);
    for (i = 0; i < n; i++) {
        pdptool_lesion_random_weight();
    }
}

void pdptool_clamp_now(int p, int s)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, i0, i1;

    i0 = pdptool_stage_index(s, 0);
    i1 = pdptool_stage_index(s, 1);
//     printf("%i %i\n", i0, i1);
//     printf("name: %i verbal: %i visual: %i\n", PDPTOOL_N_NAME, PDPTOOL_N_VERBAL, PDPTOOL_N_VISUAL);
//     printf("BM/BPTT name: %i verbal: %i visual: %i\n", BM_N_NAME, BM_N_VERBAL, BM_N_VISUAL);
//     printf("variable name: %i verbal: %i visual: %i\n", N_NAME, N_VERBAL, N_VISUAL);
//
    pdptool_unclamp();
    clamped_pattern = p;
    clamped_stage = s;
    for (i = i0; i < i1; i++) { // Input index
        state[i] = get_pattern(p, i);
        clamped[i] = 1;
    }
}

void pdptool_clamp_name(int p)
{
    pdptool_clamp(p, PDPTOOL_STAGE_NAME);
}
void pdptool_clamp_visual(int p)
{
    pdptool_clamp(p, PDPTOOL_STAGE_VISUAL);
}
void pdptool_clamp_verbal(int p)
{
    pdptool_clamp(p, PDPTOOL_STAGE_VERBAL);
}

double backup_weight[PDPTOOL_PAIRS];

void pdptool_backup_weights(void)
{
    int i;
    for (i = 0; i < PDPTOOL_PAIRS; i++) {
        backup_weight[i] = *weight[i];
    }
}

void pdptool_disrupt_weights(double noise)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    //   for (i = 0; i < BPTT_PAIRS; i++) {
    //     weight_has_noise[i] = 0;
    //   }

    for (i = 0; i < PDPTOOL_PAIRS; i++) {
        *weight[i] = backup_weight[i] + random_double(-noise, noise);
    }


}

void pdptool_undo_lesion(void)
{
    int i;
    for (i = 0; i < PDPTOOL_PAIRS; i++) {
        *weight[i] = backup_weight[i];
    }
}



int pdptool_is_clamped(int i)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif



    if (i < PDPTOOL_N) {
        return clamped[i];
    }
    else {
        return 0;
    }
}

// initialise network
void pdptool_init(void)
{
    UNITS = PDPTOOL_UNITS;
    PAIRS = PDPTOOL_PAIRS;
    p_min = 0;
    p_max = 6;
    P = p_max - p_min; //in case of bugs do a search for p to P as obviously might not start from zero...
    N = PDPTOOL_N;
    K = PDPTOOL_K;


//     init_patterns(PDPTOOL);

    random_init();

    pdptool_weights_init();

    pdptool_unclamp();
    pdptool_randomise_states();
    /*
     *      activation_threshold = ACTIVATION_THRESHOLD;       */






    //   pdptool_patterns_open("base_names.pat");

    //     pdptool_open_patterns("48_features.pat");



}




void pdptool_unclamp(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = 0; i < PDPTOOL_N; i++) { // Input index
        clamped[i] = 0;
    }
    clamped_pattern = -1;
    clamped_stage = -1;
}

int pdptool_stage_to_type(int stage)
{
    switch(stage)
    {
    case PDPTOOL_STAGE_NAME:
        return NAME;
        break;
    case PDPTOOL_STAGE_VISUAL:
        return VISUAL;
        break;
    case PDPTOOL_STAGE_VERBAL:
        return VERBAL;
        break;
    }
    return -1;
}


void pdptool_clamp(int p, int s)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, i0, i1;

    i0 = pdptool_stage_index(s, 0);
    i1 = pdptool_stage_index(s, 1);
    pdptool_unclamp();
    clamped_pattern = p;
    clamped_stage = s;
    for (i = i0; i < i1; i++) { // Input index
        clamped[i] = 1;
    }
}


void pdptool_randomise_states(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = 0; i < PDPTOOL_UNITS; i++) {
        state[i] = pdptool_random_state();
    }
}






int pdptool_clamped_type_get(void) {
    return pdptool_stage_to_type(clamped_stage);
}

/*
void pdptool_print_weight_stats(void)
{

   double average_weight = 0.0, average_weight_hh = 0.0, average_weight_hv = 0.0, average_weight_vh = 0.0;
   int i;
   for (i = 0; i < PDPTOOL_PAIRS; i++) {
       average_weight += *weight[i];
   }
   average_weight /= (double) PDPTOOL_PAIRS;

   for (i = 0; i < PDPTOOL_N*PDPTOOL_K; i++) {
       average_weight_vh += *weight[i];
   }
   average_weight_vh /= (double) (PDPTOOL_N*PDPTOOL_K);

   for (i = PDPTOOL_N*PDPTOOL_K; i < PDPTOOL_N*PDPTOOL_K + PDPTOOL_K*PDPTOOL_K; i++) {
       average_weight_hh += *weight[i];
   }
   average_weight_hh /= (double) (PDPTOOL_K*PDPTOOL_K);

   for (i = PDPTOOL_N*PDPTOOL_K + PDPTOOL_K*PDPTOOL_K; i < PDPTOOL_PAIRS; i++) {
       average_weight_hv += *weight[i];
   }
   average_weight_hv /= (double) (PDPTOOL_K*PDPTOOL_N);
   //   ( + PDPTOOL_K*PDPTOOL_K + PDPTOOL_K*PDPTOOL_N)


   printf("Total Average = %f\nInput Average = %f\nHidden Average = %f\nOutput Average = %f\n", average_weight, average_weight_vh, average_weight_hh, average_weight_hv);
}                  */