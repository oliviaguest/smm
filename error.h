#ifndef __ERROR_H // is this if statement really needed? -no
#define __ERROR_H

#include "patterns.h"
#define NAME_ERROR   0
#define VISUAL_ERROR 1
#define VERBAL_ERROR 2
#define NO_SUBPATTERNS 4

#define ERROR_I_MAX 500000

double Total_Error[ERROR_I_MAX];
double Error[3][ERROR_I_MAX];
double Pattern_Error[3][P_MAX], Total_Pattern_Error[P_MAX], Total_Stage_Error[3];

void save_error(int test, int p, int s, int c, double tmp);
void reset_errors(void);
void init_errors(void);
#endif