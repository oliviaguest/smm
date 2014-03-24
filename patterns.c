#include "patterns.h"
#include "network.h"


#define N_NOISY 5

#define STRING 28
static int pattern[P_MAX][N_SUBPATTERN][MAX_N_PATTERN];

static int tyler_pattern[P_MAX][TYLER_N_SUBPATTERN][TYLER_N_POOL];
static float noisy_tyler_pattern[N_NOISY][P_MAX][TYLER_N_SUBPATTERN][TYLER_N_POOL];
static double get_noisy_pattern_type_tyler_internal( int n, int p, int t, int i);


int specificity_level[P_MAX];

char name_string[P_MAX][STRING];
char open_patterns_filename[128];
// int pattern[P_MAX][N_SUBPATTERN][N_VISUAL];
// // int pdptool_pattern[PDPTOOL_P][3][PDPTOOL_N_VERBAL];


int type_patterns_loaded = BPTT_BM_PATTERNS;
int pdptool_style_patterns(void)
{
    return (type_patterns_loaded == PDPTOOL_PATTERNS);
}
// void pdptool_style_patterns_set(int i) {
//
//     type_patterns_loaded = PDPTOOL_PATTERNS;
// }

int     pattern_get_n_visual(void)
{

    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        return PDPTOOL_N_VISUAL;
    }
    else if (type_patterns_loaded == ROGERS_PATTERNS) {
        return ROGERS_N_VISUAL;
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        return -1;
    }
    else {
        return BM_N_VISUAL;

    }
}

int     pattern_get_n_verbal(void)
{

    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        return PDPTOOL_N_VERBAL;
    }
    else if (type_patterns_loaded == ROGERS_PATTERNS) {
        return ROGERS_N_VERBAL;
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        return -1;
    }
    else {
        return BM_N_VERBAL;

    }
}
int get_n_visual_features(int p) //return the number of visual features on for pattern p
{
    int n_visual_features = 0, i;
    for (i = 0; i < pattern_get_n_visual(); i++) {
        n_visual_features += get_pattern_type_int(p, VISUAL, i);
    }
//     printf("n_visual_features = %f\n", n_visual_features);

    return n_visual_features;
}
// void init_patterns_globals(int network_type)
// {
//   if (network_type == BPTT) {
//     N = BPTT_N;
//     N_NAME = BPTT_N_NAME;
//     N_VISUAL = BPTT_N_VISUAL;
//     N_PERCEPTUAL = BPTT_N_PERCEPTUAL;
//     N_FUNCTIONAL = BPTT_N_FUNCTIONAL;
//     N_FUNCTIONAL = BPTT_N_FUNCTIONAL;
//     N_ENCYCLOPEDIC = BPTT_N_ENCYCLOPEDIC;
//     N_VERBAL = BPTT_N_VERBAL;
//
//   }
//   else if (network_type == BM) {
//     N = BM_N;
//     N_NAME = BM_N_NAME;
//     N_VISUAL = BM_N_VISUAL;
//     N_PERCEPTUAL = BM_N_PERCEPTUAL;
//     N_FUNCTIONAL = BM_N_FUNCTIONAL;
//     N_FUNCTIONAL = BM_N_FUNCTIONAL;
//     N_ENCYCLOPEDIC = BM_N_ENCYCLOPEDIC;
//     N_VERBAL = BM_N_VERBAL;
//
//   }
//   else if (network_type == PDPTOOL) {
//     N = PDPTOOL_N;
//     N_NAME = PDPTOOL_N_NAME;
//     N_VISUAL = PDPTOOL_N_VISUAL;
//     N_PERCEPTUAL = PDPTOOL_N_PERCEPTUAL;
//     N_FUNCTIONAL = PDPTOOL_N_FUNCTIONAL;
//     N_FUNCTIONAL = PDPTOOL_N_FUNCTIONAL;
//     N_ENCYCLOPEDIC = PDPTOOL_N_ENCYCLOPEDIC;
//     N_VERBAL = PDPTOOL_N_VERBAL;
//
//   }
// }

// void init_patterns(int network_type)
// {
// //   printf("%s\n", __FUNCTION__);
//
//     int p, s, i;
//     P = P_MAX;
//     open_patterns_filename[0] = ' ';
//
//
//     for (p = 0; p < P_MAX; p++) {
//         specificity_level[p] = 0;
//         name_string[p][0] = ' ';
//
//
//         for (s = 0; s < N_SUBPATTERN; s++) {
//             for (i = 0; i < MAX_N_PATTERN; i++) {
//                 pattern[p][s][i] = -1;  //careful!
//             }
//         }
//     }
//
//
//
// //     type_patterns_loaded == PDPTOOL_PATTERNS = 0;
// }

void open_patterns(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    /*    int x = 0;*/
    FILE *fp = NULL;
    int p, s, i, i_max;
    sprintf(open_patterns_filename, "%s" , filename);
    fp = fopen(filename, "r");
// //      init_patterns(BM);
//     printf("%s\n", filename);

    type_patterns_loaded = BPTT_BM_PATTERNS;
    (fscanf(fp, " %i", &P)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: P", 0));

    p_max = P;
//        printf("%s P = %i, N = %i %i\n",filename, P, N, BM_N);

    for (p = 0; p < P; p++) { //cycle through patterns
        for (s = 0; s < N_SUBPATTERN; s++) { // and through each subpattern
            i_max = N_of(s);
            for (i = 0; i < i_max; i++) {
                (fscanf(fp, " %i", &pattern[p][s][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
            }
            /*            x++;*/
        }
    }
    /*    printf("x = %d, P*N = %d\n", x, P*N);*/
    for (p = 0; p < P; p++) { //cycle through patterns
        (fscanf(fp, " %i", &specificity_level[p])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: specificity_level", 0));
    }

    (fgets(name_string[0], STRING, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read new line between specificity_level and name_string", 0));


    for (p = 0; p < P; p++) { //cycle through patterns
        (fgets(name_string[p], STRING, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: name_string", 0));
        name_string[p][strlen(name_string[p]) - 1] = '\0';

    }

    if(fp) {
        fclose(fp);
    }

}


void open_patterns_rogers(void)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    /*    int x = 0;*/
    FILE *fp_names = NULL, *fp_visual = NULL, *fp_verbal = NULL;
    int p, s, i, fix_i = 0;

//     char tmp[1];
    int tmp;
    fp_names = fopen("rev2_name_vectors.txt", "r");
    fp_visual = fopen("rev2_vis_vecs.txt", "r");
    fp_verbal = fopen("rev2_verb_vecs.txt", "r");


    type_patterns_loaded = ROGERS_PATTERNS;
//     type_patterns_loaded = PDPTOOL_PATTERNS;

    p_max = ROGERS_P;
    P = ROGERS_P;
    //        printf("%s P = %i, N = %i %i\n",filename, P, N, BM_N);

    for (p = 0; p < P; p++) { //cycle through patterns
// 	(fscanf(fp_names, " %c", &name_string[p][s])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read name strings", 0));
// 	fgets(&name_string[p], STRING*STRING, fp_names);

// 	printf("%s\n", name_string[p]);

//read off string
        for(s = 0; s < STRING; s++) {
            name_string[p][s] = fgetc(fp_names);

            if(name_string[p][s] == '\t') {

                name_string[p][s] = '\0';

                break;
            }
            printf("%c", name_string[p][s]);

        }

//         printf("\n", sizeof(name_string[p]));

        // read off pattern bits
//         for(i = 0; i < ROGERS_N_NAME; i++) {
        i = 0;
        tmp = (int)' ';
        while (((tmp = fgetc(fp_names)) != EOF) && (tmp != '\n')) {
            //     if(i  > ROGERS_N_NAME){break;}


// 	  tmp = fgetc(fp_names);


// 	  printf("%c", tmp);

            while ((char)tmp == ' ')
            {
                tmp = fgetc(fp_names);
// 	      printf("%c", tmp);
                if(tmp  == EOF) {
                    break;
                }
                if(tmp == (int)'\n') {
                    break;
                }
                if((char)tmp == '\n') {
                    break;
                }
            }
            if (i < ROGERS_N_NAME) {
                pattern[p][NAME][i] = tmp-48;
                printf("%d", pattern[p][NAME][i]);
            }

            i++;
            if ((p > 23) && (p < 40) && (!fix_i)) {
                i--;
                fix_i = 1;
            }
            tmp = (int)' ';

        }

        printf("\n\n");
        fix_i = 0;
    }

//       rewind(fp_names);

    for (p = 0; p < P; p++) { //cycle through patterns
//                 	(fscanf(fp_names, "%s\t", &name_string[p])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read name strings", 0));
// 			printf("%s\n", name_string[p]);

//       for(i = 0; i < ROGERS_N_NAME; i++) {
//
// 	(fscanf(fp_names, " %i", &pattern[p][NAME][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read name subpatterns", 0));
//       }

        for(i = 0; i < ROGERS_N_VISUAL; i++) {
            (fscanf(fp_visual, " %d", &pattern[p][VISUAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read visual subpatterns", 0));
        }

//         for(i = 0; i < ROGERS_N_VERBAL; i++) {
//             (fscanf(fp_verbal, " %i", &pattern[p][VERBAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read verbal subpatterns", 0));
//         }


        for (i = 0; i < ROGERS_N_PERCEPTUAL; i++) {
            (fscanf(fp_verbal, " %d", &pattern[p][PERCEPTUAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }
        for (i = 0; i < ROGERS_N_FUNCTIONAL; i++) {
            (fscanf(fp_verbal, " %d", &pattern[p][FUNCTIONAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }
        for (i = 0; i < ROGERS_N_ENCYCLOPEDIC; i++) {
            (fscanf(fp_verbal, " %d", &pattern[p][ENCYCLOPEDIC][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }

    }


    for (p = 0; p < 5; p++) {
        specificity_level[p] = 0;
    }
    for (p = 5; p < 5+8; p++) {
        specificity_level[p] = 1;
    }
    for (p = 5+8; p < 5+8+3; p++) {
        specificity_level[p] = 0;
    }
    for (p = 5+8+3; p < 5+8+3+21; p++) {
        specificity_level[p] = 1;
    }
    for (p = 5+8+3+21; p < 5+8+3+21+3; p++) {
        specificity_level[p] = 0;
    }
    for (p = 5+8+3+21+3; p < 5+8+3+21+3+8; p++) {
        specificity_level[p] = 1;
    }

    if(fp_names) {
        fclose(fp_names);
    }
    if(fp_visual) {
        fclose(fp_visual);
    }
    if(fp_verbal) {
        fclose(fp_verbal);
    }

}


void tyler_open_patterns(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    /*    int x = 0;*/
    FILE *fp = NULL;
    int p, s, i, i_max;
    sprintf(open_patterns_filename, "%s" , filename);
    fp = fopen(filename, "r");
    // //      init_patterns(BM);
    //     printf("%s\n", filename);

    type_patterns_loaded = TYLER_PATTERNS;
    (fscanf(fp, " %i", &P)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: P", 0));

    p_max = P;
    //        printf("%s P = %i, N = %i %i\n",filename, P, N, BM_N);

    for (p = 0; p < P; p++) { //cycle through patterns
        for (s = 0; s < TYLER_N_SUBPATTERN; s++) { // and through each subpattern
            i_max = N_of(s);
            for (i = 0; i < i_max; i++) {
                (fscanf(fp, " %i", &tyler_pattern[p][s][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
            }
            /*            x++;*/
        }
    }
    /*    printf("x = %d, P*N = %d\n", x, P*N);*/
    for (p = 0; p < P; p++) { //cycle through patterns
        (fscanf(fp, " %i", &specificity_level[p])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: specificity_level", 0));
    }

    (fgets(name_string[0], STRING, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read new line between specificity_level and name_string", 0));


    for (p = 0; p < P; p++) { //cycle through patterns
        (fgets(name_string[p], STRING, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: name_string", 0));
    }
    if(fp) {
        fclose(fp);
    }



}

int pattern_name[P_MAX][N_SUBPATTERN][MAX_N_PATTERN]; //this is especially for pdptool as it teaches different name->rest pattern to rest->name


void pdptool_open_patterns(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    FILE *fp;
    int p, i;
    fp = fopen(filename, "r");
    char *tmp = NULL, c = ' ';
    size_t len = 0;

    type_patterns_loaded = PDPTOOL_PATTERNS;

// Ignore first two lines as they contain irrelevant stuff.
    for (i = 0; i < 2; i++) {
        (getline(&tmp, &len, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
    }

    p_max = PDPTOOL_P;
    P = PDPTOOL_P;

    for (p = 0; p < PDPTOOL_P; p++) { //cycle through patterns to collect the first set which are used when training name

        //Parsing, e.g., "robin1"
        (fgets(name_string[p], STRING, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: name_string", 0));

        // Ignore due to repetition, e.g.,  "H 1 3 name 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
        (getline(&tmp, &len, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));

        // Parsing, e.g., "T 6 2 name 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
        (fscanf(fp, "%c %d %d %4s", &c, &i, &i, tmp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
        for (i = 0; i < PDPTOOL_N_NAME; i++) {
            (fscanf(fp, " %d", &pattern_name[p][NAME][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }

        // Parsing, e.g., "T 6 2 verbal_descriptors 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 0"
        (fscanf(fp, " \n%c %d %d %18s", &c, &i, &i, tmp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
        for (i = 0; i < PDPTOOL_N_PERCEPTUAL; i++) {
            (fscanf(fp, " %d", &pattern_name[p][PERCEPTUAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }
        for (i = 0; i < PDPTOOL_N_FUNCTIONAL; i++) {
            (fscanf(fp, " %d", &pattern_name[p][FUNCTIONAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }
        for (i = 0; i < PDPTOOL_N_ENCYCLOPEDIC; i++) {
            (fscanf(fp, " %d", &pattern_name[p][ENCYCLOPEDIC][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }



        // Parsing, e.g., "T 6 2 visual_features 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
        (fscanf(fp, " \n%c %d %d %15s", &c, &i, &i, tmp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
        for (i = 0; i < PDPTOOL_N_VISUAL; i++) {
            (fscanf(fp, " %d", &pattern_name[p][VISUAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
        }

        (fscanf(fp, " \n%3s\n", tmp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
    }


    for (p = 0; p < PDPTOOL_P; p++) { //cycle through patterns to collect the second set which are used when training verbal and visual

        //Parsing, e.g., "robin1"
        (fgets(name_string[p], STRING, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: name_string", 0));

        // Ignore due to repetition, e.g.,  "H 1 3 name 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
        (getline(&tmp, &len, fp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));

        // Parsing, e.g., "T 6 2 name 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
        (fscanf(fp, "%c %d %d %4s", &c, &i, &i, tmp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
        for (i = 0; i < PDPTOOL_N_NAME; i++) {
            (fscanf(fp, " %d", &pattern[p][NAME][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }

        // Parsing, e.g., "T 6 2 verbal_descriptors 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 0"
        (fscanf(fp, " \n%c %d %d %18s", &c, &i, &i, tmp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
        for (i = 0; i < PDPTOOL_N_PERCEPTUAL; i++) {
            (fscanf(fp, " %d", &pattern[p][PERCEPTUAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }
        for (i = 0; i < PDPTOOL_N_FUNCTIONAL; i++) {
            (fscanf(fp, " %d", &pattern[p][FUNCTIONAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }
        for (i = 0; i < PDPTOOL_N_ENCYCLOPEDIC; i++) {
            (fscanf(fp, " %d", &pattern[p][ENCYCLOPEDIC][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
        }



        // Parsing, e.g., "T 6 2 visual_features 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
        (fscanf(fp, " \n%c %d %d %15s", &c, &i, &i, tmp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
        for (i = 0; i < PDPTOOL_N_VISUAL; i++) {
            (fscanf(fp, " %d", &pattern[p][VISUAL][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
        }

        (fscanf(fp, " \n%3s\n", tmp)) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"File not compatible with pdptool pattern structure", 0));
    }
    fclose(fp);


    for (p = 0; p < 5; p++) {
        specificity_level[p] = 0;
    }
    for (p = 5; p < 5+8; p++) {
        specificity_level[p] = 1;
    }
    for (p = 5+8; p < 5+8+3; p++) {
        specificity_level[p] = 0;
    }
    for (p = 5+8+3; p < 5+8+3+21; p++) {
        specificity_level[p] = 1;
    }
    for (p = 5+8+3+21; p < 5+8+3+21+3; p++) {
        specificity_level[p] = 0;
    }
    for (p = 5+8+3+21+3; p < 5+8+3+21+3+8; p++) {
        specificity_level[p] = 1;
    }

}


static int get_pattern_type_int_internal(int p, int t, int i);
static double get_pattern_type_internal(int p, int t, int i);
// static double get_pattern_internal(int p, int i);
static double get_pattern_type_pdptool_internal(int p, int t, int i);
// static double get_pattern_pdptool_internal(int p, int i);
static double get_pattern_type_tyler_internal(int p, int t, int i);


int get_pattern_int(int p, int i)
{
// #if PRINT_FUNCTION
// #endif
    int t = type_of(i);
    i -= i0_of(t);
    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        return (int) get_pattern_type_pdptool_internal(p, t, i);
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        return (int) get_pattern_type_tyler_internal(p, t, i);
    }

//     else if (type_patterns_loaded == ROGERS_PATTERNS) {
//       return (int) get_pattern_type_rogers_internal(p, t, i);
//     }
    else {
//     printf("%s (p=%d, i=%d)\n", __FUNCTION__, p, i);

//     printf("pdptool patterns: %i\n", type_patterns_loaded == PDPTOOL_PATTERNS);

        return (int) get_pattern_type_int_internal(p, t, i);
    }
}


double get_noisy_pattern(int n, int p, int i)
{
    // #if PRINT_FUNCTION
    // #endif
    int t = type_of(i);
    i -= i0_of(t);
    if (type_patterns_loaded == TYLER_PATTERNS) {
        return  get_noisy_pattern_type_tyler_internal(n, p, t, i);
    }

    //     else if (type_patterns_loaded == ROGERS_PATTERNS) {
    //       return (int) get_pattern_type_rogers_internal(p, t, i);
    //     }
    else {
        //     printf("%s (p=%d, i=%d)\n", __FUNCTION__, p, i);

        //     printf("pdptool patterns: %i\n", type_patterns_loaded == PDPTOOL_PATTERNS);

        throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid pattern request: Only Tyler patters have noise defined currently", 0);
        return 0.0;

    }

}

double get_pattern_noise(int p, int i, int noise, int n)
{


    if (debug_range(n, N_NOISY)) {

        if (noise == 1) {

            return get_noisy_pattern(n, p , i);
        } else if (noise == 0) {
            return get_pattern(p, i);
        }
        else {
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid pattern request: noise is boolean:", 1, noise);
        }



    }

    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid pattern request: There are no is preset pattern:", 1, n);
    return 0.0;

}
int get_pattern_type_int(int p, int t, int i)
{
#if PRINT_FUNCTION
    printf("%s (p=%d, i=%d)\n", __FUNCTION__, p, i);
#endif
    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        return (int) get_pattern_type_pdptool_internal(p, t, i);
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        return (int) get_pattern_type_tyler_internal(p, t, i);
    }

//     else if (type_patterns_loaded == ROGERS_PATTERNS) {
//       return (int) get_pattern_type_rogers_internal(p, t, i);
//     }
    else {
        return (int) get_pattern_type_int_internal(p, t, i);
    }
}

double get_pattern(int p, int i)
{
#if PRINT_FUNCTION
    printf("%s (p=%d, i=%d)\n", __FUNCTION__, p, i);
#endif
//   int t = type_of(i);
//   i -= i0_of(t);
//
//   if (type_patterns_loaded == PDPTOOL_PATTERNS) {
//     return (double) get_pattern_type_pdptool_internal(p, t, i);
//   }
//   return (double) get_pattern_type_internal(p, t, i);

    return (double) get_pattern_int(p, i);

}

static int get_pattern_type_int_internal(int p, int t, int i)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (debug_range(p, P))
    {
        if (debug_range(i, N))
        {
            if (debug_range(t, 5))
            {
                // 			int t = type_of(i);
                // 			i -= i0_of(t);
                // 				printf("p = %i, t = %i, i = %i\n", p, t, i);
                return pattern[p][t][i];
            }
            else
            {
                throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type t for pattern", 1, t);
                return 0.0;
            }
        }
        else
        {
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index i for pattern", 1, i);
            return 0.0;
        }
    }
    else
    {
        throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index p for pattern", 1, p);
        return 0.0;
    }
}

double get_pattern_type(int p, int t, int i)
{
#if PRINT_FUNCTION
    printf("%s (p=%d, i=%d)\n", __FUNCTION__, p, i);
#endif
    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        return get_pattern_type_pdptool_internal(p, t, i);
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        return get_pattern_type_tyler_internal(p, t, i);
    }

//     else if (type_patterns_loaded == ROGERS_PATTERNS) {
//       return get_pattern_type_rogers_internal(p, t, i);
//     }
    else {
        return get_pattern_type_internal(p, t, i);
    }
}

static double get_pattern_type_internal(int p, int t, int i)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (debug_range(p, P))
    {
        if (debug_range(i, N_of(t)))
        {
            if (debug_range(t, 5))
            {
                // 			int t = type_of(i);
                // 			i -= i0_of(t);
                // 				printf("p = %i, t = %i, i = %i\n", p, t, i);
                return (double) pattern[p][t][i];
            }
            else
            {
                throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type t for pattern", 1, t);
                return 0.0;
            }
        }
        else
        {
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index i for pattern", 1, i);
            return 0.0;
        }
    }
    else
    {
        throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index p for pattern", 1, p);
        return 0.0;
    }
}


static double get_pattern_type_tyler_internal(int p, int t, int i)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (debug_range(p, P))
    {
        if (debug_range(i, N_of(t)))
        {
            if ((t == TYLER_DISTINCTIVE) || (t == TYLER_SHARED) || (t == TYLER_FUNCTIONAL))
            {
                // 			int t = type_of(i);
                // 			i -= i0_of(t);
                // 				printf("p = %i, t = %i, i = %i\n", p, t, i);
                return (double) tyler_pattern[p][t][i];
            }
            else
            {
                throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type t for pattern", 1, t);
                return 0.0;
            }
        }
        else
        {
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index i for pattern", 1, i);
            return 0.0;
        }
    }
    else
    {
        throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index p for pattern", 1, p);
        return 0.0;
    }
}


double get_pattern_type_pdptool_internal(int p, int t, int i)
{

#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif



    if (debug_range(p, P))
    {
        if (debug_range(i, N_of(t)))
        {
            if (debug_range(t, 5))
            {
                // 			int t = type_of(i);
                // 			i -= i0_of(t);
                // 				printf("p = %i, t = %i, i = %i\n", p, t, i);
                if ((bm_get_clamped_stage() == BM_MINUS_NAME) || (bm_get_clamped_stage() == BM_PLUS_NAME) || (bm_get_clamped_type() == NAME) || (bptt_get_clamped_stage() == BPTT_STAGE_NAME) ||  (pdptool_get_clamped_stage() == PDPTOOL_STAGE_NAME))  {
//   if (t == NAME) {

// 		  printf("%s: bm: %i %i %i, bptt: %i, pdptool: %i\n", __FUNCTION__, bm_get_clamped_stage() == BM_MINUS_NAME, bm_get_clamped_stage() == BM_PLUS_NAME, bm_get_clamped_type() == NAME, bptt_get_clamped_stage() == BPTT_STAGE_NAME, pdptool_get_clamped_stage() == PDPTOOL_STAGE_NAME);

                    return (double) pattern_name[p][t][i];

                }
                else {
// 		  printf("%s: pattern\n", __FUNCTION__);

                    return (double) pattern[p][t][i];
                }
            }
            else
            {
                throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type t for pattern", 1, t);
                return 0.0;
            }
        }
        else
        {
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index i for pattern", 1, i);
            return 0.0;
        }
    }
    else
    {
        throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index p for pattern", 1, p);
        return 0.0;
    }
}

int pattern_general(int p)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (debug_range(p, P))
    {
        return !specificity_level[p];
    }
    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid pattern index", 1, p);
    return -1;
}

int is_general_name(int p)
{
    return pattern_general(p);
}


int i0_of(int s)
{

#if PRINT_FUNCTION
    printf("%s (s=%d)\n", __FUNCTION__, s);
#endif
    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        switch (s)
        {
        case NAME:
            return 0;
            break;
        case VISUAL:
            return PDPTOOL_N_NAME;
            break;
        case PERCEPTUAL:
            return PDPTOOL_N_NAME + PDPTOOL_N_VISUAL;
            break;
        case FUNCTIONAL:
            return PDPTOOL_N_NAME + PDPTOOL_N_VISUAL + PDPTOOL_N_PERCEPTUAL;
            break;
        case ENCYCLOPEDIC:
            return PDPTOOL_N_NAME + PDPTOOL_N_VISUAL + PDPTOOL_N_PERCEPTUAL + PDPTOOL_N_FUNCTIONAL;
            break;
        case VERBAL:
            return PDPTOOL_N_NAME + PDPTOOL_N_VISUAL;
            break;
        case VISIBLE:
            return 0;
            break;
        case HIDDEN:
            return PDPTOOL_N;
            break;
        default:
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, s);
            return -1;
        }
    }
    else if (type_patterns_loaded == ROGERS_PATTERNS) {
        switch (s)
        {
        case NAME:
            return 0;
            break;
        case VISUAL:
            return ROGERS_N_NAME;
            break;
        case PERCEPTUAL:
            return ROGERS_N_NAME + ROGERS_N_VISUAL;
            break;
        case FUNCTIONAL:
            return ROGERS_N_NAME + ROGERS_N_VISUAL + ROGERS_N_PERCEPTUAL;
            break;
        case ENCYCLOPEDIC:
            return ROGERS_N_NAME + ROGERS_N_VISUAL + ROGERS_N_PERCEPTUAL + ROGERS_N_FUNCTIONAL;
            break;
        case VERBAL:
            return ROGERS_N_NAME + ROGERS_N_VISUAL;
            break;
        case VISIBLE:
            return 0;
            break;
        case HIDDEN:
            return ROGERS_N;
            break;
        default:
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, s);
            return -1;
        }
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        switch (s)
        {
        case TYLER_DISTINCTIVE:
            return 0;
            break;
        case TYLER_SHARED:
            return TYLER_N_SHARED;
            break;
        case TYLER_FUNCTIONAL:
            return TYLER_N_SHARED + TYLER_N_FUNCTIONAL;
            break;
        case TYLER_VISIBLE:
        case VISIBLE:
            return 0;
            break;
        case TYLER_HIDDEN:
        case HIDDEN:
            return TYLER_N;
            break;
        default:
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, s);
            return -1;

        }
    }
    else
    {
        switch (s)
        {
        case NAME:
            return 0;
            break;
        case VISUAL:
            return BM_N_NAME;
            break;
        case PERCEPTUAL:
            return BM_N_NAME + BM_N_VISUAL;
            break;
        case FUNCTIONAL:
            return BM_N_NAME + BM_N_VISUAL + BM_N_PERCEPTUAL;
            break;
        case ENCYCLOPEDIC:
            return BM_N_NAME + BM_N_VISUAL + BM_N_PERCEPTUAL + BM_N_FUNCTIONAL;
            break;
        case VERBAL:
            return BM_N_NAME + BM_N_VISUAL;
            break;
        case VISIBLE:
            return 0;
            break;
        case HIDDEN:
            return BM_N;
            break;
        default:
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, s);
            return -1;
        }
    }
}





int N_of(int s)
{
#if PRINT_FUNCTION
    printf("%s (s=%d)\n", __FUNCTION__, s);
#endif

    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        switch (s)
        {
        case NAME:
            return PDPTOOL_N_NAME;
        case VISUAL:
            return PDPTOOL_N_VISUAL;
        case PERCEPTUAL:
            return PDPTOOL_N_PERCEPTUAL;
        case FUNCTIONAL:
            return PDPTOOL_N_FUNCTIONAL;
        case ENCYCLOPEDIC:
            return PDPTOOL_N_ENCYCLOPEDIC;
        case VERBAL:
            return PDPTOOL_N_VERBAL;
        case VISIBLE:
            return PDPTOOL_N;
        case HIDDEN:
            return PDPTOOL_K;
        default:
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, s);
            return -1;
        }
    }
    else if (type_patterns_loaded == ROGERS_PATTERNS) {
        switch (s)
        {
        case NAME:
            return ROGERS_N_NAME;
        case VISUAL:
            return ROGERS_N_VISUAL;
        case PERCEPTUAL:
            return ROGERS_N_PERCEPTUAL;
        case FUNCTIONAL:
            return ROGERS_N_FUNCTIONAL;
        case ENCYCLOPEDIC:
            return ROGERS_N_ENCYCLOPEDIC;
        case VERBAL:
            return ROGERS_N_VERBAL;
        case VISIBLE:
            return ROGERS_N;
        case HIDDEN:
            return ROGERS_K;
        default:
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, s);
            return -1;
        }
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        switch (s)
        {
        case TYLER_DISTINCTIVE:
// 	case 0:
            return TYLER_N_DISTINCTIVE;
        case TYLER_SHARED:
// 	case 1:
            return TYLER_N_SHARED;
        case TYLER_FUNCTIONAL:
// 	case 2:
// // 	case 3:
// 	case 4:
// 	case 6:
            return TYLER_N_FUNCTIONAL;
        case VISIBLE:
        case TYLER_VISIBLE:
            return TYLER_N;
        case HIDDEN:
        case TYLER_HIDDEN:
            return TYLER_K;
        default:
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, s);
            return -1;
        }
    }
    else {

        switch (s)
        {
        case NAME:
            return BM_N_NAME;
        case VISUAL:
            return BM_N_VISUAL;
        case PERCEPTUAL:
            return BM_N_PERCEPTUAL;
        case FUNCTIONAL:
            return BM_N_FUNCTIONAL;
        case ENCYCLOPEDIC:
            return BM_N_ENCYCLOPEDIC;
        case VERBAL:
            return BM_N_VERBAL;
        case VISIBLE:
            return BM_N;
        case HIDDEN:
            return BM_K;
        default:
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type", 1, s);
            return -1;
        }
    }
}

//input: state array index i (0 to UNITS)
//output: type of i (NAME, VISUAL, PERCEPTUAL, ..., HIDDEN)
int type_of(int i)
{
#if PRINT_FUNCTION
    printf("%s (i=%d)\n", __FUNCTION__, i);
#endif

    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        if (in_range(i, PDPTOOL_N))
        {

            if (in_range(i, PDPTOOL_N_NAME))
            {
                return NAME;
            }

            i -= PDPTOOL_N_NAME;

            if (in_range(i, PDPTOOL_N_VISUAL))
            {
                return VISUAL;
            }

            i -= PDPTOOL_N_VISUAL;

            if (in_range(i, PDPTOOL_N_PERCEPTUAL))
            {
                return PERCEPTUAL;
            }

            i -= PDPTOOL_N_PERCEPTUAL;

            if (in_range(i, PDPTOOL_N_FUNCTIONAL))
            {
                return FUNCTIONAL;
            }

            i -= PDPTOOL_N_FUNCTIONAL;

            if (in_range(i, PDPTOOL_N_ENCYCLOPEDIC))
            {
                return ENCYCLOPEDIC;
            }
        }

        i -= PDPTOOL_N;

        if (in_range(i, PDPTOOL_K))
        {
            return HIDDEN;
        }
    }
    else if (type_patterns_loaded == ROGERS_PATTERNS) {
        if (in_range(i, ROGERS_N))
        {

            if (in_range(i, ROGERS_N_NAME))
            {
                return NAME;
            }

            i -= ROGERS_N_NAME;

            if (in_range(i, ROGERS_N_VISUAL))
            {
                return VISUAL;
            }

            i -= ROGERS_N_VISUAL;

            if (in_range(i, ROGERS_N_PERCEPTUAL))
            {
                return PERCEPTUAL;
            }

            i -= ROGERS_N_PERCEPTUAL;

            if (in_range(i, ROGERS_N_FUNCTIONAL))
            {
                return FUNCTIONAL;
            }

            i -= ROGERS_N_FUNCTIONAL;

            if (in_range(i, ROGERS_N_ENCYCLOPEDIC))
            {
                return ENCYCLOPEDIC;
            }
        }

        i -= ROGERS_N;

        if (in_range(i, ROGERS_K))
        {
            return HIDDEN;
        }
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        if (in_range(i, TYLER_N))
        {

            if (in_range(i, TYLER_N_DISTINCTIVE))
            {
                return TYLER_DISTINCTIVE;
            }

            i -= TYLER_N_DISTINCTIVE;

            if (in_range(i, TYLER_N_SHARED))
            {
                return TYLER_SHARED;
            }

            i -= TYLER_N_SHARED;

            if (in_range(i, TYLER_N_FUNCTIONAL))
            {
                return TYLER_FUNCTIONAL;
            }
        }

        i -= TYLER_N;

        if (in_range(i, TYLER_K))
        {
            return HIDDEN;
        }
    }
    else {
        if (in_range(i, BM_N))
        {

            if (in_range(i, BM_N_NAME))
            {
                return NAME;
            }

            i -= BM_N_NAME;

            if (in_range(i, BM_N_VISUAL))
            {
                return VISUAL;
            }

            i -= BM_N_VISUAL;

            if (in_range(i, BM_N_PERCEPTUAL))
            {
                return PERCEPTUAL;
            }

            i -= BM_N_PERCEPTUAL;

            if (in_range(i, BM_N_FUNCTIONAL))
            {
                return FUNCTIONAL;
            }

            i -= BM_N_FUNCTIONAL;

            if (in_range(i, BM_N_ENCYCLOPEDIC))
            {
                return ENCYCLOPEDIC;
            }
        }

        i -= BM_N;

        if (in_range(i, BM_K))
        {
            return HIDDEN;
        }
    }


    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid index for state", 1, i+N);
    return -1;
}



char *get_name_string(int p)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (debug_range(p, P)) {
        return name_string[p];
    } else {
        throw_error(__FILE__, __LINE__, __FUNCTION__, "Invalid index for pattern", 1, p);
        return NULL;
    }

}

int name_to_pattern(int i, int p) //i is a name unit so as more than one name can map onto a pattern it must be translated
{
//     if (is_general_name(i) && is_general_name(p) && (get_category(i) == get_category(p))) {
//         return p;
//     }
//     return i;
    int q;
    for (q = 0; q < P; q++) {
        if(get_pattern_int(q, i) && get_pattern_int(p, i)) { //if both patterns have the same unit on
            if (p == q) { // they are both exactly the same pattern, so return either p or q
// 		printf("1\n");
                return p;
            }
            else if (is_general_name(q) && is_general_name(p) && (get_category(q) == get_category(p))) {  // it might also be the case that q and p are both general names and in the same category, i.e., same name, this time must return p
//    		printf("2\n");

                return p;
            }
            else {
// 	      printf("3\n");

                return q;
            }

        }   else if (get_pattern_int(q, i)) {
            return q;

        }
    }
    throw_error(__FILE__, __LINE__, __FUNCTION__, "Invalid index for pattern", 1, p);
    return -1;
}


int pattern_to_name_index(int p) //i is a name unit so as more than one name can map onto a pattern it must be translated
{
    //     if (is_general_name(i) && is_general_name(p) && (get_category(i) == get_category(p))) {
    //         return p;
    //     }
    //     return i;

    int i, n_name = (type_patterns_loaded == PDPTOOL_PATTERNS) ? PDPTOOL_N_NAME : BM_N_NAME;

    for (i = 0; i > n_name; i++) {
        if (get_pattern_int(p, i)) {
            return i;
        }
    }
    return -1;
}

enum category get_category(int p)
{
    if (type_patterns_loaded == PDPTOOL_PATTERNS) {
        if (p < 8) {
            return bird;
        }
        else if (p < 16) {
            return mammal;
        }
        else if (p < 24) {
            return vehicle;
        }
        else if (p < 32) {
            return hh_object;
        }
        else if (p < 40) {
            return tool;
        }
        else if (p < 48) {
            return fruit;
        }
    }
    else if (type_patterns_loaded == ROGERS_PATTERNS) {
        if (p < 8) {
            return bird;
        }
        else if (p < 16) {
            return mammal;
        }
        else if (p < 24) {
            return vehicle;
        }
        else if (p < 32) {
            return hh_object;
        }
        else if (p < 40) {
            return tool;
        }
        else if (p < 48) {
            return fruit;
        }
    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
//         throw_warning(__FILE__, __LINE__, __FUNCTION__, "You should not be asking for the category of these kinds of patterns (only domain is applicable), as such this function will return mammal for animate patterns and tool for inanimate patterns:", 1, p);
        if (p < 8) {
            return tool;
        }
        else {
            return mammal;
        }
    }
    else {
        int i;
        for (i = 0; i < N_CATEGORY_UNIT; i++) { // for all category units, which are mutually exclusive too
            if (get_pattern(p, N_DOMAIN_UNIT + i0_of(ENCYCLOPEDIC)+i) == 1.0) {
                return (enum category) i;
            }
        }
    }
    return -1;

}

int excluded_pattern(int p)
{
    return (EXCLUDE_FRUIT && is_fruit(p));
}

int get_n_fruit(void)
{
    int p, n_fruit = 0;
    for (p = 0; p < P; p++) {
        if (is_fruit(p)) {
            n_fruit++;
        }
    }
    return n_fruit;
}



int is_fruit(int p)
{
    return (get_category(p) == fruit);
}

int get_domain_n_pattern(int domain)
{
    if (type_patterns_loaded == PDPTOOL_PATTERNS) {

        return 24;

    }
    else if (type_patterns_loaded == ROGERS_PATTERNS) {

        return 24;

    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        //both domains have 8 patterns
        return 8;
    }
    else {
        int p, n_pattern = 0;

        if ((domain == 0) || (domain == 1)) {
            for (p = 0; p < P; p++) { // for all patterns in training set
                if (get_pattern(p, i0_of(ENCYCLOPEDIC)+domain) == 1.0) {
                    n_pattern++;
                }

                // 	printf("p = %i, domain bit = %f      n_pattern = %i, domain = %i\n", p, get_pattern(p, i0_of(ENCYCLOPEDIC)+domain), n_pattern, domain);

            }
            return n_pattern;
        }
    }

    return -1;

}

int get_domain_n_general_names(int domain)
{
    int p, n_pattern = 0;


    for (p = 0; p < P; p++) { // for all patterns in training set
        if (is_general_name(p) && (get_domain(p) == domain)) {
            n_pattern++;
        }
    }
    return n_pattern;


//     return -1;

}

int get_n_general_names(void)
{
    return (get_domain_n_general_names(0) + get_domain_n_general_names(1));
}
char domain_string[N_DOMAIN_UNIT][STRING] = {"Animal", "Artifact"};

int get_domain(int p)
{
    if (type_patterns_loaded == PDPTOOL_PATTERNS) {

        if (p < 16) {
            return 0;
        }
        else if (p < 40) {
            return 1;
        }
        else if (p < 48) {
            return 0;
        }

    }
    else    if (type_patterns_loaded == ROGERS_PATTERNS) {

        if (p < 16) {
            return 0;
        }
        else if (p < 40) {
            return 1;
        }
        else if (p < 48) {
            return 0;
        }

    }
    else if (type_patterns_loaded == TYLER_PATTERNS) {
        if (p < 8) {
            return 1;
        }
        return 0;

    }
    else {

        int i;
        for (i = 0; i < N_DOMAIN_UNIT; i++) { // for all category units, which are mutually exclusive too
            if (get_pattern(p, i0_of(ENCYCLOPEDIC)+i) == 1.0) {

                return i;
            }
        }
    }


//     throw_warning(__FILE__, __LINE__, __FUNCTION__, "Something seriously wrong with pattern(s) (probably not loaded yet/reset) specifically with", 1, p);

    return -1;

}



char category_string[6][STRING] = {"Bird", "Mammal", "Vehicle", "Household Object", "Tool", "Fruit"};
char *get_category_string(int p)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int c = (int) get_category(p);
    if (c != -1) {
        return category_string[c];
    } else {
        throw_error(__FILE__, __LINE__, __FUNCTION__, "Invalid index for category", 1, c);
        return NULL;
    }

}

char *get_domain_string(int p)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int d = get_domain(p);
    if (d != -1) {
        return domain_string[d];
    } else {
        throw_error(__FILE__, __LINE__, __FUNCTION__, "Invalid index for domain", 1, d);
        return NULL;
    }

}

// both in same category
void get_close_distractors(int p, int *array, int *length)
{
    int q, c = get_category(p);
    *length = 0;

    for(q = 0; q < P_MAX; q++) {
        if ((q != p) && (get_category(q) == c)) {
            if (!is_general_name(q))  {
                array[*length] = q;
                (*length)++;
            }
        }
    }

}

void get_patterns_that_share_a_name(int p, int *array, int *length) // returns p also !!!
{
    int q, c = get_category(p);
    *length = 0;
    if (is_general_name(p)) {
        for(q = 0; q < P_MAX; q++) {
            if (get_category(q) == c) {
                if (is_general_name(q))  {
                    array[*length] = q;
                    (*length)++;
                }
            }
        }
    } else {
        throw_error(__FILE__,  __LINE__, __FUNCTION__,"Cannot ask for shared names as pattern is base-level", 1, p);

    }

}

void get_n_close_trials(int *n)
{
    int p, q, c, d;
    n[0] = 0, n[1] = 0;
    for(p = 0; p < P_MAX; p++) {

        if (!is_general_name(p))  {

            if (!excluded_pattern(p)) {

                c = get_category(p);
                d = get_domain(p);
                for(q = 0; q < P_MAX; q++) {
                    if ((q != p) && (get_category(q) == c)) {
                        if (!is_general_name(q))  {

                            //       array[*length] = q;
                            (n[d])++;
                        }
                    }
                }
            }
        }

    }
}


// alternate categories in same domain
void get_distant_distractors(int p, int *array, int *length)
{
    int q, c = get_category(p), d = get_domain(p);
    *length = 0;

    for(q = 0; q < P_MAX; q++) {
        if ((get_category(q) != c)  && (get_domain(q) == d)) {
            if (!is_general_name(q))  {

                array[*length] = q;
                (*length)++;
            }
        }
    }
}

void get_n_distant_trials(int *n)
{
    int p, q, c, d;
    n[0] = 0, n[1] = 0;
    for(p = 0; p < P_MAX; p++) {
        if (!is_general_name(p))  {

            if (!excluded_pattern(p)) {

                c = get_category(p);
                d = get_domain(p);
                for(q = 0; q < P_MAX; q++) {
                    if ((get_category(q) != c)  && (get_domain(q) == d)) {
                        if (!is_general_name(q))  {

                            //       array[*length] = q;
                            (n[d])++;
                        }
                    }
                }
            }
        }
    }
}

// different domain
void get_unrelated_distractors(int p, int *array, int *length)
{
    int q, d = get_domain(p);
    *length = 0;

    for(q = 0; q < P_MAX; q++) {
        if ((get_domain(q) != d)) {
            if (!is_general_name(q))  {

                array[*length] = q;
                (*length)++;
            }
        }
    }
}

void get_n_unrelated_trials(int *n)
{
    int p, q, d;
    n[0] = 0, n[1] = 0;
    for(p = 0; p < P_MAX; p++) {
        if (!is_general_name(p))  {

            if (!excluded_pattern(p)) {
                d = get_domain(p);
                for(q = 0; q < P_MAX; q++) {
                    if ((get_domain(q) != d)) {
                        if (!is_general_name(q))  {

                            //       array[*length] = q;
                            (n[d])++;
                        }
                    }
                }
            }
        }
    }
}


void save_patterns_matlab(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, i;
    FILE *fp;
    fp = fopen(filename, "wt");
    fprintf(fp, "S = {");
    for (p = 0; p < P; p++) {
        fprintf(fp, "'%s'\n", get_name_string(p));
    }
    fprintf(fp, "};\n");


    int N_NAME = BPTT_N_NAME; //all the same
    int N_VISUAL =  pattern_get_n_visual();
    int N_VERBAL =   pattern_get_n_verbal();
    fprintf(fp, "All = [");

    for (p = 0; p < P; p++) {

        for (i = 0; i < N; i++) {

            fprintf(fp, "%d", get_pattern_int(p, i));

            if (i == N-1) {
                fprintf(fp, ";\n");
            }
            else
            {
                fprintf(fp, ", ");

            }

        }

    }
    fprintf(fp, "];\n");

    fprintf(fp, "Name = [");

    for (p = 0; p < P; p++) {

        for (i = 0; i < N_NAME; i++) {

            fprintf(fp, "%d", get_pattern_int(p, i));

            if (i == N_NAME-1) {
                fprintf(fp, ";\n");
            }
            else
            {
                fprintf(fp, ", ");

            }

        }

    }
    fprintf(fp, "];\n");

    fprintf(fp, "Visual = [");

    for (p = 0; p < P; p++) {

        for (i = 0; i < N_VISUAL; i++) {

            fprintf(fp, "%d", get_pattern_int(p, N_NAME +i));

            if (i == N_VISUAL-1) {
                fprintf(fp, ";\n");
            }
            else
            {
                fprintf(fp, ", ");

            }

        }

    }
    fprintf(fp, "];\n");

    fprintf(fp, "Verbal = [");

    for (p = 0; p < P; p++) {

        for (i = 0; i < N_VERBAL; i++) {

            fprintf(fp, "%d", get_pattern_int(p, N_NAME + N_VERBAL +i));

            if (i == N_VERBAL-1) {
                fprintf(fp, ";\n");
            }
            else
            {
                fprintf(fp, ", ");

            }

        }

    }
    fprintf(fp, "];\n");
    //   fprintf(fp, "\nEOF\n");
    fclose(fp);
}


int get_aoa_domain(int p) {

    if ((p >= 0) && (p < abs(P/2.0))) {
        return 0;
    }
    return 1;
}



void create_noisy_patterns(double gaussian_noise)
{
    // gaussian_noise can be zero
    int n, p, s, i, i_max;
    for (n = 0; n < N_NOISY; n++) {
        for (p = 0; p < P; p++) { //cycle through patterns
            for (s = 0; s < TYLER_N_SUBPATTERN; s++) { // and through each subpattern
                i_max = N_of(s);
                for (i = 0; i < i_max; i++) {
                    noisy_tyler_pattern[n][p][s][i] = tyler_pattern[p][s][i] + get_gaussian_noise(gaussian_noise);
                    if (noisy_tyler_pattern[n][p][s][i] > 1) noisy_tyler_pattern[n][p][s][i] = 1.0;
                    if (noisy_tyler_pattern[n][p][s][i] < 0) noisy_tyler_pattern[n][p][s][i] = 0.0;
                }
            }
        }
    }
}

void print_noisy_patterns(void)
{
    // gaussian_noise can be zero
    int n, p, s, i, i_max;
    for (n = 0; n < N_NOISY; n++) {
        for (p = 0; p < P; p++) { //cycle through patterns
            for (s = 0; s < TYLER_N_SUBPATTERN; s++) { // and through each subpattern
                i_max = N_of(s);
                for (i = 0; i < i_max; i++) {
                    printf("%f ", get_noisy_pattern_type_tyler_internal(n, p, s, i));
                }
            }

            printf("\n");

        }
        printf("\n");

    }
    printf("\n");

}



static double get_noisy_pattern_type_tyler_internal( int n, int p, int t, int i)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (debug_range(n, N_NOISY))
    {
        if (debug_range(p, P))
        {
            if (debug_range(i, N_of(t)))
            {
                if ((t == TYLER_DISTINCTIVE) || (t == TYLER_SHARED) || (t == TYLER_FUNCTIONAL))
                {
                    // 			int t = type_of(i);
                    // 			i -= i0_of(t);
                    // 				printf("p = %i, t = %i, i = %i\n", p, t, i);
                    return  noisy_tyler_pattern[n][p][t][i];
                }
                else
                {
                    throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid type t for pattern", 1, t);
                    return 0.0;
                }
            }
            else
            {
                throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index i for pattern", 1, i);
                return 0.0;
            }
        }
        else
        {
            throw_error(__FILE__,  __LINE__, __FUNCTION__,"Invalid array index p for pattern", 1, p);
            return 0.0;
        }
    }
    return 0.0;

}

void tyler_load_noisy_patterns(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    /*    int x = 0;*/
    FILE *fp = NULL;
    int n, p, s, i, i_max;
    fp = fopen(filename, "r");
    // //      init_patterns(BM);
    //     printf("%s\n", filename);



    //        printf("%s P = %i, N = %i %i\n",filename, P, N, BM_N);
    for (n = 0; n < N_NOISY; n++) {
        for (p = 0; p < P; p++) { //cycle through patterns
            for (s = 0; s < TYLER_N_SUBPATTERN; s++) { // and through each subpattern
                i_max = N_of(s);
                for (i = 0; i < i_max; i++) {
                    (fscanf(fp, "%f ", &noisy_tyler_pattern[n][p][s][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));

                }
                /*            x++;*/
            }


        }

    }



    if(fp) {
        fclose(fp);
    }



}


void tyler_save_noisy_patterns(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    /*    int x = 0;*/
    FILE *fp = NULL;
    int n, p, s, i, i_max;
    fp = fopen(filename, "w");
    // //      init_patterns(BM);
    //     printf("%s\n", filename);

    if (type_patterns_loaded != TYLER_PATTERNS) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Cannot save as noisy tyler patterns", 0);
    }

    //        printf("%s P = %i, N = %i %i\n",filename, P, N, BM_N);
    for (n = 0; n < N_NOISY; n++) {
        for (p = 0; p < P; p++) { //cycle through patterns
            for (s = 0; s < TYLER_N_SUBPATTERN; s++) { // and through each subpattern
                i_max = N_of(s);
                for (i = 0; i < i_max; i++) {
                    //                 (fscanf(fp, " %i", &tyler_pattern[p][s][i])) ? (NULL) : (throw_error(__FILE__,  __LINE__, __FUNCTION__,"Could not read patterns", 0));
                    fprintf(fp, "%f ", noisy_tyler_pattern[n][p][s][i]);

                }
                /*            x++;*/
            }

            fprintf(fp, "\n");

        }

    }



    if(fp) {
        fclose(fp);
    }



}


save_patterns_pdptool(char *filename)
{
  //to make patterns readable by PDPTOOL
  #if PRINT_FUNCTION
  printf("%s\n", __FUNCTION__);
  #endif
  int p, i;
  FILE *fp;
  fp = fopen(filename, "wt");  
  int N_NAME = BPTT_N_NAME; //all the same
  int N_VISUAL =  pattern_get_n_visual();
  int N_VERBAL =   pattern_get_n_verbal();
  printf("name: %d\n", N_NAME);
  printf("visual: %d\n", N_VISUAL);
  printf("verbal: %d\n", N_VERBAL);
  
  
  fprintf(fp, "rbp : 7 4\n\n"); //what it needs to say at the top
  for (p = 0; p < P; p++) {
    fprintf(fp, "%s\n", get_name_string(p));
    fprintf(fp, "H 1 3 name ");
    for (i = 0; i < N_NAME; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 name ");
    for (i = 0; i < N_NAME; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 verbal_descriptors ");
    for (i = 0; i < N_VERBAL; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 visual_features ");
    for (i = 0; i < N_VISUAL; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\nend\n\n");
  }
  for (p = 0; p < P; p++) {
    fprintf(fp, "%s\n", get_name_string(p));
    fprintf(fp, "H 1 3 verbal_descriptors ");
    for (i = 0; i < N_VERBAL; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 name ");
    for (i = 0; i < N_NAME; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 verbal_descriptors ");
    for (i = 0; i < N_VERBAL; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 visual_features ");
    for (i = 0; i < N_VISUAL; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\nend\n\n");
    
  }
  for (p = 0; p < P; p++) {
    fprintf(fp, "%s\n", get_name_string(p));
    fprintf(fp, "H 1 3 visual_features ");
    for (i = 0; i < N_VISUAL; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 name ");
    for (i = 0; i < N_NAME; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 verbal_descriptors ");
    for (i = 0; i < N_VERBAL; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\n");
    fprintf(fp, "T 6 2 visual_features ");
    for (i = 0; i < N_VISUAL; i++) {
      fprintf(fp, "%d ", get_pattern_int(p, i));
    }
    fprintf(fp, "\nend\n\n");
    
  }

  fclose(fp);
}
