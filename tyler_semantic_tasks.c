#include "semantic_tasks.h"
#include "tyler.h"



void tyler_save_response(int p, int sample, int level, int distribution, int n, int noise)
{
    //   //find lowest error
    int q;//, r;
    double min_error = 1.0, distance;
    for (q = 0; q < P; q++) {
        distance = tyler_euclidean_distance_noise(q);
        if (distance < min_error) {
            tyler_response[distribution][level][sample][p] = q;
            min_error = distance;
            if (min_error == 0.0) {
                break;

            }
        }
    }
//     printf("P = %i\n", P);

    tyler_distinctive_error[distribution][level][sample][p] = tyler_calculate_fabs_error_type_noise(p, TYLER_DISTINCTIVE, n, noise);
    tyler_shared_error[distribution][level][sample][p] = tyler_calculate_fabs_error_type_noise(p, TYLER_SHARED, n, noise);
    tyler_functional_error[distribution][level][sample][p] = tyler_calculate_fabs_error_type_noise(p, TYLER_FUNCTIONAL, n, noise);


}


// void tyler_save_response_network(int p, int sample, int level, int distribution, int network)
// {
//     //   //find lowest error
//     int q;//, r;
//     double min_error = 1.0, distance;
//     for (q = 0; q < P; q++) {
//         distance = tyler_euclidean_distance(q);
//         if (distance < min_error) {
//             tyler_response_network[network][distribution][level][sample][p] = q;
//             min_error = distance;
//             if (min_error == 0.0) {
//                 break;
//
//             }
//         }
//     }
//
//     tyler_distinctive_error_network[network][distribution][level][sample][p] = tyler_calculate_fabs_error_type(p, TYLER_DISTINCTIVE);
//
//     tyler_shared_error_network[network][distribution][level][sample][p] = tyler_calculate_fabs_error_type(p, TYLER_SHARED);
//
//     tyler_functional_error_network[network][distribution][level][sample][p] = tyler_calculate_fabs_error_type(p, TYLER_FUNCTIONAL);
// }
//

void tyler_feedforward_save_response(int p, int sample, int level, int distribution, int noise, int n)
{
    //   //find lowest error
    int q;//, r;
    double min_error = 1.0, distance;
    for (q = 0; q < P; q++) {
        distance = tyler_feedforward_euclidean_distance_noise(q, noise, n);
        if (distance < min_error) {
            tyler_response[distribution][level][sample][p] = q;
            min_error = distance;
            if (min_error == 0.0) {
                break;

            }
        }
    }
//          printf("P = %i\n", P);
    tyler_distinctive_error[distribution][level][sample][p] = tyler_feedforward_calculate_fabs_error_type_noise(p, TYLER_DISTINCTIVE, noise, n);
    tyler_shared_error[distribution][level][sample][p] = tyler_feedforward_calculate_fabs_error_type_noise(p, TYLER_SHARED, noise,  n);
    tyler_functional_error[distribution][level][sample][p] = tyler_feedforward_calculate_fabs_error_type_noise(p, TYLER_FUNCTIONAL, noise, n);
}

// void tyler_feedforward_save_response_network(int p, int sample, int level, int distribution, int network)
// {
//     //   //find lowest error
//     int q;//, r;
//     double min_error = 1.0, distance;
//     for (q = 0; q < P; q++) {
//         distance = tyler_feedforward_euclidean_distance(q);
//         if (distance < min_error) {
//             tyler_response_network[network][distribution][level][sample][p] = q;
//             min_error = distance;
//             if (min_error == 0.0) {
//                 break;
//
//             }
//         }
//     }
//
//     tyler_distinctive_error_network[network][distribution][level][sample][p] = tyler_feedforward_calculate_fabs_error_type(p, TYLER_DISTINCTIVE);
//
//     tyler_shared_error_network[network][distribution][level][sample][p] = tyler_feedforward_calculate_fabs_error_type(p, TYLER_SHARED);
//
//     tyler_functional_error_network[network][distribution][level][sample][p] = tyler_feedforward_calculate_fabs_error_type(p, TYLER_FUNCTIONAL);
// }
//






void aoa_save_response(int p, int sample, int level, int distribution, int n, int noise)
{
    //   //find lowest error
    int q;//, r;
    double min_error = 1.0, distance;
    for (q = 0; q < P; q++) {
        distance = tyler_euclidean_distance_noise(q, n, noise);
        if (distance < min_error) {
            aoa_response[distribution][level][sample][p] = q;
            min_error = distance;
            if (min_error == 0.0) {
                break;

            }
        }
    }

    aoa_error[distribution][level][sample][p] = tyler_calculate_fabs_error_type_noise(p, TYLER_VISIBLE, n, noise);

//     printf("aoa_response = %d\t aoa_error = %f\n", aoa_response[distribution][level][sample][p], aoa_error[distribution][level][sample][p]);

}


void aoa_feedforward_save_response(int p, int sample, int level, int distribution, int noise, int n)
{
    //   //find lowest error
    int q;//, r;
    double min_error = 1.0, distance;
    for (q = 0; q < P; q++) {
        distance = tyler_feedforward_euclidean_distance_noise(q, noise, n);
        if (distance < min_error) {
            aoa_response[distribution][level][sample][p] = q;
            min_error = distance;
            if (min_error == 0.0) {
                break;

            }
        }
    }
    //          printf("P = %i\n", P);
    aoa_error[distribution][level][sample][p] = tyler_feedforward_calculate_fabs_error_type_noise(p, TYLER_VISIBLE, noise, n);

}
