#include "bptt.h"

// initialise network
void bptt_init(void)
{
    UNITS = BPTT_UNITS;
    PAIRS = BPTT_PAIRS;
    BPTT_T_CLAMPED = (3 * BPTT_STEP);
    p_min = 0;
    p_max = 6;
    P = p_max - p_min; //in case of bugs do a search for p to P as obviously might not start from zero...
    N = BPTT_N;
    K = BPTT_K;
//     init_patterns(BPTT);

    init_random();

    bptt_init_weights();
    bptt_init_deltas();
    bptt_init_delta_weights();
    bptt_init_targets_clamped();

    bptt_unclamp_all();
    bptt_randomise_states_all();
// 	p_min = 0;
// 	p_max = P_MAX;
// 	P = p_max - p_min; //in case of bugs do a search for p to P as obviously might not start from zero...
    BPTT_C = 5000;
    bptt_learning_rate = BPTT_LEARNING_RATE;
    bptt_decay_rate = 1.0 - (bptt_learning_rate / (3.0 * (double) P));
//     activation_threshold = ACTIVATION_THRESHOLD;
// 	step_number = 0;

// 	pattern_number = 0;

// 	stage = STAGE_NAME;

    bptt_iterations_so_far = 0;

    deltas_updated = 0;
    errors_updated = 0;

//     clamped_pattern = -1;
//     clamped_stage = -1;


//     open_patterns("base_names.pat");

// 	open_patterns_filename = "one.pat";


// 	open_patterns(open_patterns_filename);


}



// create an array with indices for accessing every pattern in a random order
// void randomise_patterns(void)
// {
//       #if PRINT_FUNCTION
//     printf("%s\n", __FUNCTION__);
//     #endif
// 	int r, i, temp;
//
// 	for (i = 0; i < P; i++)
// 	{
// 		random_pattern[i] = i;
// 	}
//  	for (i = 0; i < P; i++)
//  	{
// 		r = (i + random_int(0, P)) % P;
// 		temp = random_pattern[i];
// 		random_pattern[i] = random_pattern[r];
// 		random_pattern[r] = temp;
// 	}
// }

