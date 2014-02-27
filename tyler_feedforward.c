
#include "tyler.h"
#define STRING 28
#define dt		(1.0 / (double) TYLER_STEP)


static int clamped_pattern = -1;
static int clamped_stage = -1;
static  int clamped[TYLER_FEEDFORWARD_IN];

static void tyler_feedforward_clamp_now_(int p, int s, int noise, int n);
static double tyler_feedforward_propagate_(int noise, int n);

// static double tyler_feedforward_settle_step_(int t, int noise, int n);

void tyler_feedforward_inc_iterations_so_far(void)
{
    tyler_iterations_so_far++;
}

double tyler_feedforward_random_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

//     return random_double(-1.0, 1.0);
    return random_double(-0.001, 0.001);
}




static double in_state[TYLER_FEEDFORWARD_IN];
static double hidden_state[TYLER_FEEDFORWARD_HIDDEN];
static double out_state[TYLER_FEEDFORWARD_OUT];
static double weight_in_hidden[TYLER_FEEDFORWARD_IN][TYLER_FEEDFORWARD_HIDDEN];
static double weight_hidden_out[TYLER_FEEDFORWARD_HIDDEN][TYLER_FEEDFORWARD_OUT];
static double delta_weight_in_hidden[TYLER_FEEDFORWARD_IN][TYLER_FEEDFORWARD_HIDDEN];
static double delta_weight_hidden_out[TYLER_FEEDFORWARD_HIDDEN][TYLER_FEEDFORWARD_OUT];
static double delta_bias_hidden [TYLER_FEEDFORWARD_HIDDEN];
static double delta_bias_out [TYLER_FEEDFORWARD_OUT];
static double *feedforward_weight[TYLER_FEEDFORWARD_PAIRS];
static double delta_bias_hidden[TYLER_FEEDFORWARD_HIDDEN], delta_bias_out[TYLER_FEEDFORWARD_OUT];
// static double in_delta[TYLER_FEEDFORWARD_OUT];
static double bias_hidden[TYLER_FEEDFORWARD_HIDDEN];
static double bias_out[TYLER_FEEDFORWARD_OUT];
double tyler_feedforward_get_state(int i)
{

    if (i < TYLER_FEEDFORWARD_IN) {
        return in_state[i];
    }
    else if (i < TYLER_FEEDFORWARD_IN + TYLER_FEEDFORWARD_OUT) {
        return out_state[i - TYLER_FEEDFORWARD_IN];
    }
    else if (i < TYLER_FEEDFORWARD_IN + TYLER_FEEDFORWARD_OUT + TYLER_FEEDFORWARD_HIDDEN) {
        return hidden_state[i - TYLER_FEEDFORWARD_IN - TYLER_FEEDFORWARD_OUT];

    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "State index is out of bounds", 1, i);
    return -1;
}
void tyler_feedforward_unclamp_all(void);
void tyler_feedforward_unclamp(void);
void tyler_feedforward_clamp(int p, int s);
void tyler_feedforward_set_target(int p, int s);
void tyler_feedforward_clamp_now(int p, int s);

void tyler_feedforward_unclamp_all(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif



    tyler_feedforward_unclamp();

}

static int tyler_feedforward_stage_index(int s, int upper)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    switch (s) {
    case TYLER_STAGE_ALL: {
        return(upper ? TYLER_FEEDFORWARD_IN : 0);
    }
    case TYLER_STAGE_DISTINCTIVE: {
        return(upper ? TYLER_N_DISTINCTIVE : 0);
    }
    case TYLER_STAGE_SHARED: {
        return(upper ? TYLER_N_DISTINCTIVE+TYLER_N_SHARED : TYLER_N_SHARED);
    }
    case TYLER_STAGE_FUNCTIONAL: {
        return(upper ? TYLER_FEEDFORWARD_IN : TYLER_N_DISTINCTIVE+TYLER_N_SHARED);
    }
    default: {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid stage index", 1, s);
        return(0); // If you have OCD you will want a warning message here.
    }
    }
}
void tyler_feedforward_unclamp(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = 0; i < TYLER_N; i++) { // Input index
        clamped[i] = 0;
    }
    clamped_pattern = -1;
    clamped_stage = -1;
}


void tyler_feedforward_clamp(int p, int s)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, i0, i1;

    i0 = tyler_feedforward_stage_index(s, 0);
    i1 = tyler_feedforward_stage_index(s, 1);
    tyler_feedforward_unclamp();
    clamped_pattern = p;
    clamped_stage = s;
    for (i = i0; i < i1; i++) { // Input index
//           state[t][i] = get_pattern_noise(p, i);
        clamped[i] = 1;
    }
}

void tyler_feedforward_set_target(int p, int s)
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






// void tyler_feedforward_set_target_noise(int p, int s, int noise, int n)
// {
//   tyler_feedforward_set_target(p, s);
//
// }
//

void tyler_feedforward_clamp_now_(int p, int s, int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, i0, i1;

    i0 = tyler_feedforward_stage_index(s, 0);
    i1 = tyler_feedforward_stage_index(s, 1);
    tyler_feedforward_unclamp();
    clamped_pattern = p;
    clamped_stage = s;
    for (i = i0; i < i1; i++) { // Input index
        in_state[i] =  get_pattern_noise(p, i, noise, n);
        in_state[i] = (in_state[i] < 0.0) ? 0.0 : in_state[i] ;
        in_state[i] = (in_state[i] > 1.0) ? 1.0 : in_state[i] ;
        clamped[i] = 1;
    }
}

void tyler_feedforward_clamp_now(int p, int s)
{
    tyler_feedforward_clamp_now_(p, s, 0, 0);

}

void tyler_feedforward_clamp_now_noise(int a, int b, int c, int d)
{
    tyler_feedforward_clamp_now_(a, b, c, d);

}

double tyler_feedforward_random_state(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return random_double(0.0, 1.0);
}
void tyler_feedforward_randomise_states(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        hidden_state[i] = tyler_feedforward_random_state();


    }

    for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
        in_state[i] = tyler_feedforward_random_state();
    }

    for (i = 0; i < TYLER_FEEDFORWARD_OUT; i++) {
        out_state[i] = tyler_feedforward_random_state();
    }
}

void tyler_feedforward_clamp_full_pattern(int p)
{
    tyler_feedforward_clamp(p, TYLER_STAGE_ALL);
}

void tyler_feedforward_clamp_full_gaussian_pattern(int p)
{
    tyler_feedforward_clamp(p, TYLER_STAGE_ALL);
}

void tyler_feedforward_clamp_full_pattern_now(int p)
{
    tyler_feedforward_clamp_now(p, TYLER_STAGE_ALL);
}

void tyler_feedforward_clamp_distinctive(int p)
{
    tyler_feedforward_clamp(p, TYLER_STAGE_DISTINCTIVE);
}

void tyler_feedforward_clamp_shared(int p)
{
    tyler_feedforward_clamp(p, TYLER_STAGE_SHARED);
}

void tyler_feedforward_clamp_functional(int p)
{
    tyler_feedforward_clamp(p, TYLER_STAGE_FUNCTIONAL);
}




double tyler_feedforward_propagate_type_(int t, int noise, int n) {


//      #if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
//      #endif
    if (t == TYLER_STAGE_ALL) {
        return tyler_feedforward_propagate_(noise, n);

    }

    int i, j;
    double prev_out_state[TYLER_FEEDFORWARD_OUT];
    double prev_hidden_state[TYLER_FEEDFORWARD_HIDDEN];
    double d = 0.0, input;
    if (clamped_pattern != -1) {
        int i0 = i0_of(t), i1 = N_of(t);


        for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
            in_state[i] = tyler_feedforward_random_state();

        }
//         printf("%d in_state = ", clamped_pattern);

        for (i = i0; i < i1; i++) {
            in_state[i] = get_pattern_noise(clamped_pattern, i, noise, n);


            in_state[i] = (in_state[i] < 0.0) ? 0.0 : in_state[i] ;
            in_state[i] = (in_state[i] > 1.0) ? 1.0 : in_state[i] ;

//             printf(" %1.2f", in_state[i]);
        }
//         printf(" noise = %1.2f\n", gaussian_noise);

        for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {

//             input = TYLER_FEEDFORWARD_BIAS;
            input = bias_hidden[i];

            for (j = 0; j < TYLER_FEEDFORWARD_IN; j++) {
                input += in_state[j] * weight_in_hidden[j][i];


            }
            prev_hidden_state[i] = hidden_state[i];
            hidden_state[i] = f(input);
            d += fabs(prev_hidden_state[i] - hidden_state[i]);

        }

        for (i = 0; i < TYLER_FEEDFORWARD_OUT; i++) {
//             input = TYLER_FEEDFORWARD_BIAS;
            input =  bias_out[i];


            for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
                input += hidden_state[j] * weight_hidden_out[j][i];
            }
            prev_out_state[i] = out_state[i];
            out_state[i] = f(input);
            d += fabs(prev_out_state[i] - out_state[i]);

        }


        d /= (double) (TYLER_FEEDFORWARD_OUT + TYLER_FEEDFORWARD_HIDDEN);
        return d;


    } else {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Cannot run network without input", 0);
    }


    return -1.0;



}



// double tyler_feedforward_propagate_type(int t) {
//   return  tyler_feedforward_propagate_type_(t,0,0);
//
// }

double tyler_feedforward_propagate_type_noise(int a, int b, int c) {
    return  tyler_feedforward_propagate_type_(a, b, c);

}
// double tyler_feedforward_propagate(void)
// {
//
//   return tyler_feedforward_propagate_(0, 0);
// }

double tyler_feedforward_propagate_noise(int noise, int n)
{

    return tyler_feedforward_propagate_(noise, n);
}



double tyler_feedforward_propagate_(int noise, int n)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, j;
    double prev_out_state[TYLER_FEEDFORWARD_OUT];
    double prev_hidden_state[TYLER_FEEDFORWARD_HIDDEN];
    double d = 0.0, input;

//     printf("noise = %f\n", gaussian_noise);



    if (clamped_pattern != -1) {


//         printf("%d in_state = ", clamped_pattern);

        for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
            in_state[i] =   get_pattern_noise(clamped_pattern, i, noise, n);


            in_state[i] = (in_state[i] < 0.0) ? 0.0 : in_state[i] ;
            in_state[i] = (in_state[i] > 1.0) ? 1.0 : in_state[i] ;
// 	  printf(" %1.2f", in_state[i]);

        }
//         printf(" noise = %1.2f\n", gaussian_noise);

        for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
            input = bias_hidden[i];

            for (j = 0; j < TYLER_FEEDFORWARD_IN; j++) {
                input += in_state[j] * weight_in_hidden[j][i];


            }
            prev_hidden_state[i] = hidden_state[i];
            hidden_state[i] = f(input);
            d += fabs(prev_hidden_state[i] - hidden_state[i]);

        }

        for (i = 0; i < TYLER_FEEDFORWARD_OUT; i++) {
            input = bias_out[i];

            for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
                input += hidden_state[j] * weight_hidden_out[j][i];
            }
            prev_out_state[i] = out_state[i];
            out_state[i] = f(input);
            d += fabs(prev_out_state[i] - out_state[i]);

        }


        d /= (double) (TYLER_FEEDFORWARD_OUT + TYLER_FEEDFORWARD_HIDDEN);
        return d;


    } else {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Cannot run network without input", 0);
    }


    return -1.0;



}



// double prev_weight_in_hidden[TYLER_FEEDFORWARD_IN][TYLER_FEEDFORWARD_HIDDEN];
double prev_delta_weight_in_hidden[TYLER_FEEDFORWARD_IN][TYLER_FEEDFORWARD_HIDDEN];
// double prev_weight_hidden_out[TYLER_FEEDFORWARD_HIDDEN][TYLER_FEEDFORWARD_OUT];
double prev_delta_weight_hidden_out[TYLER_FEEDFORWARD_HIDDEN][TYLER_FEEDFORWARD_OUT];
// double prev_bias_hidden[TYLER_FEEDFORWARD_HIDDEN];
double prev_delta_bias_hidden[TYLER_FEEDFORWARD_HIDDEN];

// double prev_bias_out[TYLER_FEEDFORWARD_OUT];
double prev_delta_bias_out[TYLER_FEEDFORWARD_OUT];

double backup_feedforward_weight[TYLER_FEEDFORWARD_PAIRS];

void tyler_feedforward_backup_weights(void)
{
    int i;
    for (i = 0; i < TYLER_FEEDFORWARD_PAIRS; i++) {
        backup_feedforward_weight[i] = *feedforward_weight[i];
    }
}

void tyler_feedforward_disrupt_weights(double noise)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    //   for (i = 0; i < BPTT_PAIRS; i++) {
    //     weight_has_noise[i] = 0;
    //   }

    for (i = 0; i < TYLER_FEEDFORWARD_PAIRS; i++) {
        *feedforward_weight[i] = backup_feedforward_weight[i] + random_double(-noise, noise);
    }


}

void tyler_feedforward_disrupt_weights_locus(double noise, int lesion_locus)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i;

    if(lesion_locus == TYLER_LESION_GLOBAL) {

        for (i = 0; i < TYLER_FEEDFORWARD_PAIRS; i++) {

            *feedforward_weight[i] = backup_feedforward_weight[i] + random_double(-noise, noise);
        }

    }
    else if (lesion_locus == TYLER_LESION_IN_HIDDEN) {
        for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN * TYLER_FEEDFORWARD_IN; i++) {
            *feedforward_weight[i] = backup_feedforward_weight[i] + random_double(-noise, noise);

        }
    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_HIDDEN) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Should not be asking for hidden-to-hidden lesion", 0);

        return;
    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_OUT) {
        for (i = TYLER_FEEDFORWARD_HIDDEN * TYLER_FEEDFORWARD_IN; i < TYLER_FEEDFORWARD_PAIRS; i++) {
            *feedforward_weight[i] = backup_feedforward_weight[i] + random_double(-noise, noise);
        }

    }
}


void tyler_feedforward_init_prev_weights(void)
{
    int i,j;


    if (!tyler_feedforward_prev_weights_initialised) {
//
        for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
            for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
                prev_delta_weight_in_hidden[i][j] = 0.0;
//                 prev_weight_in_hidden[i][j] = 0.0;

            }
        }
        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
//             prev_bias_out[j] = 0.0;
            prev_delta_bias_out[j] = 0.0;
        }

        for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
//             prev_bias_hidden[i] = 0.0;
            prev_delta_bias_hidden[i] = 0.0;

            for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
                prev_delta_weight_hidden_out[i][j] = 0.0;
//                 prev_weight_hidden_out[i][j] = 0.0;
            }
        }
        tyler_feedforward_prev_weights_initialised = 1;
    }

}






void tyler_feedforward_accumulate_delta_weights_(int noise, int n) {
    int i, j  ;
    double out_delta[TYLER_FEEDFORWARD_OUT];
    double hidden_delta[TYLER_FEEDFORWARD_HIDDEN];

    double hidden_target[TYLER_FEEDFORWARD_HIDDEN];
    if (clamped_pattern != -1) {


        // Output error
        for (i = 0; i < TYLER_FEEDFORWARD_OUT; i++) {
// Sum square error:
//             out_delta[i] = out_state[i] * (1.0 - out_state[i]) * cee(out_state[i], get_pattern_noise(clamped_pattern, i));
// Cross entropy error:
            out_delta[i] = out_state[i] -  get_pattern_noise(clamped_pattern, i, noise, n);



        }

        for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {

            for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
                // 	      delta_weight_hidden_out[i][j] += (1.0 - tyler_feedforward_momentum) * tyler_feedforward_learning_rate * hidden_delta[i] * out_state[j] + tyler_feedforward_momentum * (prev_weight_hidden_out[i][j] - prev_prev_weight_hidden_out[i][j]);
                //                 delta_weight_hidden_out[i][j] += tyler_feedforward_learning_rate * out_delta[j] * hidden_state[i];
                delta_weight_hidden_out[i][j] += out_delta[j] * hidden_state[i];

            }

        }

        if (tyler_feedforward_learned_biases) {
            for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {

                delta_bias_out[j] += out_delta[j];
            }


        }




        // Hidden error
        for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
            hidden_target[i] = 0.0;
            for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {

                hidden_target[i] += out_delta[j] * weight_hidden_out[i][j];

            }

            hidden_delta[i] = hidden_state[i] * (1.0 - hidden_state[i]) * hidden_target[i];



        }


        for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
            for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
                // 	      delta_weight_in_hidden[i][j] += tyler_feedforward_learning_rate * hidden_delta[j] * in_state[i];
                delta_weight_in_hidden[i][j] +=  hidden_delta[j] * in_state[i];


            }
        }

        if (tyler_feedforward_learned_biases) {

            for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
                delta_bias_hidden[j] += hidden_delta[j];

            }
        }


    } else {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Cannot run network without input", 0);
    }
}
void tyler_feedforward_accumulate_delta_weights_noise(int noise, int n) {
    tyler_feedforward_accumulate_delta_weights_(noise, n);

}
// void tyler_feedforward_accumulate_delta_weights(void) {
//   tyler_feedforward_accumulate_delta_weights_(0, 0);
//
// }
//


void tyler_feedforward_accelerate_momentum(void)
{

    double momentum_increment = TYLER_FEEDFORWARD_MOMENTUM_MAX / 500.0;
    tyler_feedforward_momentum += momentum_increment;


    if (tyler_feedforward_momentum > TYLER_FEEDFORWARD_MOMENTUM_MAX) {
        tyler_feedforward_momentum = TYLER_FEEDFORWARD_MOMENTUM_MAX;
    }

    printf("momentum = %f\n", tyler_feedforward_momentum);

}
void tyler_feedforward_adjust_weights(void) {
    int i, j;
    double tmp;
    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {

        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
//       tmp = weight_hidden_out[i][j];
//       weight_hidden_out[i][j] += tyler_feedforward_momentum * (weight_hidden_out[i][j] - prev_weight_hidden_out[i][j]) - tyler_feedforward_learning_rate * delta_weight_hidden_out[i][j];
//       prev_weight_hidden_out[i][j] = tmp;
//       delta_weight_hidden_out[i][j] = 0.0;


            tmp = tyler_feedforward_momentum * prev_delta_weight_hidden_out[i][j] - tyler_feedforward_learning_rate * delta_weight_hidden_out[i][j];
            weight_hidden_out[i][j] += tmp;
            prev_delta_weight_hidden_out[i][j] = tmp;
            delta_weight_hidden_out[i][j] = 0.0;


        }
    }
    if(tyler_feedforward_learned_biases) {

        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
//       tmp = bias_out[j];
//       bias_out[j] += tyler_feedforward_momentum * (bias_out[j] - prev_bias_out[j]) - tyler_feedforward_learning_rate * delta_bias_out[j];
//       prev_bias_out[j] = tmp;
//       delta_bias_out[j] = 0.0;

            tmp = tyler_feedforward_momentum * prev_delta_bias_out[j] - tyler_feedforward_learning_rate * delta_bias_out[j];
            bias_out[j] += tmp;
            prev_delta_bias_out[j] = tmp;
            delta_bias_out[j] = 0.0;
        }


        for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
//       tmp = bias_hidden[j];
//       bias_hidden[j] += tyler_feedforward_momentum * (bias_hidden[j] - prev_bias_hidden[j]) - tyler_feedforward_learning_rate * delta_bias_hidden[j];
//       prev_bias_hidden[j] = tmp;
//       delta_bias_hidden[j] = 0.0;

            tmp =  tyler_feedforward_momentum * prev_delta_bias_hidden[j] - tyler_feedforward_learning_rate * delta_bias_hidden[j];
            bias_hidden[j] += tmp;
            prev_delta_bias_hidden[j] = tmp;
            delta_bias_hidden[j] = 0.0;
        }

    }

    for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
//       tmp = weight_in_hidden[i][j];
//       weight_in_hidden[i][j] += tyler_feedforward_momentum * (weight_in_hidden[i][j] - prev_weight_in_hidden[i][j]) - tyler_feedforward_learning_rate * delta_weight_in_hidden[i][j];
//       prev_weight_in_hidden[i][j] = tmp;
//       delta_weight_in_hidden[i][j] = 0.0;

            tmp = tyler_feedforward_momentum * prev_delta_weight_in_hidden[i][j] - tyler_feedforward_learning_rate * delta_weight_in_hidden[i][j];
            weight_in_hidden[i][j] += tmp;
            prev_delta_weight_in_hidden[i][j] = tmp;
            delta_weight_in_hidden[i][j] = 0.0;

        }
    }

    //     tyler_feedforward_accelerate_momentum();


}
void tyler_feedforward_save_file(char *filename)
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

    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
            fprintf(fp, " %lf", weight_hidden_out[i][j]);
        }
    }

    fprintf(fp, "\n");

    for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
            fprintf(fp, " %lf", weight_in_hidden[i][j]);
        }
    }

    fprintf(fp, "\n");

    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        fprintf(fp, " %lf", bias_hidden[i]);

    }

    for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
        fprintf(fp, " %lf", bias_out[j]);
    }

    fprintf(fp, "\n\n");

    for (i = 0; i < tyler_iterations_so_far; i++) {
        fprintf(fp, " %lf", Total_Error[i]);
    }


    fprintf(fp, "\nEOF\n");



    if (fp) {
        fclose(fp);
    }

}


int tyler_feedforward_open_file(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    const char extension[4]=".dat";
    int i, j;

    for (i = 0; i < strlen(extension); i++) {

//       printf("filename[strlen(filename)-4+%d] = %c\nextension[%d] = %c\n", i, filename[strlen(filename)-4+i], i, extension[i]);

        if(filename[strlen(filename)-strlen(extension)+i] == extension[i]) {
            j =1;

        } else {
            j=0;
            break;

        }
    }
    if (j) {
        printf("filename = %s\n", filename);

        return tyler_feedforward_open_dat_file(filename);
    }

    j = 0;
    const char ext[]=".OXlearn";

    for (i = 0; i < strlen(ext); i++) {

        printf("filename[strlen(filename)-4+%d] = %c\nextension[%d] = %c\n", i, filename[strlen(filename)-strlen(ext)+i], i, ext[i]);

        if(filename[strlen(filename)-strlen(ext)+i] == ext[i]) {
            j =1;

        } else {
            j=0;
            break;

        }
    }
    if (j) {
        printf("filename = %s\n", filename);

        return tyler_feedforward_open_OXweights_file(filename);
    }


    FILE *fp = NULL;

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
    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {

            if (!fscanf(fp, " %lf", &weight_hidden_out[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_hidden_out[][]", 0);
                return 0;
            }
        }
    }


    for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
            if (!fscanf(fp, " %lf", &weight_in_hidden[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_in_hidden[][]", 0);
                return 0;
            }
        }
    }


    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        if (!fscanf(fp, " %lf", &bias_hidden[i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bias_hidden[]", 0);
            return 0;
        }
    }

    for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
        if (!fscanf(fp, " %lf", &bias_out[j])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bias_out[]", 0);
            return 0;
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

// //         char buf[BUFSIZ] = "Garbage";
//         i = 0;
    /*
            if (fgets(buf, sizeof(buf), fp) != NULL)
            {
                //       printf ("Line %4d: %s", i, buf);
                i++;
            }   */
    char *buf = malloc(sizeof(char *) * 4);;
    char *compare = "EOF\0";//malloc(sizeof(char *) * 3);;

    if (!fscanf(fp, " %s", buf)) {

        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not reach end of file", 0);
        return 0;
    }

    if(strncmp(buf, compare, 3) != 0) {


        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not reach end of file", 0);
        return 0;
    }

    if (fp) {
        fclose(fp);
    }

//         if (i == 2) {
// 	  printf("boop\n");
//             return 1;
//         }
//         else {
// 	  printf("buup\n");
//
//             return 0;
//         }
//     }

    return 1;

}
int tyler_feedforward_open_dat_file(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    printf("filename = %s\n", filename);

    FILE *fp = NULL;
    int i, j;

    char tmp = 0;
    fp = fopen(filename, "r");

    tmp = getc(fp);
    printf("tmp = %c\n", tmp);

    while (tmp != 'H') {
        tmp = getc(fp);
        printf("tmp = %c\n", tmp);

    }

    for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {

        for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {


            tmp = getc(fp);

            //       printf("tmp = %c\n", tmp);
            if (tmp != '+') {
                ungetc(tmp, fp);
            }
            if (!fscanf(fp, "%lf", &weight_in_hidden[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_in_hidden[][]", 0);
                return 0;
            }

            printf("%f\n", weight_in_hidden[i][j]);



        }

    }



    for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {

        tmp = getc(fp);

        //       printf("tmp = %c\n", tmp);
        if (tmp != '+') {
            ungetc(tmp, fp);
        }
        if (!fscanf(fp, " %lf", &bias_hidden[j])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bias_hidden[]", 0);
            return 0;
        }
        printf("%f\n",bias_hidden[j]);
    }


    tmp = getc(fp);
//   printf("tmp = %c\n", tmp);

    while (tmp != 'O') {
        tmp = getc(fp);
        //     printf("tmp = %c\n", tmp);

    }

    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {

            tmp = getc(fp);

            //       printf("tmp = %c\n", tmp);
            if (tmp != '+') {
                ungetc(tmp, fp);
            }
            if (!fscanf(fp, " %lf", &weight_hidden_out[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_hidden_out[][]", 0);
                return 0;
            }

            printf("%f\n",weight_hidden_out[i][j]);

        }
    }



    for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {


        tmp = getc(fp);

        //       printf("tmp = %c\n", tmp);
        if (tmp != '+') {
            ungetc(tmp, fp);
        }
        if (!fscanf(fp, " %lf", &bias_out[j])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bias_out[]", 0);
            return 0;
        }

        printf( " %f\n", bias_out[j]);



    }


    if (fp) {
        fclose(fp);
    }


    return 1;

}



int tyler_feedforward_open_OXweights_file(char *filename)
{

    //Generated by OXlearn in Matlab using:
    // save('OXweights.OXlearn', 'OXweightsHiddenToOutput', 'OXweightsToOutputBias', 'OXweightsInputToHidden', 'OXweightsToHiddenBias',  '-ascii', '-double', '-tabs')

#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    printf("filename = %s\n", filename);

    FILE *fp = NULL;
    int i, j;

    fp = fopen(filename, "r");




    for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
        for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {



            if (!fscanf(fp, " %lf", &weight_hidden_out[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_hidden_out[][]", 0);
                return 0;
            }

            printf("%f, ",weight_hidden_out[i][j]);

        }
        printf("\n");

    }



    for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {


        if (!fscanf(fp, " %lf", &bias_out[j])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bias_out[]", 0);
            return 0;
        }

        printf( " %f, ", bias_out[j]);



    }

    printf("\n");

    for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {

        for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {




            if (!fscanf(fp, "%lf", &weight_in_hidden[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_in_hidden[][]", 0);
                return 0;
            }

            printf("%f, ", weight_in_hidden[i][j]);



        }

        printf("\n");


    }



    for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {


        if (!fscanf(fp, " %lf", &bias_hidden[j])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bias_hidden[]", 0);
            return 0;
        }
        printf("%f, ",bias_hidden[j]);
    }
    printf("\n");











    if (fp) {
        fclose(fp);
    }


    return 1;

}

double tyler_feedforward_sorensen_distance(int p, int t)
{

    int i;
    double u = 0.0, n = 0.0;
    for (i = 0; i < TYLER_FEEDFORWARD_OUT; i++) {
        u += (double) ((int)round(out_state[i])  || get_pattern_int(p, i));

        n += (double) ((int)round(out_state[i])  && get_pattern_int(p, i));
    }

    if (u == n) {
        return 0.0;
    }
    if (u) {

        return fabs((u - 2*n) / u)      / (double) TYLER_FEEDFORWARD_OUT;
    }
    return 1.0;
}



double tyler_feedforward_jaccard_distance(int p)
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

        for (i = 0; i < TYLER_FEEDFORWARD_OUT; i++) {


            u += (double) ((int)round(out_state[i])  || get_pattern_int(p, i));

            n += (double) ((int)round(out_state[i])  && get_pattern_int(p, i));

        }
        if (u) {
            total_error = ((u - n) / u) / (double) TYLER_FEEDFORWARD_OUT;
        }

    }
//     printf("total error = %f\n", total_error);
    return total_error;
}


double tyler_feedforward_calculate_rms_error_type_(int p, int error_t, int noise, int n)
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
            total_error += sqr(get_pattern_noise( p, i, noise, n) - out_state[i]);
        }
    }
    total_error /= (double) (n_1 - n_0);
    return sqrt(total_error);
}

double tyler_feedforward_calculate_rms_error_type_noise(int p, int error_t, int noise, int n)      {
    return tyler_feedforward_calculate_rms_error_type_(p, error_t, noise, n);

}

// double tyler_feedforward_calculate_rms_error_type(int p, int error_t){
// return tyler_feedforward_calculate_rms_error_type_(p, error_t, 0 , 0);
//
// }




double tyler_feedforward_calculate_rss_error_type_(int p, int error_t, int noise, int n)
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
            total_error += sqr(get_pattern_noise(p, i, noise, n) - out_state[i]);
        }
    }
    total_error /= (double) (n_1 - n_0);
    return total_error;
}
double tyler_feedforward_calculate_rss_error_type_noise(int p, int error_t, int noise, int n)      {
    return tyler_feedforward_calculate_rss_error_type_(p, error_t, noise, n);

}
/*
double tyler_feedforward_calculate_rss_error_type(int p, int error_t){
 return  tyler_feedforward_calculate_rss_error_type_(p, error_t, 0 , 0);

} */


void tyler_feedforward_init_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int i, j;
    tyler_feedforward_prev_weights_initialised = 0;

    // Initialise the weights:
    for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
            weight_in_hidden[i][j] = tyler_feedforward_random_weight();
            feedforward_weight[TYLER_FEEDFORWARD_HIDDEN*i +j] = &(weight_in_hidden[i][j]);
        }
    }
    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
            weight_hidden_out[i][j] = tyler_feedforward_random_weight();
            feedforward_weight[TYLER_FEEDFORWARD_HIDDEN*TYLER_FEEDFORWARD_IN + TYLER_FEEDFORWARD_OUT*i + j] = &(weight_hidden_out[i][j]);

        }
    }

    //biases

    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        if(tyler_feedforward_learned_biases) {
            bias_hidden[i] = tyler_feedforward_random_weight();
        }
        else {
            bias_hidden[i] = 1.0;
        }
    }

    for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
        if(tyler_feedforward_learned_biases) {

            bias_out[j] = tyler_feedforward_random_weight();
        }
        else {
            bias_out[j] = 1.0;
        }
    }
}

void tyler_feedforward_init_delta_weights(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int i, j;

    // Initialise the weights:
    for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
//       delta_bias_in[i] = 0.0;
        for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
            delta_weight_in_hidden[i][j] = 0.0;
        }
    }
    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        delta_bias_hidden[i] = 0.0;

        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
            delta_weight_hidden_out[i][j] = 0.0;
        }
    }
    for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
        delta_bias_out[j] = 0.0;
    }
}

// void tyler_feedforward_init_prev_weights(void)
// {
// #if PRINT_FUNCTION
//     printf("%s\n", __FUNCTION__);
// #endif
//     int i, j;
//
//     // Initialise the weights:
//     for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
//         for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
//             prev_weight_in_hidden[i][j] = 0.0;
//             prev_prev_weight_in_hidden[i][j] = 0.0;
//         }
//     }
//     for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
//         for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
//             prev_weight_hidden_out[i][j] = 0.0;
//             prev_prev_weight_hidden_out[i][j] = 0.0;
//         }
//     }
// }


int tyler_feedforward_is_clamped(int i)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif



    if (i < TYLER_FEEDFORWARD_IN) {
        return clamped[i];
    }
    else if (i < TYLER_FEEDFORWARD_UNITS) {
        return 0;
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid index i for units", 1, i);
    return 0;
}



void tyler_feedforward_lesion_random_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int x = 0, i = random_int(0, TYLER_FEEDFORWARD_PAIRS-1);
    // 	printf("%i / %i a\n", i, TYLER_PAIRS);
    while ((!(*feedforward_weight[i]))) // && (x < (TYLER_PAIRS * 2)))
    {
        // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
        i = random_int(0, TYLER_FEEDFORWARD_PAIRS-1);
        x++;
        if (x > TYLER_FEEDFORWARD_PAIRS) {
            break;
        }
    }
    // 	printf("%i / %i b\n", i, TYLER_PAIRS);
    // 	printf("weight[%i] = %f 2\n", i, *weight[i]);

    *feedforward_weight[i] = 0.0;
}

void tyler_feedforward_lesion_random_ih_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
//     #define TYLER_FEEDFORWARD_PAIRS (TYLER_FEEDFORWARD_IN*TYLER_FEEDFORWARD_HIDDEN + TYLER_FEEDFORWARD_HIDDEN*TYLER_FEEDFORWARD_OUT)

    int x = 0, i = random_int(0, TYLER_FEEDFORWARD_IN*TYLER_FEEDFORWARD_HIDDEN -1);
    // 	printf("%i / %i a\n", i, TYLER_PAIRS);
    while ((!(*feedforward_weight[i]))) // && (x < (TYLER_PAIRS * 2)))
    {
        // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
        i = random_int(0, TYLER_FEEDFORWARD_IN*TYLER_FEEDFORWARD_HIDDEN-1);
        x++;
        if (x > TYLER_FEEDFORWARD_IN*TYLER_FEEDFORWARD_HIDDEN) {
            break;
        }
    }
    // 	printf("%i / %i b\n", i, TYLER_PAIRS);
    // 	printf("weight[%i] = %f 2\n", i, *weight[i]);

    *feedforward_weight[i] = 0.0;
}


void tyler_feedforward_lesion_random_ho_weight(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int x = 0, i = random_int(TYLER_FEEDFORWARD_IN*TYLER_FEEDFORWARD_HIDDEN,  TYLER_FEEDFORWARD_PAIRS -1);
    // 	printf("%i / %i a\n", i, TYLER_PAIRS);
    while ((!(*feedforward_weight[i]))) // && (x < (TYLER_PAIRS * 2)))
    {
        // 	  printf("weight[%i] = %f 1\n", i, *weight[i]);
        i = random_int(TYLER_FEEDFORWARD_IN*TYLER_FEEDFORWARD_HIDDEN,  TYLER_FEEDFORWARD_PAIRS -1);
        x++;
        if (x > TYLER_FEEDFORWARD_OUT*TYLER_FEEDFORWARD_HIDDEN) {
            break;
        }
    }
    // 	printf("%i / %i b\n", i, TYLER_PAIRS);
    // 	printf("weight[%i] = %f 2\n", i, *weight[i]);

    *feedforward_weight[i] = 0.0;
}

void tyler_feedforward_lesion_weights(double percentage)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, n = (int)round(TYLER_FEEDFORWARD_PAIRS * percentage/100.0);
    for (i = 0; i < n; i++) {
        tyler_feedforward_lesion_random_weight();
    }
}

void tyler_feedforward_lesion_weights_locus(double percentage, int lesion_locus)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int i, n;

    if(lesion_locus == TYLER_LESION_GLOBAL) {

        n = (int)round(TYLER_FEEDFORWARD_PAIRS * percentage/100.0);
        for (i = 0; i < n; i++) {

            tyler_feedforward_lesion_random_weight();
        }

    }
    else if (lesion_locus == TYLER_LESION_IN_HIDDEN) {
        n = (int)round(TYLER_FEEDFORWARD_HIDDEN * TYLER_FEEDFORWARD_IN * percentage/100.0);
        for (i = 0; i < n; i++) {
            tyler_feedforward_lesion_random_ih_weight();

        }
    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_HIDDEN) {

        return;
    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_OUT) {
        n = (int)round(TYLER_FEEDFORWARD_HIDDEN * TYLER_FEEDFORWARD_OUT * percentage/100.0);
        for (i = 0; i < n; i++) {
            tyler_feedforward_lesion_random_ho_weight();
        }

    }
}
void tyler_feedforward_unlesion_weights(void)
{
    int i;
    for (i = 0; i < TYLER_FEEDFORWARD_PAIRS; i++) {
        *feedforward_weight[i] = backup_feedforward_weight[i];
    }
}

int tyler_feedforward_get_clamped_pattern(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return clamped_pattern;
}
int tyler_feedforward_get_clamped_stage(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return clamped_stage;
}

int tyler_feedforward_get_iterations_so_far(void)
{
    return tyler_iterations_so_far;
}

double tyler_feedforward_euclidean_distance_(int p, int noise, int n)
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

        for (i = 0; i < TYLER_FEEDFORWARD_OUT; i++) {
            total_error += sqr(out_state[i] -  get_pattern_noise(p, i, noise, n));


        }
        total_error = sqrt(total_error) / (double) TYLER_FEEDFORWARD_OUT;

    }
    return total_error;
}

double tyler_feedforward_euclidean_distance_noise(int p, int noise, int n)
{
    return tyler_feedforward_euclidean_distance_(p, noise, n);

}

/*
double tyler_feedforward_euclidean_distance(int p)     {
 return tyler_feedforward_euclidean_distance_(p, 0, 0);


}    */


double tyler_feedforward_calculate_fabs_error_type_(int p, int error_t, int noise, int n)
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
            total_error += fabs( get_pattern_noise(p, i, noise, n) - out_state[i]);
        }
    }
    total_error /= (double) (n_1 - n_0);
    return total_error;
}
double tyler_feedforward_calculate_fabs_error_type_noise(int p, int error_t, int noise, int n) {
    return  tyler_feedforward_calculate_fabs_error_type_(p, error_t, noise, n);

}

// double tyler_feedforward_calculate_fabs_error_type(int p, int error_t){
//   return  tyler_feedforward_calculate_fabs_error_type_(p, error_t, 0,0);
//
// }

int tyler_feedforward_stage_to_type(int stage)
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


int tyler_feedforward_get_clamped_type(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return tyler_feedforward_stage_to_type(clamped_stage);
}

double tyler_feedforward_fabs_average_weight(int locus)
{

    double fabs_average_weight = 0.0;
    int i;

    if(locus == TYLER_LESION_GLOBAL) {

        for (i = 0; i < TYLER_FEEDFORWARD_PAIRS; i++) {
            fabs_average_weight += fabs(*feedforward_weight[i]);
        }
        fabs_average_weight /= (double) TYLER_FEEDFORWARD_PAIRS;
        return fabs_average_weight;

    }
    else if (locus == TYLER_LESION_IN_HIDDEN) {
        for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN * TYLER_FEEDFORWARD_IN; i++) {
            fabs_average_weight += fabs(*feedforward_weight[i]);
        }
        fabs_average_weight /= (double) TYLER_FEEDFORWARD_HIDDEN * TYLER_FEEDFORWARD_IN;
        return fabs_average_weight;
    }
    else if (locus == TYLER_LESION_HIDDEN_HIDDEN) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Should not be asking for hidden-to-hidden weight average", 0);
    }
    else if (locus == TYLER_LESION_HIDDEN_OUT) {
        for (i = TYLER_FEEDFORWARD_HIDDEN * TYLER_FEEDFORWARD_IN; i < TYLER_FEEDFORWARD_PAIRS; i++) {
            fabs_average_weight += fabs(*feedforward_weight[i]);
        }
        fabs_average_weight /= (double) TYLER_FEEDFORWARD_HIDDEN * TYLER_FEEDFORWARD_OUT;
        return fabs_average_weight;

    }
    return -100.0;

}


