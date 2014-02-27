#include "semantic_tasks.h"
#include "pdptool.h"

void pdptool_save_naming_response(int p, int sample, int level, int distribution)
{
    int i;
    double max_state = 0.0;
    for (i = 0; i < PDPTOOL_N_NAME; i++) {
        if (pdptool_get_state(i) > max_state) {
            naming_response[distribution][level][sample][p][0] = (double) name_to_pattern(i, p);
            naming_response[distribution][level][sample][p][1] = pdptool_get_state(i);
            max_state = pdptool_get_state(i);
        }
    }
}


void pdptool_save_naming_response_alt(int p, int sample, int level, int distribution)
{
    //find lowest error
    int q;//, r;
    double min_error = 1.0;
    for (q = 0; q < P; q++) {

        if (pdptool_jaccard_distance_type(q,NAME) < min_error) {
            naming_response[distribution][level][sample][p][0] = (double) q;
//             naming_response[distribution][level][sample][p][1] = bptt_get_state(pattern_to_name_index(q), bptt_t1);
            min_error =   pdptool_jaccard_distance_type(q,NAME);
// 	    printf("p = %i bptt_jaccard_distance_type(q = %i, VISUAL, NAME) = %f %f\n", p, q, bptt_jaccard_distance_type(q, VISUAL, NAME), bptt_jaccard_distance(q, VISUAL));

        }
    }

    int i;
    double max_state = 0.0;
    for (i = 0; i < BPTT_N_NAME; i++) {
        if (pdptool_get_state(i) > max_state) {
// 	naming_response[distribution][level][sample][p][0] = (double) name_to_pattern(i, p);
//             r = name_to_pattern(i, p);
            naming_response[distribution][level][sample][p][1] = pdptool_get_state(i);
            max_state = pdptool_get_state(i);
        }
    }

//     if (q != r) {
//         printf("r = %i, q = %i, error = %f, state = %f\n", r, q, min_error, max_state );
//     }
}

void pdptool_save_sorting_response_given_name(int p, int sample, int level, int distribution)
{

    //ALERT: These values need to be discovered for this network, as they are not the same as the other two!
    int i, i0 = i0_of(ENCYCLOPEDIC), i1 = i0 + N_DOMAIN_UNIT, i2 = i1 + N_CATEGORY_UNIT;
    int max_state = 0;
    //     extern int sorting_response_given_name[SORTING_LESION_DISTRIBUTION][SORTING_LESION_LEVEL][SORTING_SAMPLE][P_MAX][2];

    for (i = i0; i < i1; i++) { // for all category units, which are mutually exclusive too
        if (pdptool_get_state(i) > max_state) {
            sorting_response_given_name[distribution][level][sample][p][0] = i - i0;
            // 	  printf("sorting_response_given_name[%i][%i][%i][%i][%i] = %i\n" , distribution, level, sample, p, 0, sorting_response_given_name[distribution][level][sample][p][0]);
            max_state = pdptool_get_state(i);
        }
    }
    max_state = 0;

    for (i = i1; i < i2; i++) { // for all category units, which are mutually exclusive too
        if (pdptool_get_state(i) > max_state) {
            sorting_response_given_name[distribution][level][sample][p][1] = i - i1;
            // 		printf("sorting_response_given_name[%i][%i][%i][%i][%i] = %i\n" , distribution, level, sample, p, 1, sorting_response_given_name[distribution][level][sample][p][1]);
            max_state = pdptool_get_state(i);
        }
    }
}

void pdptool_save_sorting_response_given_visual(int p, int sample, int level, int distribution)
{
    //ALERT: Same problem with previous "pdptool_" function
    int i, i0 = i0_of(ENCYCLOPEDIC), i1 = i0 + N_DOMAIN_UNIT, i2 = i1 + N_CATEGORY_UNIT;
    int max_state = 0;
    //   extern int sorting_response_given_visual[SORTING_LESION_DISTRIBUTION][SORTING_LESION_LEVEL][SORTING_SAMPLE][P_MAX][2];
    for (i = i0; i < i1; i++) { // for all category units, which are mutually exclusive too
        if (pdptool_get_state(i) > max_state) {
            sorting_response_given_visual[distribution][level][sample][p][0] = i - i0;
            // 	  printf("sorting_response_given_visual[%i][%i][%i][%i][%i] = %i\n" , distribution, level, sample, p, 0, sorting_response_given_visual[distribution][level][sample][p][0]);
            max_state = pdptool_get_state(i);
        }
    }
    max_state = 0;

    for (i = i1; i < i2; i++) { // for all category units, which are mutually exclusive too
        if (pdptool_get_state(i) > max_state) {
            sorting_response_given_visual[distribution][level][sample][p][1] = i - i1;

            max_state = pdptool_get_state(i);
        }
    }
}



void pdptool_save_target_attractor(int p, int sample, int level, int distribution)
{
    int i;
    for (i = 0; i < PDPTOOL_K; i++) {
        target_attractor[distribution][level][sample][p][i] = (double) pdptool_get_state(PDPTOOL_N+i);

    }

}


double pdptool_distance_between_current_and_target_attractor(int p, int sample, int level, int distribution)
{
    int i;
    double distance = 0.0;
    for (i = 0; i < PDPTOOL_K; i++) {
        distance += sqr((double) pdptool_get_state(N+i) - target_attractor[distribution][level][sample][p][i]);

    }


    return sqrt(distance);
}



void pdptool_save_word_to_picture_response_close_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = 0.0;// = 0.0;void distance_between_current_and_target_attractor(int p, int sample, int level, int distribution, double *distance)

    distance = pdptool_distance_between_current_and_target_attractor(p, sample, level, distribution);
    if (distance < distance_target[distribution][level][sample][p]) { //if the current semantic state is closer to the target state (reached via name) then the one reached via picture.


    }
    else {
        word_to_picture_close[distribution][level][sample][p]++;
    }
}


void pdptool_save_word_to_picture_response_target(int p, int sample, int level, int distribution)
{
    distance_target[distribution][level][sample][p] = pdptool_distance_between_current_and_target_attractor(p, sample, level, distribution);


    int i;
    double distance = 0.0;
    for (i = 0; i < K; i++) {
        distance += sqr(pdptool_get_state(N+i) - target_attractor[distribution][level][sample][p][i]);
    }

}





void pdptool_save_word_to_picture_response_distant_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = pdptool_distance_between_current_and_target_attractor(p, sample, level, distribution);
    if (distance < distance_target[distribution][level][sample][p]) {

    }
    else {

        word_to_picture_distant[distribution][level][sample][p]++;
    }
}


void pdptool_save_word_to_picture_response_unrelated_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = pdptool_distance_between_current_and_target_attractor(p, sample, level, distribution);
    if (distance < distance_target[distribution][level][sample][p]) {
    }
    else {
        word_to_picture_unrelated[distribution][level][sample][p]++;
    }
}



void pdptool_save_drawing_response(int d, int l, int s, int p)
{
    double response;
    int i;
    //ALERT: Same as previous.
    for (i = 0; i < PDPTOOL_N_VISUAL; i++) {
        response = (pdptool_get_state(PDPTOOL_N_NAME+i) >= 0.25);
        if (get_pattern(p, PDPTOOL_N_NAME+i)) { //if there is a feature that needs to be added
            if (get_pattern(p, PDPTOOL_N_NAME+i) == response) { //if it is included

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

void pdptool_save_copying_response(int d, int l, int s, int p)
{
    double response;
    int i;
    for (i = 0; i < PDPTOOL_N_VISUAL; i++) {
        response = (pdptool_get_state(PDPTOOL_N_NAME+i) >= 0.25);
        if (get_pattern(p, PDPTOOL_N_NAME+i)) { //if there is a feature that needs to be added
            if (get_pattern(p, PDPTOOL_N_NAME+i) == response) { //if it is included
// 		copying_correct[d][l][s][p]++;
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

