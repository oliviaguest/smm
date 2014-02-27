#include "tyler.h"

// initialise network
void tyler_init(void)
{


    UNITS = TYLER_UNITS;
    PAIRS = TYLER_PAIRS;
    TYLER_T_CLAMPED = (3 * TYLER_STEP);
    p_min = 0;
    p_max = 6;
    P = p_max - p_min; //in case of bugs do a search for p to P as obviously might not start from zero...
    N = TYLER_N;
    K = TYLER_K;
    //     init_patterns(TYLER);
    tyler_open_patterns("tyler.pat");

    init_random();

    tyler_init_weights();
    tyler_init_deltas();
    tyler_init_delta_weights();
    tyler_unclamp_all();
    tyler_randomise_states_all();
    // 	p_min = 0;
    // 	p_max = P_MAX;
    // 	P = p_max - p_min; //in case of bugs do a search for p to P as obviously might not start from zero...
    TYLER_C = 5000;
    tyler_learning_rate = TYLER_LEARNING_RATE;
    tyler_decay_rate = 1.0 - (tyler_learning_rate / (3.0 * (double) P));
    //     activation_threshold = ACTIVATION_THRESHOLD;
    // 	step_number = 0;

    // 	pattern_number = 0;

    // 	stage = STAGE_NAME;

    tyler_iterations_so_far = 0;

    deltas_updated = 0;
    errors_updated = 0;

    //     clamped_pattern = -1;
    //     clamped_stage = -1;



    // 	open_patterns_filename = "one.pat";


    // 	open_patterns(open_patterns_filename);

    tyler_noisy_patterns_init();

}// initialise network
void tyler_feedforward_init(void)
{


    UNITS = TYLER_FEEDFORWARD_UNITS;
    PAIRS = TYLER_FEEDFORWARD_PAIRS;
    TYLER_T_CLAMPED = 0;
    p_min = 0;
    p_max = 6;
    P = p_max - p_min; //in case of bugs do a search for p to P as obviously might not start from zero...
    N = TYLER_FEEDFORWARD_IN+TYLER_FEEDFORWARD_OUT;
    K = TYLER_FEEDFORWARD_HIDDEN;
    //     init_patterns(TYLER);
    tyler_open_patterns("tyler.pat");

    init_random();

    tyler_feedforward_init_weights();
    tyler_feedforward_init_prev_weights();
    tyler_feedforward_init_delta_weights();
    tyler_feedforward_unclamp();
    tyler_feedforward_randomise_states();
    // 	p_min = 0;
    // 	p_max = P_MAX;
    // 	P = p_max - p_min; //in case of bugs do a search for p to P as obviously might not start from zero...
    TYLER_C = 4000;
    tyler_feedforward_learning_rate = TYLER_FEEDFORWARD_LEARNING_RATE;
    tyler_feedforward_momentum = TYLER_FEEDFORWARD_MOMENTUM;
//   tyler_decay_rate = 1.0 - (tyler_learning_rate / (3.0 * (double) P));
    //     activation_threshold = ACTIVATION_THRESHOLD;
    // 	step_number = 0;

    // 	pattern_number = 0;

    // 	stage = STAGE_NAME;

    tyler_iterations_so_far = 0;


    tyler_feedforward_learned_biases = 1;

    //     clamped_pattern = -1;
    //     clamped_stage = -1;

    tyler_feedforward_prev_weights_initialised = 0;


    // 	open_patterns_filename = "one.pat";


    // 	open_patterns(open_patterns_filename);
    tyler_noisy_patterns_init();


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

