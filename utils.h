
#ifndef _utils_h_
#define _utils_h_

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif



// Debugging flag
#define DEBUG 0


// Error types:
#define JACCARD		0
#define RMSD		1
#define SORENSEN	2
#define RSS		3


#define PRINT_FUNCTION 0
#define BPTT			0
#define BM			1
#define	PDPTOOL			2
#define TYLER_RECURRENT		3
#define TYLER_FEEDFORWARD	4

int UNITS;
int PAIRS;

char* itoa(int value, char* result, int base);

void init_random(void);
#define random_init(void) init_random(void)
#define f(x) logistic(x)
int random_int(int min, int max)   ;

double sqr(double a)   ;

int in_range(int i, int max_i)  ;
double random_double(double min, double max);
int debug_range(int i, int max_i);
double logistic(double x);
double inverse_logistic(double x);
double cee(double d, double y);
double cee2(double d, double y);
void throw_error(char file[], int line, const char func[], char text[], int num, ...);
void throw_warning(char file[], int line, const char func[], char text[], int num, ...);
int file_exists(const char * filename);
double sse (double d, double y);
int multiples_of(int i, int j)                              ;
double gaussrand();

double get_gaussian_noise(double range);
#endif
