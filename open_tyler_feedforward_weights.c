int tyler_feedforward_open_file(char *filename)
{
#if PRINT_FUNCTION
    printf("%s\n", __FUNCTION__);
#endif
    const char extension[4]=".dat";
    int i, j;

    for (i = 0; i < 4; i++) {

        if(filename[strlen(filename)-4+i] == extension[i]) {
            j = 1;

        } else {
            j = 0;
            break;

        }
    }
    if (j) {
        return tyler_feedforward_open_dat_file(filename);
    }


    FILE *fp = NULL;

    fp = fopen(filename, "r");
    if (!fscanf(fp, " %i", &tyler_iterations_so_far)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: tyler_iterations_so_far", 0);
        return 0;
    }

    if (!fscanf(fp, " %i %i", &p_min, &p_max)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variables: p_min and p_max", 0);
        return 0;
    }


    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {

            if (!fscanf(fp, " %lf", &weight_hidden_out[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_hidden_out[][]", 0);
                return 0;
            }
        }
    }


    for (i = 0; i < TYLER_FEEDFORWARD_IN; i++) {
        for (j = 0; j < TYLER_FEEDFORWARD_HIDDEN; j++) {
            if (!fscanf(fp, " %lf", &weight_in_hidden[i][j])) {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: weight_in_hidden[][]", 0);
                return 0;
            }
        }
    }


    for (i = 0; i < TYLER_FEEDFORWARD_HIDDEN; i++) {
        if (!fscanf(fp, " %lf", &bias_hidden[i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bias_hidden[]", 0);
            return 0;
        }
    }

    for (j = 0; j < TYLER_FEEDFORWARD_OUT; j++) {
        if (!fscanf(fp, " %lf", &bias_out[j])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: bias_out[]", 0);
            return 0;
        }
    }


    for (i = 0; i < tyler_iterations_so_far; i++) {
        if (!fscanf(fp, " %lf", &Total_Error[i])) {
            throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not read variable: Total_Error[]", 0);
            return 0;
        }
    }

    char *buf = malloc(sizeof(char *) * 4);;
    char *compare = "EOF\0";

    if (!fscanf(fp, " %s", buf)) {

        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not reach end of file", 0);
        return 0;
    }

    if(strncmp(buf, compare, 3) != 0) {


        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not reach end of file", 0);
        return 0;
    }

    if (fp) {
        fclose(fp);
    }

    return 1;

}