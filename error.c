#include "error.h"




// test : Boolean if this is a test or training
// p, s, c, pattern, stage, iteration
//tmp the current error value
void save_error(int test, int p, int s, int c, double tmp) // this function gives me a headache... it took me ages to write, but it had to be done even though it worked without doing this, I really wanted it to show error in real time...
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif


    if (test) {
        if (s == NO_SUBPATTERNS) {
            Pattern_Error[0][p] = 100.0 * (Pattern_Error[0][p] / 100.0 * test + tmp) / (double) (test + 1);
            Error[0][c] = (Error[0][c] * p + Pattern_Error[0][p]) / (double) (p + 1);
            Total_Stage_Error[0] = 0.0;
            Total_Pattern_Error[p] = Total_Pattern_Error[p] + Pattern_Error[0][p];
            Total_Error[c] = Total_Error[c] + Error[0][c];
        }
        else {
            Pattern_Error[s][p] = 100.0 * (Pattern_Error[s][p] / 100.0 * test + tmp) / (double) (test + 1);
            Error[s][c] = (Error[s][c] * p + Pattern_Error[s][p]) / (double) (p + 1);
            Total_Stage_Error[s] = (Total_Stage_Error[s] * p + Pattern_Error[s][p]) / (double) (p + 1);
            Total_Pattern_Error[p] = (Total_Pattern_Error[p] * s + Pattern_Error[s][p]) / (double) (s + 1);
            Total_Error[c] = (Total_Error[c] * s + Error[s][c]) / (double) (s + 1);
        }
    } else {

        if (s == NO_SUBPATTERNS) {
            Pattern_Error[0][p] = 100.0 * tmp;
            if (p) {
                Error[0][c] = (Error[0][c] * p + Pattern_Error[0][p]) / (double) (p + 1);
                Total_Stage_Error[0] = 0.0;

            } else {
                Error[0][c] = Pattern_Error[0][p];
                Total_Stage_Error[0] = Pattern_Error[0][p];

            }


            Total_Pattern_Error[p] = Pattern_Error[0][p];
            Total_Error[c] = Error[0][c];
        } else {

            Pattern_Error[s][p] = 100.0 * tmp;
            if (p) {
                Error[s][c] = (Error[s][c] * p + Pattern_Error[s][p]) / (double) (p + 1);
                Total_Stage_Error[s] = (Total_Stage_Error[s] * p + Pattern_Error[s][p]) / (double) (p + 1);

            } else {
                Error[s][c] = Pattern_Error[s][p];
                Total_Stage_Error[s] = Pattern_Error[s][p];

            }

            if (s) {
                Total_Pattern_Error[p] = (Total_Pattern_Error[p] * s + Pattern_Error[s][p]) / (double) (s + 1);
                Total_Error[c] = (Total_Error[c] * s + Error[s][c]) / (double) (s + 1);

            } else {
                Total_Pattern_Error[p] = Pattern_Error[s][p];
                Total_Error[c] = Error[s][c];
            }
        }
    }

//     printf("Total_Error[%i] = %f %i %i\n", c, Total_Error[c], s, p);

}



void reset_errors(void) {
    init_errors();
}
void init_errors(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    int c, s, p;
    for (c = 0; c < ERROR_I_MAX; c++) {
        Total_Error[c] = 0.0;
        for (s = 0; s < 3; s++) {
            Error[s][c] = 0.0;
            Total_Stage_Error[s] = 0.0;
            for (p = p_min; p < p_max; p++) {
                Pattern_Error[s][p] = 0.0;
                Total_Pattern_Error[p] = 0.0;
            }
        }
    }
}