#include "semantic_tasks.h"
#include "bm.h"

void bm_save_naming_response(int p, int sample, int level, int distribution)
{
    int i;
    int units_on = 0, saved_i = -1;
    for (i = 0; i < BM_N_NAME; i++) {
        if (bm_get_state(i)) { //BMs have int states
            units_on++; //because state i is activated
            saved_i = i;

        }
    }


    if (units_on == 1) {
        naming_response[distribution][level][sample][p][0] = (double) name_to_pattern(saved_i, p);
        naming_response[distribution][level][sample][p][1] = (double) bm_get_state(saved_i); // which is a 1.0, because we are sure it's giving us this pattern
    } else {
        naming_response[distribution][level][sample][p][0] = (double) bm_compute_output_interpretation_type(RMSD, VISUAL, NAME);
        if (!units_on) {
            naming_response[distribution][level][sample][p][1] =  0.0; // which is a function of the number of on units?

        }
        else {
            naming_response[distribution][level][sample][p][1] =  1.0 / (double) (units_on + 0.000001); // which is a function of the number of on units?
        }

    }
}

void bm_save_naming_response_alt(int p, int sample, int level, int distribution)
{
    //find lowest error
    int q;//r;
    double min_error = 1.0;
    for (q = 0; q < P; q++) {

        if (bm_jaccard_distance_type(q, VISUAL, NAME) < min_error) {
            naming_response[distribution][level][sample][p][0] = (double) q;
//             naming_response[distribution][level][sample][p][1] = bptt_get_state(pattern_to_name_index(q), bptt_t1);

            min_error =   bm_jaccard_distance_type(q, VISUAL, NAME);

            naming_response[distribution][level][sample][p][1] = (1.0 -min_error);

// 	    printf("p = %i bptt_jaccard_distance_type(q = %i, VISUAL, NAME) = %f %f\n", p, q, bptt_jaccard_distance_type(q, VISUAL, NAME), bptt_jaccard_distance(q, VISUAL));

        }
    }

//     int i;
//     double max_state = 0.0;
//     for (i = 0; i < BPTT_N_NAME; i++) {
//         if ((double) bm_get_state(i) > max_state) {
// // 	naming_response[distribution][level][sample][p][0] = (double) name_to_pattern(i, p);
// //             r = name_to_pattern(i, p);
//             naming_response[distribution][level][sample][p][1] = (double) bm_get_state(i);
//             max_state = (double) bm_get_state(i);
//         }
//     }

//     if (q != r) {
//         printf("r = %i, q = %i, error = %f, state = %f\n", r, q, min_error, max_state );
//     }
}


void bm_save_sorting_response_given_name(int p, int sample, int level, int distribution)
{
    int i, i0 = i0_of(ENCYCLOPEDIC), i1 = i0 + N_DOMAIN_UNIT, i2 = i1 + N_CATEGORY_UNIT;
    int max_state = 0;

    for (i = i0; i < i1; i++) { // for all category units, which are mutually exclusive too
        if (bm_get_state(i) > max_state) {
            sorting_response_given_name[distribution][level][sample][p][0] = i - i0;
            max_state = bm_get_state(i);
        }
    }
    max_state = 0;

    for (i = i1; i < i2; i++) { // for all category units, which are mutually exclusive too
        if (bm_get_state(i) > max_state) {
            sorting_response_given_name[distribution][level][sample][p][1] = i - i1;
            max_state = bm_get_state(i);
        }
    }
}

void bm_save_sorting_response_given_visual(int p, int sample, int level, int distribution)
{
    int i, i0 = i0_of(ENCYCLOPEDIC), i1 = i0 + N_DOMAIN_UNIT, i2 = i1 + N_CATEGORY_UNIT;
    int max_state = 0;
    for (i = i0; i < i1; i++) { // for all category units, which are mutually exclusive too
        if (bm_get_state(i) > max_state) {
            sorting_response_given_visual[distribution][level][sample][p][0] = i - i0;
            max_state = bm_get_state(i);
        }
    }
    max_state = 0;

    for (i = i1; i < i2; i++) { // for all category units, which are mutually exclusive too
        if (bm_get_state(i) > max_state) {
            sorting_response_given_visual[distribution][level][sample][p][1] = i - i1;
            max_state = bm_get_state(i);
        }
    }
}

void bm_save_target_attractor(int p, int sample, int level, int distribution)
{
    int i;
    for (i = 0; i < K; i++) {
        target_attractor[distribution][level][sample][p][i] = (double) bm_get_state(N+i);
    }
}

double bm_distance_between_current_and_target_attractor(int p, int sample, int level, int distribution)
{
    int i;
    double distance = 0.0;
    for (i = 0; i < K; i++) {
        distance += sqr((double) bm_get_state(N+i) - target_attractor[distribution][level][sample][p][i]);
    }
    return sqrt(distance);
}

void bm_save_word_to_picture_response_target(int p, int sample, int level, int distribution)
{
    distance_target[distribution][level][sample][p] = bm_distance_between_current_and_target_attractor(p, sample, level, distribution);
}

void bm_save_word_to_picture_response_distant_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = bm_distance_between_current_and_target_attractor(p, sample, level, distribution);

    if (distance > distance_target[distribution][level][sample][p]) {
        word_to_picture_distant[distribution][level][sample][p]++;
    }
}

void bm_save_word_to_picture_response_unrelated_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = bm_distance_between_current_and_target_attractor(p, sample, level, distribution);
    if (distance > distance_target[distribution][level][sample][p]) {
        word_to_picture_unrelated[distribution][level][sample][p]++;
    }
}

void bm_save_word_to_picture_response_close_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = bm_distance_between_current_and_target_attractor(p, sample, level, distribution);

    if (distance > distance_target[distribution][level][sample][p]) { //if the current semantic state is closer to the target state (reached via name) then the one reached via picture.
        word_to_picture_close[distribution][level][sample][p]++; // the network got it right, under the close condition
    }
}

void bm_save_drawing_response(int d, int l, int s, int p)
{
    double response;
    int i;

    for (i = 0; i < BM_N_VISUAL; i++) {
        response = (bm_get_state(BM_N_NAME+i) >= 0.25);
        if (get_pattern(p, BM_N_NAME+i)) { //if there is a feature that needs to be added
            if (get_pattern(p, BM_N_NAME+i) == response) { //if it is included
            }
            else {
                drawing_omission[d][l][s][p]++;
            }
        } else { // nothing should be drawn
            if (response) { // but if it is
                drawing_intrusion[d][l][s][p]++;
            }
        }
    }
}

void bm_save_copying_response(int d, int l, int s, int p)
{
    double response;
    int i;
    for (i = 0; i < BM_N_VISUAL; i++) {
        response = (bm_get_state(BM_N_NAME+i) >= 0.25);
        if (get_pattern(p, BM_N_NAME+i)) { //if there is a feature that needs to be added
            if (get_pattern(p, BM_N_NAME+i) == response) { //if it is included
            }
            else {
                copying_omission[d][l][s][p]++;
            }
        } else { // nothing should be drawn
            if (response) { // but if it is
                copying_intrusion[d][l][s][p]++;
            }
        }
    }
}