#include "bm.h"

// initialise network
void bm_init(void)
{
    UNITS = BM_UNITS;
    PAIRS = BM_PAIRS;
    p_min = 0;
    p_max = 6;
    P = p_max - p_min; //in case of bugs do a search for p to P as obviously might not start from zero...
    N = BM_N;
    K = BM_K;
//     init_patterns(BM);

    bm_init_random();
    bm_init_states();
    bm_init_counts();
    bm_init_weights();

    bm_weights_updated = 1;
    bm_biases_updated = 1;
    bm_training = 0;
//     bm_clamped_name = 0;
//     bm_clamped_visual = 0;
//     bm_clamped_perceptual = 0;
//     bm_clamped_functional = 0;
//     bm_clamped_encyclopedic = 0;

    bm_type_clamped = -1;
    bm_decay_rate = BM_DECAY_RATE;
    bm_r = BM_R_MAX;

    BM_C = BM_C_0;
//   P = 1;
    BM_I = 5000;


    init_random();





    bm_iterations_so_far = 0;
//     open_patterns("base_names.pat");

}

// for a random unit's index i (so it could be for a hidden or visible unit) calculate its input
// i's range: 0 - UNITS
double bm_calculate_input(int i)
{
    int j, v;
    double sum = (-1.0) * bm_get_bias(i);

    if (bm_valid_index(i, BM_N)) // i is visible
    {
        for (j = 0; j < BM_K; j++) // for all hidden units
        {
            sum += bm_get_weight(bm_map_vh(i, j)) * bm_get_hidden(j);
        }
        return sum;
    }

    i -= N; // to make it in the range of hidden

    if (bm_valid_index(i, BM_K)) // i is hidden
    {
        for (j = 0; j < BM_K; j++) // for hidden(i) <-> hidden(j)
        {
            if (j != i)
            {
                sum += bm_get_weight(bm_map_hh(i, j)) * bm_get_hidden(j);
            }
        }
        for (v = 0; v < BM_N; v++) // for hidden(i) <-> visible(v)
        {
            sum += bm_get_weight(bm_map_vh(v, i)) * bm_get_visible(v);
        }
        return sum;
    }

    bm_error("bm.c", "calculate_input", "Invalid array index for state", i + BM_N);
    return -1;
}

// input t = temperature
// type = unit type
void bm_flip_units(double temperature)//, int t)
{
    int i;
    double delta_e = 0.0, prob = 0.0;

    bm_randomise_indices();

    for (i = 0; i < BM_UNITS; i++)
    {
//         if (bm_random_index[i] != BM_CLAMPED) // to avoid updating clamped units
        if     (!bm_clamped_index(bm_random_index[i]))
        {

            delta_e = bm_calculate_input(bm_random_index[i]);

            // 			if (delta_e)
// 			{
            prob = 1.0 / (double) (1.0 + exp((-1)*delta_e / (double) temperature));

            if (random_double(0.0, 1.0) <= prob)
            {
                bm_put_state(bm_random_index[i], BM_ON);
            }
            else
            {
                bm_put_state(bm_random_index[i], BM_OFF);
            }
            // 			}
        }

    }
}

// input t = temperature
// type = unit type
double bm_flip_units_delta(double temperature)//, int t)
{
    int i;
    double delta_e = 0.0, prob = 0.0, d = 0.0;
    int prev_state;
    bm_randomise_indices();

    for (i = 0; i < BM_UNITS; i++)
    {
        //         if (bm_random_index[i] != BM_CLAMPED) // to avoid updating clamped units
        if     (!bm_clamped_index(bm_random_index[i]))
        {

            delta_e = bm_calculate_input(bm_random_index[i]);

            // 			if (delta_e)
            // 			{
            prob = 1.0 / (double) (1.0 + exp((-1)*delta_e / (double) temperature));

            prev_state = bm_get_state(bm_random_index[i]);

            if (random_double(0.0, 1.0) <= prob)
            {
                bm_put_state(bm_random_index[i], BM_ON);
            }
            else
            {
                bm_put_state(bm_random_index[i], BM_OFF);
            }
            // 			}

            if (prev_state != bm_get_state(bm_random_index[i])) {
                d++;
            }
        }

    }
    d /= (double) BM_UNITS;
//   printf("d = %f\n", d);

    return d;
}

// input t = type of units we are collecting stats for
void bm_collect_count(int t) //C for iterations
{
    // run the network C times at low temperature (t = 1?) and count
    // the number of times each pair of units is on
    int i, j, c;

    if (bm_valid_count_type(t)) {
        for (c = 0; c < BM_C; c++)
        {
            bm_flip_units(1.0);//, t);

            // units
            for (j = 0; j < BM_K; j++) // for every hidden unit
            {
                // visible <-> hidden
                for (i = 0; i < N; i++) // for every visible unit
                {

                    if ((bm_get_visible(i) == BM_ON) && (bm_get_hidden(j) == BM_ON)) // if both active
                    {
                        bm_inc_count(bm_map_vh(i, j), 1, t);
                    }

                }

                // hidden <-> hidden
                for (i = j + 1; i < BM_K; i++) // for every hidden unit
                {

                    if ((bm_get_hidden(j) == BM_ON) && (bm_get_hidden(i) == BM_ON)) // if both active
                    {
                        bm_inc_count(bm_map_hh(i, j), 1, t);
                    }
                }
            }

            // bias of every unit
            for (i = 0; i < BM_UNITS; i++)
            {
                if (bm_get_state(i) == BM_ON)
                {
                    bm_inc_count(BM_PAIRS + i, 1, t);
                }
            }
        }
    }
    else {
        bm_error("bm.c", "bm_collect_count", "Invalid  count type", t);

    }
}


int bm_compute_output_interpretation_type(int error_type, int t, int type_for_error) //given the current state of the units, which pattern do they most closely resemble?
{
    int p;
    double error[P_MAX];

    // so mystery pattern q, has been given to network and we need to find which p resembles the output the most, by choosing the lowest error and returning it's p index.
    for(p = 0; p < P; p++) {
        //error[p] = network_error_measure(network_type, error_type, p, t);
        if (error_type == RMSD) {
            error[p] = bm_calculate_rms_error_type(p, t, type_for_error);

        }
        else {
            throw_error(__FILE__,  __LINE__, __FUNCTION__, "Too lazy to implement this for any other network type or error type as never used (identical to network_compute_output_interpretation_type!", 0);
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
/*
void bm_collect_count_plus(void)
{
    bm_collect_count(VISIBLE);
}
void bm_collect_count_minus_name(void)
{
    bm_collect_count(NAME);
}
void bm_collect_count_minus_visual(void)
{
    bm_collect_count(VISUAL);
}
void bm_collect_count_minus_verbal(void)
{
    bm_collect_count(VERBAL);
}*/

// input: t = type of units clamped during annealing
void bm_anneal(void)
{
    double temperature;
    for (temperature = BM_T_MAX; temperature > BM_T_MIN; temperature *= BM_T_RATE)
    {
        bm_flip_units(temperature);//, t);
    }
}

void bm_anneal_tediously_slowly(void)
{
    double temperature;
    for (temperature = BM_T_MAX; temperature > BM_T_MIN; temperature *= BM_T_RATE)
    {
        bm_flip_units(temperature);//, t);
    }
    int i;
    for (i = 0; i < 2000; i++) {

        bm_flip_units(temperature);


    }
}

// input: t is type of units for minus phase
void bm_adjust_weights_step(int t)
{
    int i, j, k;

    bm_weights_updated = 1;
    bm_biases_updated = 1;

    // units
    for (j = 0; j < BM_K; j++)
    {
        // visible <-> hidden
        for (i = 0; i < N; i++)
        {
            k = bm_map_vh(i, j);
            bm_put_delta(k, (bm_get_count(k, BM_PLUS) - bm_get_count(k, t)));
            bm_inc_weight_step(k);
        }

        // hidden <-> hidden
        for (i = j + 1; i < BM_K; i++)
        {
            k = bm_map_hh(i, j);
            bm_put_delta(k, (bm_get_count(k, BM_PLUS) - bm_get_count(k, t)));
            bm_inc_weight_step(k);
        }
    }

    // biases
    for (i = 0; i < BM_UNITS; i++)
    {
        k = BM_PAIRS + i;
        bm_put_delta(k, (bm_get_count(k, BM_PLUS) - bm_get_count(k, t)));
        bm_inc_bias_step(i);
    }

}


void   bm_decay_learning_rate(void) {
    if (bm_r > BM_R_MIN)
    {
        bm_r *= BM_R_RATE;
    }
    else
    {
        bm_r = BM_R_MIN;
    }
}

// input: t is type of units for minus phase
void bm_adjust_weights(int t)
{
    int i, j, k;

    bm_weights_updated = 1;
    bm_biases_updated = 1;

    // units
    for (j = 0; j < BM_K; j++)
    {
        // visible <-> hidden
        for (i = 0; i < N; i++)
        {
            k = bm_map_vh(i, j);
            bm_put_delta(k, (bm_get_count(k, BM_PLUS) - bm_get_count(k, t)));
            bm_inc_weight(k, (bm_r * bm_get_delta(k)) / (double) (BM_C * P));
        }

        // hidden <-> hidden
        for (i = j + 1; i < BM_K; i++)
        {
            k = bm_map_hh(i, j);
            bm_put_delta(k, (bm_get_count(k, BM_PLUS) - bm_get_count(k, t)));
            bm_inc_weight(k, (bm_r * bm_get_delta(k)) / (double) (BM_C * P));
        }
    }

    // biases
    for (i = 0; i < BM_UNITS; i++)
    {
        k = BM_PAIRS + i;
        bm_put_delta(k, (bm_get_count(k, BM_PLUS) - bm_get_count(k, t)));
        bm_inc_bias(i, (bm_r * bm_get_delta(k)) / (double) (BM_C * P));
    }


}

void bm_adjust_weights_name(void)
{
    bm_adjust_weights(BM_MINUS_NAME);
}

void bm_adjust_weights_step_all(void)
{
    int i, j, k;

    bm_weights_updated = 1;
    bm_biases_updated = 1;

    // units
    for (j = 0; j < BM_K; j++)
    {
        // visible <-> hidden
        for (i = 0; i < BM_N; i++)
        {
            k = bm_map_vh(i, j);
            bm_put_delta(k, (bm_get_count(k, BM_PLUS) - ((bm_get_count(k, BM_MINUS_NAME) + bm_get_count(k, BM_MINUS_VERBAL) + bm_get_count(k, BM_MINUS_VISUAL)) / (double) 3.0)));
            bm_inc_weight_step(k);
        }

        // hidden <-> hidden
        for (i = j + 1; i < BM_K; i++)
        {
            k = bm_map_hh(i, j);
            bm_put_delta(k, (bm_get_count(k, BM_PLUS) - ((bm_get_count(k, BM_MINUS_NAME) + bm_get_count(k, BM_MINUS_VERBAL) + bm_get_count(k, BM_MINUS_VISUAL)) / (double) 3.0)));
            bm_inc_weight_step(k);
        }
    }

    // biases
    for (i = 0; i < BM_UNITS; i++)
    {
        k = BM_PAIRS + i;
        bm_put_delta(k, (bm_get_count(k, BM_PLUS) - ((bm_get_count(k, BM_MINUS_NAME) + bm_get_count(k, BM_MINUS_VERBAL) + bm_get_count(k, BM_MINUS_VISUAL)) / (double) 3.0)));
        bm_inc_bias_step(i);
    }


}

void bm_adjust_weights_step_all_pdptool(void)
{
    int i, j, k;

    bm_weights_updated = 1;
    bm_biases_updated = 1;

    // units
    for (j = 0; j < BM_K; j++)
    {
        // visible <-> hidden
        for (i = 0; i < BM_N; i++)
        {
            k = bm_map_vh(i, j);
            bm_put_delta(k, ((bm_get_count(k, BM_PLUS_NAME) + 2.0 * bm_get_count(k, BM_PLUS)) - (bm_get_count(k, BM_MINUS_NAME) + bm_get_count(k, BM_MINUS_VERBAL) + bm_get_count(k, BM_MINUS_VISUAL))));
            bm_inc_weight_step(k);
        }

        // hidden <-> hidden
        for (i = j + 1; i < BM_K; i++)
        {
            k = bm_map_hh(i, j);
            bm_put_delta(k, ((bm_get_count(k, BM_PLUS_NAME) + 2.0 * bm_get_count(k, BM_PLUS)) - (bm_get_count(k, BM_MINUS_NAME) + bm_get_count(k, BM_MINUS_VERBAL) + bm_get_count(k, BM_MINUS_VISUAL))));
            bm_inc_weight_step(k);
        }
    }

    // biases
    for (i = 0; i < BM_UNITS; i++)
    {
        k = BM_PAIRS + i;
        bm_put_delta(k, ((bm_get_count(k, BM_PLUS_NAME) + 2.0 * bm_get_count(k, BM_PLUS)) - (bm_get_count(k, BM_MINUS_NAME) + bm_get_count(k, BM_MINUS_VERBAL) + bm_get_count(k, BM_MINUS_VISUAL))));
        bm_inc_bias_step(i);
    }


}

int bm_collect_miss(int p)
{
    int i;
    int miss = 0;
    for (i = 0; i < BM_N; i++)
    {
        if (bm_get_visible(i) != get_pattern_int(p, i))
        {
            miss++;

        }
    }
    return miss;
}

int bm_miss_general(int p)
{
    int *b, i, miss[3] = {0, 0 , 0}, i_min = 0;

    miss[0] = bm_collect_miss(p);

    if (miss[0])
    {
        b = bm_get_base_ps(p); //given a p return the two other ps that share a name with p
        miss[0] = bm_collect_miss(b[0]);
        miss[1] = bm_collect_miss(b[1]);
        miss[2] = bm_collect_miss(b[2]);

        for (i = 0; i < 3; i++)
        {
            if (miss[i] < miss[i_min])
            {
                i_min = i;
            }

            // 			printf("pattern = %i\tmiss[%i] = %i\tmin = %i\tb = %i\n", p, i, miss[i], miss[i_min], b[i]);
        }
    }

    return miss[i_min];
}

int bm_get_miss(int p, int t)
{

    if (bm_pattern_general(p) && (t == NAME)) //if pattern is a general pattern and we're looking  at name
    {
        return bm_miss_general(p);
    }
    else
    {
        return bm_collect_miss(p);
    }

    bm_error("statstats.c", "get_miss", "Error", p);
    return -1;
}

int bm_error_type(int target_p, int t, int miss)
{
    int p, min = miss, min__p = target_p, temp = 0;

    for (p = 0; p < P; p++)
    {
        if (p != target_p)
        {
            temp = bm_get_miss(p, t);
            if (temp < min)
            {
                min = temp;
                min__p = p;
            }
        }
    }

    return min__p;
}

struct bm_weight_stats stats = {0.0, 0.0, 0.0, 0.0};

struct bm_weight_stats bm_calculate_weight_average(void)
{
    int i, pos = 0, neg = 0;
    double w = 0.0;

    if (bm_weights_updated)
    {
        for (i = 0; i < BM_PAIRS; i++)
        {
            w = bm_get_weight(i);
            if (w > 0.0)
            {
                stats.pos_average += w;
                pos++;
            }
            else
            {
                stats.neg_average += w;
                neg++;
            }
            if (w > stats.max)
            {
                stats.max = w;

            }
            if (w < stats.min)
            {
                stats.min = w;
            }
        }

        if (pos)
        {
            stats.pos_average /= (double) pos;
        }
        if (neg)
        {
            stats.neg_average /= (double) neg;
        }

        bm_weights_updated = 0;
    }
    return stats;
}

struct bm_weight_stats b = {0.0, 0.0, 0.0, 0.0};

struct bm_weight_stats bm_calculate_bias_average(void) //could tidy into one function with above
{
    int i, pos = 0, neg = 0;
    double w = 0.0;

    if (bm_biases_updated)
    {
        for (i = 0; i < BM_UNITS; i++)
        {
            w = bm_get_bias(i);
            if (w > 0.0)
            {
                b.pos_average += w;
                pos++;
            }
            else
            {
                b.neg_average += w;
                neg++;
            }
            if (w > b.max)
            {
                b.max = w;

            }
            if (w < b.min)
            {
                b.min = w;
            }
        }

        if (pos)
        {
            b.pos_average /= (double) pos;
        }
        if (neg)
        {
            b.neg_average /= (double) neg;
        }

        bm_biases_updated = 0;
    }
    return b;
}


double bm_average_delta_prob_step(void)
{
    int i;
    double average = 0.0;

    for (i = 0; i < (BM_PAIRS + BM_UNITS); i++)
    {
        if (bm_get_delta(i) > 0.0)
        {
            average += (double) bm_r * BM_STEP;
        }
        else if (bm_get_delta(i) < 0.0)
        {
            average -= (double) bm_r * BM_STEP;
        }
    }
    average /= (double) (BM_C * P * (BM_PAIRS + BM_UNITS));
    return average;

}

double bm_abs_average_delta_prob_step(void)
{
    int i;
    double average = 0.0;

    for (i = 0; i < (BM_PAIRS + BM_UNITS); i++)
    {
        if (bm_get_delta(i))
        {
            average += (double) BM_STEP;
        }
    }
    average /= (double) (BM_C * P * (BM_PAIRS + BM_UNITS));
    return average;
}


double bm_average_delta_prob(void)
{
    int i;
    double average = 0.0;

    for (i = 0; i < (BM_PAIRS + BM_UNITS); i++)
    {
        average += (double) bm_get_count(i, BM_DELTA);
    }
    average /= (double) (BM_C * P * (BM_PAIRS + BM_UNITS));
    return average;
}

double bm_abs_average_delta_prob(void)
{
    int i;
    double average = 0.0;

    for (i = 0; i < (BM_PAIRS + BM_UNITS); i++)
    {
        average += (double) abs(bm_get_count(i, BM_DELTA));
    }
    average /= (double) (BM_C * P * (BM_PAIRS + BM_UNITS));
    return average;
}



int bm_max_delta_prob(void)
{
    int i;
    int max = 0;

    for (i = 0; i < BM_PAIRS; i++)
    {
        if (fabs(bm_get_count(i, BM_DELTA) > max))
        {
            max = fabs(bm_get_count(i, BM_DELTA));
        }
    }
    return max;
}
