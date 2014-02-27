#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"


/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    // check that the base if valid
    if (base < 2 || base > 36) {
        *result = '\0';
        return result;
    }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

double gaussrand()
/**Knuth**/
{
    static double V1, V2, S;
    static int phase = 0;
    double X;

    if(phase == 0) {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;

    return X;
}
int multiples_of(int i, int j) { //is i a multiple of j
    return !(i % j);


}
void init_random(void)
{
    srand(time(NULL));
//     srand(12345);
}

int random_int(int min, int max)
{
    return (rand() * ((max - min) / (double) RAND_MAX)) + min;
}

double sqr(double a)
{
    return a * a;
}

int in_range(int i, int max_i)
{
    return ((i >= 0) && (i < max_i));
}

double random_double(double min, double max)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return (rand() * ((max - min) / (double) RAND_MAX)) + min;
}

// Debugging Array Indices
int debug_range(int i, int max_i)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

#if DEBUG
    return in_range(i, max_i);
#else
    return 1;
#endif
}

int debug_range_(int i, int min_i, int max_i)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

#if DEBUG
    i -= min_i;
    max_i -= min_i;
    return in_range(i, max_i);
#else
    return 1;
#endif
}

int file_exists(const char * filename)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    FILE *file;
    if ((file = fopen(filename, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;

}

// Error handling
void throw_error(char file[], int line, const char func[], char text[], int num, ...)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    fprintf(stderr, "\nError: %s:%i: in function %s: %s: ", file, line, func, text);
    va_list arguments;                     // A place to store the list of arguments

    va_start(arguments, num);           // Initializing arguments to store all values after num

    for(; num > 0; num-- ) {
        fprintf(stderr, "%i ", (int)va_arg(arguments, int)); // Adds the next value in argument list to sum.
    }
    va_end(arguments);                  // Cleans up the list
    fprintf(stderr, "(press \"q\" to quit, return to continue)\n");

    if (getchar() == 'q') {
        exit(0);
    }
}

// Error handling
void throw_warning(char file[], int line, const char func[], char text[], int num, ...)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    fprintf(stderr, "\nWarning: %s:%i: in function %s: %s: ", file, line, func, text);
    va_list arguments;                     // A place to store the list of arguments

    va_start(arguments, num);           // Initializing arguments to store all values after num

    for(; num > 0; num-- ) {
        fprintf(stderr, "%i ", (int)va_arg(arguments, int)); // Adds the next value in argument list to sum.
    }
    va_end(arguments);                  // Cleans up the list
//     fprintf(stderr, "(press \"q\" to quit, return to continue)\n");
//
//     if (getchar() == 'q') {
//         exit(0);
//     }
}


double logistic(double x)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (x >  15.9357739741644) {
        x = 15.9357739741644;

    }

    if (x < -15.9357739741644) {
        x = -15.9357739741644;
    }

    return  (1.0 / (1.0 + exp(-x)));
}





double inverse_logistic(double x)
{
    if (x == 1.0) {  //check denominator
        x = 0.99999988;
    }

    if (x == 0.0) {
        x = 0.00000012;
    }
    return log(x / (1.0 - x));
}

double cee(double d, double y)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return (d - y);
}

double sse(double d, double y)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    return (d - y) * y * (1.0 - y);
}

double cee2(double d, double y)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    if (y == 0) {
        y = 0.000001;
    }
    else if (y == 1) {
        y = 0.999999;
    }
    return (d - y) / (y * (1.0 - y));
}

double logit(int x)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (!x)	{
        x = 0.000000001;
    }
    if (x == 1)	{
        x = 0.999999999;
    }
    x = log(x);
    return x - (1.0 - x);
}


double get_gaussian_noise(double range)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    //     return random_double(-1.0, 1.0);
    return random_double(-range, range);
}