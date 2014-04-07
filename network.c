#include "network.h"

int network_compute_output_interpretation(int network_type, int error_type, int t) //given the current state of the units, which pattern do they most closely resemble?
{
    int p;
    double error[P_MAX];

    // so mystery pattern q, has been given to network and we need to find which p resembles the output the most, by choosing the lowest error and returning it's p index.
    for(p = 0; p < P; p++) {

        error[p] = network_error_measure(network_type, error_type, p, t);

    }

    double min_error = error[0];
    int min_p = 0;

    for(p = 1; p < P; p++) {
        if (min_error > error[p]) {
            min_error = error[p];
            min_p = p;
        }
    }
    return min_p;

}


int network_compute_output_interpretation_type(int network_type, int error_type, int t, int type_for_error) //given the current state of the units, which pattern do they most closely resemble?
{
    int p;
    double error[P_MAX];

    // so mystery pattern q, has been given to network and we need to find which p resembles the output the most, by choosing the lowest error and returning it's p index.
    for(p = 0; p < P; p++) {
        //error[p] = network_error_measure(network_type, error_type, p, t);
        if ((network_type == BM) && (error_type == RMSD)) {
            error[p] = bm_calculate_rms_error_type(p, t, type_for_error);

        }
        else {
            throw_error(__FILE__,  __LINE__, __FUNCTION__, "Too lazy to implement this for any other network type or error type as never used (identical to bm_compute_output_interpretation_type!", 0);
        }
    }

    double min_error = error[0];
    int min_p = 0;

    for(p = 1; p < P; p++) {
        if (min_error > error[p]) {
            min_error = error[p];
            min_p = p;
        }
    }
    return min_p;


}

// double get_state(int i);
double network_state_get(int network_type, int i)
{

    if (network_type == BPTT) {
        return bptt_get_state(i, bptt_t1);
    }
    else if (network_type == BM) {
        return (double) bm_get_state(i);
    }
    else if (network_type == TYLER_RECURRENT) {
        return tyler_get_state(i, tyler_t1);
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        return tyler_feedforward_get_state(i);
    }
    else if (network_type == PDPTOOL) {
        return pdptool_get_state(i);
    }
//   else {
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
    return 0.0;
//   }
}


// double get_state(int i);
double network_state_timestep_get(int network_type, int i, int t)
{

    if (network_type == BPTT) {
        return bptt_get_state(i, t);
    }
//   else if (network_type == BM) {
//     return (double) bm_get_state(i);
//   }
    else if (network_type == TYLER_RECURRENT) {
        return tyler_get_state(i, t);
    }
//   else if (network_type == PDPTOOL) {
//     return pdptool_get_state(i);
//   }
    //   else {
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type for this function", 1, network_type);
    return 0.0;
    //   }
}


// void network_decay_weights(int network_type)
// {
//     if (network_type == BPTT) {
//         bptt_decay_weights();
//         return;
//     }
//     else if (network_type == BM) {
//         bm_decay_weights();
//         return;
//     }
//     else if (network_type == TYLER_RECURRENT) {
//               tyler_decay_weights();
//       return;
//     }
//     else if (network_type == PDPTOOL) {
// //               pdptool_decay_weights();
//       return;
//     }
//
//     throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
//
// }

void network_randomise_states(int network_type)
{
    if (network_type == BPTT) {
        bptt_randomise_states_all();
        return;
    }
    else if (network_type == BM) {
        bm_randomise_states();
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_randomise_states_all();
        return;
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_randomise_states();
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_randomise_states();
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}


// File Handling
void network_file_save(int network_type, char *filename)
{
    if (network_type == BPTT) {
        bptt_save_file(filename);
        return;
    }
    else if (network_type == BM) {
        bm_save_weights(filename);
        return;
    }
    else if (network_type == PDPTOOL) {
        //         pdptool_file_save(filename); //ALERT
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_save_file(filename);
        return;
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_save_file(filename);
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}

void network_open_file(int network_type, char *filename)
{
    if (network_type == BPTT) {
        if (bptt_open_file(filename) == 0) {
            pdptool_load_weights_trained_on_smm_patterns(filename);
        }
        return;
    }
    else if (network_type == BM) {
        bm_load_weights(filename);
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_load_weights(filename);
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_open_file(filename);
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_feedforward_open_file(filename);
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}



// void propagate(int p, int s);
// double propagate_timestep(int p, int s, int t);
// bptt.c

// void network_init(int network_type);
void network_adjust_weights(int network_type)
{
    if (network_type == BPTT) {
        bptt_adjust_weights();
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_adjust_weights();
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_feedforward_adjust_weights();
        return;
    }
    else if (network_type == BM) {
//         bm_adjust_weights(); //ALERT
        return;

    }
    else if (network_type == PDPTOOL) {
//         pdptool_adjust_weights; //ALERT
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);


}

double network_error_measure_noise(int network_type, int error_type, int p, int t, int noise, int n)
{
    if (network_type == BPTT) {
        if (error_type == JACCARD) {
            return bptt_jaccard_distance(p, t);
        }
        else if (error_type == RMSD) {
            return bptt_calculate_rms_error(p, t);
        }
        else if (error_type == SORENSEN) {
            return bptt_sorensen_distance(p, t);
        }
        else if (error_type == RSS) {
//	  return bptt_calculate_rss_error(p, t);
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Error type not defined for network type", 2, error_type, network_type);

        }
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid error type", 1, error_type);
        return 0;
    }
    else if (network_type == TYLER_RECURRENT) {
        if (error_type == JACCARD) {
            return tyler_jaccard_distance(p);
        }
        else if (error_type == RMSD) {
            return tyler_calculate_rms_error_type_noise(p, t, noise, n);
        }
        else if (error_type == SORENSEN) {
            return tyler_sorensen_distance(p, t);
        }
        else if (error_type == RSS) {
            return tyler_calculate_rss_error_type_noise(p, t, noise, n);
//             throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Error type not defined for network type", 2, error_type, network_type);

        }
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid error type", 1, error_type);
        return 0;
    }
    else if (network_type == TYLER_FEEDFORWARD) {

        if (error_type == JACCARD) {
            return tyler_feedforward_jaccard_distance(p);
        }
        else if (error_type == RMSD) {
//             printf("beep\n");
            return tyler_feedforward_calculate_rms_error_type_noise(p, t, noise, n);

        }
        else if (error_type == SORENSEN) {
            return tyler_feedforward_sorensen_distance(p, t);
        }
        else if (error_type == RSS) {
            return tyler_feedforward_calculate_rss_error_type_noise(p, t, noise, n);
        }
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid error type", 1, error_type);
        return 0;
    }
    else if (network_type == BM) {
        if (error_type == JACCARD) {
            return bm_jaccard_distance(p, t);
        }
        else if (error_type == RMSD) {
            return bm_calculate_rms_error(p, t);
        }
        else if (error_type == SORENSEN) {
            return bm_sorensen_distance(p, t);
        }
        else if (error_type == RSS) {
// 	  return bm_calculate_rss_error(p, t);
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Error type not defined for network type", 2, error_type, network_type);

        }
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid error type", 1, error_type);
        return 0;
    }
    else if (network_type == PDPTOOL) {
        if (error_type == 0) {
            return pdptool_jaccard_distance(p);
        }
        else if (error_type == 1) {
            return pdptool_calculate_rms_error(p);
        }
        else if (error_type == SORENSEN) {
            // 	  return pdptool_sorensen_distance(p, t);
        }
        else if (error_type == RSS) {
            //return pdptool_calculate_rss_error(p);
            throw_warning(__FILE__,  __LINE__, __FUNCTION__, "Error type not defined for network type", 2, error_type, network_type);

        }
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid error type", 1, error_type);
        return 0;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
    return 0;
}


double network_error_measure(int network_type, int error_type, int p, int t) {
    return network_error_measure_noise(network_type,  error_type,  p,  t,0 ,0);

}



void network_clamp(int network_type, int p, int s)
{
    if (network_type == BPTT) {
        bptt_clamp(p, s, bptt_t1);
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_clamp(p, s, tyler_t1);
        return;
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_clamp(p, s);
        return;
    }
    else if (network_type == BM) {
        bm_clamp(p, bm_network_state_to_unit_type(s));
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_clamp(p, s);
        return;
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}
void network_clamp_now_noise(int network_type, int p, int s, int noise, int n)
{
    if (network_type == BPTT) {
        bptt_clamp_now(p, s, bptt_t1);
        return;
    }
    else if (network_type == BM) {
        bm_clamp(p, bm_network_state_to_unit_type(s));
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_clamp_now(p, s);
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_clamp_now_noise(p, s, tyler_t1, noise, n);
        return;
    }

    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_clamp_now_noise(p, s, noise, n);
        return;
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}

void network_weights_init(int network_type)
{
    if (network_type == BPTT) {
        bptt_init_weights();
        return;
    }
    else if (network_type == BM) {
        bm_init_weights();
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_weights_init();
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_init_weights();
        return;
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_init_weights();
        return;
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}

int network_is_state_clamped(int network_type, int i)
{
    if (network_type == BPTT) {
        return bptt_is_clamped(i, bptt_t1);
    }
    else if (network_type == BM) {
        return bm_clamped_index(i);
    }
    else if (network_type == PDPTOOL) {
        return pdptool_is_clamped(i);
    }
    else if (network_type == TYLER_RECURRENT) {
        return tyler_is_clamped(i, tyler_t1);
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        return tyler_feedforward_is_clamped(i);
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
    return 0;

}
void network_unclamp(int network_type)
{
    if (network_type == BPTT) {
        bptt_unclamp_all();
        return;

    }
    else if (network_type == BM) {
        bm_unclamp();
        return;



    }
    else if (network_type == PDPTOOL) {
        pdptool_unclamp();
        return;

    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_unclamp_all();
        return;

    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_unclamp();
        return;

    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);


}


void network_init(int network_type)
{
    if (network_type == BPTT) {
        bptt_init();
        return;

    }
    else if (network_type == BM) {
        bm_init();
        return;



    }
    else if (network_type == PDPTOOL) {
        pdptool_init();
        return;

    }

    else if (network_type == TYLER_RECURRENT) {
        tyler_init();

        return;

    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_init();
        return;

    }
    network_clamp_name(network_type, 0);
    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);


}

double network_settle_step_(int network_type, int t, int noise, int n)
// noise is flag for whether to return a
{
    if (network_type == BPTT) {
        return bptt_settle_step(t);
    }
    else if (network_type == TYLER_RECURRENT) {
        return tyler_settle_step_noise(t, noise, n);
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        return tyler_feedforward_propagate_type_noise(t, noise, n);
    }
    else if (network_type == BM) {
//         bm_clamp(p, s); //ALERT

    }
    else if (network_type == PDPTOOL) {
        return pdptool_settle_step(t);
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
    return 0.0;

}

double network_settle_step(int network_type, int t)
// noise is flag for whether to return a
{
    return network_settle_step_(network_type, t, 0, 0);

}


int network_clamped_pattern_get(int network_type)
{
    if (network_type == BPTT) {
        return bptt_get_clamped_pattern();
    }
    else if (network_type == TYLER_RECURRENT) {
        return tyler_get_clamped_pattern();
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        return tyler_feedforward_get_clamped_pattern();
    }
    else if (network_type == BM) {
        return bm_get_clamped_pattern();
    }
    else if (network_type == PDPTOOL) {
        return pdptool_get_clamped_pattern();
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
    return 0;
}

int network_clamped_stage_get(int network_type)
{
    if (network_type == BPTT) {
        return bptt_clamped_stage_get();
    }
    else if (network_type == TYLER_RECURRENT) {
        return tyler_clamped_stage_get();
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        return tyler_feedforward_clamped_stage_get();
    }
    else if (network_type == BM) {
        return bm_clamped_stage_get();
    }
    else if (network_type == PDPTOOL) {
        return pdptool_clamped_stage_get();
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
    return 0;
}

int network_clamped_type_get(int network_type)
{
    if (network_type == BPTT) {
        return bptt_clamped_type_get();
    }
    else if (network_type == TYLER_RECURRENT) {
        return tyler_clamped_type_get();
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        return tyler_feedforward_clamped_type_get();
    }
    else if (network_type == BM) {
        return bm_clamped_type_get();
    }
    else if (network_type == PDPTOOL) {
        return pdptool_clamped_type_get();
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
    return 0;
}

// double network_pattern_get(int network_type, int p, int i)
// {
//   if (network_type == BPTT) {
//         return pattern_get(p, i);
//   }
//   else if (network_type == BM) {
//     return pattern_get(p, i);
//   }
//   else if (network_type == PDPTOOL) {
// //     printf("pdptool_get_pattern = %f\n", pdptool_get_pattern(p, pdptool_stage_to_type(pdptool_get_clamped_stage()), i));
//     return pdptool_get_pattern(p, pdptool_stage_to_type(pdptool_get_clamped_stage()), i);
//
//   }
//   throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
//   return -1;
//
// }
void network_lesion_random_weight(int network_type)
{
    if (network_type == BPTT) {
        bptt_lesion_random_weight();
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_lesion_random_weight();
        return;
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_lesion_random_weight();
        return;
    }
    else if (network_type == BM) {
        bm_lesion_random_weight();
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_lesion_random_weight();
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
}


void network_lesion_weights(int network_type, double percentage)
{
    if (network_type == BPTT) {
        bptt_lesion_weights(percentage);
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_lesion_weights(percentage);
        return;
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_lesion_weights(percentage);
        return;
    }
    else if (network_type == BM) {
        bm_lesion_weights(percentage);
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_lesion_weights(percentage);
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}


// void network_lesion_random_weight(int network_type);
// void network_clamp_visual(int network_type, int p);
void network_clamp_name(int network_type, int p)
{

    if (network_type == BPTT) {
        bptt_clamp_name(p, bptt_t1);
        return;
    }
    else if (network_type == BM) {
        bm_clamp_name(p);
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network function", 1, network_type);
        return;
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network function", 1, network_type);
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_clamp_name(p);
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);
}


void network_backup_weights(int network_type)
{
    if (network_type == BPTT) {
        bptt_backup_weights();
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_backup_weights();
        return;
    }
    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_backup_weights();
        return;
    }
    else if (network_type == BM) {
        bm_backup_weights();
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_backup_weights();
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}
void network_unlesion_weights(int network_type)
{
    if (network_type == BPTT) {
        bptt_unlesion_weights();
        return;
    }
    else if (network_type == BM) {
        bm_unlesion_weights();
        return;
    }
    else if (network_type == TYLER_RECURRENT) {
        tyler_unlesion_weights();
        return;
    }    else if (network_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_unlesion_weights();
        return;
    }
    else if (network_type == PDPTOOL) {
        pdptool_unlesion_weights();
        return;
    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, network_type);

}

/** Hierarchical Clustering Algorithms **/

static double attractor_matrix[P_MAX*10*10*10][K_MAX]; // matrix of the raw vectors to be used further down in calculating their distances.
static int n_attractor = 0; // this is dimension that contains the patterns.

void network_save_current_attractor_for_clustering(int network_type) // should be called from within a loop in the main GUI that settles/anneals the network and then allows for this function to collect the internal states.
{
    int j;
    for (j = 0; j < K; j++) {
        attractor_matrix[n_attractor][j] = network_state_get(network_type, N + j);
    }
    n_attractor++;
}


static double settling_attractor_matrix[P_MAX][300][K_MAX]; // matrix of the raw vectors to be used further down in calculating their distances.
static int t_attractor[P_MAX]; // this is dimension that contains the patterns.


void network_save_current_settling_attractor_for_clustering(int network_type, int p)
{
    int j;
    for (j = 0; j < K; j++) {
        settling_attractor_matrix[p][t_attractor[p]][j] = network_state_get(network_type, N + j);
    }
    t_attractor[p]++;
}


void init_attractor_matrix_for_clustering(void)
{
    n_attractor = 0;
    int p;
    for (p = 0; p < P; p++) {
        t_attractor[p] = 0;
    }
}

void print_attractor_matrix(void)
{
    int i, p;
    for (p = 0; p < n_attractor; p++) {
        printf("attractor[%i] = ", p);
        for (i = 0; i < K; i++) {

            printf("%f, ", attractor_matrix[p][i]);

        }
        printf("\n");
    }

}


///////////////////////////////////////////////////////////////////////////


// #define N_CLUSTER (P_MAX * P_MAX)
//
//
// static int n_cluster = 0;
//
// static double euclidean_distance_between_attractors(int p, int q) // calculate distance between two raw vectors (attractors), only called from create_proximity_matrix().
// {
//     double distance = 0.0;
//     int i;
//     for (i = 0; i < K; i++) {
//         distance += sqr(attractor_matrix[p][i] - attractor_matrix[q][i]);
//     }
//     return sqrt(distance);
// }



// double proximity_matrix[N_CLUSTER * (int) (N_CLUSTER / 2.0)]; // distances between each vector (attractor) pair, and after first update distances between each cluster.
//
// void clustering_init(void)
// {
//     n_attractor = 0;
//     int p, i;
//     for (p = 0; p < P_MAX; p++) {
//         for (i = 0; i < K_MAX; i++) {
//             attractor_matrix[p][i] = 0.0;
//         }
//     }
//
//     for(i = 0; i < N_CLUSTER * (int) (N_CLUSTER / 2.0); i++) {
//         proximity_matrix[i] = -66.6;
//     }
// }
//

// maps i, j onto proximity matrix
// static int map(int i, int j, int n)
// {
//
//     if (i > j)
//     {
//         // i must always be 1 or more less than j
//         int temp;
//         temp = i;
//         i = j;
//         j = temp;
//     }
//
//     if ((i < n-1) && (j < n) && (i != j))
//     {
//         return n + j - 1 + i * (n - i/2.0 - 1.5);
//     }
//
//     throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid proximity matrix indices", 2, i, j);
//
//     return -1;
//
// }
//
// static void create_proximity_matrix(void) // populate the matrix of distances between each attractor pair, only called from a clustering function, e.g., complete_linkage_clustering().
// {
//     int j = 0, i;
//     n_cluster = n_attractor;
//
//     while (j < (n_attractor-1)) {
//         for (i = j+1; i < n_attractor; i++) {
//             proximity_matrix[map(j, i, n_cluster)] = euclidean_distance_between_attractors(j, i);
//         }
//         j++;
//         // e.g., d(0, 1), d(0, 2), ..., d(1, 2), ..., d(n_attractor-1, n_attractor).
//     }
// }
//
// void print_proximity_matrix(void)
// {
//     int j = 0, i;
//     while (j < (n_cluster-1)) {
//         for (i = j+1; i < n_cluster; i++) {
//             printf("proximity[%i][%i] = %f \n", j, i, proximity_matrix[map(j, i, n_cluster)]);
//         }
//         j++;
//         if (j==2) break;
// //     printf("\n");
//     }
// }
//

// Complete-linkage Clustering (Naive version)
// void complete_linkage_clustering(void) // it doesn't matter what the network type is
// {
//     create_proximity_matrix();
//
//     int i, j = 0;
//
//     int level[N_CLUSTER]; // clustering level
//     int m = 0; // sequence number
//     double max_distance = 0.0;
//     int max_i = -1, max_j = -1;
//     printf("max distance = %f, j = %d, i = %d\n", max_distance, max_j, max_i);
//
//     double old_proximity_matrix[N_CLUSTER];
//     printf("max distance = %f, j = %d, i = %d\n", max_distance, max_j, max_i);
//
//     level[0] = 0.0;
//
//     // big loop
//
//     max_distance = 0.0;
//     printf("max distance = %f, j = %d, i = %d\n", max_distance, max_j, max_i);
//
//     // find which clusters to merge
//     while (j < (n_cluster-1)) {
//
//         for (i = j+1; i < n_cluster; i++) {
//
//             if (proximity_matrix[map(j, i, n_cluster)] != -1.0) { // if not already clustered
//                 if (max_distance != fmax(proximity_matrix[map(j, i, n_cluster)], max_distance)) { // update running maximum distance
//                     max_distance = proximity_matrix[map(j, i, n_cluster)];
//                     max_j = j; //update maximum i, and j.
//                     max_i = i;
//                 }
//             }
//         }
//         j++;
//     }
//     m++;
//     level[m] = max_distance;
//
//     printf("max distance = %f, j = %d, i = %d\n", max_distance, max_j, max_i);
//
//
// //   printf("proximity[%i][%i] = %f \n", j, i, proximity_matrix[map(j, i, n_cluster)]);
//
//
//     j = 0;
//     while (j < (n_cluster-1)) {
//         for (i = j+1; i < n_cluster; i++) {
//
//             if ((max_i == i) || (max_j == i) || (max_i == j) || (max_j == j)) { // if we have come across an old cluster, a distance in other words from/to max_i or max_j, we must remove it
//
//                 proximity_matrix[map(j, i, n_cluster)] = -1.0;
//
//             }
//
//             old_proximity_matrix[map(j, i, n_cluster)] = proximity_matrix[map(j, i, n_cluster)];
//
//         }
//         j++;
//     }
//     printf("max distance = %f, j = %d, i = %d\n", max_distance, max_j, max_i);
//
//     n_cluster++; //this causes basically one extra row and one extra column in the matrix, but because it's an orthogonal matrix only half the matrix plus the main diagonal are needed and thus only those values are saved, using the map function to access them.
//
//     j = 0;
//
//     while (j < (n_cluster-1)) {
//         for (i = j+1; i < n_cluster; i++) {
//
//             printf("%d\n", __LINE__);
// //       if (old_proximity_matrix[map(j, i, n_cluster-1)] == -1.0) {
// // 	proximity_matrix[map(j, i, n_cluster)] = -1.0;
// //       }
// //       else
//             {
//                 if ((i < n_cluster-1) && (j < n_cluster-1)) {
//                     printf("%d\n", __LINE__);
//
//                     proximity_matrix[map(j, i, n_cluster)] = old_proximity_matrix[map(j, i, n_cluster-1)];
//                     printf("%d\n", __LINE__);
//
//                 }
//
//                 if (j == n_cluster-1) { // this is the new cluster, so it needs its distances to all others computed
//                     printf("%d\n", __LINE__);
//
//                     proximity_matrix[map(n_cluster-1, i, n_cluster)] = fmax(old_proximity_matrix[map(max_i, i, n_cluster-1)], old_proximity_matrix[map(max_j, i, n_cluster-1)]);
//                     printf("%d\n", __LINE__);
//
//                 }
//
//
//
//                 if (i == n_cluster-1) {// same as above
//                     printf("%d\n", __LINE__);
//
//                     proximity_matrix[map(j, n_cluster-1, n_cluster)] = fmax(old_proximity_matrix[map(max_i, j, n_cluster-1)], old_proximity_matrix[map(max_j, j, n_cluster-1)]);
//                     printf("%d\n", __LINE__);
//
//                 }
//             }
//
//         }
//         printf("%d\n", __LINE__);
//
//         j++;
//         printf("%d\n", __LINE__);
//
//     }
//
//
//
// // if
//     // create new proximity_matrix, delete stuff relating to old clusters
//
//
//     /*
//
//
//
//         if (proximity_matrix[map(j, i)] != -1.0) { // if not already clustered
//
//
//
//         }
//       }
//       j++;
//     }*/
//
//     //    print_proximity_matrix();
//
//     // remove just merged clusters
// //   while (j < (n_cluster-1)) {
// //     for (i = j+1; i < n_cluster; i++) {
// //       if ((j == max_j) || (i == max_i) || (j == max_i) || (i == max_j)) { // if not already clustered
// // 	  proximity_matrix[j][i] = -1.0;
// //       }
// //     }
// //   }
//
//
//
//
//
//
// }


void network_save_attractors_matlab(int network_type, char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int a, j;
    FILE *fp = NULL;
    fp = fopen(filename, "wt");
    printf("%s\n", filename);
    fprintf(fp, "X = [");
    for (a = 0; a < n_attractor; a++) {
        for (j = 0; j < K; j++) { // level of leasioning
            fprintf(fp, "%f ", attractor_matrix[a][j]);
        }
        fprintf(fp, ";\n");
    }
    fprintf(fp, "];\n");

    int t, p;

    fprintf(fp, "X_settling = [");
    for (p = 0; p < P; p++) {
        for (t = 0; t < t_attractor[p]; t++) { // level of leasioning
            for (j = 0; j < K; j++) { // level of leasioning
                fprintf(fp, "%f ", settling_attractor_matrix[p][t][j]);
            }
            fprintf(fp, ";\n");
        }
    }
    fprintf(fp, "];\n");


    fprintf(fp, "X_settling_pattern = [");
    for (p = 0; p < P; p++) {
        for (t = 0; t < t_attractor[p]; t++) { // level of leasioning
//             for (j = 0; j < K; j++) { // level of leasioning
            fprintf(fp, "%d ", p);
//             }
            fprintf(fp, ";\n");
        }
    }
    fprintf(fp, "];\n");



//   fprintf(fp, "\nEOF\n");
    if (fp) {
        fclose(fp);
    }

}




int network_file_open(int network_type, char* filename) {
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//     printf("%s: filename: %s\n", __FUNCTION__, filename);

    if (network_type == BPTT) {

        if (bptt_open_file(filename) == 0) {
            pdptool_load_weights_trained_on_smm_patterns(filename);
        }
        return;


    }
    else if (network_type == BM) {

        return bm_load_weights(filename);



    }
    else if (network_type == PDPTOOL) {

        return pdptool_load_weights(filename);



    }
    else if (network_type == TYLER_RECURRENT) {
        printf("beeep!\n\n\n\n");

        return tyler_load_weights(filename);



    }

    else if (network_type == TYLER_FEEDFORWARD) {
        return tyler_feedforward_load_weights(filename);



    }

    return 0;


}

int network_get_iterations_so_far(int network_type)
{
//     printf("%s: loop: net_type: %s \n", __FUNCTION__, network_type_get_string(network_type));


    if (network_type == BPTT) {


        return bptt_get_iterations_so_far();



    }
    else if (network_type == TYLER_RECURRENT) {

        return tyler_get_iterations_so_far();

    }
    else if (network_type == TYLER_FEEDFORWARD) {

        return tyler_feedforward_get_iterations_so_far();



    }    else if (network_type == BM) {

        return bm_get_iterations_so_far();



    }
    else if (network_type == PDPTOOL) {

        return 0;



    }
    return 0;
}


char *network_type_get_string(int net_type)
{
    if (net_type == BPTT) {
        return "BPTT";
    }
    else if (net_type == BM) {
        return "BM";
    }
    else if (net_type == PDPTOOL) {
        return "PDPTOOL";
    }
    else if (net_type == TYLER_RECURRENT) {
        return "TYLER_RECURRENT";
    }
    else if (net_type == TYLER_FEEDFORWARD) {
        return "TYLER_FEEDFORWARD";
    }
    else {
        return "Error!";
    }

}



int network_get_default_iterations(int net_type)
{
    if (net_type == BPTT) {
        return BPTT_C;
    }
    else if (net_type == BM) {
        return BM_I;
    }
    else if (net_type == PDPTOOL) {
        return BM_I;
    }
    else if (net_type == TYLER_RECURRENT) {
        return TYLER_C;
    }
    else if (net_type == TYLER_FEEDFORWARD) {
        return TYLER_C;
    }
    else {
        throw_error(__FILE__,  __LINE__, __FUNCTION__, "Invalid network type", 1, net_type);
        return -1;
    }

}


int network_n_timesteps_get(int network_type)
{
//   printf("%s: loop: net_type: %s \n", __FUNCTION__, network_type_get_string(network_type));


    if (network_type == BPTT) {


        return BPTT_T;



    }
    else if (network_type == BM) {

        return 0;



    }
    else if (network_type == PDPTOOL) {

        return PDPTOOL_T;



    }
    else if (network_type == TYLER_RECURRENT) {

        return TYLER_T;



    }
    return 0;
}



int network_N_get(int network_type)
{
    //   printf("%s: loop: net_type: %s \n", __FUNCTION__, network_type_get_string(network_type));


    if (network_type == BPTT) {


        return BPTT_N;



    }
    else if (network_type == BM) {

        return BM_N;



    }
    else if (network_type == PDPTOOL) {

        return PDPTOOL_N;



    }
    else if (network_type == TYLER_RECURRENT) {

        return TYLER_N;



    }    else if (network_type == TYLER_FEEDFORWARD) {

        return TYLER_FEEDFORWARD_N;



    }
    return 0;

}



int network_input_units_n_get(int network_type)
{
    //   printf("%s: loop: net_type: %s \n", __FUNCTION__, network_type_get_string(network_type));


    if (network_type == BPTT) {


        return BPTT_N;



    }
    else if (network_type == BM) {

        return BM_N;



    }
    else if (network_type == PDPTOOL) {

        return PDPTOOL_N;



    }
    else if (network_type == TYLER_RECURRENT) {

        return TYLER_N;



    }    else if (network_type == TYLER_FEEDFORWARD) {

        return TYLER_FEEDFORWARD_IN;



    }
    return 0;
}

int network_UNITS_get(int network_type)
{
    //   printf("%s: loop: net_type: %s \n", __FUNCTION__, network_type_get_string(network_type));


    if (network_type == BPTT) {


        return BPTT_UNITS;



    }
    else if (network_type == BM) {

        return BM_UNITS;



    }
    else if (network_type == PDPTOOL) {

        return PDPTOOL_UNITS;



    }
    else if (network_type == TYLER_RECURRENT) {

        return TYLER_UNITS;



    }

    else if (network_type == TYLER_FEEDFORWARD) {

        return TYLER_FEEDFORWARD_UNITS;



    }
    return 0;
}

void network_get_weight_stats(int network_type, double *array)

{

    if (network_type == BPTT) {
        bptt_get_weight_stats(array);



    }
    else if (network_type == BM) {
        bm_get_weight_stats(array);



    } /*
 else if (network_type == PDPTOOL) {
     pdptool_print_weight_stats();



 }
 else if (network_type == TYLER_RECURRENT) {
    tyler_print_weight_stats();



 }

 else if (network_type == TYLER_FEEDFORWARD) {

   tyler_feedforward_print_weight_stats();



 }           */
}


/*
void network_print_weight_stats(int network_type)

{

if (network_type == BPTT) {
bptt_print_weight_stats();



}
else if (network_type == BM) {
bm_print_weight_stats();



}
else if (network_type == PDPTOOL) {
pdptool_print_weight_stats();



}
else if (network_type == TYLER_RECURRENT) {
tyler_print_weight_stats();



}

else if (network_type == TYLER_FEEDFORWARD) {

tyler_feedforward_print_weight_stats();



}
}           */