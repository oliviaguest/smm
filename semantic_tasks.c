#include "semantic_tasks.h"



// Input/Output naming responses to/from file
void save_naming_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, s, l, d;
    FILE *fp;
    printf("naming: %s\n", filename);

    fp = fopen(filename, "wt");
    fprintf(fp, "%d %d %d\n", naming_lesion_distribution, naming_lesion_level, naming_sample);
    for (d = 0; d < naming_lesion_distribution; d++) { // distribution of lesion
        for (l = 0; l < naming_lesion_level; l++) { // level of leasioning
            for (s = 0; s < naming_sample; s++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    fprintf(fp, "%lf %lf\n", naming_response[d][l][s][p][0], naming_response[d][l][s][p][1]);
                }
            }
        }
    }

    fprintf(fp, "\nEOF\n");
    fclose(fp);
}

void save_naming_responses_1(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, s, l, d;
    FILE *fp;
    char new_filename[256];
    sprintf(new_filename, "%s_1", filename);
    fp = fopen(new_filename, "wt");
    fprintf(fp, "%d %d %d\n", naming_lesion_distribution, naming_lesion_level, naming_sample);
    for (d = 0; d < naming_lesion_distribution; d++) { // distribution of lesion
        for (l = 0; l < naming_lesion_level; l++) { // level of leasioning
            for (s = 0; s < naming_sample; s++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    fprintf(fp, "%lf %lf\n", naming_response_1[d][l][s][p][0], naming_response_1[d][l][s][p][1]);
                }
            }
        }
    }

    fprintf(fp, "\nEOF\n");
    fclose(fp);
}


void save_naming_responses_alt(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, s, l, d;
    FILE *fp;
    char new_filename[256];
    sprintf(new_filename, "%s_alt", filename);
    fp = fopen(new_filename, "wt");
    fprintf(fp, "%d %d %d\n", naming_lesion_distribution, naming_lesion_level, naming_sample);
    for (d = 0; d < naming_lesion_distribution; d++) { // distribution of lesion
        for (l = 0; l < naming_lesion_level; l++) { // level of leasioning
            for (s = 0; s < naming_sample; s++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    fprintf(fp, "%lf %lf\n", naming_response_alt[d][l][s][p][0], naming_response_alt[d][l][s][p][1]);
                }
            }
        }
    }

    fprintf(fp, "\nEOF\n");
    fclose(fp);
}



void open_naming_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    printf("%s\n", filename);
    if (file_exists(filename)) {
        int p, s, l, d;
        FILE *fp;
        fp = fopen(filename, "r");
        if (!fscanf(fp, "%d %d %d\n", &naming_lesion_distribution, &naming_lesion_level, &naming_sample)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: naming_lesion_distribution, naming_lesion_level, naming_sample", 0);
            init_naming_response(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
        }

        for (d = 0; d < naming_lesion_distribution; d++) { // distribution of lesion
            for (l = 0; l < naming_lesion_level; l++) { // level of leasioning
                for (s = 0; s < naming_sample; s++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        if (!fscanf(fp, "%lf %lf\n", &naming_response[d][l][s][p][0], &naming_response[d][l][s][p][1])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: naming_response[][][][][]", 0);
                            init_naming_response(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
                            fclose(fp);
                            return;
                        }
                    }
                }
            }
        }
        fclose(fp);
    }
    else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open naming responses", 0);
    }
}


void open_naming_responses_1(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    char new_filename[256];
    sprintf(new_filename, "%s_1", filename);
    printf("%s\n", new_filename);

    if (file_exists(new_filename)) {
        int p, s, l, d;
        FILE *fp;
        fp = fopen(new_filename, "r");
        if (!fscanf(fp, "%d %d %d\n", &naming_lesion_distribution, &naming_lesion_level, &naming_sample)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: naming_lesion_distribution, naming_lesion_level, naming_sample", 0);
            init_naming_response_1(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
        }

        for (d = 0; d < naming_lesion_distribution; d++) { // distribution of lesion
            for (l = 0; l < naming_lesion_level; l++) { // level of leasioning
                for (s = 0; s < naming_sample; s++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        if (!fscanf(fp, "%lf %lf\n", &naming_response_1[d][l][s][p][0], &naming_response_1[d][l][s][p][1])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: naming_response_1[][][][][]", 0);
                            init_naming_response_1(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
                            fclose(fp);
                            return;
                        }
                    }
                }
            }
        }
        fclose(fp);
    }
    else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open naming responses", 0);
    }
}

void open_naming_responses_alt(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    char new_filename[256];
    sprintf(new_filename, "%s_alt", filename);
    printf("%s\n", new_filename);

    if (file_exists(new_filename)) {
        int p, s, l, d;
        FILE *fp;
        fp = fopen(new_filename, "r");
        if (!fscanf(fp, "%d %d %d\n", &naming_lesion_distribution, &naming_lesion_level, &naming_sample)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: naming_lesion_distribution, naming_lesion_level, naming_sample", 0);
            init_naming_response_alt(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
        }

        for (d = 0; d < naming_lesion_distribution; d++) { // distribution of lesion
            for (l = 0; l < naming_lesion_level; l++) { // level of leasioning
                for (s = 0; s < naming_sample; s++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        if (!fscanf(fp, "%lf %lf\n", &naming_response_alt[d][l][s][p][0], &naming_response_alt[d][l][s][p][1])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: naming_response_alt[][][][][]", 0);
                            init_naming_response_alt(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
                            fclose(fp);
                            return;
                        }
                    }
                }
            }
        }
        fclose(fp);
    }
    else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open naming responses", 0);
    }
}



// Input/Output sorting responses to/from file
void save_sorting_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, s, l, d;
    FILE *fp;
    fp = fopen(filename, "wt");
    fprintf(fp, "%d %d %d\n", sorting_lesion_distribution, sorting_lesion_level, sorting_sample);

    for (d = 0; d < sorting_lesion_distribution; d++) { // distribution of lesion
        for (l = 0; l < sorting_lesion_level; l++) { // level of leasioning
            for (s = 0; s < sorting_sample; s++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    fprintf(fp, "%d %d\n", sorting_response_given_visual[d][l][s][p][0], sorting_response_given_visual[d][l][s][p][1]);
                    fprintf(fp, "%d %d\n", sorting_response_given_name[d][l][s][p][0], sorting_response_given_name[d][l][s][p][1]);
                }
            }
        }
    }

    fprintf(fp, "\nEOF\n");
    fclose(fp);
}

void open_sorting_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    if (file_exists(filename)) {
        int p, s, l, d;
        FILE *fp;
        fp = fopen(filename, "r");
        if (!fscanf(fp, "%d %d %d\n", &sorting_lesion_distribution, &sorting_lesion_level, &sorting_sample)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: sorting_lesion_distribution, sorting_lesion_level, sorting_sample", 0);
            init_sorting_response(SORTING_LESION_DISTRIBUTION, SORTING_LESION_LEVEL, SORTING_SAMPLE);
        }

        for (d = 0; d < sorting_lesion_distribution; d++) { // distribution of lesion
            for (l = 0; l < sorting_lesion_level; l++) { // level of leasioning
                for (s = 0; s < sorting_sample; s++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        if (!fscanf(fp, "%d %d\n", &sorting_response_given_visual[d][l][s][p][0], &sorting_response_given_visual[d][l][s][p][1])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: sorting_response_given_visual[][][][][]", 0);
                            init_sorting_response(SORTING_LESION_DISTRIBUTION, SORTING_LESION_LEVEL, SORTING_SAMPLE);
                            fclose(fp);
                            return;
                        }

                        if (!fscanf(fp, "%d %d\n", &sorting_response_given_name[d][l][s][p][0], &sorting_response_given_name[d][l][s][p][1])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: sorting_response_given_name[][][][][]", 0);
                            init_sorting_response(SORTING_LESION_DISTRIBUTION, SORTING_LESION_LEVEL, SORTING_SAMPLE);
                            fclose(fp);
                            return;
                        }
                    }
                }
            }
        }
        fclose(fp);
    }
    else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open sorting responses", 0);
    }
}



// Input/Output naming responses to/from file
void save_word_to_picture_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, sample, level, distribution;
    FILE *fp;
    fp = fopen(filename, "wt");
    fprintf(fp, "%d %d %d\n", word_to_picture_lesion_distribution, word_to_picture_lesion_level, word_to_picture_sample);

    for (distribution = 0; distribution < word_to_picture_lesion_distribution; distribution++) { // distribution of lesion
        for (level = 0; level < word_to_picture_lesion_level; level++) { // level of leasioning
            for (sample = 0; sample < word_to_picture_sample; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    fprintf(fp, "%d %d %d\n", word_to_picture_close[distribution][level][sample][p], word_to_picture_distant[distribution][level][sample][p], word_to_picture_unrelated[distribution][level][sample][p]);
                }
            }
        }
    }

    fprintf(fp, "\nEOF\n");
    fclose(fp);
}

void open_word_to_picture_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif


    if (file_exists(filename)) {

        int p, sample, level, distribution;
        FILE *fp;


        fp = fopen(filename, "r");
        if (!fscanf(fp, "%d %d %d\n", &word_to_picture_lesion_distribution, &word_to_picture_lesion_level, &word_to_picture_sample)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: word_to_picture_lesion_distribution, word_to_picture_lesion_level, word_to_picture_sample", 0);
            init_word_to_picture_response(WORD_TO_PICTURE_LESION_DISTRIBUTION, WORD_TO_PICTURE_LESION_LEVEL, WORD_TO_PICTURE_SAMPLE);
        }

        for (distribution = 0; distribution < word_to_picture_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < word_to_picture_lesion_level; level++) { // level of leasioning
                for (sample = 0; sample < word_to_picture_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern

                        if (!fscanf(fp, "%d %d %d\n", &word_to_picture_close[distribution][level][sample][p], &word_to_picture_distant[distribution][level][sample][p],  &word_to_picture_unrelated[distribution][level][sample][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: word_to_picture_close[][][][], word_to_picture_distant[][][][], word_to_picture_unrelated[][][][]", 0);
                            init_word_to_picture_response(WORD_TO_PICTURE_LESION_DISTRIBUTION, WORD_TO_PICTURE_LESION_LEVEL, WORD_TO_PICTURE_SAMPLE);
                            fclose(fp);
                            return;

                        }

                    }
                }
            }
        }
        fclose(fp);
    }
    else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open word to picture responses file:", 0);

    }
}


void save_drawing_and_copying_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, s, l, d;
    FILE *fp;
    fp = fopen(filename, "wt");
    fprintf(fp, "%d %d %d\n", drawing_and_copying_lesion_distribution, drawing_and_copying_lesion_level, drawing_and_copying_sample);
    for (d = 0; d < drawing_and_copying_lesion_distribution; d++) { // distribution of lesion
        for (l = 0; l < drawing_and_copying_lesion_level; l++) { // level of leasioning
            for (s = 0; s < drawing_and_copying_sample; s++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    fprintf(fp, "%d %d\n", drawing_omission[d][l][s][p], drawing_intrusion[d][l][s][p]);
                    fprintf(fp, "%d %d\n", copying_omission[d][l][s][p], copying_intrusion[d][l][s][p]);

                }
            }
        }
    }

    fprintf(fp, "\nEOF\n");
    fclose(fp);
}

void open_drawing_and_copying_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif

    printf("%s\n", filename);

    if (file_exists(filename)) {

        int p, s, l, d;
        FILE *fp;

        fp = fopen(filename, "r");


        if (!fscanf(fp, "%d %d %d\n", &drawing_and_copying_lesion_distribution, &drawing_and_copying_lesion_level, &drawing_and_copying_sample)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: drawing_and_copying_lesion_distribution, drawing_and_copying_lesion_level, drawing_and_copying_sample", 0);
            init_drawing_and_copying_response(DRAWING_AND_COPYING_LESION_DISTRIBUTION, DRAWING_AND_COPYING_LESION_LEVEL, DRAWING_AND_COPYING_SAMPLE);
            fclose(fp);
            return;
        }

        for (d = 0; d < drawing_and_copying_lesion_distribution; d++) { // distribution of lesion
            for (l = 0; l < drawing_and_copying_lesion_level; l++) { // level of leasioning
                for (s = 0; s < drawing_and_copying_sample; s++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        if (!fscanf(fp, "%d %d\n", &drawing_omission[d][l][s][p], &drawing_intrusion[d][l][s][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: drawing_omission[][][][], drawing_intrusion[][][][]", 0);
                            init_drawing_and_copying_response(DRAWING_AND_COPYING_LESION_DISTRIBUTION, DRAWING_AND_COPYING_LESION_LEVEL, DRAWING_AND_COPYING_SAMPLE);
                            fclose(fp);
                            return;
                        }
                        if (!fscanf(fp, "%d %d\n", &copying_omission[d][l][s][p], &copying_intrusion[d][l][s][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: copying_omission[][][][], copying_intrusion[][][][]", 0);
                            init_drawing_and_copying_response(DRAWING_AND_COPYING_LESION_DISTRIBUTION, DRAWING_AND_COPYING_LESION_LEVEL, DRAWING_AND_COPYING_SAMPLE);
                            fclose(fp);
                            return;
                        }


                    }
                }
            }
        }

        fclose(fp);
    }
    else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open drawing and copying responses", 0);

    }
}



void init_naming_response(int d, int l, int s)
{
    naming_lesion_distribution = d, naming_lesion_level = l, naming_sample = s;
    int p, sample, level, distribution;
    for (distribution = 0; distribution < NAMING_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
        for (level = 0; level < NAMING_LESION_LEVEL; level++) { // level of leasioning
            for (sample = 0; sample < NAMING_SAMPLE; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    naming_response[distribution][level][sample][p][0] = -1.0;	// pattern output
                    naming_response[distribution][level][sample][p][1] = 0.0;	// output activation
                }
            }
        }
    }
}


void init_naming_response_1(int d, int l, int s)
{
    naming_lesion_distribution = d, naming_lesion_level = l, naming_sample = s;
    int p, sample, level, distribution;
    for (distribution = 0; distribution < NAMING_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
        for (level = 0; level < NAMING_LESION_LEVEL; level++) { // level of leasioning
            for (sample = 0; sample < NAMING_SAMPLE; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    naming_response_1[distribution][level][sample][p][0] = -1.0;	// pattern output
                    naming_response_1[distribution][level][sample][p][1] = 0.0;	// output activation
                }
            }
        }
    }
}


void init_naming_response_alt(int d, int l, int s)
{
    naming_lesion_distribution = d, naming_lesion_level = l, naming_sample = s;
    int p, sample, level, distribution;
    for (distribution = 0; distribution < NAMING_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
        for (level = 0; level < NAMING_LESION_LEVEL; level++) { // level of leasioning
            for (sample = 0; sample < NAMING_SAMPLE; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    naming_response_alt[distribution][level][sample][p][0] = -1.0;	// pattern output
                    naming_response_alt[distribution][level][sample][p][1] = 0.0;	// output activation
                }
            }
        }
    }
}



// void print_naming_responses(void)
// {
//
//     int p, sample, level, distribution;
//
//     printf("\nConfrontation Naming\nInput Pattern\t\tOutput Pattern\t\tOutput Activation\n");
//     for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
//         for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
//             for (p = 0; p < P; p++) { // pattern
//                 printf("%i\t", p);
//                 for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
//                     printf("\t\t%i\t\t\t%f", (int) naming_response[distribution][level][sample][p][0], naming_response[distribution][level][sample][p][1]);
//                 }
//                 printf("\n");
//             }
//         }
//     }
// }





void init_sorting_response(int d, int l, int s)
{
    int p, sample, level, distribution;
    sorting_lesion_distribution = d, sorting_lesion_level = l, sorting_sample = s;

    for (distribution = 0; distribution < SORTING_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
        for (level = 0; level < SORTING_LESION_LEVEL; level++) { // level of leasioning
            for (sample = 0; sample < SORTING_SAMPLE; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    sorting_response_given_visual[distribution][level][sample][p][0] = -1;	// general domain
                    sorting_response_given_visual[distribution][level][sample][p][1] = -1; 	// specific category

                    sorting_response_given_name[distribution][level][sample][p][0] = -1;	// general domain
                    sorting_response_given_name[distribution][level][sample][p][1] = -1;  	// specific category
                }
            }
        }
    }
}
















void init_word_to_picture_response(int d, int l, int s)
{
    int p, sample, level, distribution;
    word_to_picture_lesion_distribution = d, word_to_picture_lesion_level = l, word_to_picture_sample = s;

    for (distribution = 0; distribution < WORD_TO_PICTURE_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
        for (level = 0; level < WORD_TO_PICTURE_LESION_LEVEL; level++) { // level of leasioning
            for (sample = 0; sample < WORD_TO_PICTURE_SAMPLE; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    distance_target[distribution][level][sample][p] = (double) sqrt(K);
// 			    p_target[distribution][level][sample][p] = -1;
// 			    distance_close_distractor[distribution][level][sample][p] = (double) sqrt(K);
// 			    p_close_distractor[distribution][level][sample][p] = -1;
// 			    distance_distant_distractor[distribution][level][sample][p] = (double) sqrt(K);
// 			    p_distant_distractor[distribution][level][sample][p] = -1;
// 			    distance_unrelated_distractor[distribution][level][sample][p] = (double) sqrt(K);
// 			    p_unrelated_distractor[distribution][level][sample][p] = -1;
// 			    word_to_picture_correct[distribution][level][sample][p] = 0;
                    word_to_picture_close[distribution][level][sample][p] = 0;
                    word_to_picture_distant[distribution][level][sample][p] = 0;
                    word_to_picture_unrelated[distribution][level][sample][p] = 0;


                }
            }
        }
    }
}









// void print_word_to_picture_responses(void)
// {

/*
    int p, sample, level, distribution;

    // printf("\nWord-to-Picture Matching\nInput Pattern\t\tOutput Pattern\t\tOutput Activation\n");
    for (distribution = 0; distribution < WORD_TO_PICTURE_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
	    for (level = 0; level < WORD_TO_PICTURE_LESION_LEVEL; level++) { // level of leasioning
		for (p = 0; p < P; p++) { // pattern
		      printf("\nWord-to-Picture Matching\nVisual Target Condition\n");
		      printf("Pattern\t\tResponse\tDistance\n");
		      for (p = 0; p < P; p++) {
			printf("%i\t\t%i\t\t%f\n", p, p_target[distribution][level][sample][p], distance_target[distribution][level][sample][p]);
		      }
		      printf("\nClose Condition\n");
		      printf("Pattern\t\tResponse\tDistance\n");
		      for (p = 0; p < P; p++) {
			printf("%i\t\t%i\t\t%f\n", p, p_close_distractor[distribution][level][sample][p], distance_close_distractor[distribution][level][sample][p]);
		      }
		      printf("\nDistant Condition\n");
		      printf("Pattern\t\tResponse\tDistance\n");
		      for (p = 0; p < P; p++) {
			printf("%i\t\t%i\t\t%f\n", p, p_distant_distractor[distribution][level][sample][p], distance_distant_distractor[distribution][level][sample][p]);
		      }
		      printf("\nUnrelated Condition\n");
		      printf("Pattern\t\tResponse\tDistance\n");
		      for (p = 0; p < P; p++) {
			printf("%i\t\t%i\t\t%f\n", p, p_unrelated_distractor[distribution][level][sample][p], distance_unrelated_distractor[distribution][level][sample][p]);
		      }
		}
	    }
    }*/
// }







// Drawing and Delayed Copying task


void init_drawing_and_copying_response(int d, int l, int s)
{
    drawing_and_copying_lesion_distribution = d, drawing_and_copying_lesion_level = l, drawing_and_copying_sample = s;
    int p, sample, level, distribution;
    for (distribution = 0; distribution < DRAWING_AND_COPYING_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
        for (level = 0; level < DRAWING_AND_COPYING_LESION_LEVEL; level++) { // level of leasioning
            for (sample = 0; sample < DRAWING_AND_COPYING_SAMPLE; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P_MAX; p++) { // pattern
                    /*			drawing_correct[distribution][level][sample][p] = 0;*/
                    drawing_omission[distribution][level][sample][p] = 0;
                    drawing_intrusion[distribution][level][sample][p] = 0;
                    /*			copying_correct[distribution][level][sample][p] = 0;*/
                    copying_omission[distribution][level][sample][p] = 0;
                    copying_intrusion[distribution][level][sample][p] = 0;
                }
            }
        }
    }


}

void init_tyler_response(int d, int l, int s)
{
    tyler_lesion_distribution = d, tyler_lesion_level = l, tyler_sample = s;
    int p, sample, level, distribution;
    for (distribution = 0; distribution < TYLER_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
        for (level = 0; level < TYLER_LESION_LEVEL; level++) { // level of leasioning
            for (sample = 0; sample < TYLER_SAMPLE; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P_MAX; p++) { // pattern
                    tyler_response[distribution][level][sample][p] = -1;

                    tyler_distinctive_error[distribution][level][sample][p] = 0;
                    tyler_shared_error[distribution][level][sample][p] = 0;
                    tyler_functional_error[distribution][level][sample][p] = 0;
                }
            }
        }
    }
}



void init_aoa_response(int d, int l, int s)
{
    aoa_lesion_distribution = d, aoa_lesion_level = l, aoa_sample = s;
    int p, sample, level, distribution;
    for (distribution = 0; distribution < AOA_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
        for (level = 0; level < AOA_LESION_LEVEL; level++) { // level of leasioning
            for (sample = 0; sample < AOA_SAMPLE; sample++) { // to ensure network in "right" attractor
                for (p = 0; p < P_MAX; p++) { // pattern
                    aoa_response[distribution][level][sample][p] = -1;
                    aoa_error[distribution][level][sample][p] = 0;
// 		    aoa_distinctive_error[distribution][level][sample][p] = 0;
// 		    aoa_shared_error[distribution][level][sample][p] = 0;
// 		    aoa_functional_error[distribution][level][sample][p] = 0;
                }
            }
        }
    }
}

// void init_tyler_response_network(int n, int d, int l, int s)
// {
//     tyler_network = n, tyler_lesion_distribution = d, tyler_lesion_level = l, tyler_sample = s;
//     int network, p, sample, level, distribution;
//     for (network = 0; network < TYLER_NETWORK; network++) { // distribution of lesion
//         for (distribution = 0; distribution < TYLER_LESION_DISTRIBUTION; distribution++) { // distribution of lesion
//             for (level = 0; level < TYLER_LESION_LEVEL; level++) { // level of leasioning
//                 for (sample = 0; sample < TYLER_SAMPLE; sample++) { // to ensure network in "right" attractor
//                     for (p = 0; p < P_MAX; p++) { // pattern
//                         tyler_response_network[network][distribution][level][sample][p] = -1;
//
//                         tyler_distinctive_error_network[network][distribution][level][sample][p] = 0;
//                         tyler_shared_error_network[network][distribution][level][sample][p] = 0;
//                         tyler_functional_error_network[network][distribution][level][sample][p] = 0;
//                     }
//                 }
//             }
//         }
//     }
// }
//


void save_tyler_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, s, l, d;
    FILE *fp;
    printf("tyler: %s\n", filename);

    fp = fopen(filename, "wt");
    fprintf(fp, "%d %d %d\n", tyler_lesion_distribution, tyler_lesion_level, tyler_sample);
    for (d = 0; d < tyler_lesion_distribution; d++) { // distribution of lesion
        for (l = 0; l < tyler_lesion_level; l++) { // level of leasioning
            for (s = 0; s < tyler_sample; s++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    fprintf(fp, "%d\n", tyler_response[d][l][s][p]);
                    fprintf(fp, "%lf\n", tyler_distinctive_error[d][l][s][p]);
                    fprintf(fp, "%lf\n", tyler_shared_error[d][l][s][p]);
                    fprintf(fp, "%lf\n", tyler_functional_error[d][l][s][p]);

                }
            }
        }
    }

    fprintf(fp, "\nEOF\n");
    fclose(fp);
}

void open_tyler_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
//     printf("%s\n", filename);
    if (file_exists(filename)) {
        int p, s, l, d;
        FILE *fp;
        fp = fopen(filename, "r");
        if (!fscanf(fp, "%d %d %d\n", &tyler_lesion_distribution, &tyler_lesion_level, &tyler_sample)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: tyler_lesion_distribution, tyler_lesion_level, tyler_sample", 0);
            init_tyler_response(TYLER_LESION_DISTRIBUTION, TYLER_LESION_LEVEL, TYLER_SAMPLE);
        }

        for (d = 0; d < tyler_lesion_distribution; d++) { // distribution of lesion
            for (l = 0; l < tyler_lesion_level; l++) { // level of leasioning
                for (s = 0; s < tyler_sample; s++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        if (!fscanf(fp, "%d\n", &tyler_response[d][l][s][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_response[][][][][]", 0);
                            init_tyler_response(TYLER_LESION_DISTRIBUTION, TYLER_LESION_LEVEL, TYLER_SAMPLE);
                            fclose(fp);
                            return;
                        }
                        if (!fscanf(fp, "%lf\n", &tyler_distinctive_error[d][l][s][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_distinctive_error[][][][][]", 0);
                            init_tyler_response(TYLER_LESION_DISTRIBUTION, TYLER_LESION_LEVEL, TYLER_SAMPLE);
                            fclose(fp);
                            return;
                        }
                        if (!fscanf(fp, "%lf\n", &tyler_shared_error[d][l][s][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_shared_error[][][][][]", 0);
                            init_tyler_response(TYLER_LESION_DISTRIBUTION, TYLER_LESION_LEVEL, TYLER_SAMPLE);
                            fclose(fp);
                            return;
                        }
                        if (!fscanf(fp, "%lf\n", &tyler_functional_error[d][l][s][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_functional_error[][][][][]", 0);
                            init_tyler_response(TYLER_LESION_DISTRIBUTION, TYLER_LESION_LEVEL, TYLER_SAMPLE);
                            fclose(fp);
                            return;
                        }


                    }
                }
            }
        }
        fclose(fp);
    }
    else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open tyler responses", 0);
    }
}




void save_aoa_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    int p, s, l, d;
    FILE *fp;
    printf("aoa: %s\n", filename);

    fp = fopen(filename, "wt");
    printf("%d %d %d\n", aoa_lesion_distribution, aoa_lesion_level, aoa_sample);

    fprintf(fp, "%d %d %d\n", aoa_lesion_distribution, aoa_lesion_level, aoa_sample);
    for (d = 0; d < aoa_lesion_distribution; d++) { // distribution of lesion
        for (l = 0; l < aoa_lesion_level; l++) { // level of leasioning
            for (s = 0; s < aoa_sample; s++) { // to ensure network in "right" attractor
                for (p = 0; p < P; p++) { // pattern
                    fprintf(fp, "%d\n", aoa_response[d][l][s][p]);
                    fprintf(fp, "%lf\n", aoa_error[d][l][s][p]);
// 		    fprintf(fp, "%lf\n", aoa_distinctive_error[d][l][s][p]);
// 		    fprintf(fp, "%lf\n", aoa_shared_error[d][l][s][p]);
// 		    fprintf(fp, "%lf\n", aoa_functional_error[d][l][s][p]);

                }
            }
        }
    }

    fprintf(fp, "\nEOF\n");
    fclose(fp);
}

void open_aoa_responses(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    printf("%s\n", filename);
    if (file_exists(filename)) {
        int p, s, l, d;
        FILE *fp;
        fp = fopen(filename, "r");
        if (!fscanf(fp, "%d %d %d\n", &aoa_lesion_distribution, &aoa_lesion_level, &aoa_sample)) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: aoa_lesion_distribution, aoa_lesion_level, aoa_sample", 0);
            init_aoa_response(AOA_LESION_DISTRIBUTION, AOA_LESION_LEVEL, AOA_SAMPLE);
        }

        for (d = 0; d < aoa_lesion_distribution; d++) { // distribution of lesion
            for (l = 0; l < aoa_lesion_level; l++) { // level of leasioning
                for (s = 0; s < aoa_sample; s++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        if (!fscanf(fp, "%d\n", &aoa_response[d][l][s][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: aoa_response[][][][]", 0);
                            init_aoa_response(AOA_LESION_DISTRIBUTION, AOA_LESION_LEVEL, AOA_SAMPLE);
                            fclose(fp);
                            return;
                        }
                        if (!fscanf(fp, "%lf\n", &aoa_error[d][l][s][p])) {
                            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: aoa_error[][][][]", 0);
                            init_aoa_response(AOA_LESION_DISTRIBUTION, AOA_LESION_LEVEL, AOA_SAMPLE);
                            fclose(fp);
                            return;
                        }
// 			if (!fscanf(fp, "%lf\n", &aoa_distinctive_error[d][l][s][p])) {
// 			  throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: aoa_distinctive_error[][][][][]", 0);
// 			  init_aoa_response(AOA_LESION_DISTRIBUTION, AOA_LESION_LEVEL, AOA_SAMPLE);
// 			  fclose(fp);
// 			  return;
// 			}
// 			if (!fscanf(fp, "%lf\n", &aoa_shared_error[d][l][s][p])) {
// 			  throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: aoa_shared_error[][][][][]", 0);
// 			  init_aoa_response(AOA_LESION_DISTRIBUTION, AOA_LESION_LEVEL, AOA_SAMPLE);
// 			  fclose(fp);
// 			  return;
// 			}
// 			if (!fscanf(fp, "%lf\n", &aoa_functional_error[d][l][s][p])) {
// 			  throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: aoa_functional_error[][][][][]", 0);
// 			  init_aoa_response(AOA_LESION_DISTRIBUTION, AOA_LESION_LEVEL, AOA_SAMPLE);
// 			  fclose(fp);
// 			  return;
// 			}


                    }
                }
            }
        }
        fclose(fp);
    }
    else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open aoa responses", 0);
    }
}

/*
void save_tyler_responses_network(char *filename)
{
#if PRINT_FUNCTION
   printf("%s\n", __FUNCTION__);
#endif
   int p, s, l, d, n;
   FILE *fp;
   printf("tyler: %s\n", filename);

   fp = fopen(filename, "wt");
   fprintf(fp, "%d %d %d %d\n", tyler_network, tyler_lesion_distribution, tyler_lesion_level, tyler_sample);
   for (n = 0; n < tyler_network; n++) { // distribution of lesion
       for (d = 0; d < tyler_lesion_distribution; d++) { // distribution of lesion
           for (l = 0; l < tyler_lesion_level; l++) { // level of leasioning
               for (s = 0; s < tyler_sample; s++) { // to ensure network in "right" attractor
                   for (p = 0; p < P; p++) { // pattern
                       fprintf(fp, "%d\n", tyler_response_network[n][d][l][s][p]);
                       fprintf(fp, "%lf\n", tyler_distinctive_error_network[n][d][l][s][p]);
                       fprintf(fp, "%lf\n", tyler_shared_error_network[n][d][l][s][p]);
                       fprintf(fp, "%lf\n", tyler_functional_error_network[n][d][l][s][p]);

                   }

               }
           }
       }
   }

   fprintf(fp, "\nEOF\n");
   fclose(fp);
}

void open_tyler_responses_network(char *filename)
{
#if PRINT_FUNCTION
   printf("%s\n", __FUNCTION__);
#endif
   printf("%s\n", filename);
   if (file_exists(filename)) {
       int p, s, l, d, n;
       FILE *fp;
       fp = fopen(filename, "r");
       if (!fscanf(fp, "%d %d %d %d\n", &tyler_network, &tyler_lesion_distribution, &tyler_lesion_level, &tyler_sample)) {
           throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: tyler_network, tyler_lesion_distribution, tyler_lesion_level, tyler_sample", 0);
           init_tyler_response(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
       }

       for (n = 0; n < tyler_network; n++) { // distribution of lesion
           for (d = 0; d < tyler_lesion_distribution; d++) { // distribution of lesion
               for (l = 0; l < tyler_lesion_level; l++) { // level of leasioning
                   for (s = 0; s < tyler_sample; s++) { // to ensure network in "right" attractor
                       for (p = 0; p < P; p++) { // pattern
                           if (!fscanf(fp, "%d\n", &tyler_response_network[n][d][l][s][p])) {
                               throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_response_network[][][][][][]", 0);
                               init_tyler_response(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
                               fclose(fp);
                               return;
                           }
                           if (!fscanf(fp, "%lf\n", &tyler_distinctive_error_network[n][d][l][s][p])) {
                               throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_distinctive_error_network[][][][][]", 0);
                               init_tyler_response(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
                               fclose(fp);
                               return;
                           }
                           if (!fscanf(fp, "%lf\n", &tyler_shared_error_network[n][d][l][s][p])) {
                               throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_shared_error_network[][][][][]", 0);
                               init_tyler_response(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
                               fclose(fp);
                               return;
                           }
                           if (!fscanf(fp, "%lf\n", &tyler_functional_error_network[n][d][l][s][p])) {
                               throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_functional_error_network[][][][][]", 0);
                               init_tyler_response(NAMING_LESION_DISTRIBUTION, NAMING_LESION_LEVEL, NAMING_SAMPLE);
                               fclose(fp);
                               return;
                           }


                       }
                   }
               }
           }
       }
       fclose(fp);
   }
   else {
       throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open tyler responses", 0);
   }
}

             */