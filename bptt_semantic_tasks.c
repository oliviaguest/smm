#include "semantic_tasks.h"
#include "bptt.h"

void bptt_save_naming_response(int p, int sample, int level, int distribution)
{
    int i;
    double max_state = 0.0;
    for (i = 0; i < BPTT_N_NAME; i++) {
        if (bptt_get_state(i, bptt_t1) > max_state) {
            naming_response[distribution][level][sample][p][0] = (double) name_to_pattern(i, p);
            naming_response[distribution][level][sample][p][1] = bptt_get_state(i, bptt_t1);
            max_state = bptt_get_state(i, bptt_t1);
        }
    }
}


void bptt_save_naming_response_1(int p, int sample, int level, int distribution)
{
    int i;
    double max_state = 0.0;
    for (i = 0; i < BPTT_N_NAME; i++) {
        if (bptt_get_state(i, bptt_t1) > max_state) {
            naming_response_1[distribution][level][sample][p][0] = (double) bptt_determine_naming_response(p);
            naming_response_1[distribution][level][sample][p][1] = bptt_get_state(i, bptt_t1);
            max_state = bptt_get_state(i, bptt_t1);
        }
    }
}

int bptt_determine_naming_response(int target_pattern)
{
    int i, network_i = -1, p, network_p = -1, pattern_i = -1;
    double max_activation = 0.0;
    for (i = 0; i < BPTT_N_NAME; i++) {
        if (max_activation < bptt_get_state(i, bptt_t1)) {
            network_i = i;
            max_activation = bptt_get_state(i, bptt_t1);
        }



        if (get_pattern_int(target_pattern, i)) {
            pattern_i = i;
        }

    }



    if (network_i == pattern_i) {
        return NAMING_CORRECT;
    }

    for(p = 0; p < P; p++) {
        if (get_pattern(p, network_i)) {
            network_p = p;
        }
    }
// 	    Responses were classified as
// 	    (a) correct when the patient gave the same response typically provided by controls,
// 	    (b) a superordinate error when the patient gave a correct but more general response than that provided by controls,
// 	    (c) a semantic error when the patient gave an incorrect response from the same semantic domain as the correct response,
// 	    (d) a cross-domain error when the patient gave an incorrect response from the wrong semantic domain, and
// 	    (e) an omission error for all remaining errors.

    if ((get_category(network_p) == get_category(target_pattern)) && pattern_general(network_p)) {
        // Same category, and obviously same domain -> semantic error
        return NAMING_SUPERORDINATE;
    }

    if (get_domain(network_p) == get_domain(target_pattern)) {
        // Same category, and obviously same domain -> semantic error
        return NAMING_SEMANTIC;
    }

    if (get_domain(network_p) != get_domain(target_pattern)) {
        // Different category but same domain -> semantic error
        return NAMING_CROSSDOMAIN;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Unable to interpret network response to name input", 1, p);
    return -1;

}
void bptt_save_naming_response_alt(int p, int sample, int level, int distribution)
{
    //find lowest error
    int q;//, r;
    double min_error = 1.0;
    for (q = 0; q < P; q++) {

        if (bptt_jaccard_distance_type(q, VISUAL, NAME) < min_error) {
            naming_response_alt[distribution][level][sample][p][0] = (double) q;
//             naming_response[distribution][level][sample][p][1] = bptt_get_state(pattern_to_name_index(q), bptt_t1);
            min_error =   bptt_jaccard_distance_type(q, VISUAL, NAME);
// 	    printf("p = %i bptt_jaccard_distance_type(q = %i, VISUAL, NAME) = %f %f\n", p, q, bptt_jaccard_distance_type(q, VISUAL, NAME), bptt_jaccard_distance(q, VISUAL));

        }
    }

    int i;
    double max_state = 0.0;
    for (i = 0; i < BPTT_N_NAME; i++) {
        if (bptt_get_state(i, bptt_t1) > max_state) {
// 	naming_response[distribution][level][sample][p][0] = (double) name_to_pattern(i, p);
//             r = name_to_pattern(i, p);
            naming_response_alt[distribution][level][sample][p][1] = bptt_get_state(i, bptt_t1);
            max_state = bptt_get_state(i, bptt_t1);
        }
    }

//     if (q != r) {
//         printf("r = %i, q = %i, error = %f, state = %f\n", r, q, min_error, max_state );
//     }
}

void bptt_save_sorting_response_given_name(int p, int sample, int level, int distribution)
{
    int i, i0 = i0_of(ENCYCLOPEDIC), i1 = i0 + N_DOMAIN_UNIT, i2 = i1 + N_CATEGORY_UNIT;
    double max_state = 0.0;

    for (i = i0; i < i1; i++) { // for all domain units, which are mutually exclusive too
        if (bptt_get_state(i, bptt_t1) > max_state) {
            sorting_response_given_name[distribution][level][sample][p][0] = i - i0;
            max_state = bptt_get_state(i, bptt_t1);
        }
    }
    max_state = 0.0;

    for (i = i1; i < i2; i++) { // for all category units, which are mutually exclusive too
        if (bptt_get_state(i, bptt_t1) > max_state) {
            sorting_response_given_name[distribution][level][sample][p][1] = i - i1;
            max_state = bptt_get_state(i, bptt_t1);
        }
    }
}

void bptt_save_sorting_response_given_visual(int p, int sample, int level, int distribution)
{
    int i, i0 = i0_of(ENCYCLOPEDIC), i1 = i0 + N_DOMAIN_UNIT, i2 = i1 + N_CATEGORY_UNIT;
    double max_state = 0.0;
    for (i = i0; i < i1; i++) { // for all category units, which are mutually exclusive too
        if (bptt_get_state(i, bptt_t1) > max_state) {
            sorting_response_given_visual[distribution][level][sample][p][0] = i - i0;
            max_state = bptt_get_state(i, bptt_t1);
        }
    }
    max_state = 0.0;

    for (i = i1; i < i2; i++) { // for all category units, which are mutually exclusive too
        if (bptt_get_state(i, bptt_t1) > max_state) {
            sorting_response_given_visual[distribution][level][sample][p][1] = i - i1;
            max_state = bptt_get_state(i, bptt_t1);
        }
    }
}

void bptt_save_target_attractor(int p, int sample, int level, int distribution)
{
    int i;
    for (i = 0; i < K; i++) {
        target_attractor[distribution][level][sample][p][i] = bptt_get_state(N+i, bptt_t1);
    }
}

double bptt_distance_between_current_and_target_attractor(int p, int sample, int level, int distribution)
{
    int i;
    double distance = 0.0;
    for (i = 0; i < K; i++) {
        distance += sqr(bptt_get_state(N+i, bptt_t1) - target_attractor[distribution][level][sample][p][i]);
    }
    return sqrt(distance);
}

void bptt_save_word_to_picture_response_target(int p, int sample, int level, int distribution)
{
    distance_target[distribution][level][sample][p] = bptt_distance_between_current_and_target_attractor(p, sample, level, distribution);

}

void bptt_save_word_to_picture_response_distant_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = bptt_distance_between_current_and_target_attractor(p, sample, level, distribution);

    if (distance > distance_target[distribution][level][sample][p]) {
        word_to_picture_distant[distribution][level][sample][p]++;
    }
}

void bptt_save_word_to_picture_response_unrelated_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = bptt_distance_between_current_and_target_attractor(p, sample, level, distribution);
    if (distance > distance_target[distribution][level][sample][p]) {
        word_to_picture_unrelated[distribution][level][sample][p]++;
    }
}

void bptt_save_word_to_picture_response_close_distractor(int p, int q, int sample, int level, int distribution)
{
    double distance = bptt_distance_between_current_and_target_attractor(p, sample, level, distribution);

    if (distance > distance_target[distribution][level][sample][p]) { //if the current semantic state is closer to the target state (reached via name) then the one reached via picture.
        word_to_picture_close[distribution][level][sample][p]++;
    }
}

void bptt_save_drawing_response(int d, int l, int s, int p)
{
    double response;
    int i;

    for (i = 0; i < BPTT_N_VISUAL; i++) {
        response = (bptt_get_state(BPTT_N_NAME+i, bptt_t1) >= 0.25);
        if (get_pattern(p, BPTT_N_NAME+i)) { //if there is a feature that needs to be added
            if (get_pattern(p, BPTT_N_NAME+i) == response) { //if it is included
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

void bptt_save_copying_response(int d, int l, int s, int p)
{
    double response;
    int i;
    for (i = 0; i < BPTT_N_VISUAL; i++) {
        response = (bptt_get_state(BPTT_N_NAME+i, bptt_t1) >= 0.25);
        if (get_pattern(p, BPTT_N_NAME+i)) { //if there is a feature that needs to be added
            if (get_pattern(p, BPTT_N_NAME+i) == response) { //if it is included
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