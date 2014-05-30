


/** Implementation of modelling work done originally by
 * Rogers at al. (2004) and Lambon Ralph (2007),
 * and by Tyler (2001).
 *
 * Olivia Guest, 2012
 *
 * Feel free to contact me with queries or
 * any potential bugs.
 * Email: guest.olivia@gmail.com
 **/






#include  <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include  <string.h>
#include  <sys/types.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>



#include "unrolled_network_viewer.h"
#include "graph.h"
#include "xbm.h"
#include "network.h"
#include "error_view.h"
#include "attractor_view.h"
#include "lib_cairox.h"
#include "semantic_tasks.h"

#include "naming_graph.h"
// #include "naming_bar_chart.h"
#include "sorting_graph.h"
#include "word_to_picture_graph.h"
#include "drawing_and_copying_graph.h"
#include "tyler_graph.h"
#include "aoa_graph.h"

// #define TEST_N_PATTERN  0
//
// #define TEST_NOISE 0
// #define TARGET_N_PATTERN n
// // can be n to make it the same
// #define TARGET_NOISE  0
// //was random_int(0,3)
#define FAMILIAR_PATTERNS 1

#define TEST_N_PATTERN  1
#define TEST_NOISE 0
#define TARGET_N_PATTERN 1
#define TARGET_NOISE  0




int accelerate_momentum = 0;
// Box Properties, used to pack widgets into.
int expand = 0; // expand controls whether the widgets are laid out in the box to fill in all the extra space in the box so the box is expanded to fill the area allotted to it
int fill = 0; // fill controls whether the extra space is allocated to the widgets themselves
int spacing = 2; // spacing is added between widgets.
int padding = 2; // padding is added on either side of a widget.
int vertical_padding = 2;
int automated_training = 0;
int automated_testing = 0;


// int noise = 0;
// double noise = 0.00;

int weight_update_type = BPTT_WEIGHT_UPDATE_BATCH;

// char *auto_name =  "no_noise_";

typedef struct {
    GtkWidget *fixed;
    GtkWidget **check_button;
} check_button_struct;

typedef struct {
    GtkWidget *train;
    GtkWidget *test;
    GtkWidget *networks;
    GtkWidget *text;
    GtkWidget *pop_up;
    int aoa;
} networks_struct;
int column_width = 75;

static const char aoa_auto_name[2][6] = {"no_aoa", "aoa"};

// double auto_momentum = 0.0;
// double auto_learning_rate = 0.25;

//tyler_recurrent_automated_weights_init_to_one_";

// "tyler_feedforward_automated_tyler_training_cond_pattern_wise_";
// tyler_feedforward_automated_500_epochs_299_tyler_task_5.pdf

// "tyler_feedforward_automated_alt_eq_fixed_biases_";

// "tyler_feedforward_automated_"
// // "tyler_feedforward_automated_alt_eq_"

int data_type = MODEL;

int aoa =0;
int aoa_train = 0;


#define PRINT_FUNC 0

#define ZEROING 0
#define NOISE 1

int lesion_type = ZEROING;
int lesion_locus = 0;

int decay_learning_rate = 0;

#define ACTIVATION_THRESHOLD 0.5
double activation_threshold = ACTIVATION_THRESHOLD;

//For opening new windows
char ** global_argv;


int net_type = TYLER_FEEDFORWARD;
int prev_net_type = TYLER_FEEDFORWARD;
GtkWidget *window;
GtkWidget *b_box;

GtkWidget *graph;
GtkWidget *naming_graph;
GtkWidget *sorting_graph;
GtkWidget *word_to_picture_graph;
GtkWidget *drawing_and_copying_graph;
GtkWidget *tyler_graph, *tyler_graph_hh, *tyler_graph_ho, *tyler_graph_ih, *tyler_graph_networks;
GtkWidget *aoa_graph, *aoa_graph_hh, *aoa_graph_ho, *aoa_graph_ih, *aoa_graph_networks;
GtkWidget *notebook;
// GtkWidget *table;
GtkWidget *vertical_box;
GtkWidget *weights_button, *patterns_button;
GtkWidget *naming_label, *naming_label2, *naming_label3, *naming_scale, *naming_combo_box, *naming_combo_box_2;
GtkWidget *word_to_picture_label, *word_to_picture_label2, *word_to_picture_combo_box, *word_to_picture_combo_box2;
GtkWidget *drawing_and_copying_label, *drawing_and_copying_combo_box;
GtkWidget *error_view, *patterns_viewer, *unrolled_network, *attractor_view;
GtkWidget *unrolled_network_viewer_combo_box, *unrolled_network_viewer_label;
GtkWidget *xbm_p, *xbm_p_label, *xbm_p_combo_box;
GtkWidget *learning_rate_spin_button;
GtkWidget *momentum_spin_button;
GtkWidget *noise_spin_button;
GtkWidget *training_algo_combo_box, *settle_combo_box, *anneal_combo_box, *pattern_combo_box;
GtkWidget *stage_spin_button, *pattern_spin_button;
GtkWidget *distribution_spin_button, *level_spin_button, *sample_spin_button;
GtkWidget *error_combo_box;
GtkWidget *attractor_view_button, *attractor_view_button2, *attractor_view_label, *attractor_view_label2, *attractor_view_combo_box;
GtkWidget *recent_chooser_menu;
GtkWidget *net_type_combo_box;
GtkWidget *real_time_item, *patient_data_item, *rogers_data_item;
GtkWidget *train_button, *iterations_spin_button;
GtkWidget *tyler_combo_box, *tyler_label;
GtkWidget *aoa_combo_box, *aoa_label, *aoa_combo_box;
GtkWidget *t_clamped_label, *t_clamped_spin_button;
GtkWidget *lesion_locus_combo_box;


GtkWidget *naming_item, *alt_naming_item, *clear_naming_item, *save_naming_item;
GtkWidget *sorting_item, *clear_sorting_item, *save_sorting_item;
GtkWidget *word_to_picture_item, *clear_word_to_picture_item, *save_word_to_picture_item;
GtkWidget *drawing_item, *clear_drawing_item, *save_drawing_item;
GtkWidget *tyler_item, *clear_tyler_item, *save_tyler_item;
GtkWidget *aoa_item, *clear_aoa_item, *save_aoa_item;


// #define STATES_PAGE			0
// #define GRAPH_PAGE			1
// #define ERROR_PAGE			2
// #define P_PAGE				3
// #define PATTERNS_PAGE			4
// #define NAMING_PAGE			5
// #define SORTING_PAGE			6
// #define WORD_TO_PICTURE_PAGE		7
// #define DRAWING_AND_COPYING_PAGE	8


gboolean real_time = FALSE;

char default_folder_for_saving[2] = "~/";
static char open_filename[256] = "\0";
char open_path[128]  = "~/";
char open_name[128];
char window_title[128];

void send_error_to_graph(void);
void send_error_to_error_view(void);
void send_attractor_to_attractor_view(void);
void reset_semantic_tests(void);
void bptt_train_patterns_serially(void);
void tyler_train_patterns_serially(void);
void init_train_flags(void);
void tyler_feedforward_train_patterns_serially(void);
void update_error_combo_box(void);
void update_training_algo_combo_box(void);
#define STRING 256
// char path[64] = "./CategoryNames/New/";
// char file_name[64] = "";
// char file_name_no_extension[64] = "mcclelland_ta_run_3";
char naming_file_name[STRING] = "\0";
char sorting_file_name[STRING] = "\0";
char word_to_picture_file_name[STRING] = "\0";
char drawing_and_copying_file_name[STRING] = "\0";
char tyler_task_file_name[STRING] = "\0";
char tyler_task_hh_file_name[STRING] = "\0";
char tyler_task_ih_file_name[STRING] = "\0";
char tyler_task_ho_file_name[STRING] = "\0";

char aoa_task_file_name[STRING] = "\0";
char aoa_task_hh_file_name[STRING] = "\0";
char aoa_task_ih_file_name[STRING] = "\0";
char aoa_task_ho_file_name[STRING] = "\0";
char attractor_view_file_name[128] = "";

int paused = 0;
int training = 0;
int testing = 0;
int abort_training = 0;



int autosave = 1;

static gboolean gb_stop = FALSE;
void set_naming_graph_type(GtkComboBox *combo_box, GtkNaming_Graph *naming_graph);

void redraw_current_page(void);
static void new_weights(GtkMenuItem *item);
void train_patterns_serially(GtkNotebook *notebook);
GtkWidget *extract_widget_from_page(GtkNotebook *notebook, guint page_num);
void reset_pattern_combo_box(void);
void init_momentum(GtkSpinButton *spin_button, gpointer user_data);
void tyler_task_networks(GtkButton *button, GtkNotebook *notebook);

void init_learning_rate(GtkSpinButton *spin_button, gpointer user_data);
void tyler_train_patterns_serially_aoa(void);
#define X_MARGIN 20
#define Y_MARGIN 20

int W, H;

// distribution, lesion, and samples values for semantic testing, does not have to be same value for all tests
int d = 2, l = 5, sample = 1;






// how many times network sampled for overall error
#define BPTT_TEST	20
#define PDPTOOL_TEST	20
#define BM_TEST		2
#define TYLER_TEST	20
//used to be 500


int training_type = 3;
int settling_type = 3;
int annealing_type = 3;
int error_type = 1;



// Training types for BPTT
#define williams	0
#define williams_ta	1
#define mcclleland	2
#define mcclleland_ta	3



double lesion_percentage = 15, historic_lesion_percentage = 0;


void set_window_title(void);
void init_file_names(void);
void reset_pattern_spin_button(void);
void reset_stage_spin_button(void);
void open_responses(void);
void ghost_button(gchar *label, gboolean value);
void ghost_controls(gboolean value);
GdkPixbuf *create_pixbuf(const gchar * filename);
void save_network(char *filename);
void save_as_dialog(GtkButton *button, GtkWindow *window);
void save(GtkButton *button, GtkWindow *window);
void init_unrolled_network_viewers(void);
void init_network(void);
void open_all_related_files(void);
void auto_open_file(void);
int determine_file_type_and_open(char file[256]);
int determine_network_type(char file[256]);
void open_dialog(GtkButton *button, GtkWindow *window);
void open_patterns_dialog(GtkButton *button, GtkWindow *window);
void open_weights_button(GtkButton *button, GtkWindow *window);
void open_recent_item_activated(GtkRecentChooser* chooser, gpointer data);
void open_patterns_button(GtkButton *button, GtkWindow *window);
void set_learning_rate(GtkSpinButton *spin_button, gpointer user_data);
void set_momentum(GtkSpinButton *spin_button, gpointer user_data);
void set_autosave(GtkCheckMenuItem *autosave_item, gpointer user_data);
void update_learning_rate_spin_button(void);
void update_pattern_spin_button(void);
void update_stage_spin_button(void);
void set_unrolled_network_viewer_type(GtkComboBox *combo_box, GtkNotebook *notebook);
void set_pattern_file(GtkComboBox *combo_box, GtkNotebook *notebook);
void set_activation_threshold(GtkScale *scale, GtkLabel *label);
void set_word_to_picture_current_lesion_level(GtkComboBox *combo_box, GtkNotebook *notebook);
void set_xbm_p_type(GtkComboBox *combo_box, GtkNotebook *notebook);
void update_recent_chooser_filter(void);
void set_net_type(GtkComboBox *combo_box, GtkNotebook *notebook);
void set_iterations(GtkSpinButton *spin_button, gpointer user_data);
void set_lesion_distribution(GtkSpinButton *spin_button, gpointer user_data);
void set_lesion_level(GtkSpinButton *spin_button, gpointer user_data);
void set_sample(GtkSpinButton *spin_button, gpointer user_data);
void set_real_time(GtkCheckMenuItem *item, gpointer user_data);
void gtkx_flush_events(void);
gint delete(GtkWidget *widget, GtkWidget *event, gpointer data);
GtkWidget* extract_widget_from_page(GtkNotebook *notebook, guint page_num);
void set_word_to_picture_combo_box_lesion_levels(void);
void widget_hide(GtkWidget *widget);
void widget_show(GtkWidget *widget);
void widget_set_sensitive(GtkWidget *widget, int value);
void redraw_unknown_widget(GtkWidget *widget);
void notebook_redraw_current_page(GtkNotebook *notebook, gpointer arg1, guint page_num, gpointer user_data);
void redraw_current_page(void);
void new_window(GtkMenuItem *item);
void refresh_button(GtkButton *button, GtkNotebook *notebook);
void real_time_redraw(void);
void reset_semantic_tests(void);
static void new_weights(GtkMenuItem *item);
static void reset_weights(void);
void bptt_train_with_subpattern(int p, int s);
void bptt_train_with_subpattern_alt(int p, int s);
void init_misses(void);
void bm_train_patterns_serially(void);
void train_patterns(GtkButton *button, GtkNotebook *notebook);
void bm_real_time_clamped_anneal(void);
void bptt_real_time_settle_for(int t_max);
void pdptool_real_time_settle_for(int t_max);
void bptt_real_time_settle_equilibrium(void);
void pdptool_real_time_settle_equilibrium(void);
void bptt_real_time_unclamped_settle(void);
void pdptool_real_time_unclamped_settle(void);
void pdptool_real_time_rogers_settle(void);
void pdptool_semantic_task_settle(void);
void pdptool_real_time_settle(void);
void bptt_real_time_rogers_settle(void);
void bptt_semantic_task_settle(void);
void bptt_real_time_settle(void);
void bptt_train_patterns_serially(void);
void set_stage(GtkSpinButton *spinbutton, GtkNotebook *notebook);
int set_pattern(GtkSpinButton *spinbutton, GtkNotebook *notebook);
void bptt_test(void);
void pdptool_test(void);
void bm_test(void);
void test_button(GtkWidget *button, GtkNotebook *notebook);
void set_attractor_view_type(GtkWidget *combo_box, gpointer user_data);
void clicked_attractor_view_button(GtkWidget *button, gpointer user_data);
void save_graph(GtkButton *button, GtkWindow *window);
void set_training_type(GtkComboBox *combo_box, gpointer user_data);
void set_settling_type(GtkComboBox *combo_box, gpointer user_data);

void set_lesion_type(GtkComboBox *combo_box, gpointer user_data);
void set_lesion_locus(GtkComboBox *combo_box, gpointer user_data);

void set_error_type(GtkComboBox *combo_box, gpointer user_data);
void set_word_to_picture_graph_type(GtkComboBox *combo_box, GtkWord_to_picture_Graph *word_to_picture_graph);
void set_drawing_and_copying_graph_type(GtkComboBox *combo_box, GtkDrawing_and_copying_Graph *drawing_and_copying_graph);
void set_naming_graph_type(GtkComboBox *combo_box, GtkNaming_Graph *naming_graph);
void settle_button(GtkWidget *button, GtkNotebook *notebook);
void reset_states_button(GtkWidget *button, GtkNotebook *notebook);
void settle_step_button(GtkWidget *button, GtkNotebook *notebook);
void menuitem_response(gpointer data);
void naming_task(GtkButton *button, GtkNotebook *notebook);
// void naming_task_alt(GtkButton *button, GtkNotebook *notebook);
void sorting_task(GtkButton *button, GtkNotebook *notebook);
void word_to_picture_task(GtkButton *button, GtkNotebook *notebook);
void drawing_and_copying_task(GtkButton *button, GtkNotebook *notebook);
void send_error_to_error_view(void);
void send_error_to_graph(void);
void naming_graph_clear(void);
void save_naming_graph_pdf(void);
void save_network_viewer_pdf(void);
void sorting_graph_clear(void);
void save_sorting_graph_pdf(void);
void drawing_and_copying_graph_clear(void);
void save_drawing_and_copying_graph_pdf(void);
void word_to_picture_graph_clear(void);
void save_word_to_picture_graph_pdf(void);
void real_time_network_run(int net_type);
void create_clusters(GtkWidget widget, gpointer *data);
void save_tyler_responses_locus_network(int l);
void save_tyler_responses_locus(int l);

void tyler_original_task_(GtkButton *button, networks_struct *data);

int is_file_open()
{
    return (open_filename[0] != '\0');

}

void set_window_title(void) {
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    sprintf(window_title, "%s", open_name);
    gtk_window_set_title(GTK_WINDOW(window), window_title);

}

// void redraw_fixed(GtkFixed *fixed, GtkWidget **check_button)
//  {
//
//
//  }

int early_pattern[P_MAX];
int familiar_pattern[P_MAX];
int frequent_pattern[P_MAX];
int train_pattern[P_MAX];
int test_pattern[P_MAX];
#define EARLY 0
#define FAMILIAR 1
#define FREQUENT 2
#define TRAIN 3
#define TEST 4
// #define noise familiar_pattern[p]


void pattern_check_button_init(GtkWidget **check_button)
{

    int p, i;
    for (p = 0; p < P_MAX; p++) {
        early_pattern[p] = 1;
        familiar_pattern[p] = FAMILIAR_PATTERNS;
        frequent_pattern[p] = 1;
        train_pattern[p] = 1;
        test_pattern[p] = 1;

        for (i = 0; i < 5; i++) {
            if (i == EARLY) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button[p*5+i]), early_pattern[p]);
            } else if (i == FAMILIAR) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button[p*5+i]), familiar_pattern[p]);

            } else if (i == FREQUENT) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button[p*5+i]), frequent_pattern[p]);

            } else if (i == TRAIN) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button[p*5+i]), train_pattern[p]);

            } else if (i == TEST) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button[p*5+i]),  test_pattern[p]);
                /*
                	  } else {
                	    throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Invalid pattern setting!", i);    */
            }
        }
    }

}

void set_pattern_check_button (GtkToggleButton *togglebutton, gpointer *user_data)

{
    int p = ((int *)user_data)[0];
    int i = ((int *)user_data)[1];

//     printf("i = %d p = %d \n ", i, p);



    int q;
    if (i == EARLY) {

        early_pattern[p] = gtk_toggle_button_get_active (togglebutton);

//         printf("early_pattern =");

//         for (q = 0; q < P; q++) {
//             printf("%d ", early_pattern[q] );
//         }

//         printf("\n");


    } else if (i == FAMILIAR) {
        familiar_pattern[p] = gtk_toggle_button_get_active (togglebutton);

    } else if (i == FREQUENT) {
        frequent_pattern[p] = gtk_toggle_button_get_active (togglebutton);

    } else if (i == TRAIN) {
        train_pattern[p] = gtk_toggle_button_get_active (togglebutton);

    } else if (i == TEST) {
        test_pattern[p] = gtk_toggle_button_get_active (togglebutton);

    } else {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Invalid pattern setting!", i);
    }






}
void resize_check_buttons(GtkWidget *widget, GtkAllocation *allocation, gpointer *data) {
    printf("width = %d, height = %d\n", allocation->width, allocation->height);

    int p, y, i, x;

    //     x_space = (width - X_MARGIN * 2.0 - unit_width) / (double) (input_units);

    for (p = 0; p < P_MAX; p++) {
        for (i = 0; i < 5; i++) {
            gtk_widget_hide( ((check_button_struct*) data)->check_button[p*5+i]);
        }
    }


    for (p = 0; p < P; p++) {
        y = Y_MARGIN + (patterns_viewer->allocation.height - 25 - Y_MARGIN - Y_MARGIN) * ((p+1) / (double) P) - 2;
        for (i = 0; i < 5; i++) {

            x = column_width * (i + 0.5);
// 	              printf("x = %d\n", x);
            gtk_fixed_move(GTK_FIXED( ((check_button_struct*) data)->fixed), ( (check_button_struct*) data)->check_button[p*5+i], x, y);
            gtk_widget_show( ((check_button_struct*) data)->check_button[p*5+i]);

        }
//      gtk_widget_set_size_request(check_button[p], 15, 10);
//      g_signal_connect(G_OBJECT(check_button[p]), "toggled", G_CALLBACK(set_pattern_aoa), NULL);


    }
//    free(data);
}

void tyler_graphs_update(void) {
    if (GTK_IS_COMBO_BOX(tyler_combo_box)) {

        if(lesion_locus == TYLER_LESION_GLOBAL) {
            gtk_tyler_graph_update(GTK_TYLER_GRAPH(tyler_graph));
            gtk_tyler_graph_redraw(tyler_graph);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph), gtk_combo_box_get_active(GTK_COMBO_BOX(tyler_combo_box)));
            gtk_tyler_graph_redraw  (tyler_graph);

        }
        else if (lesion_locus == TYLER_LESION_IN_HIDDEN) {
            gtk_tyler_graph_update(GTK_TYLER_GRAPH(tyler_graph_ih));
            gtk_tyler_graph_redraw(tyler_graph_ih);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_ih), gtk_combo_box_get_active(GTK_COMBO_BOX(tyler_combo_box)));


        }
        else if (lesion_locus == TYLER_LESION_HIDDEN_HIDDEN) {
            gtk_tyler_graph_update(GTK_TYLER_GRAPH(tyler_graph_hh));
            gtk_tyler_graph_redraw(tyler_graph_hh);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_hh), gtk_combo_box_get_active(GTK_COMBO_BOX(tyler_combo_box)));

        }
        else if (lesion_locus == TYLER_LESION_HIDDEN_OUT) {
            gtk_tyler_graph_update(GTK_TYLER_GRAPH(tyler_graph_ho));
            gtk_tyler_graph_redraw(tyler_graph_ho);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_ho), gtk_combo_box_get_active(GTK_COMBO_BOX(tyler_combo_box)));

        }
    }

}


void aoa_graphs_update(void) {

    if (GTK_IS_COMBO_BOX(aoa_combo_box)) {


        if(lesion_locus == TYLER_LESION_GLOBAL) {
            gtk_aoa_graph_update(GTK_AOA_GRAPH(aoa_graph));
            gtk_aoa_graph_redraw(aoa_graph);
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph), gtk_combo_box_get_active(GTK_COMBO_BOX(aoa_combo_box)));
            gtk_aoa_graph_redraw  (aoa_graph);

        }
        else if (lesion_locus == TYLER_LESION_IN_HIDDEN) {
            gtk_aoa_graph_update(GTK_AOA_GRAPH(aoa_graph_ih));
            gtk_aoa_graph_redraw(aoa_graph_ih);
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_ih), gtk_combo_box_get_active(GTK_COMBO_BOX(aoa_combo_box)));
            gtk_aoa_graph_redraw  (aoa_graph_ih);


        }
        else if (lesion_locus == TYLER_LESION_HIDDEN_HIDDEN) {
            gtk_aoa_graph_update(GTK_AOA_GRAPH(aoa_graph_hh));
            gtk_aoa_graph_redraw(aoa_graph_hh);
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_hh), gtk_combo_box_get_active(GTK_COMBO_BOX(aoa_combo_box)));
            gtk_aoa_graph_redraw  (aoa_graph_hh);

        }
        else if (lesion_locus == TYLER_LESION_HIDDEN_OUT) {
            gtk_aoa_graph_update(GTK_AOA_GRAPH(aoa_graph_ho));
            gtk_aoa_graph_redraw(aoa_graph_ho);
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_ho), gtk_combo_box_get_active(GTK_COMBO_BOX(aoa_combo_box)));
            gtk_aoa_graph_redraw  (aoa_graph_ho);

        }

    }


}
// void init_file_names(void)
// {
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//     pid_t pid;
//     if ((pid = getpid()) < 0) {
//       perror("unable to get pid\n");
//     } else {
//       printf("The process id is: %d the name part is: %s\n", pid, open_name);
//     }
//     sprintf(naming_file_name, "%s%s.naming", open_path, open_name);
// //     printf("%s\n", naming_file_name);
//     sprintf(sorting_file_name, "%s%s.sorting", open_path, open_name);
//     sprintf(word_to_picture_file_name, "%s%s.word_to_picture", open_path, open_name);
//     sprintf(drawing_and_copying_file_name,  "%s%s.drawing_and_copying", open_path, open_name);
//     sprintf(tyler_task_file_name,  "%s%s.tyler_task", open_path, open_name);
//     sprintf(tyler_task_ih_file_name,  "%s%s.tyler_task_ih", open_path, open_name);
//     sprintf(tyler_task_ho_file_name,  "%s%s.tyler_task_ho", open_path, open_name);
//     sprintf(tyler_task_hh_file_name,  "%s%s.tyler_task_hh", open_path, open_name);
//     //     printf("%s\n", drawing_and_copying_file_name);
//
// //     sprintf(attractor_view_file_name,  "%s%s.attractor_view", open_path, open_name);
// }
void reset_pattern_spin_button(void)
{
    if (GTK_IS_SPIN_BUTTON(pattern_spin_button)) {
        gtk_spin_button_set_range(GTK_SPIN_BUTTON(pattern_spin_button), p_min, p_max-1);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(pattern_spin_button), 0);
    }
}
void reset_stage_spin_button(void)
{
    if (GTK_IS_SPIN_BUTTON(stage_spin_button)) {
//         gtk_spin_button_set_value(GTK_SPIN_BUTTON(stage_spin_button), 0);

        if ((net_type == TYLER_RECURRENT) || (net_type == TYLER_FEEDFORWARD)) {
            gtk_spin_button_set_range(GTK_SPIN_BUTTON(stage_spin_button), 0, 3);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(stage_spin_button), 3);

        }
        else {
            gtk_spin_button_set_range(GTK_SPIN_BUTTON(stage_spin_button), 0, 2);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(stage_spin_button), 0);

        }
    }


}

void open_responses(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//     pid_t pid;
//     if ((pid = getpid()) < 0) {
//       perror("unable to get pid\n");
//     } else {
//       printf("The process id is: %d the naming_file_name is: %s\n", pid, naming_file_name);
//     }
    if (file_exists(naming_file_name)) {
        open_naming_responses(naming_file_name);
        open_naming_responses_1(naming_file_name);
        open_naming_responses_alt(naming_file_name);
        gtk_naming_graph_update(GTK_NAMING_GRAPH(naming_graph));

    }

    if (file_exists(sorting_file_name)) {
        open_sorting_responses(sorting_file_name);
        gtk_sorting_graph_update(GTK_SORTING_GRAPH(sorting_graph));

    }

    if (file_exists(word_to_picture_file_name)) {

        open_word_to_picture_responses(word_to_picture_file_name);
        gtk_word_to_picture_graph_update(GTK_WORD_TO_PICTURE_GRAPH(word_to_picture_graph));
        set_word_to_picture_combo_box_lesion_levels();
    }

    if (file_exists(drawing_and_copying_file_name)) {
        open_drawing_and_copying_responses(drawing_and_copying_file_name);
//         printf("1 %s\n", drawing_and_copying_file_name);
        gtk_drawing_and_copying_graph_update(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph));
//         printf("2 %s\n", drawing_and_copying_file_name);


    }

    if (file_exists(tyler_task_file_name)) {

        open_tyler_responses(tyler_task_file_name);
        gtk_tyler_graph_update(GTK_TYLER_GRAPH(tyler_graph));
        //       set_word_to_picture_combo_box_lesion_levels();
    }

    if (file_exists(tyler_task_ih_file_name)) {

        open_tyler_responses(tyler_task_ih_file_name);
        gtk_tyler_graph_update(GTK_TYLER_GRAPH(tyler_graph_ih));
        //       set_word_to_picture_combo_box_lesion_levels();
    }

    if (file_exists(tyler_task_hh_file_name)) {

        open_tyler_responses(tyler_task_hh_file_name);
        gtk_tyler_graph_update(GTK_TYLER_GRAPH(tyler_graph_hh));
        //       set_word_to_picture_combo_box_lesion_levels();
    }

    if (file_exists(tyler_task_ho_file_name)) {

        open_tyler_responses(tyler_task_ho_file_name);
        gtk_tyler_graph_update(GTK_TYLER_GRAPH(tyler_graph_ho));
        //       set_word_to_picture_combo_box_lesion_levels();
    }

    if (file_exists(aoa_task_file_name)) {

        open_aoa_responses(aoa_task_file_name);
        gtk_aoa_graph_update(GTK_AOA_GRAPH(aoa_graph));
        //       set_word_to_picture_combo_box_lesion_levels();
    }

    if (file_exists(aoa_task_ih_file_name)) {

        open_aoa_responses(aoa_task_ih_file_name);
        gtk_aoa_graph_update(GTK_AOA_GRAPH(aoa_graph_ih));
        //       set_word_to_picture_combo_box_lesion_levels();
    }

    if (file_exists(aoa_task_hh_file_name)) {

        open_aoa_responses(aoa_task_hh_file_name);
        gtk_aoa_graph_update(GTK_AOA_GRAPH(aoa_graph_hh));
        //       set_word_to_picture_combo_box_lesion_levels();
    }

    if (file_exists(aoa_task_ho_file_name)) {

        open_aoa_responses(aoa_task_ho_file_name);
        gtk_aoa_graph_update(GTK_AOA_GRAPH(aoa_graph_ho));
        //       set_word_to_picture_combo_box_lesion_levels();
    }



    if (file_exists(attractor_view_file_name)) {
        gtk_attractor_view_open(GTK_ATTRACTOR_VIEW(attractor_view), attractor_view_file_name);
        gtk_attractor_view_update(GTK_ATTRACTOR_VIEW(attractor_view));
    }


    gtk_attractor_view_redraw(attractor_view);

    gtk_naming_graph_redraw(naming_graph);
    gtk_sorting_graph_redraw(sorting_graph);
    gtk_word_to_picture_graph_redraw(word_to_picture_graph);
    gtk_drawing_and_copying_graph_redraw(drawing_and_copying_graph);
    gtk_tyler_graph_redraw(tyler_graph);
    gtk_tyler_graph_redraw(tyler_graph_hh);
    gtk_tyler_graph_redraw(tyler_graph_ho);
    gtk_tyler_graph_redraw(tyler_graph_ih);
    gtk_aoa_graph_redraw(aoa_graph);
    gtk_aoa_graph_redraw(aoa_graph_hh);
    gtk_aoa_graph_redraw(aoa_graph_ho);
    gtk_aoa_graph_redraw(aoa_graph_ih);




}


void ghost_button(gchar *label, gboolean value)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if GTK_IS_CONTAINER(vertical_box) {
        GList *list;
        GList *children = gtk_container_get_children(GTK_CONTAINER(vertical_box));
        for (list = children; list; list = g_list_next(list)) {
            if GTK_IS_CONTAINER(list->data) {
                GList *inner_list;
                GList *inner_children = gtk_container_get_children(GTK_CONTAINER(list->data));
                for (inner_list = inner_children; inner_list; inner_list = g_list_next(inner_list)) {
                    if (GTK_IS_BUTTON(inner_list->data)) {
                        if (!strcmp(label, gtk_button_get_label(inner_list->data))) {
                            gtk_widget_set_sensitive(inner_list->data, !value);
                        }
                    }
                }
                g_list_free(inner_children);
            }
        }
        g_list_free(children);
    }
}



void ghost_controls(gboolean value)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if GTK_IS_CONTAINER(vertical_box) {
        GList *list;
        GList *children = gtk_container_get_children(GTK_CONTAINER(vertical_box));
        for (list = children; list; list = g_list_next(list)) {
            if GTK_IS_CONTAINER(list->data) {
                if (strcmp("tab-specific-controls", gtk_widget_get_name(list->data))) { //exclude these as they are GUI-related

                    GList *inner_list;
                    GList *inner_children = gtk_container_get_children(GTK_CONTAINER(list->data));
                    for (inner_list = inner_children; inner_list; inner_list = g_list_next(inner_list)) {
                        if (GTK_IS_BUTTON(inner_list->data)) {
                            if (strcmp("_Refresh", gtk_button_get_label(inner_list->data)) && strcmp("Real-t_ime", gtk_button_get_label(inner_list->data)) && strcmp("E_xit", gtk_button_get_label(inner_list->data))) { //exclude these two as they are GUI-related
                                gtk_widget_set_sensitive(inner_list->data, !value);
//                             printf("%s\n", gtk_button_get_label(inner_list->data));
                            }
                        }
                        if (GTK_IS_COMBO_BOX(inner_list->data) || GTK_IS_SPIN_BUTTON(inner_list->data)) {
//                             if (GTK_IS_COMBO_BOX(inner_list->data)) {
                            gtk_widget_set_sensitive(inner_list->data, !value);
//                             }

                        }
                    }
                    g_list_free(inner_children);
                    g_list_free(inner_list);

                }
            }
        }
        g_list_free(children);
        g_list_free(list);

    }
}

GdkPixbuf *create_pixbuf(const gchar * filename)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
//     pixbuf = gdk_pixbuf_new_from_file_at_scale(filename, 100, 100, TRUE, &error);
    if(!pixbuf)
    {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }


    return pixbuf;
}

void save_network(char *filename) {
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if (net_type == BPTT) {
        bptt_save_file(filename);
    }
    else if (net_type == BM) {
        bm_save_weights(filename);
    }
    else if (net_type == TYLER_RECURRENT) {
        tyler_save_file(filename);
    }
    else if (net_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_save_file(filename);
    }

}
void save_as_dialog(GtkButton *button, GtkWindow *window)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Save Weights As...", window, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), open_path);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Weight Files");
    gtk_file_filter_add_pattern(filter, "*.weight");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);

    if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        gsize *bytes_read = NULL, *bytes_written = NULL;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        filename = g_filename_to_utf8(filename, -1, bytes_read, bytes_written, NULL);
//         open_filename = filename;
        sprintf(open_filename, "%s", filename);
        save_network(filename);
        g_free (filename);
//         set_window_title();
        gtk_window_set_title(GTK_WINDOW(window), open_filename);

    }

    printf("%s\n", open_filename);

    gtk_widget_destroy (dialog);
}
/*

void save_as_pattern_dialog(GtkButton *button, GtkWindow *window)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Save Weights As Pattern...", window, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), open_path);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Weight Files");
    gtk_file_filter_add_pattern(filter, "*.weight");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);

    if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        gsize *bytes_read = NULL, *bytes_written = NULL;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        filename = g_filename_to_utf8(filename, -1, bytes_read, bytes_written, NULL);
        auto_name = filename;
        sprintf(auto_name, "%s", filename);
//     save_network(filename);
        g_free (filename);
        //         set_window_title();
        gtk_window_set_title(GTK_WINDOW(window), auto_name);

    }

    printf("%s\n", auto_name);

    gtk_widget_destroy (dialog);
}
                */
void save(GtkButton *button, GtkWindow *window)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if (open_filename[0] != '\0') {
        save_network(open_filename);
    }
    else
    {
        save_as_dialog(NULL, window);
    }
}

void init_unrolled_network_viewers(void)
{

    gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), ROLLED_STATES);
    gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(patterns_viewer), PATTERNS);

}
void init_lesion(void)
{
    historic_lesion_percentage = 0;
    ghost_button("Lesion", 0);
}

void init_iterations(void)
{
//    set_iterations(iterations_spin_button, gpointer user_data);
    if (GTK_IS_SPIN_BUTTON(iterations_spin_button)) {
        if ((net_type == TYLER_RECURRENT) || (net_type == TYLER_FEEDFORWARD)) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(iterations_spin_button), TYLER_C);
        }
        else if (net_type == BPTT) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(iterations_spin_button), BPTT_C);

        }
        else if (net_type == BM) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(iterations_spin_button), BM_I);

        }
    }
}

void set_task_spin_buttons(void)
{



    if (GTK_IS_SPIN_BUTTON(distribution_spin_button) && GTK_IS_SPIN_BUTTON(level_spin_button) && GTK_IS_SPIN_BUTTON(sample_spin_button)) {


        if ((net_type == TYLER_RECURRENT) || (net_type == TYLER_FEEDFORWARD)) {
            gtk_spin_button_set_range(GTK_SPIN_BUTTON(distribution_spin_button),0,  TYLER_LESION_DISTRIBUTION);
            gtk_spin_button_set_range(GTK_SPIN_BUTTON(level_spin_button),0,  TYLER_LESION_LEVEL);
            gtk_spin_button_set_range( GTK_SPIN_BUTTON(sample_spin_button),0, TYLER_SAMPLE);
            if (net_type == TYLER_FEEDFORWARD) {
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(distribution_spin_button), TYLER_LESION_DISTRIBUTION);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(level_spin_button), TYLER_LESION_LEVEL);
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(sample_spin_button),1);
            }
        }
        else {
            gtk_spin_button_set_range( GTK_SPIN_BUTTON(distribution_spin_button),0, DRAWING_AND_COPYING_LESION_DISTRIBUTION);
            gtk_spin_button_set_range(GTK_SPIN_BUTTON(level_spin_button), 0, DRAWING_AND_COPYING_LESION_LEVEL);
            gtk_spin_button_set_range( GTK_SPIN_BUTTON(sample_spin_button), 0,DRAWING_AND_COPYING_SAMPLE);

        }

    }
}


void init_network(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    lesion_percentage = 15, historic_lesion_percentage = 0;

    network_init(net_type);
    reset_pattern_combo_box();

    if (GTK_IS_COMBO_BOX(pattern_combo_box)) {

        set_pattern_file(GTK_COMBO_BOX(pattern_combo_box), GTK_NOTEBOOK(notebook));
    }

    gtk_graph_reset(GTK_GRAPH(graph));

    set_task_spin_buttons();


    if (prev_net_type != net_type) {  //preserve GUI settings
        init_iterations();
    }
//
    if (GTK_IS_SPIN_BUTTON(momentum_spin_button)) {
        set_momentum(GTK_SPIN_BUTTON(momentum_spin_button), NULL);
    }
    if (GTK_IS_SPIN_BUTTON(learning_rate_spin_button)) {

        set_learning_rate(GTK_SPIN_BUTTON(learning_rate_spin_button), NULL);
    }
//       redraw_current_page();
//     }
//     else
//     {
//       update_learning_rate_spin_button();
//       update_momentum_spin_button();
//     }
    init_unrolled_network_viewers();
    init_errors();
    reset_semantic_tests();
    init_lesion();
//     init_iterations();
    abort_training = 1;
    init_train_flags();

}



void open_all_related_files(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//   pid_t pid;//, ppid;
//   gid_t gid;

    /* get the process id */
//   if ((pid = getpid()) < 0) {
//     perror("unable to get pid\n");
//   } else {
//     printf("The process id is: %d the file currently open is: %s\n", pid, open_filename);
//   }

    /* get the parent process id */
//   if ((ppid = getppid()) < 0) {
//     perror("unable to get the ppid\n");
//   } else {
//     printf("The parent process id is %d\n", ppid);
//   }

    /* get the group process id */
//   if ((gid = getgid()) < 0) {
//     perror("unable to get the group id\n");
//   } else {
//     printf("The group id is %d\n", gid);
//   }



    int i = 0, name_i = 0;
    init_network();
    reset_semantic_tests();


    ghost_controls(1);
    gtk_widget_set_sensitive(notebook, 0);
    gtk_window_set_title(GTK_WINDOW(window), "Loading...");



    static char save_open_filename[256];
    sprintf(save_open_filename, "%s", open_filename);
// //     network_file_open(net_type, open_filename);
//     printf("%s\n", open_filename);
//     printf("%s\n", save_open_filename);
    //

    if (determine_network_type(&open_filename[0])) {

        sprintf(open_filename, "%s", save_open_filename);

//         sprintf(open_filename, "%s", save_open_filename);

//         printf("before opening: %s\n", open_filename);

        network_file_open(net_type, &open_filename[0]);
//         printf("after: %s\n", open_filename);







//         printf("%s: open file: %s\n", __FUNCTION__, open_filename);



        //     gtk_graph_set_max_iterations(GTK_GRAPH(graph), network_get_iterations_so_far(net_type));


        sprintf(open_path, "%s", open_filename);

        int max_i = strlen(open_path);
// 	printf("max_i = %d\n", max_i);
        for (i = max_i; i >= 0; i--)
        {

            name_i = max_i - i;

            if (open_path[i] == '/') {
                name_i = max_i - i - 1;

                break;
            }
        }

        int open_name_length = 0;
        for (i = 0; i < name_i; i++) {

// 	  printf("%c", open_path[i]);

            if (open_path[max_i - name_i + i] == '.') {
                break;
            }
            open_name[i] = open_path[max_i - name_i + i];
            open_name_length++;
        }

//         printf("\n", open_path[i]);


        open_name[open_name_length] = '\0';




        open_path[max_i - name_i] = '\0';

//         if ((pid = getpid()) < 0) {
// 	  perror("unable to get pid\n");
// 	} else {
// 	  printf("The process id is: %d the name part is: %s\n", pid, open_name);
// 	}
//         printf("%s: open name: %s\n", __FUNCTION__, open_name);
//         init_file_names();

        sprintf(naming_file_name, "%s%s.naming", open_path, open_name);
//     printf("%s\n", naming_file_name);
        sprintf(sorting_file_name, "%s%s.sorting", open_path, open_name);
        sprintf(word_to_picture_file_name, "%s%s.word_to_picture", open_path, open_name);
        sprintf(drawing_and_copying_file_name,  "%s%s.drawing_and_copying", open_path, open_name);
        sprintf(tyler_task_file_name,  "%s%s.tyler_task", open_path, open_name);
        sprintf(tyler_task_ih_file_name,  "%s%s.tyler_task_ih", open_path, open_name);
        sprintf(tyler_task_ho_file_name,  "%s%s.tyler_task_ho", open_path, open_name);
        sprintf(tyler_task_hh_file_name,  "%s%s.tyler_task_hh", open_path, open_name);
        sprintf(aoa_task_file_name,  "%s%s.aoa_task", open_path, open_name);
        sprintf(aoa_task_ih_file_name,  "%s%s.aoa_task_ih", open_path, open_name);
        sprintf(aoa_task_ho_file_name,  "%s%s.aoa_task_ho", open_path, open_name);
        sprintf(aoa_task_hh_file_name,  "%s%s.aoa_task_hh", open_path, open_name);
        //  printf("%s: open file: %s\n", __FUNCTION__, open_filename);


        open_responses();




        send_error_to_graph();
        send_error_to_error_view();
        //     set_window_title();
        if (GTK_IS_COMBO_BOX(net_type_combo_box)) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(net_type_combo_box), net_type);
        }
        if (GTK_IS_COMBO_BOX(pattern_combo_box)) {
            set_pattern_file(GTK_COMBO_BOX(pattern_combo_box), GTK_NOTEBOOK(notebook));
        }
        gtk_graph_reset(GTK_GRAPH(graph));
        gtk_graph_redraw(graph);

        sprintf(open_filename, "%s", save_open_filename);

//         printf("before opening: %s\n", open_filename);

        network_file_open(net_type, &open_filename[0]);

//         printf("after: %s\n", open_filename);
//         printf("%s: open file: %s\n", __FUNCTION__, open_filename);


        open_responses();
        send_error_to_graph();
        send_error_to_error_view();

        gtk_window_set_title(GTK_WINDOW(window), open_filename);

//         printf("end: %s\n", open_filename);


    }
    else
    {

        new_weights(NULL);
    }
    gtk_widget_set_sensitive(notebook, 1);

    ghost_controls(0);

    if(net_type == BPTT) {
        bptt_print_weight_stats();

    }
    redraw_current_page();
}

void auto_open_file(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//     if (net_type == BPTT) {
//
//         open_filename = "./BaseNames/New/mcclelland_ta_3.weight";
//         sprintf(open_path, "./BaseNames/New/");
//         sprintf(open_name, "mcclelland_ta_3");
//
//
//
//     }
//     else if (net_type == BM) {
//
//
//
//         open_filename = "./BaseNames/New/bm_6_patterns_2.weight";
//         sprintf(open_path, "./BaseNames/New/");
//         sprintf(open_name, "bm");
//         //
//     }
//     else if (net_type == PDPTOOL) {
//
//
//
//         open_filename = "./rogers2.mat";
//         sprintf(open_path, "~/");
//         sprintf(open_name, "rogers2");
//         //
//     }

    if (open_filename[0] != '\0') {
        open_all_related_files();
//         set_window_title();
    }
//     printf("pdptool patterns: %i\n",     pdptool_style_patterns());


}
int determine_file_type_and_open(char file[256])
{
    int i = net_type;
//     static char save_open_filename[256];
//     sprintf(save_open_filename, "%s", open_filename);


    //first try current net_type

    network_init(i);

    if (network_file_open(i, file)) {
        //             open_filename = save_open_filename;
        network_init(i);

        net_type = i;
        //             printf("%s: if: net_type: %s bptt: %d bm:%d network: %d open_filename: %s\n", __FUNCTION__, network_type_get_string(net_type), bptt_get_iterations_so_far(), bm_get_iterations_so_far(), network_get_iterations_so_far(net_type), open_filename);
        //             printf("%s: if: i: %s bptt: %d bm:%d network: %d open_filename: %s\n", __FUNCTION__, network_type_get_string(i), bptt_get_iterations_so_far(), bm_get_iterations_so_far(), network_get_iterations_so_far(i), open_filename);
        return 1;
    }
    network_init(i);

    for (i = 0; i < 5; i++) {
//         printf("%s: loop: net_type: %s bptt: %d bm:%d network: %d open_filename: %s\n", __FUNCTION__, network_type_get_string(net_type), bptt_get_iterations_so_far(), bm_get_iterations_so_far(), network_get_iterations_so_far(net_type), open_filename);
//         printf("%s: loop: i: %s bptt: %d bm:%d network: %d open_filename: %s\n", __FUNCTION__, network_type_get_string(i), bptt_get_iterations_so_far(), bm_get_iterations_so_far(), network_get_iterations_so_far(i), open_filename);
        network_init(i);

        if (network_file_open(i, file)) {
//             open_filename = save_open_filename;
            network_init(i);

            net_type = i;
//             printf("%s: if: net_type: %s bptt: %d bm:%d network: %d open_filename: %s\n", __FUNCTION__, network_type_get_string(net_type), bptt_get_iterations_so_far(), bm_get_iterations_so_far(), network_get_iterations_so_far(net_type), open_filename);
//             printf("%s: if: i: %s bptt: %d bm:%d network: %d open_filename: %s\n", __FUNCTION__, network_type_get_string(i), bptt_get_iterations_so_far(), bm_get_iterations_so_far(), network_get_iterations_so_far(i), open_filename);
            return 1;
        }
        network_init(i);

//         open_filename = save_open_filename;



    }
    return 0;

}


void open_dialog(GtkButton *button, GtkWindow *window)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new ("Open Weights", window, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog), open_path);
    GtkFileFilter *filter = gtk_file_filter_new();

    if (net_type == PDPTOOL) {
        filter = gtk_file_filter_new();

        gtk_file_filter_set_name(filter, "pdptool Weight Files");
        gtk_file_filter_add_pattern(filter, "*.mat");
        gtk_file_filter_add_pattern(filter, "*.wt");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);


        filter = gtk_file_filter_new();

        gtk_file_filter_set_name(filter, "Weight Files");
        gtk_file_filter_add_pattern(filter, "*.weight");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);
        filter = gtk_file_filter_new();

        gtk_file_filter_set_name(filter, "Data Files");
        gtk_file_filter_add_pattern(filter, "*.dat");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);

        filter = gtk_file_filter_new();

        gtk_file_filter_set_name(filter, "OXlearn Files");
        gtk_file_filter_add_pattern(filter, "*.OXlearn");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);

    }  else {

        filter = gtk_file_filter_new();

        gtk_file_filter_set_name(filter, "Weight Files");
        gtk_file_filter_add_pattern(filter, "*.weight");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);

        filter = gtk_file_filter_new();

        gtk_file_filter_set_name(filter, "Data Files");
        gtk_file_filter_add_pattern(filter, "*.dat");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);


        filter = gtk_file_filter_new();

        gtk_file_filter_set_name(filter, "OXlearn Files");
        gtk_file_filter_add_pattern(filter, "*.OXlearn");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);

        filter = gtk_file_filter_new();

        gtk_file_filter_set_name(filter, "pdptool Weight Files");
        gtk_file_filter_add_pattern(filter, "*.mat");
        gtk_file_filter_add_pattern(filter, "*.wt");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);






    }
    filter = gtk_file_filter_new();

    gtk_file_filter_set_name(filter, "All Files");
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), filter);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gsize *bytes_read = NULL, *bytes_written = NULL;
        filename = g_filename_to_utf8(filename, -1, bytes_read, bytes_written, NULL);
//         open_filename = filename;
        sprintf(open_filename, "%s", filename);
        g_free(filename);

        open_all_related_files();
    }

    gtk_widget_destroy (dialog);


}
int determine_network_type(char file[256])
{

//     static char save_open_filename[256];
//     sprintf(save_open_filename, "%s", open_filename);
    if (!determine_file_type_and_open(&file[0])) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Invalid file type!", 0);
        return 0;

    } else {
//     open_all_related_files();
        return 1;
//         open_filename = save_open_filename;
//         printf("%s: else: net_type: %s bptt: %d bm:%d network: %d open_filename: %s\n", __FUNCTION__, network_type_get_string(net_type), bptt_get_iterations_so_far(), bm_get_iterations_so_far(), network_get_iterations_so_far(net_type), open_filename);

    }
}

void open_patterns_dialog(GtkButton *button, GtkWindow *window)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new ("Open Patterns", window, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);


    GtkFileFilter *pattern_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(pattern_filter, "Pattern Files");
    gtk_file_filter_add_pattern(pattern_filter, "*.pat");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (dialog), pattern_filter);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gsize *bytes_read = NULL, *bytes_written = NULL;
        open_patterns(g_filename_to_utf8(filename, -1, bytes_read, bytes_written, NULL));
        gtk_button_set_label(GTK_BUTTON(patterns_button), g_filename_to_utf8(filename, -1, bytes_read, bytes_written, NULL));

        g_free(filename);
    }

    gtk_widget_destroy (dialog);
}
void open_weights_button(GtkButton *button, GtkWindow *window)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    open_dialog(button, window);
}

void clear_recent_items(GtkRecentChooser* chooser, gpointer data)
{
    GtkRecentManager *manager = gtk_recent_manager_get_default();
    gtk_recent_manager_purge_items(manager, NULL);

}

void open_recent_item_activated(GtkRecentChooser* chooser, gpointer data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    char filename[256], uri[256];
    int i;

    sprintf(uri, "%s", gtk_recent_chooser_get_current_uri(chooser));
//     printf("%s\n", uri);

    GtkRecentManager *manager = gtk_recent_manager_get_default();
    GError *error = NULL;
    GtkRecentInfo *info = gtk_recent_manager_lookup_item(manager, uri, &error);
    if (error)
    {
        g_warning ("Could not find the file: %s", error->message);
        g_error_free(error);
    }
    else
    {

        ghost_controls(1);
        gtk_window_set_title(GTK_WINDOW(window), "Loading...");

        for (i = 0; i < 256; i++) {
            filename[i] = '\0';
        }

        for (i = 7; i < strlen(uri); i++) { // uri to filename format
            filename[i-7] = uri[i]; //first 7 chars are "file://"
        }

        filename[i] = '\0';  //end of string

//         printf("filename: %s\n", filename);

        if (file_exists(filename)) {
//             open_filename = filename;
            sprintf(open_filename, "%s", filename);

            open_all_related_files();

        }
        else // this is a sanity check, just in case uri -> filename results in some kind of corruption
        {
            printf("File cannot be opened.\n");
            gtk_recent_manager_remove_item(manager, uri, NULL);

        }

    }
    ghost_controls(0);

    gtk_recent_info_unref(info);

    redraw_current_page();

}

void open_patterns_button(GtkButton *button, GtkWindow *window)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    open_patterns_dialog(button, window);
}

void set_learning_rate(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if (net_type == BPTT) {
        bptt_learning_rate = gtk_spin_button_get_value(spin_button);
        bptt_decay_rate = 1.0 - (bptt_learning_rate / (double) (3*P));
    }
    else if (net_type == TYLER_RECURRENT) {
        tyler_learning_rate = gtk_spin_button_get_value(spin_button);
        //         tyler_decay_rate = 1.0 - (tyler_learning_rate / (double) P);
        tyler_decay_rate =  1.0;
    }

    else if (net_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_learning_rate = gtk_spin_button_get_value(spin_button);
        //         tyler_decay_rate = 1.0 - (tyler_learning_rate / (double) P);
    }
    else if (net_type == BM) {
        bm_r =    gtk_spin_button_get_value(spin_button);
    }
    //     else if (net_type == PDPTOOL) {
    //       pdptool_learning_rate = gtk_spin_button_get_value(spin_button);
    //       pdptool_decay_rate = 1.0 - (learning_rate / (double) P);
    //     }
}

void init_learning_rate(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//   if (net_type == BPTT) {
//     bptt_learning_rate = gtk_spin_button_set_value(spin_button, BPTT_LEARNING_RATE);
//     bptt_decay_rate = 1.0 - (bptt_learning_rate / (double) (3*P));
//   }
//   else if (net_type == TYLER_RECURRENT) {
//     tyler_learning_rate = gtk_spin_button_get_value(spin_button);
//     //         tyler_decay_rate = 1.0 - (tyler_learning_rate / (double) P);
//     tyler_decay_rate =  1.0;
//   }

//   else

    if (net_type == TYLER_FEEDFORWARD) {
        gtk_spin_button_set_value(spin_button, TYLER_FEEDFORWARD_LEARNING_RATE);
        //         tyler_decay_rate = 1.0 - (tyler_learning_rate / (double) P);
    }
//   else if (net_type == BM) {
//     bm_r =    gtk_spin_button_get_value(spin_button);
//   }
    //     else if (net_type == PDPTOOL) {
    //       pdptool_learning_rate = gtk_spin_button_get_value(spin_button);
    //       pdptool_decay_rate = 1.0 - (learning_rate / (double) P);
    //     }
}


void set_noise(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

//     noise = gtk_spin_button_get_value(spin_button);

}


// double get_noise(void)
// {
//   #if PRINT_FUNC
//   printf("%s\n", __FUNCTION__);
//   #endif
//
// return noise;
// }
void set_momentum(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if (net_type == BPTT) {

    }
    else if (net_type == TYLER_RECURRENT) {

    }

    else if (net_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_momentum = gtk_spin_button_get_value(spin_button);
        //         tyler_decay_rate = 1.0 - (tyler_learning_rate / (double) P);
    }
    else if (net_type == BM) {
    }
    //     else if (net_type == PDPTOOL) {
    //       pdptool_learning_rate = gtk_spin_button_get_value(spin_button);
    //       pdptool_decay_rate = 1.0 - (learning_rate / (double) P);
    //     }
}

void init_momentum(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if (net_type == BPTT) {

    }
    else if (net_type == TYLER_RECURRENT) {

    }

    else if (net_type == TYLER_FEEDFORWARD) {
        gtk_spin_button_set_value(spin_button, TYLER_FEEDFORWARD_MOMENTUM_MIN);
        //         tyler_decay_rate = 1.0 - (tyler_learning_rate / (double) P);
    }
    else if (net_type == BM) {
    }
    //     else if (net_type == PDPTOOL) {
    //       pdptool_learning_rate = gtk_spin_button_get_value(spin_button);
    //       pdptool_decay_rate = 1.0 - (learning_rate / (double) P);
    //     }
}
void set_autosave(GtkCheckMenuItem *autosave_item, gpointer user_data)
{

    autosave = gtk_check_menu_item_get_active(autosave_item);
    printf("autosave: %d\n", autosave);
}

void update_learning_rate_spin_button(void) //because the BM updates its own learning rate internally!
{
    // RPC adjusted this so that it only works if the spin button exists...
    if (GTK_IS_SPIN_BUTTON(learning_rate_spin_button)) {
        if (net_type == BPTT) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(learning_rate_spin_button), bptt_learning_rate);
        }
        else if (net_type == TYLER_RECURRENT) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(learning_rate_spin_button), tyler_learning_rate);
        }
        else if (net_type == TYLER_FEEDFORWARD) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(learning_rate_spin_button), tyler_feedforward_learning_rate);
        }
        else if (net_type == BM) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(learning_rate_spin_button), bm_r);
        }
        else if (net_type == PDPTOOL) {

        }
    }
} void update_momentum_spin_button(void) //because the BM updates its own learning rate internally!
{
    // RPC adjusted this so that it only works if the spin button exists...
    if (GTK_IS_SPIN_BUTTON(momentum_spin_button)) {
        if (net_type == BPTT) {
            gtk_widget_set_sensitive((momentum_spin_button), 0);
        }
        else if (net_type == TYLER_RECURRENT) {
            gtk_widget_set_sensitive((momentum_spin_button), 0);
        }
        else if (net_type == TYLER_FEEDFORWARD) {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(momentum_spin_button), tyler_feedforward_momentum);
        }
        else if (net_type == BM) {
            gtk_widget_set_sensitive((momentum_spin_button), 0);
        }
        else if (net_type == PDPTOOL) {
            gtk_widget_set_sensitive((momentum_spin_button), 0);


        }
    }
}



void update_pattern_spin_button(void)
{
    if (net_type == BPTT) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(pattern_spin_button), bptt_get_clamped_pattern());
    }
    else if (net_type == BM) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(pattern_spin_button), bm_get_clamped_pattern());
    }
    else if (net_type == TYLER_RECURRENT) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(pattern_spin_button), tyler_get_clamped_pattern());
    }
    else if (net_type == TYLER_FEEDFORWARD) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(pattern_spin_button), tyler_feedforward_get_clamped_pattern());
    }
    else if (net_type == PDPTOOL) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(pattern_spin_button), pdptool_get_clamped_pattern());

    }
}

void update_stage_spin_button(void)
{
    if (net_type == BPTT) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(stage_spin_button), bptt_get_clamped_stage());
    }
    else if (net_type == BM) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(stage_spin_button), bm_get_clamped_stage());
    }
    else if (net_type == TYLER_RECURRENT) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(stage_spin_button), tyler_get_clamped_stage());
    }
    else if (net_type == TYLER_FEEDFORWARD) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(stage_spin_button), tyler_feedforward_get_clamped_stage());
    }
    else if (net_type == PDPTOOL) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(stage_spin_button), pdptool_get_clamped_stage());

    }
}

void set_unrolled_network_viewer_type(GtkComboBox *combo_box, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//     if (gtk_combo_box_get_active(combo_box) == ROLLED) {
//         gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), ROLLED_STATES);
//     }
//     else if (gtk_combo_box_get_active(combo_box) == STATES) {
//     }

    gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), gtk_combo_box_get_active(combo_box));

    redraw_current_page();
}
void reset_pattern_combo_box(void)
{

//     gtk_widget_set_sensitive(pattern_combo_box, 1);

    if (GTK_IS_COMBO_BOX(pattern_combo_box)) {

//         gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(pattern_combo_box), 6);
//         gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(pattern_combo_box), 7);
        int previous = gtk_combo_box_get_active(GTK_COMBO_BOX(pattern_combo_box));
        if ((net_type == TYLER_RECURRENT) || (net_type == TYLER_FEEDFORWARD)) {

            if (gtk_combo_box_get_active(GTK_COMBO_BOX(pattern_combo_box)) < 6) {
                gtk_combo_box_set_active(GTK_COMBO_BOX(pattern_combo_box), 6);

            }

//             gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(pattern_combo_box), 6, "Tyler (2000)");
//             gtk_combo_box_set_active(GTK_COMBO_BOX(pattern_combo_box), 6);

// 	    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(pattern_combo_box), 7, "Tyler-like 1");
// 	    gtk_combo_box_set_active(GTK_COMBO_BOX(pattern_combo_box), 7);

// 	    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(pattern_combo_box), 8, "Tyler-like 2");

//             gtk_widget_set_sensitive(pattern_combo_box, 0);

        }
        else {



//             gtk_combo_box_insert_text(GTK_COMBO_BOX(pattern_combo_box), 0, "Base Names");
// 	    gtk_combo_box_insert_text(GTK_COMBO_BOX(pattern_combo_box), 1, "Category Names");
// 	    gtk_combo_box_insert_text(GTK_COMBO_BOX(pattern_combo_box), 2, "Category Names 2");
//
// 	    gtk_combo_box_insert_text(GTK_COMBO_BOX(pattern_combo_box), 3, "6 Patterns");
// 	    gtk_combo_box_insert_text(GTK_COMBO_BOX(pattern_combo_box), 4, "PDPTOOL patterns");
//
// 	    gtk_combo_box_insert_text(GTK_COMBO_BOX(pattern_combo_box), 5, "Original Rogers");
// 	    gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(pattern_combo_box), 6, "Tyler (2000)");
            if (previous != 6) {
                gtk_combo_box_set_active(GTK_COMBO_BOX(pattern_combo_box), previous);
            }
            else {
                gtk_combo_box_set_active(GTK_COMBO_BOX(pattern_combo_box), 1);

            }
//             gtk_widget_set_sensitive(pattern_combo_box, 1);


        }

    }
}
void set_pattern_file(GtkComboBox *combo_box, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

    if (gtk_combo_box_get_active(combo_box) == 0) {
//         if (net_type != PDPTOOL)
        {

            open_patterns("base_names.pat");

        }
    }
    else if (gtk_combo_box_get_active(combo_box) == 1) {
        //         if (net_type != PDPTOOL)
        {

            open_patterns("category_names.pat");
            save_patterns_matlab("category_names.m");
            save_patterns_pdptool("category_names_pdptool.pat");


        }
    }
    else if (gtk_combo_box_get_active(combo_box) == 2) {
        //         if (net_type != PDPTOOL)
        {

            open_patterns("category_names_2.pat");

        }
    }
    else if (gtk_combo_box_get_active(combo_box) == 3) {
//         if (net_type != PDPTOOL)
        {
            open_patterns("one_of_each.pat");

        }
    }
    else if (gtk_combo_box_get_active(combo_box) == 4) {

        pdptool_open_patterns("features.pat");

    }

    else if (gtk_combo_box_get_active(combo_box) == 5) {
        open_patterns_rogers();
        save_patterns_matlab("rogers_patterns.m");


    }
    else if (gtk_combo_box_get_active(combo_box) == 6) {

        tyler_open_patterns("tyler.pat");
//         gtk_widget_set_sensitive(GTK_WIDGET(combo_box), 0);

    }

    else if (gtk_combo_box_get_active(combo_box) == 7) {

        tyler_open_patterns("tyler_orthogonal.pat");
//         gtk_widget_set_sensitive(GTK_WIDGET(combo_box), 0);

    }

//     else if (gtk_combo_box_get_active(combo_box) == 8) {
//
//         tyler_open_patterns("tyler_like_2.pat");
//         //       gtk_widget_set_sensitive(GTK_WIDGET(combo_box), 0);
//
//     }
//
//     else if (gtk_combo_box_get_active(combo_box) == 9) {
//
//         tyler_open_patterns("tyler_like_3.pat");
//         //       gtk_widget_set_sensitive(GTK_WIDGET(combo_box), 0);
//
//     }

//     if (net_type == PDPTOOL)
//     {
//         gtk_combo_box_set_active(combo_box, 3);
//         pdptool_open_patterns("48_features.pat");
//
//     }

    reset_stage_spin_button();

    reset_pattern_spin_button();
    redraw_current_page();

}



void set_activation_threshold(GtkScale *scale, GtkLabel *label)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    activation_threshold = gtk_range_get_value(GTK_RANGE(scale));

    gtk_naming_graph_update(GTK_NAMING_GRAPH(naming_graph));
    gtk_naming_graph_redraw(naming_graph);


    char s[5];
    sprintf(s, "%1.2f", activation_threshold);
    gtk_label_set_text(label, s);
}

void set_word_to_picture_current_lesion_level(GtkComboBox *combo_box, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//   printf("set: %i\n", gtk_combo_box_get_active(combo_box));
    gtk_word_to_picture_graph_set_current_level(GTK_WORD_TO_PICTURE_GRAPH(word_to_picture_graph), gtk_combo_box_get_active(combo_box));
    gtk_word_to_picture_graph_redraw(word_to_picture_graph);
}


void set_xbm_p_type(GtkComboBox *combo_box, GtkNotebook *notebook)
{
    if (GTK_IS_COMBO_BOX(combo_box)) {
        if (gtk_combo_box_get_active(combo_box) == 0) {
            if (pdptool_style_patterns()) {
                gtk_xbm_prob(GTK_XBM(xbm_p), BM_PLUS_NAME, BM_MINUS_NAME);

            }
            else {
                gtk_xbm_prob(GTK_XBM(xbm_p), BM_PLUS, BM_MINUS_NAME);
            }
        }
        else if (gtk_combo_box_get_active(combo_box) == 1) {
            gtk_xbm_prob(GTK_XBM(xbm_p), BM_PLUS, BM_MINUS_VISUAL);
        }
        else if (gtk_combo_box_get_active(combo_box) == 2) {
            gtk_xbm_prob(GTK_XBM(xbm_p), BM_PLUS, BM_MINUS_VERBAL);
        }

        redraw_current_page();
    }

}


void update_recent_chooser_filter(void)
{
    GtkRecentFilter *filter = gtk_recent_filter_new();
    if (net_type == PDPTOOL) {
        gtk_recent_filter_add_pattern(filter, "*.wt");
        gtk_recent_filter_add_pattern(filter, "*.mat");

    }
    else {
        gtk_recent_filter_add_pattern(filter, "*.weight");
    }
    gtk_recent_chooser_add_filter(GTK_RECENT_CHOOSER(recent_chooser_menu), filter);
}

void set_weight_update_type (GtkComboBox *combo_box, GtkNotebook *notebook)
{
    weight_update_type = gtk_combo_box_get_active(combo_box);

}

void set_weight_decay (GtkComboBox *combo_box, GtkNotebook *notebook)
{
    decay_learning_rate = gtk_combo_box_get_active(combo_box);

}



void set_net_type(GtkComboBox *combo_box, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

    prev_net_type = net_type;
    net_type = gtk_combo_box_get_active(combo_box);

    network_unclamp(net_type);


    printf("previous is %s, current is %s\n", network_type_get_string(prev_net_type), network_type_get_string(net_type));


    init_unrolled_network_viewers();

    init_network();

    if (is_file_open()) {
        reset_weights();
    } else {
        new_weights(NULL);
    }

    init_errors();
    auto_open_file();
    reset_stage_spin_button();
    reset_pattern_spin_button();


    update_recent_chooser_filter();
    // RPC thinks this is needed when you set the network type:

    if (prev_net_type == net_type) {  //preserve GUI settings

        if (GTK_IS_SPIN_BUTTON(momentum_spin_button)) {
            set_momentum(GTK_SPIN_BUTTON(momentum_spin_button), NULL);
        }
        if (GTK_IS_SPIN_BUTTON(learning_rate_spin_button)) {

            set_learning_rate(GTK_SPIN_BUTTON(learning_rate_spin_button), NULL);
        }
    }
    else
    {
        init_momentum(GTK_SPIN_BUTTON(momentum_spin_button), NULL);
        init_learning_rate(GTK_SPIN_BUTTON(learning_rate_spin_button), NULL);
    }

    /*update_momentum_spin_button(); */
    reset_pattern_combo_box();
    update_error_combo_box();
    redraw_current_page();
    update_training_algo_combo_box();


//     printf("net_type = %i\n", net_type);

}

void set_lesion_percentage(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    lesion_percentage = gtk_spin_button_get_value(spin_button);

    if (historic_lesion_percentage+lesion_percentage <= 100) {
        ghost_button("Lesion", 0);
    } else {
        ghost_button("Lesion", 1);
    }
}

void set_clamped_states(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if (net_type == BPTT) {
        BPTT_T_CLAMPED = gtk_spin_button_get_value_as_int(spin_button);
        bptt_init_deltas();
        bptt_init_delta_weights();
        bptt_unclamp_all();
        bptt_randomise_states_all();
        redraw_current_page();
    }
//     else if (net_type == BM) {
//     BM_I = gtk_spin_button_get_value_as_int(spin_button);

//     }
    else if (net_type == TYLER_RECURRENT) {
        TYLER_T_CLAMPED = gtk_spin_button_get_value_as_int(spin_button);
        tyler_init_deltas();
        tyler_init_delta_weights();
        tyler_unclamp_all();
        tyler_randomise_states_all();
        redraw_current_page();
    }
}

void set_iterations(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//     if (net_type == BPTT) {
    BPTT_C = gtk_spin_button_get_value_as_int(spin_button);
//     }
//     else if (net_type == BM) {
    BM_I = gtk_spin_button_get_value_as_int(spin_button);

//     }
//     else if (net_type == TYLER_RECURRENT) {
    TYLER_C = gtk_spin_button_get_value_as_int(spin_button);
//     printf("TYLER_C = %d\n", TYLER_C);
//     }
}

void set_lesion_distribution(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double value = gtk_spin_button_get_value(spin_button);

    if (value) {
        d = (int) value;
//       printf("d = %i\n", d);

    }
    else {
        gtk_spin_button_get_range(spin_button, NULL, &value);
        gtk_spin_button_set_value(spin_button, value);
    }
}


void set_lesion_level(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double value = gtk_spin_button_get_value(spin_button);

    if (value) {
        l = (int) value;
//       printf("l = %i\n", l);

    }
    else {
        gtk_spin_button_get_range(spin_button, NULL, &value);
        gtk_spin_button_set_value(spin_button, value);
    }
}

void set_sample(GtkSpinButton *spin_button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double value = gtk_spin_button_get_value(spin_button);

    if (value) {
        sample = (int) value;
//       printf("sample = %i\n", sample);
    }
    else {
        gtk_spin_button_get_range(spin_button, NULL, &value);
        gtk_spin_button_set_value(spin_button, value);
    }
}

void set_real_time(GtkCheckMenuItem *item, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    real_time = gtk_check_menu_item_get_active(item);
    //     printf("real-time: %d\n", real_time);
}


void set_legend(GtkCheckMenuItem *item, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_word_to_picture_graph_set_legend(GTK_WORD_TO_PICTURE_GRAPH(word_to_picture_graph), gtk_check_menu_item_get_active(item));
    gtk_naming_graph_set_legend(GTK_NAMING_GRAPH(naming_graph), gtk_check_menu_item_get_active(item));
    gtk_sorting_graph_set_legend(GTK_SORTING_GRAPH(sorting_graph), gtk_check_menu_item_get_active(item));
    gtk_drawing_and_copying_graph_set_legend(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph), gtk_check_menu_item_get_active(item));
    redraw_current_page();
    //     printf("real-time: %d\n", real_time);
}

// void print_weight_stats(GtkMenuItem *item, gpointer user_data)
// {
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//     network_print_weight_stats(net_type);
//
// }

void set_data_type_patient(GtkCheckMenuItem *item, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(rogers_data_item), FALSE);
    gtk_naming_graph_set_data_type(GTK_NAMING_GRAPH(naming_graph), MODEL);
    gtk_sorting_graph_set_data_type(GTK_SORTING_GRAPH(sorting_graph), MODEL);
    gtk_drawing_and_copying_graph_set_data_type(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph), MODEL);
    gtk_tyler_graph_set_data_type(GTK_TYLER_GRAPH(tyler_graph), MODEL);

    data_type = MODEL;

    if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(rogers_data_item))) {



        if (gtk_check_menu_item_get_active(item)) {
            gtk_naming_graph_set_data_type(GTK_NAMING_GRAPH(naming_graph), PATIENTS);
            gtk_sorting_graph_set_data_type(GTK_SORTING_GRAPH(sorting_graph), PATIENTS);
            gtk_drawing_and_copying_graph_set_data_type(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph), PATIENTS);
            gtk_tyler_graph_set_data_type(GTK_TYLER_GRAPH(tyler_graph), PATIENTS);

            data_type = PATIENTS;

        }
    }





}

void patterns_to_train_with_dialogue(GtkCheckMenuItem *item, gpointer user_data)
{
//   double array[8];
//   network_get_weight_stats(net_type, &array[0]);
//   GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
// 					     "Total Mean = %f\nInput Mean = %f\nHidden Mean = %f\nOutput Mean = %f\n\nTotal Absolute Mean = %f\nInput Absolute Mean = %f\nHidden Absolute Mean = %f\nOutput Absolute Mean = %f", array[0], array[1], array[2], array[3], array[4], array[5], array[6], array[7]);
//   gtk_widget_show(dialog);
//
//
//   g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy),  dialog);
}



void weight_stats_dialogue(GtkCheckMenuItem *item, gpointer user_data)
{
    double array[8];
    network_get_weight_stats(net_type, &array[0]);
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
                        "Total Mean = %f\nInput Mean = %f\nHidden Mean = %f\nOutput Mean = %f\n\nTotal Absolute Mean = %f\nInput Absolute Mean = %f\nHidden Absolute Mean = %f\nOutput Absolute Mean = %f", array[0], array[1], array[2], array[3], array[4], array[5], array[6], array[7]);
    gtk_widget_show(dialog);


    g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy),  dialog);
}

void set_data_type_rogers_model(GtkCheckMenuItem *item, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(patient_data_item), FALSE);
    gtk_naming_graph_set_data_type(GTK_NAMING_GRAPH(naming_graph), MODEL);
    gtk_sorting_graph_set_data_type(GTK_SORTING_GRAPH(sorting_graph), MODEL);
    gtk_drawing_and_copying_graph_set_data_type(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph), MODEL);
    gtk_tyler_graph_set_data_type(GTK_TYLER_GRAPH(tyler_graph), MODEL);

    data_type = MODEL;

    if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(patient_data_item))) {



        if (gtk_check_menu_item_get_active(item)) {
            gtk_naming_graph_set_data_type(GTK_NAMING_GRAPH(naming_graph), ROGERS);
            gtk_sorting_graph_set_data_type(GTK_SORTING_GRAPH(sorting_graph), ROGERS);
            gtk_drawing_and_copying_graph_set_data_type(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph), ROGERS);
            gtk_tyler_graph_set_data_type(GTK_TYLER_GRAPH(tyler_graph), TYLER);

            data_type = ROGERS;
        }
    }


}

void gtkx_flush_events(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
}
gint delete(GtkWidget *widget, GtkWidget *event, gpointer data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_main_quit();
    exit(0);
}

GtkWidget* extract_widget_from_page(GtkNotebook *notebook, guint page_num)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    return gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), page_num);
}


void set_word_to_picture_combo_box_lesion_levels(void) {
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int i = 0;
    char s[6];
    int lesion_levels = gtk_tree_model_iter_n_children(gtk_combo_box_get_model(GTK_COMBO_BOX(word_to_picture_combo_box2)), NULL);
    if (GTK_IS_COMBO_BOX(word_to_picture_combo_box2)) {

        for (i = lesion_levels; i >= 0; i--)  {
            gtk_combo_box_set_active(GTK_COMBO_BOX(word_to_picture_combo_box2), i);
            gtk_combo_box_remove_text(GTK_COMBO_BOX(word_to_picture_combo_box2), i);
        }

        for (i = 0; i < word_to_picture_lesion_level; i++) {
            sprintf(s, "%1.1f%%", i *  100.0 / (double) word_to_picture_lesion_level);
            gtk_combo_box_text_insert_text(GTK_COMBO_BOX_TEXT(word_to_picture_combo_box2), i, s);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(word_to_picture_combo_box2), (int) rintf(word_to_picture_lesion_level / 2.0));
    }

}

void widget_hide(GtkWidget *widget)
{
    if (GTK_IS_WIDGET(widget)) {
        gtk_widget_hide(widget);
    }
}
void widget_show(GtkWidget *widget)
{
    if (GTK_IS_WIDGET(widget)) {
        gtk_widget_show(widget);
    }
}

void widget_set_sensitive(GtkWidget *widget, int value)
{
    if (GTK_IS_WIDGET(widget)) {
        gtk_widget_set_sensitive(widget, value);
    }
}

void redraw_unknown_widget(GtkWidget *widget) {


    //Default settings that occur GUI-wide and need to be enforced.
//     widget_hide(attractor_view);  // never used


    widget_hide(t_clamped_label);
    widget_hide(t_clamped_spin_button);

    widget_hide(naming_label);
    widget_hide(naming_label2);
    widget_hide(naming_label3);
    widget_hide(naming_scale);
    widget_hide(naming_combo_box);
    widget_hide(naming_combo_box_2);
    widget_hide(word_to_picture_combo_box);
    widget_hide(word_to_picture_label);
    widget_hide(word_to_picture_label2);
    widget_hide(word_to_picture_combo_box2);
    widget_hide(drawing_and_copying_combo_box);
    widget_hide(drawing_and_copying_label);
    widget_hide(unrolled_network_viewer_combo_box);
    widget_hide(unrolled_network_viewer_label);
    widget_hide(xbm_p_combo_box);
    widget_hide(xbm_p_label);
//     widget_hide(xbm_p);
    widget_hide(attractor_view_button);
    widget_hide(attractor_view_label);
    widget_hide(attractor_view_label2);
    widget_hide(attractor_view_combo_box);


    widget_set_sensitive(training_algo_combo_box, 0);
//     widget_set_sensitive(settle_combo_box, 1);
    widget_hide(anneal_combo_box);
    widget_show(settle_combo_box);

    widget_hide(tyler_combo_box);
    widget_hide(tyler_label);
//     gtk_tyler_graph_redraw(widget);



    widget_hide(xbm_p);
    widget_hide(tyler_graph);
    widget_hide(tyler_graph_networks);
    widget_hide(tyler_graph_ih);
    widget_hide(tyler_graph_ho);
    widget_hide(tyler_graph_hh);
    widget_hide(aoa_graph);
    widget_hide(aoa_graph_networks);
    widget_hide(aoa_graph_ih);
    widget_hide(aoa_graph_ho);
    widget_hide(aoa_graph_hh);
    widget_hide(aoa_combo_box);


    widget_hide(tyler_combo_box);
    widget_hide(tyler_label);

    widget_hide(aoa_combo_box);
    widget_hide(aoa_label);
    widget_set_sensitive(momentum_spin_button, 0);
    widget_set_sensitive(sample_spin_button,1);

    widget_show(naming_graph);
    widget_show(word_to_picture_graph);
    widget_show(sorting_graph);
    widget_show(drawing_and_copying_graph);
    widget_show(attractor_view);

    if (net_type == BM) {
        widget_show(xbm_p);
        widget_hide(settle_combo_box);
        widget_show(anneal_combo_box);




    }
    else if ((net_type == TYLER_RECURRENT) || (net_type == TYLER_FEEDFORWARD)) {
        widget_hide(naming_graph);
        widget_hide(word_to_picture_graph);
        widget_hide(sorting_graph);
        widget_hide(drawing_and_copying_graph);
        widget_show(aoa_combo_box);


//         if (aoa) {

        widget_show(aoa_graph);
        widget_show(aoa_graph_networks);
        widget_show(aoa_graph_ih);
        widget_show(aoa_graph_ho);

//         widget_hide(tyler_graph);
//         widget_hide(tyler_graph_networks);
//         widget_hide(tyler_graph_ih);
//         widget_hide(tyler_graph_ho);
//         }
//         else
//         {
        /*
                widget_hide(aoa_graph);
                widget_hide(aoa_graph_networks);
                widget_hide(aoa_graph_ih);
                widget_hide(aoa_graph_ho);  */


        widget_show(tyler_graph);
        widget_show(tyler_graph_networks);
        widget_show(tyler_graph_ih);
        widget_show(tyler_graph_ho);
//         }

        if (net_type == TYLER_RECURRENT) {
//             if (aoa) {
            widget_hide(tyler_graph_hh);


            widget_show(aoa_graph_hh);
//             }
//             else {
            widget_show(tyler_graph_hh);

            widget_hide(aoa_graph_hh);

//             }

        }


        if (net_type == TYLER_FEEDFORWARD) {
            widget_set_sensitive(momentum_spin_button, 1);
            widget_set_sensitive(sample_spin_button,0);
//             widget_hide(attractor_view);

        }



//         widget_set_sensitive(pattern_combo_box, 1);

        widget_set_sensitive(training_algo_combo_box, 1);


    }
    else if ((net_type == BPTT) && (!training)) {
        widget_set_sensitive(training_algo_combo_box, 1);
    }


    if ((net_type == TYLER_RECURRENT) || (net_type == BPTT)) {
        widget_show(t_clamped_label);
        widget_show(t_clamped_spin_button);
    }

    if ((net_type == BM) || (net_type == BPTT) || (net_type == PDPTOOL)) {
        widget_set_sensitive(naming_item, 1);
        widget_set_sensitive(alt_naming_item, 1);
        widget_set_sensitive(clear_naming_item, 1);
        widget_set_sensitive(save_naming_item, 1);
        widget_set_sensitive(sorting_item, 1);
        widget_set_sensitive(clear_sorting_item, 1);
        widget_set_sensitive(save_sorting_item, 1);

        widget_set_sensitive(word_to_picture_item, 1);
        widget_set_sensitive(clear_word_to_picture_item, 1);
        widget_set_sensitive(save_word_to_picture_item, 1);

        widget_set_sensitive(drawing_item, 1);
        widget_set_sensitive(clear_drawing_item, 1);
        widget_set_sensitive(save_drawing_item, 1);

        widget_set_sensitive(rogers_data_item, 1);
        widget_set_sensitive(patient_data_item, 1);

    }
    else {
        widget_set_sensitive(naming_item, 0);
//         widget_set_sensitive(alt_naming_item, 0);
        widget_set_sensitive(clear_naming_item, 0);
        widget_set_sensitive(save_naming_item, 0);
        widget_set_sensitive(sorting_item, 0);
        widget_set_sensitive(clear_sorting_item, 0);
        widget_set_sensitive(save_sorting_item, 0);

        widget_set_sensitive(word_to_picture_item, 0);
        widget_set_sensitive(clear_word_to_picture_item, 0);
        widget_set_sensitive(save_word_to_picture_item, 0);

        widget_set_sensitive(drawing_item, 0);
        widget_set_sensitive(clear_drawing_item, 0);
        widget_set_sensitive(save_drawing_item, 0);

        widget_set_sensitive(rogers_data_item, 1);
        widget_set_sensitive(patient_data_item, 0);



    }



    // Widget-specific settings, i.e., based on what has been requested to be redrawn.

    if (GTK_IS_GRAPH(widget)) {
        gtk_graph_redraw(widget);
    }
    else if (GTK_IS_XBM(widget))
    {
        if (net_type == BM) {
            widget_show(xbm_p_combo_box);
            widget_show(xbm_p_label);
            gtk_xbm_redraw(GTK_XBM(widget));
        }
        else {
            widget_hide(xbm_p);
        }
    }
    else if (GTK_IS_UNROLLED_NETWORK_VIEWER(widget))
    {
        if (gtk_unrolled_network_viewer_get_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(widget)) != PATTERNS)
        {

            if ((net_type == BPTT)||(net_type == TYLER_RECURRENT)) {
                widget_show(unrolled_network_viewer_combo_box);
                widget_show(unrolled_network_viewer_label);
            }

        }
        else {
            widget_hide(unrolled_network_viewer_combo_box);
            widget_hide(unrolled_network_viewer_label);
        }
        gtk_unrolled_network_viewer_redraw(widget);
    }
    else if (GTK_IS_ERROR_VIEW(widget))
    {
        gtk_error_view_redraw(widget);
    }
    else if (GTK_IS_ATTRACTOR_VIEW(widget))
    {
        widget_show(attractor_view_button);
        widget_show(attractor_view_label);
        widget_show(attractor_view_label2);
        widget_show(attractor_view_combo_box);
        gtk_attractor_view_redraw(widget);

    }
    else if (GTK_IS_NAMING_GRAPH(widget))
    {
        widget_show(naming_label);
        widget_show(naming_label2);
        widget_show(naming_label3);
        widget_show(naming_scale);
        widget_show(naming_combo_box);
        widget_show(naming_combo_box_2);

        gtk_naming_graph_redraw(widget);
    }
    else if (GTK_IS_SORTING_GRAPH(widget))
    {
        gtk_sorting_graph_redraw(widget);
    }
    else if (GTK_IS_WORD_TO_PICTURE_GRAPH(widget))
    {
        if (gtk_combo_box_get_active(GTK_COMBO_BOX(word_to_picture_combo_box)) == 0) {
            widget_set_sensitive(word_to_picture_combo_box2, 0);
            widget_set_sensitive(word_to_picture_label2, 0);
        }
        else {
            widget_set_sensitive(word_to_picture_combo_box2, 1);
            widget_set_sensitive(word_to_picture_label2, 1);
        }
        widget_show(word_to_picture_combo_box);
        widget_show(word_to_picture_label);
        widget_show(word_to_picture_label2);
        widget_show(word_to_picture_combo_box2);
        gtk_word_to_picture_graph_redraw(widget);
    }
    else if (GTK_IS_DRAWING_AND_COPYING_GRAPH(widget))
    {
        widget_show(drawing_and_copying_combo_box);
        widget_show(drawing_and_copying_label);
        gtk_drawing_and_copying_graph_redraw(widget);
    }
    else if (GTK_IS_TYLER_GRAPH(widget)) {
        widget_show(tyler_combo_box);
        widget_show(tyler_label);
        gtk_tyler_graph_redraw(widget);
        widget_hide(attractor_view_button);
        widget_hide(attractor_view_label);
        widget_hide(attractor_view_label2);
        widget_hide(attractor_view_combo_box);
        widget_hide(drawing_and_copying_graph);


//         widget_hide(drawing_and_copying_combo_box);
//         widget_hide(drawing_and_copying_label);
    }

    else if (GTK_IS_AOA_GRAPH(widget)) {
        widget_show(aoa_combo_box);
        widget_show(aoa_label);
        widget_hide(attractor_view_button);
        widget_hide(attractor_view_label);
        widget_hide(attractor_view_label2);
        widget_hide(attractor_view_combo_box);
        widget_hide(tyler_combo_box);
        widget_hide(tyler_label);
        widget_hide(drawing_and_copying_graph);
        gtk_aoa_graph_redraw(widget);


        //         widget_hide(drawing_and_copying_combo_box);
        //         widget_hide(drawing_and_copying_label);
    }
//     int w, h;
//     gtk_window_get_size(GTK_WINDOW(window), &w, &h);
//     printf("w = %d, h = %d\n", w, h);
    gtkx_flush_events();

}
void notebook_redraw_current_page(GtkNotebook *notebook, gpointer arg1, guint page_num, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    redraw_unknown_widget(extract_widget_from_page(GTK_NOTEBOOK(notebook), page_num));

}


void redraw_current_page(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
//   if (GTK_IS_NOTEBOOK(notebook)){
//   redraw_page(gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
//   }
//   else {
//     printf("on noes!\n");
//     exit(0);
//   }

    redraw_unknown_widget(extract_widget_from_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook))));


}


void new_window(GtkMenuItem *item)
{

    pid_t child;


    child = fork();
    if (child == 0) { // child
        setenv("proc_b", "", 1);
        execvp(global_argv[0], global_argv);
    } else if (child < 0) // error
        perror("fork");



}


void refresh_button(GtkButton *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif


//  	guint page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
// 	if (page_num == GRAPH_PAGE)
// 	{
// 		gtk_graph_redraw(extract_widget_from_page(notebook, page_num));
// // 		printf("page_num = %i\n", page_num);
// 	}
// 	else if (page_num == NAMING_PAGE)
// 	{
// 		gtk_graph_redraw(extract_widget_from_page(notebook, page_num));
//
// 	}
// 	else
// 	{
// 		gtk_unrolled_network_viewer_redraw(extract_widget_from_page(notebook, page_num));
//
// // 		printf("page_num = %i\n", page_num);
// 	}

    redraw_current_page();

}

void real_time_redraw(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if (real_time) {
        redraw_current_page();
    }
    gtkx_flush_events();
}

// static void initialise_states(GtkWidget *button, GtkNotebook *notebook)
// {
//     step_number = 0;
//     randomise_states_all();
//     redraw_current_page();
// }


void reset_semantic_tests(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_naming_graph_reset(GTK_NAMING_GRAPH(naming_graph));
    gtk_sorting_graph_reset(GTK_SORTING_GRAPH(sorting_graph));
    gtk_word_to_picture_graph_reset(GTK_WORD_TO_PICTURE_GRAPH(word_to_picture_graph));
    gtk_drawing_and_copying_graph_reset(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph));

}

static void reset_weights(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    open_filename[0] = '\0';
    open_path[0] = '\0';
    open_name[0] = '\0';

    init_network();
    redraw_current_page();
    gtk_graph_reset(GTK_GRAPH(graph));
    gtkx_flush_events();
    reset_semantic_tests();




}

static void new_weights(GtkMenuItem *item)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    reset_weights();
    gtk_window_set_title(GTK_WINDOW(window), "New weights");

}
void bptt_train_with_subpattern(int p, int s)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t;

    bptt_unclamp_all();
    bptt_randomise_states_all();
    for (t = 0; t < BPTT_T_CLAMPED; t++) {
        bptt_clamp(p, s, t);
    }

    for (t = 0; t < BPTT_T; t++) {

        bptt_update_states(t);

//         _gaussian_noise(t);
// 	bptt_update_states_targets_clamped(t);

        real_time_redraw();
    }
    switch(training_type)
    {
    case williams:
        bptt_accumulate_deltas_williams(p);
        break;
    case williams_ta:
        bptt_accumulate_deltas_williams_ta(p);
        break;
    case mcclleland:
        bptt_accumulate_deltas_mcclleland(p);
        break;
    case mcclleland_ta:
        bptt_accumulate_deltas_mcclleland_ta(p);
        break;
    }




}


void bptt_train_with_subpattern_alt(int p, int s)
{
//as described in Lambon Ralph's email
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t;

    bptt_unclamp_all();
    bptt_randomise_states_all();
    for (t = 0; t < 3*4; t++) {
        bptt_clamp(p, s, t);
    }

//     for (t = 3*4; t < 5*4; t++) {
// unclamped
//     }
//
    for (t = 5*4; t < 7*4; t++) {
        bptt_clamp_all_stages(p, t);
    }

    for (t = 0; t < BPTT_T; t++) {

        bptt_update_states_gaussian_noise(t);

        // 	bptt_update_states_targets_clamped(t);

        real_time_redraw();
    }
    switch(training_type)
    {
    case williams:
        bptt_accumulate_deltas_williams(p);
        break;
    case williams_ta:
        bptt_accumulate_deltas_williams_ta(p);
        break;
    case mcclleland:
        bptt_accumulate_deltas_mcclleland(p);
        break;
    case mcclleland_ta:
        bptt_accumulate_deltas_mcclleland_ta(p);
        break;
    }




}

void tyler_train_with_pattern(int p)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t;

    tyler_unclamp_all();
    tyler_randomise_states_all();
    for (t = 0; t < TYLER_T_CLAMPED; t++) {
        tyler_clamp_full_pattern(p, t);
    }

    for (t = 0; t < TYLER_T; t++) {
        tyler_update_states(t);
        real_time_redraw();
    }
    switch(training_type)
    {
    case 0:
        tyler_accumulate_deltas_williams(p);
        break;
    case 1:
        tyler_accumulate_deltas_williams_ta(p);
        break;
    case 2:
        tyler_accumulate_deltas_mcclleland(p);
        break;
    case 3:
        tyler_accumulate_deltas_mcclleland_ta(p);
        break;
    }




}

void tyler_feedforward_train_with_pattern(int p, int noise, int n)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif


    tyler_feedforward_unclamp();
    tyler_feedforward_randomise_states();
    tyler_feedforward_clamp_full_pattern(p);
    real_time_redraw();


    tyler_feedforward_propagate_noise(noise, n);

//     if ((gtk_combo_box_get_active(GTK_COMBO_BOX(training_algo_combo_box)) == Olivia) || (gtk_combo_box_get_active(GTK_COMBO_BOX(training_algo_combo_box)) == Olivia_alt)) {
    tyler_feedforward_accumulate_delta_weights_noise(TARGET_NOISE, TARGET_N_PATTERN);
//     }
//     else if ((gtk_combo_box_get_active(GTK_COMBO_BOX(training_algo_combo_box)) == Rick) || (gtk_combo_box_get_active(GTK_COMBO_BOX(training_algo_combo_box)) == Rick_alt)) {
//         tyler_feedforward_accumulate_delta_weights_rick();
//
//     }

//             tyler_feedforward_accumulate_delta_weights_rick();

    real_time_redraw();


}

/*
void tyler_feedforward_train_with_pattern_noise(int p, int noise, int n)
{
#if PRINT_FUNC
printf("%s\n", __FUNCTION__);
#endif


tyler_feedforward_unclamp();
tyler_feedforward_randomise_states();
tyler_feedforward_clamp_full_pattern(p);
real_time_redraw();

tyler_feedforward_propagate_noise(noise, n);

//     if ((gtk_combo_box_get_active(GTK_COMBO_BOX(training_algo_combo_box)) == Olivia) || (gtk_combo_box_get_active(GTK_COMBO_BOX(training_algo_combo_box)) == Olivia_alt)) {
tyler_feedforward_accumulate_delta_weights_noise(noise, n);
//     }
//     else if ((gtk_combo_box_get_active(GTK_COMBO_BOX(training_algo_combo_box)) == Rick) || (gtk_combo_box_get_active(GTK_COMBO_BOX(training_algo_combo_box)) == Rick_alt)) {
//         tyler_feedforward_accumulate_delta_weights_rick();
//
//     }

//             tyler_feedforward_accumulate_delta_weights_rick();

real_time_redraw();


}
*/


// void tyler_feedforward_train_patterns_serially_(void)
// {
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//     int c, p, c_min = tyler_feedforward_get_iterations_so_far(), c_max = ((tyler_feedforward_get_iterations_so_far() + TYLER_C) > TYLER_C_MAX) ? (0) : (tyler_feedforward_get_iterations_so_far() + TYLER_C);
//
//     while (paused) {
//         gtkx_flush_events();
//     }
//     if (abort_training) {
//         printf("aborted!\n");
//         return;
//     }
//     gtkx_flush_events();
//     printf("c_min = %i c_max = %i tyler_get_iterations_so_far = %i\n", c_min, c_max, tyler_get_iterations_so_far());
//     for (c = c_min; c < c_max; c++) {
//
//         fprintf(stdout, "Training cycle: %3d", tyler_feedforward_get_iterations_so_far());
//         fflush(stdout);
//
//         for (p = p_min; p < p_max; p++) {
//             //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++)
//             // {
//             tyler_feedforward_train_with_pattern(p);
//
//             real_time_redraw(); //ALERT
//             while (paused) {
//                 gtkx_flush_events();
//             }
//             if (abort_training) {
////               printf("Training aborted!\n");
//               return;
//             }
//             gtkx_flush_events();
//             //       }
//         }
//
//         tyler_feedforward_adjust_weights();
// //     tyler_decay_weights();
//
//         while (paused) {
//             gtkx_flush_events();
//         }
//         if (abort_training) {
//            printf("Training aborted!\n");
//             return;
//         }
//         gtkx_flush_events();
//
//         Total_Error[c] = 0.0;
//
//         //     for (s = 0; s < 3; s++) {
//         //       Error[s][c] = 0.0;
//         //     }
//
//
//         tyler_feedforward_unclamp();
//
//
//
// //     for (p = p_min; p < p_max; p++) {
// //       //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++) {
// //
// // 	tyler_randomise_states(tyler_t1);
// // 	// 			  clamp(p, s, tyler_t1);
// //
// // 	// 			  real_time_settle();
// //
// // 	tyler_clamp_full_pattern(p,tyler_t1);
// // 	tyler_real_time_settle_for(TYLER_T); //for 3*4 timesteps
// // 	real_time_redraw();
// // 	tyler_unclamp_time(tyler_t1);
// // 	tyler_real_time_unclamped_settle();
// // 	real_time_redraw();
// // 	save_error(FALSE, p, 0, c, network_error_measure(net_type, error_type, p, 0));
// // 	while (paused) {
// // 	  gtkx_flush_events();
// // 	}
// // 	if (abort_training) {
// // 	  printf("aborted!\n");
// // 	  return;
// // 	}
// // 	gtkx_flush_events();
// //
// // 	real_time_redraw();
// // 	//       }
// //     }
//
//         // 		Total_Error[c] = 100.0 * Total_Error[c] / (double) (3.0 * P);
//         //       for (s = STAGE_NAME; s <= STAGE_VERBAL; s++) {
//         // 		  Error[s][c] = 100 * Error[s][c] / (double) P;
//         //
//         // 	      for (p = p_min; p < p_max; p++) {
//         // 		  Pattern_Error[p][s][c] = 100 * Pattern_Error[p][s][c];
//         // 	      }
//         // 	    }
//         fprintf(stdout, "Error: %f\n", Total_Error[c]);
//         // RPC things that c is not reinitialised when weights are initialised
//         // RPC thinkgs this causes the graph to not be drawn correctly after reinitialisation
//         //        printf("Total_Error[%d] = %f %p\n", c, Total_Error[c], &Total_Error[c]);
//
//         tyler_feedforward_inc_iterations_so_far();
//         gtk_graph_set_max_iterations(GTK_GRAPH(graph), tyler_feedforward_get_iterations_so_far());
//         gtk_graph_redraw(graph);
//         while (paused) {
//             gtkx_flush_events();
//         }
//         if (abort_training) {
//            printf("Training aborted!\n");
//             return;
//         }
//         gtkx_flush_events();
//
//         if (is_file_open() && autosave) {
//             tyler_save_file(open_filename);
//         }
//
//     }
// }

int pattern_miss[P_MAX];
int type_miss[7];

void init_misses(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int p, t;

    for (p = 0; p < P; p++)
    {
        pattern_miss[p] = 0;
    }

    for (t = 0; t < 7; t++)
    {
        type_miss[t] = 0;
    }
}


// void xbm_train(int p, int t)
// {

// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif


/*
  // unclamp
  bm_unclamp();

  // 	printf("Training...\n\tPhase: Minus Name\n\tPattern: %i/%i\n\tIteration: %i/%i\n\tRate of Learning: %f\n", p+1, P, i+1, i_max, r);

  real_time_redraw();
  gtkx_flush_events();

  // randomise states
  bm_randomise_states();

  real_time_redraw();
  gtkx_flush_events();

  // clamp name
  bm_clamp(p, t);

  real_time_redraw();
  gtkx_flush_events();

  // anneal
  bm_anneal();

  real_time_redraw();
  gtkx_flush_events();

  // collect count with t units clamped
  bm_collect_count(t);

  real_time_redraw();
  gtkx_flush_events();



//   printf("\tTarget Pattern: %s\tOutput Pattern: %s\n", bm_get_name(p), bm_get_name(bm_error_type(p, t, tmp)));

  real_time_redraw();
  gtkx_flush_events();

//   if (!(training))
//   {
//     stop_training(table);
//     return;
//   }


  while(paused)
  {
      gtkx_flush_events();
  }                           */
// }


void bm_train_pdptool_patterns(int i, int i_max)
{

//   #if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
//   #endif



    int p;
    double tmp = 0.0;


    for (p = 0; p < P; p++) {

        while(paused)
        {
            gtkx_flush_events();
        }

        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }

        real_time_redraw();

        bm_unclamp();
        bm_randomise_states();
        bm_clamp_name(p);
        bm_anneal();
        bm_collect_count(BM_MINUS_NAME);

        tmp = network_error_measure(net_type, error_type, p, NAME);
        printf("Training...\n\tStage: Minus Name %i/%i\n\tPattern: %i/%i\n\tIteration: %i/%i\n\tRate of Learning: %f\n\tError: %f%%\n", 1, 5, p+1, P, i+1, i_max, bm_r, tmp * 100.0);
        Total_Error[i] += tmp;
        Error[NAME_ERROR][i] += tmp;

        real_time_redraw();

        while(paused)
        {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }


        gtkx_flush_events();

        bm_unclamp();
        bm_randomise_states();
        bm_clamp_pdptool_name_plus(p);
        bm_anneal();
        bm_collect_count(BM_PLUS_NAME);

        printf("Training...\n\tStage: Plus Name %i/%i\n\tPattern: %i/%i\n\tIteration: %i/%i\n\tRate of Learning: %f\n\tError: n/a\n", 2, 5, p+1, P, i+1, i_max, bm_r);

        real_time_redraw();

        while(paused)
        {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }


        bm_unclamp();
        bm_randomise_states();
        bm_clamp_visual(p);
        bm_anneal();
        bm_collect_count(BM_MINUS_VISUAL);

        tmp = network_error_measure(net_type, error_type, p, VISUAL);
        printf("Training...\n\tStage: Minus Visual %i/%i\n\tPattern: %i/%i\n\tIteration: %i/%i\n\tRate of Learning: %f\n\tError: %f%%\n", 3, 5, p+1, P, i+1, i_max, bm_r, tmp * 100.0);
        Total_Error[i] += tmp;
        Error[VISUAL_ERROR][i] += tmp;

        real_time_redraw();

        while(paused)
        {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }


        gtkx_flush_events();

        bm_unclamp();
        bm_randomise_states();
        bm_clamp_verbal(p);
        bm_anneal();
        bm_collect_count(BM_MINUS_VERBAL);

        tmp = network_error_measure(net_type, error_type, p, VERBAL);
        printf("Training...\n\tStage: Minus Verbal %i/%i\n\tPattern: %i/%i\n\tIteration: %i/%i\n\tRate of Learning: %f\n\tError: %f%%\n", 4, 5, p+1, P, i+1, i_max, bm_r, tmp * 100.0);
        Total_Error[i] += tmp;
        Error[VERBAL_ERROR][i] += tmp;

        real_time_redraw();

        while(paused)
        {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }


        gtkx_flush_events();

        bm_unclamp();
        bm_randomise_states();
        bm_clamp_pdptool_visual_verbal_plus(p);
        bm_anneal();
        bm_collect_count(BM_PLUS);

        printf("Training...\n\tStage: Plus Visual/Verbal %i/%i\n\tPattern: %i/%i\n\tIteration: %i/%i\n\tRate of Learning: %f\n\tError: n/a\n", 5, 5, p+1, P, i+1, i_max, bm_r);

        real_time_redraw();

        while(paused)
        {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }


        gtkx_flush_events();

    }

    Total_Error[i] /= (double) (3.0 * P);
    Total_Error[i] *= 100.0;

    Error[NAME_ERROR][i] /= (double) P; // as we are calculating error on name twive when it's a shared/category name subpattern
    Error[VISUAL_ERROR][i] /= (double) P;
    Error[VERBAL_ERROR][i] /= (double) P;

    Error[NAME_ERROR][i] *= 100.0;
    Error[VISUAL_ERROR][i] *= 100.0;
    Error[VERBAL_ERROR][i] *= 100.0;
    gtkx_flush_events();

    bm_adjust_weights_step_all_pdptool();
//     bm_decay_weights();

    gtkx_flush_events();

}

void bm_train_patterns(int i, int i_max)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

    int p, s, t;
    double tmp = 0.0;


    for (p = 0; p < P; p++) {

        for (s = 0; s < BM_S; s++) {


            t = bm_network_state_to_unit_type(s);
            bm_unclamp();
            bm_randomise_states();
            bm_clamp(p, t);

            if (i > 10000) {
                bm_anneal_tediously_slowly();
            }
            else {
                bm_anneal();

            }
            bm_collect_count(s);
            gtkx_flush_events();

            real_time_redraw();


            if (s != BM_PLUS) {
                tmp = network_error_measure(net_type, error_type, p, t);

                printf("Training...\n\tStage: %i/%i\n\tPattern: %i/%i\n\tIteration: %i/%i\n\tRate of Learning: %f\n\tError: %f%%\n", s+1, BM_S, p+1, P, i+1, i_max, bm_r, tmp * 100.0);


                Total_Error[i] += tmp;
                if (s == BM_MINUS_NAME) {
                    Error[NAME_ERROR][i] += tmp;

                }
                else if (s == BM_MINUS_VISUAL) {

                    Error[VISUAL_ERROR][i] += tmp;

                }
                else if (s == BM_MINUS_VERBAL) {

                    Error[VERBAL_ERROR][i] += tmp;
                }

            }
            else
            {
                printf("Training...\n\tStage: %i/%i\n\tPattern: %i/%i\n\tIteration: %i/%i\n\tRate of Learning: %f\n\tError: n/a\n", s+1, BM_S, p+1, P, i+1, i_max, bm_r);

            }

            while(paused)
            {
                gtkx_flush_events();
            }
            if (abort_training) {
                printf("Training aborted!\n");
                return;
            }

        }
    }


    gtkx_flush_events();

    bm_adjust_weights_step_all();
//     bm_decay_weights();


    Total_Error[i] /= (3.0 * P);
    Total_Error[i] *= 100.0;

    Error[NAME_ERROR][i] /= (double) P;
    Error[VISUAL_ERROR][i] /= (double) P;
    Error[VERBAL_ERROR][i] /= (double) P;

    Error[NAME_ERROR][i] *= 100.0;
    Error[VISUAL_ERROR][i] *= 100.0;
    Error[VERBAL_ERROR][i] *= 100.0;



    gtkx_flush_events();


}


void bm_train_patterns_serially(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int i, i_min = bm_get_iterations_so_far(), i_max = ((bm_get_iterations_so_far() + BM_I) > BM_I_MAX) ? (0) : (bm_get_iterations_so_far() + BM_I);
//     bm_decay_weights();
//     return;


    send_error_to_graph();

    while (paused) {
        gtkx_flush_events();
    }
    if (abort_training) {
        printf("aborted!\n");
        return;
    }

    gtkx_flush_events();

    for (i = i_min; i < i_max; i++) {

        if (decay_learning_rate) {
            if (!(i % 1000)) {
                bm_reset_learning_rate();
                update_learning_rate_spin_button();
            }
        }
        bm_init_counts();
        init_misses();
        Total_Error[i] = 0.0;
        Error[NAME_ERROR][i] = 0.0;
        Error[VISUAL_ERROR][i] = 0.0;
        Error[VERBAL_ERROR][i] = 0.0;


        if (pdptool_style_patterns()) { // PDPTOOL's Patterns


            bm_train_pdptool_patterns(i, i_max);



        }
        else {


            bm_train_patterns(i, i_max);



        }

        if (decay_learning_rate) {
            bm_decay_learning_rate();

            update_learning_rate_spin_button();
        }



        gtkx_flush_events();
        redraw_current_page();

        bm_inc_iterations_so_far();
        gtk_graph_set_max_iterations(GTK_GRAPH(graph), bm_get_iterations_so_far());
        gtk_graph_redraw(graph);

        while (paused) {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }
        gtkx_flush_events();

        if (is_file_open() && autosave) {
            save_network(open_filename);
        }

    }


}


void init_train_flags(void)
{
    training = 0;
    paused = 0;
    if (GTK_IS_BUTTON(train_button)) {
        gtk_button_set_label(GTK_BUTTON(train_button), "_Start");

    }
    ghost_controls(FALSE);
}
void train_patterns(GtkButton *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    abort_training = 0;
    if (!testing) {
        if ((!paused) && (!training)) { //if not paused and not training, start training
            training = 1;
            paused = 0;

            gtk_button_set_label(button, "Pau_se");
            ghost_controls(TRUE);
            ghost_button("Pau_se", FALSE);

            if (net_type == BPTT) {
                bptt_train_patterns_serially();

            }
            else if (net_type == TYLER_RECURRENT) {
                tyler_train_patterns_serially();

            }

            else if (net_type == TYLER_FEEDFORWARD) {
                tyler_feedforward_train_patterns_serially();

            }

            else if (net_type == BM) {
                bm_train_patterns_serially();

            }
            else if (net_type == PDPTOOL) {

            }
            training = 0;
            paused = 0;
            abort_training = 0;

            gtk_button_set_label(button, "_Start");
            ghost_controls(FALSE);
            return;
        }


    }


    if (training) {
        paused = !paused;

        //             ghost_controls(FALSE);
    }
    else {
        paused = 0;
    }



    if (paused && training) {
        gtk_button_set_label(button, "Re_sume");
        ghost_controls(FALSE);
    } else if (paused) {
        gtk_button_set_label(button, "_Start");
        ghost_controls(FALSE);
    } else {
        gtk_button_set_label(button, "Pau_se");
        ghost_controls(TRUE);
        ghost_button("Pau_se", FALSE);
    }



//     while(paused)
//     {
//       gtkx_flush_events();
//     }
//     testing  = 0;
//     training = 0;
//     paused = 0;

//     printf("\n\npaused = %i\n", paused);

}

void bm_real_time_anneal_equilibrium(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double temperature = BM_T_MAX;
    double d = 1.0;
//     int i = 0;

//     printf("d = %f, t = %f\n", d, temperature);

    //     double d = 1.0;
    while ((d > 0.00001) || (temperature > 0.0001)) {

//       if (temperature < BM_T_MIN) {
// 	return;
//       }
        //         d = bptt_settle_step(t);



        d = bm_flip_units_delta(temperature);//, t);
        real_time_redraw();
        gtkx_flush_events();
// 	printf("%i, d = %1.9f, t = %f\n", i, d, temperature);


        temperature *= BM_T_RATE;
//         i++;
    }

}


void bm_real_time_clamped_anneal(void)
{
    double temperature;
//     int i = 0;
    for (temperature = BM_T_MAX; temperature > BM_T_MIN; temperature *= BM_T_RATE)
    {
        bm_flip_units_delta(temperature);//, t);
        real_time_redraw();
        gtkx_flush_events();
//         printf("%i, d = %f, t = %f\n", i, d, temperature);

//         i++;

    }
}

void bm_real_time_semiclamped_anneal(void)
{
    double temperature;
    int i = 0, p, t;
    if (bm_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        t = bm_network_state_to_unit_type(gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button)));
    }
    else {
        p = bm_get_clamped_pattern();
        t = bm_get_clamped_type();
    }
    bm_clamp(p, t);
    for (temperature = BM_T_MAX; temperature > BM_T_MIN; temperature *= BM_T_RATE)
    {

        bm_flip_units(temperature);//, t);
        real_time_redraw();
        gtkx_flush_events();
        //         printf("t = %f\n", temperature);
//         if (i > 200) {
//             bm_unclamp();
//         }
//         printf("temperature = %f, i = %d\n", temperature, i);

//         i++;
    }
    bm_unclamp();

    for (i = 0; i < 50; i++) {
        //
        bm_flip_units(BM_T_MIN);
        real_time_redraw();
        gtkx_flush_events();


    }
//     bm_clamp(p, t);
    //     printf("%i\n", i);
}


// void bm_real_time_semiclamped_anneal(void)
// {
//     double temperature;
//     int i = 0, p, t;
//     if (bm_get_clamped_pattern() == -1)
//     {
//         p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
//         t = bm_network_state_to_unit_type(gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button)));
//     }
//     else {
//         p = bm_get_clamped_pattern();
//         t = bm_get_clamped_type();
//     }
//     bm_clamp(p, t);
//     for (temperature = BM_T_MAX; temperature > BM_T_MIN; temperature *= BM_T_RATE)
//     {
//
//         bm_flip_units(temperature);
//         real_time_redraw();
//         gtkx_flush_events();
// //     if (i > 180) {
// //     }
//
// //     i++;
//
//     }
//
//     bm_unclamp();
//
//     for (i = 0; i < 2000; i++) {
//
//         bm_flip_units(temperature);
//         real_time_redraw();
//         gtkx_flush_events();
//
//
//     }
// //       printf("input = %d, output = %d\n", p, n(net_type, error_type, NAME));
//
//
//
// }
//
void bm_real_time_semiclamped_anneal_save_settling_attractor(void)
{
    double temperature;
    int i = 0, p, t;
    if (bm_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        t = bm_network_state_to_unit_type(gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button)));
    }
    else {
        p = bm_get_clamped_pattern();
        t = bm_get_clamped_type();
    }
    bm_clamp(p, t);
    for (temperature = BM_T_MAX; temperature > BM_T_MIN; temperature *= BM_T_RATE)
    {

        bm_flip_units(temperature);//, t);
        network_save_current_settling_attractor_for_clustering(net_type, p) ;

        real_time_redraw();
        gtkx_flush_events();
        //         printf("t = %f\n", temperature);
        if (i > 180) {
            bm_unclamp();
        }

        i++;
    }
    //     bm_clamp(p, t);
    //     printf("%i\n", i);
}


void bm_real_time_anneal_semantic_task(void)
{
    double temperature;
    int i = 0, p, t;
    if (bm_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        t = bm_network_state_to_unit_type(gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button)));
    }
    else {
        p = bm_get_clamped_pattern();
        t = bm_get_clamped_type();
    }
    bm_clamp(p, t);

    for (temperature = BM_T_MAX; temperature > BM_T_MIN; temperature *= BM_T_RATE)
    {

        bm_flip_units(temperature);//, t);
        real_time_redraw();
        gtkx_flush_events();
        //         printf("t = %f\n", temperature);
        if (i > 180) {
            bm_unclamp();
        }
//         printf("i = %d\n", i);
        i++;
    }
//   bm_clamp(p, t);
    //     printf("%i\n", i);
}


void bm_real_time_unclamped_anneal(void)
{
    double temperature;
//   int i = 0;
//   if (i > 180) {
    bm_unclamp();
//   }
    for (temperature = BM_T_MAX; temperature > BM_T_MIN; temperature *= BM_T_RATE)
    {

        bm_flip_units(temperature);//, t);
        real_time_redraw();
        gtkx_flush_events();
        //         printf("t = %f\n", temperature);


//     i++;
    }
    //     printf("%i\n", i);
}

void bptt_real_time_settle_for(int t_max)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t;
    for (t = 0; t < t_max; t++) {
        bptt_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();
    }
}

void tyler_real_time_settle_for(int t_max)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t;
    for (t = 0; t < t_max; t++) {
        tyler_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();
    }
}

void tyler_real_time_settle_for_noise(int t_max, int noise, int n)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t;
    for (t = 0; t < t_max; t++) {
        tyler_settle_step_noise(t, noise, n);
        real_time_redraw();
        gtkx_flush_events();
    }
}

void pdptool_real_time_settle_for(int t_max)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t;
//     double d = 1.0;
    for (t = 0; t < t_max; t++) {
        //d = pdptool_settle_step(t);
        pdptool_settle_step(t);
// 	printf("d = %f\n", d);
        real_time_redraw();
        gtkx_flush_events();
    }



}



// void button_unclamp_all(GtkButton *button)
// {
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//     bptt_unclamp_all();
//     real_time_redraw();
//     gtkx_flush_events();
// }

void bptt_real_time_settle_equilibrium(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t = 0;
//     double d = 1.0;
    while (d > BPTT_SETTLING_THRESHOLD) {
        if (t > 100) {
            return;
        }
//         d = bptt_settle_step(t);
        bptt_settle_step(t);
        gtkx_flush_events();
        t++;
        real_time_redraw();
        gtkx_flush_events();
    }
}
void tyler_real_time_settle_equilibrium(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t = 0;
    //     double d = 1.0;
    while (d > TYLER_SETTLING_THRESHOLD) {
        if (t > 100) {
            return;
        }
        //         d = bptt_settle_step(t);
        tyler_settle_step(t);
        gtkx_flush_events();
        t++;
        real_time_redraw();
        gtkx_flush_events();
    }
}

void tyler_real_time_settle_equilibrium_noise(int noise, int n)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t = 0;
    //     double d = 1.0;
    while (d > TYLER_SETTLING_THRESHOLD) {
        if (t > 100) {
            return;
        }
        //         d = bptt_settle_step(t);
        tyler_settle_step_noise(t, noise, n);
        gtkx_flush_events();
        t++;
        real_time_redraw();
        gtkx_flush_events();
    }
}
void pdptool_real_time_settle_equilibrium(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t = 0;
    double d = 1.0;

    while (d > PDPTOOL_SETTLING_THRESHOLD) {
        if (t > 100) {
            return;
        }
        d = pdptool_settle_step(t);
//         pdptool_settle_step(t);
        gtkx_flush_events();
        t++;
        real_time_redraw();
        gtkx_flush_events();
    }
}

// #undef SETTLE_PROPERLY

void bptt_real_time_unclamped_settle(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double d = 1.0;
    int t = 1;

    bptt_unclamp_all();

    // #ifdef SETTLE_PROPERLY
    while (d > BPTT_SETTLING_THRESHOLD) {
        if (t > 100) {
            return;
        }
        d = bptt_settle_step(t++);

        //bptt_settle_step(t++);
        real_time_redraw();
        gtkx_flush_events();
    }
    // #else
    //     while (t < 500) { // RC WAZ 'ERE
    //         d = bptt_settle_step(t++);
    //         real_time_redraw();
    //         gtkx_flush_events();
    //     }
    // #endif
    real_time_redraw();
    gtkx_flush_events();
}


void bptt_real_time_settle_for_save_settling_attractor(int t_max, int p)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t;
    for (t = 0; t < t_max; t++) {
        bptt_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();
        network_save_current_settling_attractor_for_clustering(net_type, p) ;

    }
}

void bptt_real_time_unclamped_settle_save_settling_attractor(int p)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double d = 1.0;
    int t = 1;

//     bptt_unclamp_all();

    // #ifdef SETTLE_PROPERLY
    while (d > BPTT_SETTLING_THRESHOLD) {
        if (t > (100-12)) {
            return;
        }
        d = bptt_settle_step(t++);
        network_save_current_settling_attractor_for_clustering(net_type, p) ;
        //bptt_settle_step(t++);
        real_time_redraw();
        gtkx_flush_events();
// 	    printf("Settled for %d times\n", t);
    }


    // #else
    //     while (t < 500) { // RC WAZ 'ERE
    //         d = bptt_settle_step(t++);
    //         real_time_redraw();
    //         gtkx_flush_events();
    //     }
    // #endif
    real_time_redraw();
    gtkx_flush_events();
}

void tyler_real_time_unclamped_settle(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double d = 1.0;
    int t = 1;

    tyler_unclamp_all();

    // #ifdef SETTLE_PROPERLY
    while (d > TYLER_SETTLING_THRESHOLD) {
        if (t > 100) {
            return;
        }
        d = tyler_settle_step(t++);

        //bptt_settle_step(t++);
        real_time_redraw();
        gtkx_flush_events();
    }
    // #else
    //     while (t < 500) { // RC WAZ 'ERE
    //         d = bptt_settle_step(t++);
    //         real_time_redraw();
    //         gtkx_flush_events();
    //     }
    // #endif
    real_time_redraw();
    gtkx_flush_events();
}

void tyler_real_time_unclamped_settle_noise(int noise, int n)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double d = 1.0;
    int t = 1;

    tyler_unclamp_all();

    // #ifdef SETTLE_PROPERLY
    while (d > TYLER_SETTLING_THRESHOLD) {
        if (t > 100) {
            return;
        }
        d = tyler_settle_step_noise(t++, noise, n);

        //bptt_settle_step(t++);
        real_time_redraw();
        gtkx_flush_events();
    }
    // #else
    //     while (t < 500) { // RC WAZ 'ERE
    //         d = bptt_settle_step(t++);
    //         real_time_redraw();
    //         gtkx_flush_events();
    //     }
    // #endif
    real_time_redraw();
    gtkx_flush_events();
}

void pdptool_real_time_unclamped_settle(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    double d = 1.0;
    int t = 1;

    pdptool_unclamp();

// #ifdef SETTLE_PROPERLY
    while (d > PDPTOOL_SETTLING_THRESHOLD) {
        if (t > 100) {
            return;
        }
        d = pdptool_settle_step(t++);
        real_time_redraw();
        gtkx_flush_events();
    }
// #else
//     while (t < 500) { // RC WAZ 'ERE
//         d = pdptool_settle_step(t++);
//         real_time_redraw();
//         gtkx_flush_events();
//     }
// #endif
    real_time_redraw();
    gtkx_flush_events();
}

void pdptool_real_time_rogers_settle(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t, p, s;
//     double d = 1.0;
    if (pdptool_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        s = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button));
    }
    else {
        p = pdptool_get_clamped_pattern();
        s = pdptool_get_clamped_stage();
    }

    pdptool_clamp_now(p, s);

    for (t = 0; t < 12; t++) {    // 12 clamped
//         d = pdptool_settle_step(t);
        pdptool_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();


    }

    pdptool_unclamp();

    for (t = 12; t < 29; t++) {   // 17 unclamped
        d = pdptool_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();


    }

    //     exit(0);

    //     pdptool_print_weights();

    pdptool_clamp_now(p, s);
}
//

void pdptool_semantic_task_settle(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int p, s;// t;
//     double d = 1.0;
    if (pdptool_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        s = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button));
    }
    else {
        p = pdptool_get_clamped_pattern();
        s = pdptool_get_clamped_stage();
    }

    pdptool_clamp_now(p, s);
    pdptool_real_time_settle_for(12); //for 3*4 timesteps
    pdptool_unclamp();
    pdptool_real_time_unclamped_settle();

    //     exit(0);

    //     pdptool_print_weights();

    pdptool_clamp_now(p, s);
}
//


void pdptool_real_time_settle(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif


    switch(settling_type)
    {
    case 0:
        pdptool_real_time_settle_equilibrium();
        break;
    case 1:
        pdptool_real_time_settle_for(PDPTOOL_T);
        break;
    case 2:
        pdptool_real_time_rogers_settle();
        break;
    case 3:
        pdptool_semantic_task_settle();
        break;

    }
}
void bptt_real_time_rogers_settle(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t, p, s;
//     double d = 1.0;
    if (bptt_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        s = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button));
    }
    else {
        p = bptt_get_clamped_pattern();
        s = bptt_get_clamped_stage();
    }

    bptt_clamp_now(p, s, bptt_t1);

    for (t = 0; t < 12; t++) {    // 12 clamped
//         d = bptt_settle_step(t);
        bptt_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();
    }

    bptt_unclamp_time(bptt_t1);

    for (t = 12; t < 29; t++) {   // 17 unclamped
        d = bptt_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();
    }
    bptt_clamp_now(p, s, bptt_t1);
}


void tyler_real_time_rogers_settle(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t, p, s;
    //     double d = 1.0;
    if (tyler_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        s = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button));
    }
    else {
        p = tyler_get_clamped_pattern();
        s = tyler_get_clamped_stage();
    }

    tyler_clamp_now(p, s,tyler_t1);

    for (t = 0; t < 12; t++) {    // 12 clamped
//         d = bptt_settle_step(t);
        tyler_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();
    }

    tyler_unclamp_time(tyler_t1);

    for (t = 12; t < 29; t++) {   // 17 unclamped
        d = tyler_settle_step(t);
        real_time_redraw();
        gtkx_flush_events();
    }
    //   tyler_clamp_full_pattern_now(p, tyler_t1);
}


void tyler_real_time_rogers_settle_noise(int noise, int n)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int t, p, s;
    //     double d = 1.0;
    if (tyler_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        s = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button));
    }
    else {
        p = tyler_get_clamped_pattern();
        s = tyler_get_clamped_stage();
    }

    tyler_clamp_now_noise(p, s,tyler_t1, noise, n);

    for (t = 0; t < 12; t++) {    // 12 clamped
//         d = bptt_settle_step(t);
        tyler_settle_step_noise(t, noise, n);
        real_time_redraw();
        gtkx_flush_events();
    }

    tyler_unclamp_time(tyler_t1);

    for (t = 12; t < 29; t++) {   // 17 unclamped
        d = tyler_settle_step_noise(t, noise, n);
        real_time_redraw();
        gtkx_flush_events();
    }
    //   tyler_clamp_full_pattern_now(p, tyler_t1);
}



void bptt_semantic_task_settle(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int p, s;//, t;
    //     double d = 1.0;
    if (bptt_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        s = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button));
    }
    else {
        p = bptt_get_clamped_pattern();
        s = bptt_get_clamped_stage();
    }

    bptt_clamp_now(p, s, bptt_t1);

    bptt_real_time_settle_for(12); //for 3*4 timesteps
    bptt_unclamp_time(bptt_t1);
    bptt_real_time_unclamped_settle();
//   bptt_clamp_now(p, s, bptt_t1);
}

void tyler_semantic_task_settle(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int p, s;//, t;
    //     double d = 1.0;
    if (tyler_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        s = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button));
    }
    else {
        p = tyler_get_clamped_pattern();
        s = tyler_get_clamped_stage();
    }

    tyler_clamp_now(p, s, tyler_t1);

    tyler_real_time_settle_for(12); //for 3*4 timesteps
    tyler_unclamp_time(tyler_t1);
    tyler_real_time_unclamped_settle();
    //   tyler_clamp_now(p, s, tyler_t1);
}

void tyler_semantic_task_settle_noise(int noise, int n)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int p, s;//, t;
    //     double d = 1.0;
    if (tyler_get_clamped_pattern() == -1)
    {
        p = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pattern_spin_button));
        s = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stage_spin_button));
    }
    else {
        p = tyler_get_clamped_pattern();
        s = tyler_get_clamped_stage();
    }

    tyler_clamp_now_noise(p, s, tyler_t1, noise, n);

    tyler_real_time_settle_for_noise(12, noise, n); //for 3*4 timesteps
    tyler_unclamp_time(tyler_t1);
    tyler_real_time_unclamped_settle_noise(noise, n);
    //   tyler_clamp_now(p, s, tyler_t1);
}

void bptt_real_time_settle(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif




    switch(settling_type)
    {
    case 0:
        bptt_real_time_settle_equilibrium();
        break;
    case 1:
        bptt_real_time_settle_for(BPTT_T);
        break;
    case 2:
        bptt_real_time_rogers_settle();
        break;
    case 3:
        bptt_semantic_task_settle();
        break;


    }
}
void tyler_real_time_settle(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif




    switch(settling_type)
    {
    case 0:
        tyler_real_time_settle_equilibrium();
        break;
    case 1:
        tyler_real_time_settle_for(TYLER_T);
        break;
    case 2:
        tyler_real_time_rogers_settle();
        break;
    case 3:
        tyler_semantic_task_settle();
        break;


    }
}

void tyler_real_time_settle_noise(int noise, int n)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif




    switch(settling_type)
    {
    case 0:
        tyler_real_time_settle_equilibrium_noise(noise, n);
        break;
    case 1:
        tyler_real_time_settle_for_noise(TYLER_T, noise, n);
        break;
    case 2:
        tyler_real_time_rogers_settle_noise(noise, n);
        break;
    case 3:
        tyler_semantic_task_settle_noise(noise, n);
        break;


    }
}


void bm_real_time_anneal_for(int t) {
}

void bm_real_time_rogers_settle() {
}

void bm_semantic_task_settle() {
}


void bm_real_time_anneal(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif




    switch(annealing_type)
    {
    case 0:
        bm_real_time_anneal_equilibrium();
        break;
    case 1:
        bm_real_time_clamped_anneal();
        break;
    case 2:
        bm_real_time_semiclamped_anneal();
        break;
    case 3:
        bm_real_time_unclamped_anneal();
        break;


    }
}

void bptt_train_patterns_serially(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int i, c, p, s, c_min = bptt_get_iterations_so_far(), c_max = ((bptt_get_iterations_so_far() + BPTT_C) > BPTT_C_MAX) ? (0) : (bptt_get_iterations_so_far() + BPTT_C);
    char tmp_filename_no_ext[256], tmp_filename[256];
    sprintf(tmp_filename_no_ext, "%s", open_filename);
    for (i = strlen(tmp_filename_no_ext)-1; i > 0; i--) {

        printf("tmp_filename_no_ext = %s\n", tmp_filename_no_ext);


        if (tmp_filename_no_ext[i] == '.') {

            tmp_filename_no_ext[i]  = '\0';
            break;
        }

        tmp_filename_no_ext[i]  = '\0';

    }

    printf("tmp_filename_no_ext = %s\n", tmp_filename_no_ext);

    while (paused) {
        gtkx_flush_events();
    }
    if (abort_training) {
        printf("aborted!\n");
        return;
    }
    gtkx_flush_events();
    printf("c_min = %i c_max = %i bptt_get_iterations_so_far = %i\n", c_min, c_max, bptt_get_iterations_so_far());
    for (c = c_min; c < c_max; c++) {
        if (multiples_of(c, 5000)) {
            sprintf(tmp_filename, "%s_%i.weight", tmp_filename_no_ext, c);
            bptt_save_file(tmp_filename);
        }
        fprintf(stdout, "Training cycle: %3d", bptt_get_iterations_so_far());
        fflush(stdout);

        for (p = p_min; p < p_max; p++) {
            for (s = BPTT_STAGE_NAME; s <= BPTT_STAGE_VERBAL; s++) {
                bptt_train_with_subpattern_alt(p, s);

                real_time_redraw(); //ALERT
                while (paused) {
                    gtkx_flush_events();
                }
                if (abort_training) {
                    printf("Training aborted!\n");
                    return;
                }
                gtkx_flush_events();
                if (weight_update_type == BPTT_WEIGHT_UPDATE_STAGE) {
                    bptt_adjust_weights();
                    bptt_decay_weights();
                }
            }
            if (weight_update_type == BPTT_WEIGHT_UPDATE_PATTERN) {
                bptt_adjust_weights();
                bptt_decay_weights();
            }
        }

        if (weight_update_type == BPTT_WEIGHT_UPDATE_BATCH) {
            bptt_adjust_weights();
            bptt_decay_weights();
        }

        while (paused) {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }
        gtkx_flush_events();

        Total_Error[c] = 0.0;

        for (s = 0; s < 3; s++) {
            Error[s][c] = 0.0;
        }


        bptt_unclamp_all();
        for (p = p_min; p < p_max; p++) {
            for (s = BPTT_STAGE_NAME; s <= BPTT_STAGE_VERBAL; s++) {

                bptt_randomise_states(bptt_t1);
                // 			  clamp(p, s, bptt_t1);

                // 			  real_time_settle();

                bptt_clamp(p, s, bptt_t1);
                bptt_real_time_settle_for(BPTT_T); //for 3*4 timesteps
                real_time_redraw();
                bptt_unclamp_time(bptt_t1);
                bptt_real_time_unclamped_settle();
                real_time_redraw();
                save_error(FALSE, p, s, c, network_error_measure(net_type, error_type, p, s));
                while (paused) {
                    gtkx_flush_events();
                }
                if (abort_training) {
                    printf("Training aborted!\n");
                    return;
                }
                gtkx_flush_events();

                real_time_redraw();
            }
        }

// 		Total_Error[c] = 100.0 * Total_Error[c] / (double) (3.0 * P);
//       for (s = STAGE_NAME; s <= STAGE_VERBAL; s++) {
// 		  Error[s][c] = 100 * Error[s][c] / (double) P;
//
// 	      for (p = p_min; p < p_max; p++) {
// 		  Pattern_Error[p][s][c] = 100 * Pattern_Error[p][s][c];
// 	      }
// 	    }
        fprintf(stdout, "Error: %f\n", Total_Error[c]);
// RPC things that c is not reinitialised when weights are initialised
// RPC thinkgs this causes the graph to not be drawn correctly after reinitialisation
//        printf("Total_Error[%d] = %f %p\n", c, Total_Error[c], &Total_Error[c]);

        bptt_inc_iterations_so_far();
        gtk_graph_set_max_iterations(GTK_GRAPH(graph), bptt_get_iterations_so_far());
        gtk_graph_redraw(graph);
        while (paused) {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }
        gtkx_flush_events();

        if (is_file_open() && autosave) {
            bptt_save_file(open_filename);
        }

    }
}

void tyler_train_patterns_serially(void)
{

//     if (aoa_train)
//     {
//         tyler_train_patterns_serially_aoa();
//
//         return;
//     }

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int n, c, p, c_min = tyler_get_iterations_so_far(), c_max = ((tyler_get_iterations_so_far() + TYLER_C) > TYLER_C_MAX) ? (0) : (tyler_get_iterations_so_far() + TYLER_C), keep_training;

    while (paused) {
        gtkx_flush_events();
    }
    if (abort_training) {
        printf("aborted!\n");
        return;
    }
    gtkx_flush_events();
    printf("c_min = %i c_max = %i tyler_get_iterations_so_far = %i\n", c_min, c_max, tyler_get_iterations_so_far());
    for (c = c_min; c < c_max; c++) {

        fprintf(stdout, "Training cycle: %3d", tyler_get_iterations_so_far());
        fflush(stdout);

        for (p = 0; p < P; p++) {
            n = random_int(0, 3);
//       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++)
// {
            if (train_pattern[p] & early_pattern[p]) {

                fprintf(stdout, "Pattern: %d", p);

                tyler_train_with_pattern(p);

                real_time_redraw(); //ALERT
                while (paused) {
                    gtkx_flush_events();
                }
                if (abort_training) {
                    printf("Training aborted!\n");
                    return;
                }
                gtkx_flush_events();

            }
//       }
        }

        tyler_adjust_weights();
        tyler_decay_weights();

        while (paused) {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }
        gtkx_flush_events();

        Total_Error[c] = 0.0;

//     for (s = 0; s < 3; s++) {
//       Error[s][c] = 0.0;
//     }


        tyler_unclamp_all();
        for (p = p_min; p < p_max; p++) {
//       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++) {

            tyler_randomise_states(tyler_t1);
            // 			  clamp(p, s, tyler_t1);

            // 			  real_time_settle();

            tyler_clamp_full_pattern(p,tyler_t1);
            tyler_real_time_settle_for(TYLER_T); //for 3*4 timesteps
            real_time_redraw();
            tyler_unclamp_time(tyler_t1);
            tyler_real_time_unclamped_settle();
            real_time_redraw();
            save_error(FALSE, p, NO_SUBPATTERNS, c, network_error_measure(net_type, error_type, p, VISIBLE));
            while (paused) {
                gtkx_flush_events();
            }
            if (abort_training) {
                printf("Training aborted!\n");
                return;
            }
            gtkx_flush_events();

            real_time_redraw();
//       }
        }

        // 		Total_Error[c] = 100.0 * Total_Error[c] / (double) (3.0 * P);
        //       for (s = STAGE_NAME; s <= STAGE_VERBAL; s++) {
        // 		  Error[s][c] = 100 * Error[s][c] / (double) P;
        //
        // 	      for (p = p_min; p < p_max; p++) {
        // 		  Pattern_Error[p][s][c] = 100 * Pattern_Error[p][s][c];
        // 	      }
        // 	    }
        fprintf(stdout, "Error: %f\n", Total_Error[c]);
        // RPC things that c is not reinitialised when weights are initialised
        // RPC thinkgs this causes the graph to not be drawn correctly after reinitialisation
        //        printf("Total_Error[%d] = %f %p\n", c, Total_Error[c], &Total_Error[c]);

        tyler_inc_iterations_so_far();
        gtk_graph_set_max_iterations(GTK_GRAPH(graph), tyler_get_iterations_so_far());
        gtk_graph_redraw(graph);
        while (paused) {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }
        gtkx_flush_events();

        if (is_file_open() && autosave) {
            tyler_save_file(open_filename);
        }

        if (error_type == RMSD) {
            keep_training = 0;
            for (p = 0; p < p_max; p++) {
                //             if (Total_Error[c] > 0.015) {

// 	      printf("Pattern: %f\n", Pattern_Error[0][p]);

                if (Pattern_Error[0][p] > 0.01) {
                    keep_training = 1;
                    break;
                }
            }

            if (!keep_training) {
                c = c_max;

                printf("RSS error per pattern under 0.01, training ended (Tyler et al., 2000).\n");
                //                 printf("Mean RSS error under 0.015, training ended (Tyler et al., 2000).\n");
                break;

            }

        }


    }
}

// void tyler_train_patterns_serially_aoa(void)
// {
//
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//     int c, p, c_min = tyler_get_iterations_so_far(), c_max = ((tyler_get_iterations_so_far() + TYLER_C) > TYLER_C_MAX) ? (0) : (tyler_get_iterations_so_far() + TYLER_C);
//     int midpoint = abs(P/2.0);
//     while (paused) {
//         gtkx_flush_events();
//     }
//     if (abort_training) {
//         printf("aborted!\n");
//         return;
//     }
//     gtkx_flush_events();
//     int a, keep_training;
//     printf("c_min = %i c_max = %i tyler_get_iterations_so_far = %i\n", c_min, c_max, tyler_get_iterations_so_far());
//     for (c = c_min; c < c_max; c++) {
//
//         fprintf(stdout, "Training stage: Early AoA cycle: %3d ", tyler_get_iterations_so_far());
// //     fprintf(stdout, "%d ", midpoint);
//
// //     fflush(stdout);
//
//         for (p = 0; p < midpoint; p++) {
//             //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++)
//             // {
//             tyler_train_with_pattern(p);
//
//             real_time_redraw(); //ALERT
//             while (paused) {
//                 gtkx_flush_events();
//             }
//             if (abort_training) {
//                 printf("Training aborted!\n");
//                 return;
//             }
//             gtkx_flush_events();
//             //       }
//         }
//
//         tyler_adjust_weights();
//         tyler_decay_weights();
//
//         while (paused) {
//             gtkx_flush_events();
//         }
//         if (abort_training) {
//             printf("Training aborted!\n");
//             return;
//         }
//         gtkx_flush_events();
//
//         Total_Error[c] = 0.0;
//
//         //     for (s = 0; s < 3; s++) {
//         //       Error[s][c] = 0.0;
//         //     }
//
//
//         tyler_unclamp_all();
//         for (p = p_min; p < midpoint; p++) {
//             //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++) {
//
//             tyler_randomise_states(tyler_t1);
//             // 			  clamp(p, s, tyler_t1);
//
//             // 			  real_time_settle();
//
//             tyler_clamp_full_pattern(p,tyler_t1);
//             tyler_real_time_settle_for(TYLER_T); //for 3*4 timesteps
//             real_time_redraw();
//             tyler_unclamp_time(tyler_t1);
//             tyler_real_time_unclamped_settle();
//             real_time_redraw();
//             save_error(FALSE, p, NO_SUBPATTERNS, c, network_error_measure(net_type, error_type, p, VISIBLE));
//             while (paused) {
//                 gtkx_flush_events();
//             }
//             if (abort_training) {
//                 printf("Training aborted!\n");
//                 return;
//             }
//             gtkx_flush_events();
//
//             real_time_redraw();
//             //       }
//         }
//
//         // 		Total_Error[c] = 100.0 * Total_Error[c] / (double) (3.0 * P);
//         //       for (s = STAGE_NAME; s <= STAGE_VERBAL; s++) {
//         // 		  Error[s][c] = 100 * Error[s][c] / (double) P;
//         //
//         // 	      for (p = p_min; p < p_max; p++) {
//         // 		  Pattern_Error[p][s][c] = 100 * Pattern_Error[p][s][c];
//         // 	      }
//         // 	    }
//         fprintf(stdout, "Error: %f\n", Total_Error[c]);
//         // RPC things that c is not reinitialised when weights are initialised
//         // RPC thinkgs this causes the graph to not be drawn correctly after reinitialisation
//         //        printf("Total_Error[%d] = %f %p\n", c, Total_Error[c], &Total_Error[c]);
//
//         tyler_inc_iterations_so_far();
//         gtk_graph_set_max_iterations(GTK_GRAPH(graph), tyler_get_iterations_so_far());
//         gtk_graph_redraw(graph);
//         while (paused) {
//             gtkx_flush_events();
//         }
//         if (abort_training) {
//             printf("Training aborted!\n");
//             return;
//         }
//         gtkx_flush_events();
//
//         if (is_file_open() && autosave) {
//             tyler_save_file(open_filename);
//         }
//
//
//         if (error_type == RMSD) {
//             keep_training = 0;
//             for (p = 0; p < midpoint; p++) {
//                 //             if (Total_Error[c] > 0.015) {
//                 if (Pattern_Error[0][p] > 0.01) {
//                     keep_training = 1;
//                     break;
//                 }
//             }
//
//             if (!keep_training) {
//                 c = c_max;
//
//                 printf("RSS error per pattern under 0.01, training ended (Tyler et al., 2000).\n");
//                 //                 printf("Mean RSS error under 0.015, training ended (Tyler et al., 2000).\n");
//                 break;
//
//             }
//
//         }
//     }
//
//     for (c = c_max; c < c_max*2; c++) {
//
//         fprintf(stdout, "Training stage: Late AoA cycle: %3d ", tyler_get_iterations_so_far());
//         //     fprintf(stdout, "%d ", midpoint);
//
//         //     fflush(stdout);
//
//         for (p = 0; p < midpoint; p++) {
//             //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++)
//             // {
//             tyler_train_with_pattern(p);
//
//             real_time_redraw(); //ALERT
//             while (paused) {
//                 gtkx_flush_events();
//             }
//             if (abort_training) {
//                 printf("Training aborted!\n");
//                 return;
//             }
//             gtkx_flush_events();
//             //       }
//         }
//         for (a = 0; a < 2; a++) {  // fucntion of the Late AoA to "compensate" for the Early AoA
//             for (p = midpoint; p < P; p++) {
//                 //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++)
//                 // {
//                 tyler_train_with_pattern(p);
//
//                 real_time_redraw(); //ALERT
//                 while (paused) {
//                     gtkx_flush_events();
//                 }
//                 if (abort_training) {
//                     printf("Training aborted!\n");
//                     return;
//                 }
//                 gtkx_flush_events();
//                 //       }
//             }
//         }
//
//         tyler_adjust_weights();
//         tyler_decay_weights();
//
//         while (paused) {
//             gtkx_flush_events();
//         }
//         if (abort_training) {
//             printf("Training aborted!\n");
//             return;
//         }
//         gtkx_flush_events();
//
//         Total_Error[c] = 0.0;
//
//         //     for (s = 0; s < 3; s++) {
//         //       Error[s][c] = 0.0;
//         //     }
//
//
//         tyler_unclamp_all();
//         for (p = p_min; p < P; p++) {
//             //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++) {
//
//             tyler_randomise_states(tyler_t1);
//             // 			  clamp(p, s, tyler_t1);
//
//             // 			  real_time_settle();
//
//             tyler_clamp_full_pattern(p,tyler_t1);
//             tyler_real_time_settle_for(TYLER_T); //for 3*4 timesteps
//             real_time_redraw();
//             tyler_unclamp_time(tyler_t1);
//             tyler_real_time_unclamped_settle();
//             real_time_redraw();
//             save_error(FALSE, p, NO_SUBPATTERNS, c, network_error_measure(net_type, error_type, p, VISIBLE));
//             while (paused) {
//                 gtkx_flush_events();
//             }
//             if (abort_training) {
//                 printf("Training aborted!\n");
//                 return;
//             }
//             gtkx_flush_events();
//
//             real_time_redraw();
//             //       }
//         }
//
//         // 		Total_Error[c] = 100.0 * Total_Error[c] / (double) (3.0 * P);
//         //       for (s = STAGE_NAME; s <= STAGE_VERBAL; s++) {
//         // 		  Error[s][c] = 100 * Error[s][c] / (double) P;
//         //
//         // 	      for (p = p_min; p < p_max; p++) {
//         // 		  Pattern_Error[p][s][c] = 100 * Pattern_Error[p][s][c];
//         // 	      }
//         // 	    }
//         fprintf(stdout, "Error: %f\n", Total_Error[c]);
//         // RPC things that c is not reinitialised when weights are initialised
//         // RPC thinkgs this causes the graph to not be drawn correctly after reinitialisation
//         //        printf("Total_Error[%d] = %f %p\n", c, Total_Error[c], &Total_Error[c]);
//
//         tyler_inc_iterations_so_far();
//         gtk_graph_set_max_iterations(GTK_GRAPH(graph), tyler_get_iterations_so_far());
//         gtk_graph_redraw(graph);
//         while (paused) {
//             gtkx_flush_events();
//         }
//         if (abort_training) {
//             printf("Training aborted!\n");
//             return;
//         }
//         gtkx_flush_events();
//
//         if (is_file_open() && autosave) {
//             tyler_save_file(open_filename);
//         }
//
//         if (error_type == RMSD) {
//             keep_training = 0;
//             for (p = 0; p < p_max; p++) {
//                 //             if (Total_Error[c] > 0.015) {
//                 if (Pattern_Error[0][p] > 0.01) {
//                     keep_training = 1;
//                     break;
//                 }
//             }
//
//             if (!keep_training) {
//                 c = c_max;
//
//                 printf("RSS error per pattern under 0.01, training ended (Tyler et al., 2000).\n");
//                 //                 printf("Mean RSS error under 0.015, training ended (Tyler et al., 2000).\n");
//                 break;
//
//             }
//
//         }
//
//
//     }
// }

/*
void tyler_feedforward_train_patterns_serially_aoa(void)
{

 if (weight_update_type == TYLER_WEIGHT_UPDATE_STAGE) {
     throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Cannot train Tyler et al. (2000) by adjusting weight after every sub-pattern", 0);
     return;
 }
#if PRINT_FUNC
 printf("%s\n", __FUNCTION__);
#endif
 int n =  0;

 int a, c, p, keep_training, c_min = tyler_feedforward_get_iterations_so_far(), c_max = ((tyler_feedforward_get_iterations_so_far() + TYLER_C) > TYLER_C_MAX) ? (0) : (tyler_feedforward_get_iterations_so_far() + TYLER_C);




 tyler_feedforward_init_delta_weights();
 tyler_feedforward_init_prev_weights();

 int midpoint = abs(P/2.0);


 while (paused) {
     gtkx_flush_events();
 }
 if (abort_training) {
     printf("aborted!\n");
     return;
 }
 gtkx_flush_events();
 printf("c_min = %i c_max = %i tyler_feedforward_get_iterations_so_far = %i\n", c_min, c_max, tyler_feedforward_get_iterations_so_far());
 for (c = c_min; c < c_max; c++) {

     fprintf(stdout, "Training stage: Early AoA cycle: %3d ",tyler_feedforward_get_iterations_so_far());

     fflush(stdout);
     // 	update_momentum_spin_button();
     // 	update_learning_rate_spin_button();
n = random_int(0, 3);
     for (p = 0; p < midpoint; p++) {

         tyler_feedforward_train_with_pattern(p,noise, n);

         real_time_redraw(); //ALERT
         while (paused) {
             gtkx_flush_events();
         }
         if (abort_training) {
             printf("Training aborted!\n");
             return;
         }
         gtkx_flush_events();
         //       }

         //             if (weight_update_type == TYLER_WEIGHT_UPDATE_PATTERN) {
         // // 	      printf("Pattern-wise!\n");
         //
         //
         //                 tyler_feedforward_adjust_weights();
         //
         //                 if (accelerate_momentum) {
         //                     tyler_feedforward_accelerate_momentum();
         //                 }
         //
         //                 update_momentum_spin_button();
         //
         //
         //
         //             }

     }
     if (weight_update_type == TYLER_WEIGHT_UPDATE_BATCH) {

         tyler_feedforward_adjust_weights();
         // 	    ();
         if (accelerate_momentum) {
             tyler_feedforward_accelerate_momentum();
             update_momentum_spin_button();
         }

     }

     while (paused) {
         gtkx_flush_events();
     }
     if (abort_training) {
         printf("Training aborted!\n");
         return;
     }
     gtkx_flush_events();

     Total_Error[c] = 0.0;

     tyler_feedforward_unclamp();
     for (p = 0; p < midpoint; p++) {
         tyler_feedforward_randomise_states();

         tyler_feedforward_clamp_full_pattern(p);
         tyler_feedforward_propagate_noise(noise, n);
         real_time_redraw();

         save_error(FALSE, p, NO_SUBPATTERNS, c, network_error_measure(net_type, error_type, p, VISIBLE));
         //for 3*4 timesteps
         real_time_redraw();
         while (paused) {
             gtkx_flush_events();
         }
         if (abort_training) {
             printf("Training aborted!\n");
             return;
         }
         gtkx_flush_events();

         real_time_redraw();
     }

     fprintf(stdout, "Error: %f\n", Total_Error[c]);
     tyler_feedforward_inc_iterations_so_far();
     gtk_graph_set_max_iterations(GTK_GRAPH(graph), tyler_feedforward_get_iterations_so_far());
     gtk_graph_redraw(graph);
     while (paused) {
         gtkx_flush_events();
     }
     if (abort_training) {
         printf("Training aborted!\n");
         return;
     }
     gtkx_flush_events();

     if (is_file_open() && autosave) {
         tyler_feedforward_save_file(open_filename);
     }




     if (error_type == RMSD) {
         keep_training = 0;
         for (p = 0; p < midpoint; p++) {
             //             if (Total_Error[c] > 0.015) {
             if (Pattern_Error[0][p] > 0.01) {
                 keep_training = 1;
                 break;
             }
         }

         if (!keep_training) {
             c = c_max;

             printf("RSS error per pattern under 0.01, training ended (Tyler et al., 2000).\n");
             //                 printf("Mean RSS error under 0.015, training ended (Tyler et al., 2000).\n");
             break;

         }

     }

 }


 for (c = c_max; c < c_max*2; c++) {

     fprintf(stdout, "Training stage: Late AoA cycle: %3d ", tyler_feedforward_get_iterations_so_far());

     fflush(stdout);
     // 	update_momentum_spin_button();
     // 	update_learning_rate_spin_button();
     for (p = 0; p < midpoint; p++) {
         tyler_feedforward_train_with_pattern(p, noise, n);

         real_time_redraw(); //ALERT
         while (paused) {
             gtkx_flush_events();
         }
         if (abort_training) {
             printf("Training aborted!\n");
             return;
         }
         gtkx_flush_events();
         //       }

         //             if (weight_update_type == TYLER_WEIGHT_UPDATE_PATTERN) {
         // // 	      printf("Pattern-wise!\n");
         //
         //
         //                 tyler_feedforward_adjust_weights();
         //
         //                 if (accelerate_momentum) {
         //                     tyler_feedforward_accelerate_momentum();
         //                 }
         //
         //                 update_momentum_spin_button();
         //
         //
         //
         //             }

     }

     for (a = 0; a < 2; a++) {  // fucntion of the Late AoA to "compensate" for the Early AoA
         for (p = midpoint; p < P; p++) {
             tyler_feedforward_train_with_pattern(p, noise, n);

             real_time_redraw(); //ALERT
             while (paused) {
                 gtkx_flush_events();
             }
             if (abort_training) {
                 printf("Training aborted!\n");
                 return;
             }
             gtkx_flush_events();
             //       }

             //             if (weight_update_type == TYLER_WEIGHT_UPDATE_PATTERN) {
             // // 	      printf("Pattern-wise!\n");
             //
             //
             //                 tyler_feedforward_adjust_weights();
             //
             //                 if (accelerate_momentum) {
             //                     tyler_feedforward_accelerate_momentum();
             //                 }
             //
             //                 update_momentum_spin_button();
             //
             //
             //
             //             }

         }
     }
     if (weight_update_type == TYLER_WEIGHT_UPDATE_BATCH) {

         tyler_feedforward_adjust_weights();
         // 	    ();
         if (accelerate_momentum) {
             tyler_feedforward_accelerate_momentum();
             update_momentum_spin_button();
         }

     }

     while (paused) {
         gtkx_flush_events();
     }
     if (abort_training) {
         printf("Training aborted!\n");
         return;
     }
     gtkx_flush_events();

     Total_Error[c] = 0.0;

     tyler_feedforward_unclamp();
     for (p = p_min; p < P; p++) {
         tyler_feedforward_randomise_states();

         tyler_feedforward_clamp_full_pattern(p);
         tyler_feedforward_propagate_noise(noise, n);
         real_time_redraw();

         save_error(FALSE, p, NO_SUBPATTERNS, c, network_error_measure(net_type, error_type, p, VISIBLE));
         //for 3*4 timesteps
         real_time_redraw();
         while (paused) {
             gtkx_flush_events();
         }
         if (abort_training) {
             printf("Training aborted!\n");
             return;
         }
         gtkx_flush_events();

         real_time_redraw();
     }

     fprintf(stdout, "Error: %f\n", Total_Error[c]);
     tyler_feedforward_inc_iterations_so_far();
     gtk_graph_set_max_iterations(GTK_GRAPH(graph), tyler_feedforward_get_iterations_so_far());
     gtk_graph_redraw(graph);
     while (paused) {
         gtkx_flush_events();
     }
     if (abort_training) {
         printf("Training aborted!\n");
         return;
     }
     gtkx_flush_events();

     if (is_file_open() && autosave) {
         tyler_feedforward_save_file(open_filename);
     }

     if (error_type == RMSD) {
         keep_training = 0;
         for (p = 0; p < p_max; p++) {
             //             if (Total_Error[c] > 0.015) {
             if (Pattern_Error[0][p] > 0.01) {
                 keep_training = 1;
                 break;
             }
         }

         if (!keep_training) {
             c = c_max;

             printf("RSS error per pattern under 0.01, training ended (Tyler et al., 2000).\n");
             //                 printf("Mean RSS error under 0.015, training ended (Tyler et al., 2000).\n");
             break;

         }

     }


//         if (error_type == RMSD) {
//             keep_training = 0;
//             for (p = p_min; p < p_max; p++) {
//                 //             if (Total_Error[c] > 0.015) {
//                 if (Pattern_Error[0][p] > 0.01) {
//                     keep_training = 1;
//                     break;
//                 }
//             }
//
//             if (!keep_training) {
//                 c = c_max;
//
//                 printf("RSS error per pattern under 0.01, training ended (Tyler et al., 2000).\n");
//                 //                 printf("Mean RSS error under 0.015, training ended (Tyler et al., 2000).\n");
//
//             }
//
//         }

 }
}     */
void tyler_feedforward_train_patterns_serially(void)
{

//     if (aoa_train) {
//         tyler_feedforward_train_patterns_serially_aoa();
//
//         return;
//     }
//

    if (weight_update_type == TYLER_WEIGHT_UPDATE_STAGE) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Cannot train Tyler et al. (2000) by adjusting weight after every sub-pattern", 0);
        return;
    }
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int n = 0, c, p, keep_training, c_min = tyler_feedforward_get_iterations_so_far(), c_max = ((tyler_feedforward_get_iterations_so_far() + TYLER_C) > TYLER_C_MAX) ? (0) : (tyler_feedforward_get_iterations_so_far() + TYLER_C);
    int f, F = 10, freq_N = 0;



    tyler_feedforward_init_delta_weights();
    tyler_feedforward_init_prev_weights();





    for (p = 0; p < P; p++) {
        if (frequent_pattern[p]) {
            freq_N++;
        }
    }
    int ratio = (int) (P / ((double) freq_N));
    if ((ratio != 2) && ( ratio != 1)) {
        throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Training settings not allowed: The frequency of patterns must follow the domain/category boundary!", 3, freq_N, P,  ratio);
        abort_training = 1;
    }

    while (paused) {
        gtkx_flush_events();
    }

    if (abort_training) {
        printf("Training aborted!\n");
        return;
    }
    gtkx_flush_events();
    printf("c_min = %i c_max = %i tyler_feedforward_get_iterations_so_far = %i\n", c_min, c_max, tyler_feedforward_get_iterations_so_far());
    for (c = c_min; c < c_max; c++) {

        fprintf(stdout, "Training cycle: %3d", tyler_feedforward_get_iterations_so_far());
        fflush(stdout);
// 	update_momentum_spin_button();
// 	update_learning_rate_spin_button();
        n = random_int(0, 3);
        for (p = 0; p < P; p++) {
// 	           tyler_feedforward_train_with_pattern(p);

//             if (train_pattern[p]) {
// // // 	    if (early_pattern[p]) {
// // //
// // //             tyler_feedforward_train_with_pattern(p);
// // // 	    }
// // //
// 	    if (frequent_pattern[p]) {
// 	      for (f = 0; f < F; f++) {
// 		tyler_feedforward_train_with_pattern(p);
// 	      }
// 	    }
//
//            if (frequent_pattern[p]) {   //if a pattern is frequent
// 	    for (f = 0; f < ratio; f++) { // train it, e.g., twice as often
//
// 		  if (familiar_pattern[p]) {
// 		      tyler_feedforward_train_with_gaussian_pattern(noise, p);
// 		      printf("Frequent and Familiar Pattern: %d \n", p);
// 		  } else {
// 		      tyler_feedforward_train_with_pattern(p);
// 		      printf("Frequent Pattern: %d \n", p);
//
// 		  }
//
// 	    }
// 	   } else {  //if it's not but others are, train it once
//
// 	     if (familiar_pattern[p]) {
// 	       tyler_feedforward_train_with_gaussian_pattern(noise, p);
// 	       printf("Familar Pattern: %d \n", p);
//
// 	     } else {
// 	       tyler_feedforward_train_with_pattern(p);
// 	       printf("Neutral Pattern: %d \n", p);
//
//
// 	     }
            if (c <= (c_max/2.0)) {
                tyler_feedforward_train_with_pattern(p, familiar_pattern[p], n);
                printf("Neutral Pattern: %d \n", p);
            }
            else if (frequent_pattern[p] && (c > (c_max/2.0))) {   //if a pattern is frequent
                for (f = 0; f < ratio; f++) { // train it, e.g., twice as often

// 			    if (familiar_pattern[p]) {
// 			      tyler_feedforward_train_with_gaussian_pattern(noise, p);
// 			      printf("Frequent and Familiar Pattern: %d \n", p);
// 			    } else {
                    tyler_feedforward_train_with_pattern(p, familiar_pattern[p], n);
                    printf("Frequent Pattern: %d \n", p);

// 			    }

                }
            }


            /*else {  //if it's not but others are, train it once

                  if (familiar_pattern[p]) {
            	tyler_feedforward_train_with_gaussian_pattern(noise, p);
            	printf("Familar Pattern: %d \n", p);

                  } else {
            	tyler_feedforward_train_with_pattern(p);
            	printf("Neutral Pattern: %d \n", p);


                  }


             } */



// 	    } else {
//
// 	      tyler_feedforward_train_with_pattern(p);
//
// 	    }
//

// 	    }
            //       }

//             if (weight_update_type == TYLER_WEIGHT_UPDATE_PATTERN) {
// // 	      printf("Pattern-wise!\n");
//
//
//                 tyler_feedforward_adjust_weights();
//
//                 if (accelerate_momentum) {
//                     tyler_feedforward_accelerate_momentum();
//                 }
//
//                 update_momentum_spin_button();
//
//
//

//             }
            real_time_redraw(); //ALERT
            while (paused) {
                gtkx_flush_events();
            }
            if (abort_training) {
                printf("Training aborted!\n");
                return;
            }
            gtkx_flush_events();


        }
        if (weight_update_type == TYLER_WEIGHT_UPDATE_BATCH) {

            tyler_feedforward_adjust_weights();
// 	    ();
            if (accelerate_momentum) {
                tyler_feedforward_accelerate_momentum();
                update_momentum_spin_button();
            }

        }

        while (paused) {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }
        gtkx_flush_events();

        Total_Error[c] = 0.0;

        tyler_feedforward_unclamp();
        for (p = p_min; p < p_max; p++) {

            tyler_feedforward_randomise_states();

            tyler_feedforward_clamp_full_pattern(p);
            tyler_feedforward_propagate_noise(TEST_NOISE, TEST_N_PATTERN);
            real_time_redraw();

            save_error(FALSE, p, NO_SUBPATTERNS, c, network_error_measure_noise(net_type, error_type, p, VISIBLE, TEST_NOISE, TEST_N_PATTERN));
//for 3*4 timesteps
            real_time_redraw();
            while (paused) {
                gtkx_flush_events();
            }
            if (abort_training) {
                printf("Training aborted!\n");
                return;
            }
            gtkx_flush_events();

            real_time_redraw();
        }

        fprintf(stdout, "Error: %f\n", Total_Error[c]);
        tyler_feedforward_inc_iterations_so_far();
        gtk_graph_set_max_iterations(GTK_GRAPH(graph), tyler_feedforward_get_iterations_so_far());
        gtk_graph_redraw(graph);
        while (paused) {
            gtkx_flush_events();
        }
        if (abort_training) {
            printf("Training aborted!\n");
            return;
        }
        gtkx_flush_events();

        if (is_file_open() && autosave) {
            tyler_feedforward_save_file(open_filename);
        }




        if (error_type == RMSD) {
            keep_training = 0;
            for (p = p_min; p < p_max; p++) {
//             if (Total_Error[c] > 0.015) {
                if (Pattern_Error[0][p] > 0.01) {


// 		  printf("RSS error per pattern over 0.01, training continuing (Tyler et al., 2000).\n");

                    keep_training = 1;
                    break;
                }
            }

            if (!keep_training) {
                c = c_max;

                printf("Mean RSS error under 0.015, training ended (Tyler et al., 2000).\n");
                break;
            }

        }

    }
}
// void step_cycle(GtkWidget *button, GtkNotebook *notebook)
// {
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//     if ((step_number >= 0)&&(step_number < T)) {
// 	  propagate_timestep(pattern_number, stage, step_number);
// 	  unclamp_time(step_number - 1);
//     } else {
// 	  unclamp_all();
//           step_number = 0;
// 	  randomise_states(step_number);
// 	  clamp_now(pattern_number, stage, step_number);
//     }
//     step_number++;
//     redraw_current_page();
// }

// void step_pattern(GtkWidget *button, GtkNotebook *notebook)
// {
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//
//     int p = 0, s = 0;
//       step_number = 0;
//     if (net_type == BM)
//     {
//         p = bm_get_p_clamped();
//         (bm_get_clamped_type() != -1) ? (s = bm_type_to_state(bm_get_clamped_type())) : (s = 0);
//         if ((p < (p_max - 1)) && (p >= p_min)) {
//             p++;
//         }
//         else {
//             p = p_min;
//         }
//
//         bm_unclamp();
//         bm_randomise_states();
//         bm_clamp(p, bm_network_state_to_unit_type(s));
//     }
//     else if (net_type == BPTT)
//     {
//         p = bptt_clamped_pattern_get(), s = bptt_clamped_stage_get();
//         if ((p < (p_max - 1)) && (p >= p_min)) {
//             p++;
//         }
//         else {
//             p = p_min;
//         }
//         if (s < 0) {
//             s = 0;
//         }
//         bptt_unclamp_all();
//         bptt_randomise_states_all();
//         bptt_clamp_now(p, s, bptt_t1);
//     }
//     else if (net_type == PDPTOOL)
//     {
//         p = pdptool_get_clamped_pattern(), s = pdptool_get_clamped_stage();
//         if ((p < (p_max - 1)) && (p >= p_min)) {
//             p++;
//         }
//         else {
//             p = p_min;
//         }
//         if (s < 0) {
//             s = 0;
//         }
//         pdptool_unclamp();
//         pdptool_randomise_states();
//         pdptool_clamp_now(p, s);
// //     }
//
//
//
//
//
//     redraw_current_page();
// }

// void step_stage(GtkWidget *button, GtkNotebook *notebook)
// {
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//
//     int p = 0, s = 0;
//     if (net_type == BM)
//     {
//         p = bm_get_p_clamped(), s = bm_type_to_state(bm_get_clamped_type());
//         if (s < BM_MINUS_VERBAL) {
//
//             s++;
//         }
//         else {
//             s = BM_MINUS_NAME;
//         }
//         bm_unclamp();
//         bm_randomise_states();
//         bm_clamp(p, bm_network_state_to_unit_type(s));
//     }
//     else if (net_type == BPTT)
//     {
//         p = get_clamped_pattern(), s = get_clamped_stage();
//
//         if (p < 0) {
//             p = 0;
//         }
//         if (s < STAGE_VERBAL) {
//             s++;
//         }
//         else {
//             s = STAGE_NAME;
//         }
//         unclamp_all();
//         randomise_states_all();
//         clamp_now(p, s, bptt_t1);
//     }
//     else if (net_type == PDPTOOL)
//     {
//         p = pdptool_get_clamped_pattern(), s = pdptool_get_clamped_stage();
//
//         if (p < 0) {
//             p = 0;
//         }
//         if (s < PDPTOOL_STAGE_VERBAL) {
//             s++;
//         }
//         else {
//             s = PDPTOOL_STAGE_NAME;
//         }
//         pdptool_unclamp();
//         pdptool_randomise_states();
//         pdptool_clamp_now(p, s);
//         /*	pdptool_print_weights();*/
//     }
//
//
//
//
//
//     redraw_current_page();
// }


void set_stage(GtkSpinButton *spinbutton, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

    int n = TEST_N_PATTERN;//ALERT

    int p, s = gtk_spin_button_get_value_as_int(spinbutton);

    p = network_clamped_pattern_get(net_type);

    if (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pattern_spin_button)) !=  p) {
        p = 0;
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(pattern_spin_button), p);
    }


    network_unclamp(net_type);
    network_randomise_states(net_type);
    network_clamp_now_noise(net_type, p, s, familiar_pattern[p], n);
    redraw_current_page();
}




int set_pattern(GtkSpinButton *spinbutton, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int p = gtk_spin_button_get_value_as_int(spinbutton), s;
    int n = TEST_N_PATTERN;//random_int(0, 3);//ALERT
    if (net_type == BM)
    {
        s = bm_get_clamped_type();
        if (s == -1) {
            s = 0;
        }
        bm_unclamp();
        bm_randomise_states();
        bm_clamp(p, s);   //not needed to use bm_network_state_to_unit_type, as s is from network
    }
    else if (net_type == BPTT)
    {
        s = bptt_clamped_stage_get();
        if (s == -1) {
            s = 0;
        }
        bptt_unclamp_all();
        bptt_randomise_states_all();
        bptt_clamp_now(p, s, bptt_t1);
    }
    else if (net_type == TYLER_RECURRENT)
    {
        s = tyler_clamped_stage_get();
        if (s == -1) {
            s = 0;
        }
        tyler_unclamp_all();
        tyler_randomise_states_all();
        tyler_clamp_now_noise(p, s, tyler_t1, familiar_pattern[p], n);
    }

    else if (net_type == TYLER_FEEDFORWARD)
    {
        s = tyler_feedforward_clamped_stage_get();
        if (s == -1) {
            s = 0;
        }
        tyler_feedforward_unclamp();
        tyler_feedforward_randomise_states();
        tyler_feedforward_clamp_now_noise(p, s, familiar_pattern[p], n);
    }
    else if (net_type == PDPTOOL)
    {
        s = pdptool_get_clamped_stage();
        if (s == -1) {
            s = 0;
        }
        pdptool_unclamp();
        pdptool_randomise_states();
        pdptool_clamp_now(p, s);
    }

    redraw_current_page();
    return FALSE;
}



void bptt_test(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    testing = TRUE;
    int p, s;

    int current_error_i = ((bptt_get_iterations_so_far()) && (bptt_get_iterations_so_far() < BPTT_C_MAX)) ? (bptt_get_iterations_so_far()) : (0);
    int test;


    Total_Error[current_error_i] = 0.0;
    for (s = 0; s < 3; s++) {
        Error[s][current_error_i] = 0.0;

    }


    for (test = 0; test < BPTT_TEST; test++) {

        for (p = p_min; p < p_max; p++) {
            for (s = BPTT_STAGE_NAME; s <= BPTT_STAGE_VERBAL; s++) {

                //stages of clamping


                bptt_unclamp_all();
                bptt_randomise_states(bptt_t1);
                bptt_clamp(p, s, bptt_t1);



                bptt_real_time_settle();
                gtkx_flush_events();
                real_time_redraw();
                gtkx_flush_events();

                save_error(test, p, s, current_error_i, network_error_measure(net_type, error_type, p, s));
                gtkx_flush_events();

                if (real_time) {
                    update_pattern_spin_button();
                    update_stage_spin_button();
                }


                real_time_redraw();
                gtkx_flush_events();
            }

        }
    }
    testing = FALSE;
    redraw_current_page();

}
void tyler_test(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    testing = TRUE;
    int p, s;

    int current_error_i = ((tyler_get_iterations_so_far()) && (tyler_get_iterations_so_far() < TYLER_C_MAX)) ? (tyler_get_iterations_so_far()) : (0);
    int test;


    Total_Error[current_error_i] = 0.0;
    for (s = 0; s < 3; s++) {
        Error[s][current_error_i] = 0.0;

    }


    for (test = 0; test < TYLER_TEST; test++) {

        for (p = p_min; p < p_max; p++) {
            //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++) {

            //stages of clamping


            tyler_unclamp_all();
            tyler_randomise_states(tyler_t1);
            tyler_clamp_full_pattern(p, tyler_t1);



            tyler_real_time_settle();
            gtkx_flush_events();
            real_time_redraw();
            gtkx_flush_events();

            save_error(test, p, NO_SUBPATTERNS, current_error_i, network_error_measure(net_type, error_type, p, VISIBLE));
            gtkx_flush_events();

            if (real_time) {
                update_pattern_spin_button();
                update_stage_spin_button();
            }


            real_time_redraw();
            gtkx_flush_events();
            //       }

        }
    }
    testing = FALSE;
    redraw_current_page();

}


void tyler_feedforward_test(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    testing = TRUE;
    int p, s;
    int n = TEST_N_PATTERN; //ALERT
    int noise = TEST_NOISE;
    int current_error_i = ((tyler_feedforward_get_iterations_so_far()) && (tyler_feedforward_get_iterations_so_far() < TYLER_C_MAX)) ? (tyler_feedforward_get_iterations_so_far()) : (0);
    int test;


    Total_Error[current_error_i] = 0.0;
    for (s = 0; s < 3; s++) {
        Error[s][current_error_i] = 0.0;

    }


    for (test = 0; test < TYLER_TEST; test++) {

        for (p = p_min; p < p_max; p++) {
            //       for (s = TYLER_STAGE_DISTINCTIVE; s <= TYLER_STAGE_FUNCTIONAL; s++) {

            //stages of clamping


            tyler_feedforward_unclamp();
            tyler_feedforward_randomise_states();
            tyler_feedforward_clamp_full_pattern(p);



            tyler_feedforward_propagate_noise(noise, n);
            gtkx_flush_events();
            real_time_redraw();
            gtkx_flush_events();

            save_error(test, p, NO_SUBPATTERNS, current_error_i, network_error_measure(net_type, error_type, p, VISIBLE));
            gtkx_flush_events();

            if (real_time) {
                update_pattern_spin_button();
                update_stage_spin_button();
            }


            real_time_redraw();
            gtkx_flush_events();
            //       }

        }
    }
    testing = FALSE;
    redraw_current_page();

}


void pdptool_test(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    testing = TRUE;
    int p, s;

    int current_error_i = 0;
    int test;


    Total_Error[current_error_i] = 0.0;
    for (s = 0; s < 3; s++) {
        Error[s][current_error_i] = 0.0;

    }


    for (test = 0; test < PDPTOOL_TEST; test++) {

        for (p = p_min; p < p_max; p++) {
            for (s = PDPTOOL_STAGE_NAME; s <= PDPTOOL_STAGE_VERBAL; s++) {

                //stages of clamping
                pdptool_unclamp();
                pdptool_randomise_states();
                pdptool_clamp(p, s);



                pdptool_real_time_settle();
                gtkx_flush_events();
                real_time_redraw();
                gtkx_flush_events();

                save_error(test, p, s, current_error_i, network_error_measure(net_type, error_type, p, s));
                gtkx_flush_events();

                if (real_time) {
                    update_pattern_spin_button();
                    update_stage_spin_button();
                }


                real_time_redraw();
                gtkx_flush_events();


            }


        }
    }
    testing = FALSE;
    redraw_current_page();
}

void bm_test(void)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    testing = TRUE;
    int p, s;

    int current_error_i = ((bm_get_iterations_so_far()) && (bm_get_iterations_so_far() < BM_I_MAX)) ? (bm_get_iterations_so_far()) : (0);
    int test;

    Total_Error[current_error_i] = 0.0;
    for (s = 0; s < 3; s++) {
        Error[s][current_error_i] = 0.0;

    }

    for (test = 0; test < BM_TEST; test++) {

        for (p = 0; p < P; p++) {
            for (s = BM_MINUS_NAME; s <= BM_MINUS_VERBAL; s++) {


                //stages of clamping
                bm_unclamp();
                bm_randomise_states();
                bm_clamp(p, bm_network_state_to_unit_type(s));



                bm_real_time_anneal();
                gtkx_flush_events();


                save_error(test, p, s, current_error_i, network_error_measure(net_type, error_type, p, s));

                gtkx_flush_events();


                if (real_time) {
                    update_pattern_spin_button();
                    update_stage_spin_button();
                }


                real_time_redraw();
                gtkx_flush_events();


            }

        }
    }
    testing = FALSE;

    redraw_current_page();

}


void test_button(GtkWidget *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    ghost_controls(TRUE);
    if (net_type == BPTT) {
        bptt_test();
    }
    else if (net_type == BM) {
        bm_test();
    }
    else if (net_type == TYLER_RECURRENT) {
        tyler_test();
    }
    else if (net_type == TYLER_FEEDFORWARD) {
        tyler_feedforward_test();
    }
    else if (net_type == PDPTOOL) {
        pdptool_test();
    }
    ghost_controls(FALSE);

    update_pattern_spin_button();
    update_stage_spin_button();
}


void set_attractor_view_type(GtkWidget *combo_box, gpointer user_data)
{
    gtk_attractor_view_set_type(GTK_ATTRACTOR_VIEW(attractor_view), gtk_combo_box_get_active(GTK_COMBO_BOX(combo_box)));

    redraw_current_page();
    gtkx_flush_events();
}


void save_attractor_view(GtkWidget *button, gpointer user_data)
{
    gtk_attractor_view_save(GTK_ATTRACTOR_VIEW(attractor_view), attractor_view_file_name);
}
void clicked_attractor_view_button(GtkWidget *button, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int d, distribution = ATTRACTOR_DISTRIBUTION_LEVEL, l, level = ATTRACTOR_LESION_LEVEL, s, sample = ATTRACTOR_SAMPLE, t = 0, p;

    gtk_attractor_view_init_response();
    init_attractor_matrix_for_clustering();

    char attractor_filename[256];

    ghost_controls(TRUE);
    if (net_type == BPTT) {

        bptt_unclamp_all();

        bptt_backup_weights();

        //ALERT we are just checking name as input at the moment...

        for (d = 0; d < distribution; d++) { // distribution of lesion
            for (l = 0; l < level; l++) { // level of leasioning
                for (p = 0; p < P; p++) { // pattern
                    printf("Run: %i/%i, Level: %i/%i, Pattern: %i/%i\n", d+1, distribution, l+1, level, p+1, P);
                    for (s = 0; s < sample; s++) { // to ensure network in "right" attractor
//                         for (t = 0; t < 3; t++) {
                        bptt_randomise_states(bptt_t1);

                        bptt_clamp(p, t, bptt_t1);
                        bptt_real_time_settle_for_save_settling_attractor(12, p); //for 3*4 timesteps
//                         bptt_unclamp_time(bptt_t1);
                        bptt_real_time_unclamped_settle_save_settling_attractor(p);
// 			    printf("output interpreted as: %d\n", network_compute_output_interpretation(net_type, error_type));
//                             gtk_attractor_view_save_response(d, l, s, t, p, network_compute_output_interpretation(net_type, error_type));
// 			    gtk_attractor_view_save_attractor(d, l, s, t, p, network_compute_output_interpretation(net_type, error_type));
                        network_save_current_attractor_for_clustering(net_type);

                        real_time_redraw();
                        gtkx_flush_events();
//                         }
                    }
                }
                if (lesion_type == ZEROING) {
                    bptt_lesion_weights(100.0 / (double) level); //incrememnetal percentage of weights are zeroed
                }
                else if (lesion_type == NOISE) {
                    bptt_disrupt_weights(1.45 / (double) level * l);
                }

                printf("%s_%d.m\n", open_name, l);
                sprintf(attractor_filename, "%s_%d.m", open_name, l);
                network_save_attractors_matlab(net_type, attractor_filename);
                init_attractor_matrix_for_clustering();
            }

            bptt_undo_lesion();
        }


    }
    else if (net_type == BM) {

        bm_unclamp();

        bm_backup_weights();

        //ALERT we are just checking name as input at the moment...

        for (d = 0; d < distribution; d++) { // distribution of lesion
            for (l = 0; l < level; l++) { // level of leasioning
                for (p = 0; p < P; p++) { // pattern
                    printf("Run: %i/%i, Level: %i/%i, Pattern: %i/%i\n", d+1, distribution, l+1, level, p+1, P);
                    for (s = 0; s < sample; s++) {

                        bm_randomise_states();

                        bm_clamp(p, t);
                        //                         real_time_settle_for(12); //for 3*4 timesteps
                        //                         unclamp_time(bptt_t1);
                        //                         real_time_unclamped_settle(notebook);
                        bm_real_time_semiclamped_anneal_save_settling_attractor();
                        network_save_current_attractor_for_clustering(net_type);

                        real_time_redraw();
                        gtkx_flush_events();

                    }
                }

                if (lesion_type == ZEROING) {

                    bm_lesion_weights(100.0 / (double) level); //incrememnetal percentage of weights are zeroed

                }
                else if (lesion_type == NOISE) {
                    bm_disrupt_weights(1.45 / (double) level * l);
                }

                printf("%s_%d.m\n", open_name, l);
                sprintf(attractor_filename, "%s_%d.m", open_name, l);
                network_save_attractors_matlab(net_type, attractor_filename);
                init_attractor_matrix_for_clustering();
            }
            bm_undo_lesion();
        }


    }
    else if (net_type == TYLER_FEEDFORWARD) {
        int locus = TYLER_LESION_GLOBAL;
        int n= TEST_N_PATTERN;//ALERT

        double fabs_average_weight = 2*tyler_feedforward_fabs_average_weight(locus);
        tyler_feedforward_unclamp();
        tyler_feedforward_backup_weights();
        for (d = 0; d < distribution; d++) { // distribution of lesion
            for (l = 0; l < level; l++) { // level of leasioning
                for (p = 0; p < P; p++) { // pattern
                    printf("Run: %i/%i, Level: %i/%i, Pattern: %i/%i\n", d+1, distribution, l+1, level, p+1, P);
                    // 		    for (s = 0; s < sample; s++) {        no samples needed it's feedforward
                    tyler_feedforward_randomise_states();

                    tyler_feedforward_clamp_full_pattern(p);
                    tyler_feedforward_propagate_noise(familiar_pattern[p], n); //for 3*4 timesteps
                    tyler_feedforward_unclamp();
                    network_save_current_attractor_for_clustering(net_type);


                    real_time_redraw();
                    gtkx_flush_events();

                }
                // 		    }
                if (lesion_type == ZEROING) {
                    tyler_feedforward_lesion_weights_locus(100.0 / (double) level, locus);

                }
                else if (lesion_type == NOISE) {
                    tyler_feedforward_disrupt_weights(fabs_average_weight / (double) level * l);
                }

                printf("%s_%d.m\n", open_name, l);
                sprintf(attractor_filename, "%s_%d.m", open_name, l);
                network_save_attractors_matlab(net_type, attractor_filename);
                init_attractor_matrix_for_clustering();
            }
            tyler_feedforward_undo_lesion();
        }
    }
    else if (net_type == TYLER_RECURRENT) {
        int locus = TYLER_LESION_GLOBAL;
        int n= TEST_N_PATTERN;//ALERT

        double fabs_average_weight = 2*tyler_fabs_average_weight(locus);
        tyler_unclamp_all();
        tyler_backup_weights();
        for (d = 0; d < distribution; d++) { // distribution of lesion
            for (l = 0; l < level; l++) { // level of leasioning
                for (p = 0; p < P; p++) { // pattern
                    printf("Run: %i/%i, Level: %i/%i, Pattern: %i/%i\n", d+1, distribution, l+1, level, p+1, P);
                    // 		    for (s = 0; s < sample; s++) {        no samples needed it's feedforward

                    tyler_randomise_states(tyler_t1);

                    tyler_clamp_full_pattern(p, tyler_t1);
                    tyler_real_time_settle_for(12); //for 3*4 timesteps
                    tyler_unclamp_time(tyler_t1);
                    tyler_real_time_unclamped_settle();
                    network_save_current_attractor_for_clustering(net_type);


                    real_time_redraw();
                    gtkx_flush_events();

                }
                // 		    }
                if (lesion_type == ZEROING) {
                    tyler_lesion_weights_locus(100.0 / (double) level, locus);

                }
                else if (lesion_type == NOISE) {
                    tyler_disrupt_weights(fabs_average_weight / (double) level * l);
                }

                printf("%s_%d.m\n", open_name, l);
                sprintf(attractor_filename, "%s_%d.m", open_name, l);
                network_save_attractors_matlab(net_type, attractor_filename);
                init_attractor_matrix_for_clustering();
            }
            tyler_undo_lesion();
        }
    }
    printf("Done!\n");

//     gtk_attractor_view_update(attractor_view);

    ghost_controls(FALSE);
    //   gtk_attractor_view_save(attractor_view, open_filename);
    update_pattern_spin_button();
    update_stage_spin_button();
}

void save_graph(GtkButton *button, GtkWindow *window)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    GtkWidget *dialog;
//     static char *filename;
    dialog = gtk_file_chooser_dialog_new ("Save Graph", window, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), open_path);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        gsize *bytes_read = NULL, *bytes_written = NULL;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        filename = g_filename_to_utf8(filename, -1, bytes_read, bytes_written, NULL);
//         open_filename =   filename;
        sprintf(open_filename, "%s", filename);

        gtk_graph_pdf(graph, filename);
        g_free(filename);
//         set_window_title();
        gtk_window_set_title(GTK_WINDOW(window), open_filename);

    }
    gtk_widget_destroy (dialog);
}

void update_training_algo_combo_box(void)
{


    int i;
    if(GTK_IS_COMBO_BOX(training_algo_combo_box)) {


        for (i = 40; i >= 0; i--)  {
            gtk_combo_box_set_active(GTK_COMBO_BOX(training_algo_combo_box), i);
            gtk_combo_box_remove_text(GTK_COMBO_BOX(training_algo_combo_box), i);
        }


        if (net_type == TYLER_FEEDFORWARD) {


            gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "Olivia");
            gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "Olivia Alt");
            gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "Rick");
            gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "Rick Alt");

            gtk_combo_box_set_active(GTK_COMBO_BOX(training_algo_combo_box), 0);

        }

        else {




            gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "Williams");
            gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "Williams' with TA");
            gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "McClelland");
            gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "~McClelland");

            gtk_combo_box_set_active(GTK_COMBO_BOX(training_algo_combo_box), 3);
        }
    }
}


void set_training_type(GtkComboBox *combo_box, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    training_type = gtk_combo_box_get_active(combo_box);
}
void set_settling_type(GtkComboBox *combo_box, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    settling_type = gtk_combo_box_get_active(combo_box);
}


void set_annealing_type(GtkComboBox *combo_box, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    annealing_type = gtk_combo_box_get_active(combo_box);
}

void set_lesion_type(GtkComboBox *combo_box, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    lesion_type = gtk_combo_box_get_active(combo_box);
}

void set_lesion_locus(GtkComboBox *combo_box, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    lesion_locus = gtk_combo_box_get_active(combo_box);
}

void set_error_type(GtkComboBox *combo_box, gpointer user_data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    error_type = gtk_combo_box_get_active(combo_box);

    redraw_current_page();
    gtkx_flush_events();
}


void update_error_combo_box(void)
{
    if (net_type == TYLER_FEEDFORWARD) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(error_combo_box), 3);
    }
}

void set_word_to_picture_graph_type(GtkComboBox *combo_box, GtkWord_to_picture_Graph *word_to_picture_graph)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_word_to_picture_graph_set_type(word_to_picture_graph, gtk_combo_box_get_active(combo_box));

    if (gtk_combo_box_get_active(combo_box) == 0) {
        gtk_widget_set_sensitive(word_to_picture_combo_box2, 0);
        gtk_widget_set_sensitive(word_to_picture_label2, 0);
    }
    else {
        gtk_widget_set_sensitive(word_to_picture_combo_box2, 1);
        gtk_widget_set_sensitive(word_to_picture_label2, 1);
    }


    gtk_word_to_picture_graph_redraw(GTK_WIDGET(word_to_picture_graph));
}

void set_drawing_and_copying_graph_type(GtkComboBox *combo_box, GtkDrawing_and_copying_Graph *drawing_and_copying_graph)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_drawing_and_copying_graph_set_type(drawing_and_copying_graph, gtk_combo_box_get_active(combo_box));
    gtk_drawing_and_copying_graph_redraw(GTK_WIDGET(drawing_and_copying_graph));
}

void set_tyler_graph_type(GtkComboBox *combo_box, GtkTyler_Graph *tyler_graph)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_tyler_graph_set_type(tyler_graph, gtk_combo_box_get_active(combo_box));
    gtk_tyler_graph_redraw(GTK_WIDGET(tyler_graph));

    gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_hh), gtk_combo_box_get_active(combo_box));
    gtk_tyler_graph_redraw(tyler_graph_hh);

    gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_ih), gtk_combo_box_get_active(combo_box));
    gtk_tyler_graph_redraw(tyler_graph_ih);

    gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_ho), gtk_combo_box_get_active(combo_box));
    gtk_tyler_graph_redraw(tyler_graph_ho);


    gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), gtk_combo_box_get_active(combo_box));
    gtk_tyler_graph_redraw(tyler_graph_networks);
}

void set_aoa_graph_type(GtkComboBox *combo_box, GtkAoA_Graph *aoa_graph)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_aoa_graph_set_type(aoa_graph, gtk_combo_box_get_active(combo_box));
    gtk_aoa_graph_redraw(GTK_WIDGET(aoa_graph));

    gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_hh), gtk_combo_box_get_active(combo_box));
    gtk_aoa_graph_redraw(aoa_graph_hh);

    gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_ih), gtk_combo_box_get_active(combo_box));
    gtk_aoa_graph_redraw(aoa_graph_ih);

    gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_ho), gtk_combo_box_get_active(combo_box));
    gtk_aoa_graph_redraw(aoa_graph_ho);


    gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_networks), gtk_combo_box_get_active(combo_box));
    gtk_aoa_graph_redraw(aoa_graph_networks);
}

void set_aoa(GtkComboBox *box, gpointer user_data)
{
//     if (gtk_combo_box_get_active(box) == 2) {
//         aoa = 0;
//     }          else {
//         aoa = 1;
//
//     }

    if (gtk_combo_box_get_active(box) == 0) {
        aoa_train = 1;
    } else {
        aoa_train = 0;
    }
    redraw_current_page();
}
void set_naming_graph_type(GtkComboBox *combo_box, GtkNaming_Graph *naming_graph)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_naming_graph_set_type(naming_graph, gtk_combo_box_get_active(combo_box));
    gtk_naming_graph_redraw(GTK_WIDGET(naming_graph));
}

void set_naming_graph_way(GtkComboBox *combo_box, GtkNaming_Graph *naming_graph)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    gtk_naming_graph_set_way(naming_graph, gtk_combo_box_get_active(combo_box));
//   gtk_naming_graph_update(naming_graph);
    gtk_naming_graph_redraw(GTK_WIDGET(naming_graph));
}




void settle_button(GtkWidget *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

    int n= TEST_N_PATTERN;//ALERT

    int p, s;
    if (net_type == BM) {
        (bm_get_p_clamped() != -1) ? (p = bm_get_p_clamped()) : (p = 0);
        (bm_get_clamped_type() != -1) ? (s = bm_get_clamped_type()) : (s = 0);
        bm_unclamp();
        bm_randomise_states();
        bm_clamp(p, s);    // not needed to use bm_network_state_to_unit_type as s is from network
        bm_real_time_anneal();
    }
    else if (net_type == BPTT) {
        p = bptt_clamped_pattern_get(), s = bptt_clamped_stage_get();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        bptt_unclamp_all();
        bptt_randomise_states(bptt_t1);
        bptt_clamp(p, s, bptt_t1);
        bptt_real_time_settle();
    }
    else if (net_type == TYLER_RECURRENT) {
        p = tyler_clamped_pattern_get(), s = tyler_clamped_stage_get();

        printf("s= %i\n", s);
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        tyler_unclamp_all();
        tyler_randomise_states(tyler_t1);
        tyler_clamp(p, s, tyler_t1);
        tyler_real_time_settle_noise(familiar_pattern[p], n);
        tyler_set_target(p, s, tyler_t1);
    }

    else if (net_type == TYLER_FEEDFORWARD) {
        p = tyler_feedforward_clamped_pattern_get(), s = tyler_feedforward_clamped_stage_get();

        printf("s= %i\n", s);
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        tyler_feedforward_unclamp();
        tyler_feedforward_randomise_states();
        tyler_feedforward_clamp(p, tyler_stage_to_type(s));
        tyler_feedforward_propagate_noise(familiar_pattern[p], n);

        //_type(tyler_stage_to_type(s));
        tyler_feedforward_set_target(p, tyler_stage_to_type(s));
    }
    else if (net_type == PDPTOOL) {
        p = pdptool_get_clamped_pattern(), s = pdptool_get_clamped_stage();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        pdptool_unclamp();
        pdptool_randomise_states();
        pdptool_clamp(p, s);
        pdptool_real_time_settle();
    }

    redraw_current_page();
    gtkx_flush_events();
}


void reset_states_button(GtkWidget *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int p, s;
    int n= TEST_N_PATTERN;//ALERT
    if (net_type == BM) {
        (bm_get_p_clamped() != -1) ? (p = bm_get_p_clamped()) : (p = 0);
        (bm_get_clamped_type() != -1) ? (s = bm_get_clamped_type()) : (s = 0);
        bm_unclamp();
        bm_randomise_states();
        bm_clamp(p, s);

    }
    else if (net_type == BPTT) {
        p = bptt_clamped_pattern_get(), s = bptt_clamped_stage_get();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        bptt_unclamp_all();
        bptt_randomise_states(bptt_t1);
        bptt_clamp_now(p, s, bptt_t1);

    }
    else if (net_type == TYLER_RECURRENT) {
        p = tyler_clamped_pattern_get(), s = tyler_clamped_stage_get();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        tyler_unclamp_all();
        tyler_randomise_states(tyler_t1);
        tyler_clamp_now_noise(p, s, tyler_t1, familiar_pattern[p], n);

    }
    else if (net_type == TYLER_FEEDFORWARD) {
        p = tyler_feedforward_clamped_pattern_get(), s = tyler_feedforward_clamped_stage_get();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        tyler_feedforward_unclamp();
        tyler_feedforward_randomise_states();
        tyler_feedforward_clamp_now_noise(p, s, familiar_pattern[p], n);

    }
    else if (net_type == PDPTOOL) {
        p = pdptool_get_clamped_pattern(), s = pdptool_get_clamped_stage();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        pdptool_unclamp();
        pdptool_randomise_states();
        pdptool_clamp_now(p, s);

    }

    redraw_current_page();
    gtkx_flush_events();
}

void settle_step_button(GtkWidget *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    int p, s;
    int n= TEST_N_PATTERN;//ALERT

    if (net_type == BM) {
//       //ALERT find equivalent!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//         (bm_get_p_clamped() != -1) ? (p = bm_get_p_clamped()) : (p = 0);
//         (bm_get_clamped_type() != -1) ? (s = bm_type_to_state(bm_get_clamped_type())) : (s = 0);
//         bm_unclamp();
//         bm_randomise_states();
//         bm_clamp(p, bm_network_state_to_unit_type(s));
//         bm_real_time_clamped_anneal();
    }
    else if (net_type == BPTT) {
        p = bptt_clamped_pattern_get(), s = bptt_clamped_stage_get();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        bptt_unclamp_all();
        //         randomise_states(bptt_t1);
        bptt_clamp(p, s, bptt_t1);
        bptt_settle_step(1);
    }
    else if (net_type == TYLER_RECURRENT) {
        p = tyler_clamped_pattern_get(), s = tyler_clamped_stage_get();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        tyler_unclamp_all();
        //         randomise_states(tyler_t1);
        tyler_clamp(p, s, tyler_t1);
        tyler_settle_step_noise(1, familiar_pattern[p], n);
    }
    else if (net_type == TYLER_FEEDFORWARD) {
        p = tyler_feedforward_clamped_pattern_get(), s = tyler_feedforward_clamped_stage_get();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        tyler_feedforward_unclamp();
        //         randomise_states(tyler_t1);
        tyler_feedforward_clamp(p, s);
        tyler_feedforward_propagate_noise(familiar_pattern[p], n);
    }
    else if (net_type == PDPTOOL) {
        p = pdptool_get_clamped_pattern(), s = pdptool_get_clamped_stage();
        if ((p < 0) || (s < 0)) {
            p = 0;
            s = 0;
        }
        pdptool_unclamp();
//         pdptool_randomise_states();
        pdptool_clamp(p, s);
        pdptool_settle_step(0);
    }

    redraw_current_page();
    gtkx_flush_events();
}

void menuitem_response(gpointer data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

}

void lesion_button(GtkWidget *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    if (historic_lesion_percentage+lesion_percentage <= 100) {
        ghost_controls(1);
        int i, n = (int)round(PAIRS * lesion_percentage/100.0);
        for (i = 0; i < n; i++) {
            network_lesion_random_weight(net_type);
            gtkx_flush_events();
        }
        ghost_controls(0);
        historic_lesion_percentage += lesion_percentage;
    } else {
        ghost_button("Lesion", 1);
    }
    if (historic_lesion_percentage+lesion_percentage <= 100) {
        ghost_controls(0);
    } else {
        ghost_button("Lesion", 1);
    }
}

// to do:
//make a list if buttons that are mututally exclusive and then once one is pressed they are go grey and then once they are done they all go back
//
void naming_task(GtkButton *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif


    init_naming_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));
    init_naming_response_1(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));
    init_naming_response_alt(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));

    ghost_controls(1);
    gtk_naming_graph_reset(GTK_NAMING_GRAPH(naming_graph));

    int p, sample, level, distribution;



    if (net_type == BPTT) {

        bptt_unclamp_all();

        bptt_backup_weights();

        for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
                printf("BPTT NAMING Run: %i/%i, Level: %i/%i\n", distribution+1, naming_lesion_distribution, level+1, naming_lesion_level);
                for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        bptt_randomise_states(bptt_t1);

                        bptt_clamp_visual(p, bptt_t1);
                        bptt_real_time_settle_for(12); //for 3*4 timesteps
                        bptt_unclamp_time(bptt_t1);
                        bptt_real_time_unclamped_settle();
                        bptt_save_naming_response(p, sample, level, distribution);
                        bptt_save_naming_response_1(p, sample, level, distribution);
                        bptt_save_naming_response_alt(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                    }
                }
                if (lesion_type == ZEROING) {
                    bptt_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
                }
                else if (lesion_type == NOISE) {
                    bptt_disrupt_weights(1.45 / (double) naming_lesion_level * level);
// 		  bptt_disrupt_weights((double)level);
                    //ALERT
                }
            }
            bptt_undo_lesion();
        }
    }
//     else if (net_type == TYLER_RECURRENT) {
//
//       tyler_unclamp_all();
//
//       tyler_backup_weights();
//
//       for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
//             for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
//                 printf("BPTT NAMING Run: %i/%i, Level: %i/%i\n", distribution+1, naming_lesion_distribution, level+1, naming_lesion_level);
// 		for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
//                     for (p = 0; p < P; p++) { // pattern
//                         tyler_randomise_states(tyler_t1);
//
// 			tyler_clamp_visual(p, tyler_t1);
// 			tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			tyler_unclamp_time(tyler_t1);
// 			tyler_real_time_unclamped_settle();
// 			tyler_save_naming_response(p, sample, level, distribution);
//
// 			real_time_redraw();
// 			gtkx_flush_events();
//
// 		    }
// 		}
// 		if (lesion_type == ZEROING) {
// 		  tyler_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
// 		}
// 		else if (lesion_type == NOISE) {
// 		  tyler_disrupt_weights(1.45 / (double) naming_lesion_level * level);
// 		}
// 	    }
// 	    tyler_undo_lesion();
//       }
//     }
    else if (net_type == PDPTOOL) {

        pdptool_unclamp();

        pdptool_backup_weights();

        for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
                printf("PDPTOOL NAMING Run: %i/%i, Level: %i/%i\n", distribution+1, naming_lesion_distribution, level+1, naming_lesion_level);
                for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        pdptool_randomise_states();

                        pdptool_clamp_visual(p);
                        pdptool_real_time_settle_for(12); //for 3*4 timesteps
                        pdptool_unclamp();
                        pdptool_real_time_unclamped_settle();
                        pdptool_save_naming_response(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                    }
                }
                pdptool_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
            }
            pdptool_undo_lesion();
        }
    }
    else if (net_type == BM) {

        bm_unclamp();

        bm_backup_weights();

        for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
                for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        printf("BM NAMING Run: %i/%i, Level: %i/%i, Sample:%i/%i, Pattern: %i/%i\n", distribution+1, naming_lesion_distribution, level+1, naming_lesion_level, sample+1, naming_sample, p+1, P);

                        bm_randomise_states();

                        bm_clamp_visual(p);
//                         real_time_settle_for(12); //for 3*4 timesteps
//                         unclamp_time(bptt_t1);
//                         real_time_unclamped_settle(notebook);
// 			bm_real_time_semiclamped_anneal();
                        bm_real_time_anneal();
                        bm_save_naming_response(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                    }
                }
                bm_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
            }
            bm_undo_lesion();
        }
    }

    gtk_naming_graph_update(GTK_NAMING_GRAPH(naming_graph));
    gtk_naming_graph_redraw(naming_graph);

//     print_naming_responses();

//     if (file_exists(naming_file_name)) {
//       dialog = gtk_dialog_new_with_buttons("Overwrite ",
// 					    GTK_WINDOW( window ),
// 					    GTK_DIALOG_MODAL,
// 					    GTK_STOCK_YES, 1,
// 					    GTK_STOCK_NO,  2,
// 					    NULL );
//
//     } else {
    if (is_file_open()) {
        save_naming_responses(naming_file_name);
        save_naming_responses_1(naming_file_name);
        save_naming_responses_alt(naming_file_name);
    }
    printf("Done!\n");
    ghost_controls(0);

}

void tyler_graphs_reset(void) {
    if(lesion_locus == TYLER_LESION_GLOBAL) {
        gtk_tyler_graph_reset(GTK_TYLER_GRAPH(tyler_graph));

    }
    else if (lesion_locus == TYLER_LESION_IN_HIDDEN) {
        gtk_tyler_graph_reset(GTK_TYLER_GRAPH(tyler_graph_ih));

    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_HIDDEN) {
        gtk_tyler_graph_reset(GTK_TYLER_GRAPH(tyler_graph_hh));
    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_OUT) {
        gtk_tyler_graph_reset(GTK_TYLER_GRAPH(tyler_graph_ho));
    }

}

void aoa_graphs_reset(void) {
    if(lesion_locus == TYLER_LESION_GLOBAL) {
        gtk_aoa_graph_reset(GTK_AOA_GRAPH(aoa_graph));

    }
    else if (lesion_locus == TYLER_LESION_IN_HIDDEN) {
        gtk_aoa_graph_reset(GTK_AOA_GRAPH(aoa_graph_ih));

    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_HIDDEN) {
        gtk_aoa_graph_reset(GTK_AOA_GRAPH(aoa_graph_hh));
    }
    else if (lesion_locus == TYLER_LESION_HIDDEN_OUT) {
        gtk_aoa_graph_reset(GTK_AOA_GRAPH(aoa_graph_ho));
    }

}

void save_tyler_responses_locus(int l)
{
    if(l == TYLER_LESION_GLOBAL) {
        save_tyler_responses(tyler_task_file_name);

    }
    else if (l == TYLER_LESION_IN_HIDDEN) {
        save_tyler_responses(tyler_task_ih_file_name);

    }
    else if (l== TYLER_LESION_HIDDEN_HIDDEN) {
        save_tyler_responses(tyler_task_hh_file_name);
    }
    else if (l == TYLER_LESION_HIDDEN_OUT) {
        save_tyler_responses(tyler_task_ho_file_name);
    }
}


void save_aoa_responses_locus(int l)
{
    if(l == TYLER_LESION_GLOBAL) {
        save_aoa_responses(aoa_task_file_name);

    }
    else if (l == TYLER_LESION_IN_HIDDEN) {
        save_aoa_responses(aoa_task_ih_file_name);

    }
    else if (l== TYLER_LESION_HIDDEN_HIDDEN) {
        save_aoa_responses(aoa_task_hh_file_name);
    }
    else if (l == TYLER_LESION_HIDDEN_OUT) {
        save_aoa_responses(aoa_task_ho_file_name);
    }
}

// void save_tyler_responses_locus_network(int l)
// {
//     if(l == TYLER_LESION_GLOBAL) {
//         save_tyler_responses_network("automated.tyler_task");
//     }
//     else if (l == TYLER_LESION_IN_HIDDEN) {
//         save_tyler_responses_network("automated.tyler_task_ih");
//
//     }
//     else if (l== TYLER_LESION_HIDDEN_HIDDEN) {
//         save_tyler_responses_network("automated.tyler_task_hh");
//     }
//     else if (l == TYLER_LESION_HIDDEN_OUT) {
//         save_tyler_responses_network("automated.tyler_task_ho");
//     }
//
// //     sprintf(tyler_task_file_name,  "%s.tyler_task", open_filename);
// //     sprintf(tyler_task_ih_file_name,  "%s.tyler_task_ih", open_filename);
// //     sprintf(tyler_task_ho_file_name,  "%s.tyler_task_ho", open_filename);
// //     sprintf(tyler_task_hh_file_name,  "%s.tyler_task_hh", open_filename);
// }

int working  = 0;
void tyler_feedforward_train_networks_save(int network, const char *auto_name)
{
    ghost_controls(1);

    working = 1;
    net_type = TYLER_FEEDFORWARD;
    gtk_combo_box_set_active(GTK_COMBO_BOX(net_type_combo_box), net_type);

    int i = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(iterations_spin_button));
    double l = gtk_spin_button_get_value(GTK_SPIN_BUTTON(learning_rate_spin_button));
    double m = gtk_spin_button_get_value(GTK_SPIN_BUTTON(momentum_spin_button));
    init_network();

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(iterations_spin_button), i);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(learning_rate_spin_button),l);

    gtkx_flush_events();
    if(accelerate_momentum) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(momentum_spin_button), TYLER_FEEDFORWARD_MOMENTUM_MIN);

    }
    else {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(momentum_spin_button), m);
    }




    sprintf(open_filename, "%s%d.weight", auto_name, network);
    gtk_window_set_title(GTK_WINDOW(window), open_filename);
    save_network(open_filename);

    abort_training = 0;
    tyler_feedforward_train_patterns_serially();
    working = 0;

    ghost_controls(1);

}

void tyler_train_networks_save(int network, const char *auto_name)
{
    ghost_controls(1);

    working = 1;
    net_type = TYLER_RECURRENT;
    init_network();
    gtk_combo_box_set_active(GTK_COMBO_BOX(net_type_combo_box), net_type);


    sprintf(open_filename, "%s%d.weight", auto_name, network);
    gtk_window_set_title(GTK_WINDOW(window), open_filename);
    save_network(open_filename);

    abort_training = 0;
    tyler_train_patterns_serially();
    working = 0;

    ghost_controls(1);

}
// void aoa_task_networks(GtkButton *button, GtkNotebook *notebook)
//
// {
//
//     aoa = 1;
//
//     tyler_task_networks(button, notebook);
//
//     aoa = 0;
// }
// void tyler_task_networks(GtkButton *button, GtkNotebook *notebook)
// {
// // #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// // #endif
//
//
//
//
//     ghost_controls(1);
//
//
//
//
//     gtk_spin_button_set_value(GTK_SPIN_BUTTON(distribution_spin_button), TYLER_LESION_DISTRIBUTION);
//     gtk_spin_button_set_value(GTK_SPIN_BUTTON(level_spin_button), TYLER_LESION_LEVEL);
//     gtk_spin_button_set_value(GTK_SPIN_BUTTON(sample_spin_button), 1);
//
//     //     int local_locus = lesion_locus;
//     //           int tyler_network = 2;
// //     int automated_training = 1;
// //     int automated_testing = 1;
//     double fabs_average_weight;
//     int tyler_network = TYLER_NETWORK; // How many networks to train, or have been trained (depending on automated_training flag).
//
//     gtk_tyler_graph_reset(GTK_TYLER_GRAPH(tyler_graph_networks));
//     gtk_tyler_graph_networks(GTK_TYLER_GRAPH(tyler_graph_networks), tyler_network); // set number of networks that have been trained
//     gtk_tyler_set_auto_name(GTK_TYLER_GRAPH(tyler_graph_networks), &auto_name[0]);
//
//     int p, sample, level, distribution, locus, network;
//
//     //ALERT//
//     locus = TYLER_LESION_GLOBAL;
//
//
//     if (net_type == TYLER_FEEDFORWARD) {
//
//
//         if (automated_training) {
//             for (network = 0; network < tyler_network; network++) {
//                 tyler_feedforward_train_networks_save(network, &auto_name[0]);
//             }
//         }
//
//         if (automated_testing) {
//
//             for (network = 0; network < tyler_network; network++) {
//
//                 sprintf(open_filename, "%s%d.weight", auto_name, network);
//
//                 if (!file_exists(open_filename)) {
//                     throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_filename, "(set automated_training training flag?)");
//                 }
//                 open_all_related_files();
//                 init_tyler_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));
//
//                 fabs_average_weight = 2*tyler_feedforward_fabs_average_weight(locus);
//
//                 tyler_feedforward_unclamp();
//
//                 tyler_feedforward_backup_weights();
//                 ghost_controls(1);
//
//                 for (distribution = 0; distribution < tyler_lesion_distribution; distribution++) { // distribution of lesion
//                     for (level = 0; level < tyler_lesion_level; level++) { // level of leasioning
//                         printf("TYLER FEEDFORWARD TASK: Network: %i/%i, Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", network+1, tyler_network, locus+1, 3, distribution+1, tyler_lesion_distribution, level+1, tyler_lesion_level);
//                         for (sample = 0; sample < tyler_sample; sample++) { // to ensure network in "right" attractor
//                             for (p = 0; p < P; p++) { // pattern
//                                 tyler_feedforward_randomise_states();
//
//                                 tyler_feedforward_clamp_full_pattern(p);
//                                 tyler_feedforward_propagate_gaussian(noise); //for 3*4 timesteps
//                                 tyler_feedforward_unclamp();
//                                 tyler_feedforward_save_response(p, sample, level, distribution);
//
//                                 real_time_redraw();
//                                 gtkx_flush_events();
//
//                             }
//                         }
//                         if (lesion_type == ZEROING) {
//                             tyler_feedforward_lesion_weights_locus(100.0 / (double) tyler_lesion_level, locus);
//
//                         }
//                         else if (lesion_type == NOISE) {
//                             tyler_feedforward_disrupt_weights_locus(fabs_average_weight / (double) tyler_lesion_level * level, locus);
//                         }
//                     }
//                     tyler_feedforward_undo_lesion();
//                 }
//
//
//
//                 if (autosave && is_file_open()) {
//
//                     save_tyler_responses_locus(locus);
//                 }
//             }
//         }
//     }
//
//
//     else if (net_type == TYLER_RECURRENT) {
//
//
//         if (automated_training) {
//             for (network = 0; network < tyler_network; network++) {
//                 tyler_train_networks_save(network, &auto_name[0]);
//             }
//         }
//
//         if (automated_testing) {
//
//             for (network = 0; network < tyler_network; network++) {
//
//                 sprintf(open_filename, "%s%d.weight", auto_name, network);
//
//                 if (!file_exists(open_filename)) {
//                     throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_filename, "(set automated_training training flag?)");
//                 }
//                 open_all_related_files();
//                 init_tyler_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));
//
//                 fabs_average_weight = 2*tyler_feedforward_fabs_average_weight(locus);
//
//                 tyler_unclamp_all();
//
//                 tyler_backup_weights();
//                 ghost_controls(1);
//
//                 for (distribution = 0; distribution < tyler_lesion_distribution; distribution++) { // distribution of lesion
//                     for (level = 0; level < tyler_lesion_level; level++) { // level of leasioning
//                         printf("TYLER TASK: Network: %i/%i, Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", network+1, tyler_network, locus+1, 3, distribution+1, tyler_lesion_distribution, level+1, tyler_lesion_level);
//                         for (sample = 0; sample < tyler_sample; sample++) { // to ensure network in "right" attractor
//                             for (p = 0; p < P; p++) { // pattern
//                                 tyler_randomise_states(tyler_t1);
//
//                                 tyler_clamp_full_pattern(p, tyler_t1);
//                                 tyler_real_time_settle_for(12); //for 3*4 timesteps
//                                 tyler_unclamp_time(tyler_t1);
//                                 tyler_real_time_unclamped_settle();
//                                 tyler_save_response(p, sample, level, distribution);
//
//                                 real_time_redraw();
//                                 gtkx_flush_events();
//
//                             }
//                         }
//                         if (lesion_type == ZEROING) {
//                             tyler_lesion_weights_locus(100.0 / (double) tyler_lesion_level, locus);
//
//                         }
//                         else if (lesion_type == NOISE) {
//                             tyler_disrupt_weights(fabs_average_weight / (double) tyler_lesion_level * level);
//                         }
//                     }
//                     tyler_undo_lesion();
//                 }
//
//
//
//                 if (autosave && is_file_open()) {
//
//                     save_tyler_responses_locus(locus);
//                 }
//             }
//         }
//     }
//     char open_name_auto[1280];
//
//     int i;
//
//     for (network = 0; network < tyler_network; network++) {
//         redraw_current_page();
//
//         if (aoa) {
//
//             sprintf(open_name_auto, "%s%d.aoa_task", auto_name, network);
//             if (file_exists(open_name_auto)) {
// //                 printf("main: %s\n", open_name_auto);
//
//
//                 open_aoa_responses(open_name_auto);
//
//                 gtk_aoa_graph_update_networks_accumulate(GTK_AOA_GRAPH(aoa_graph_networks), network);
// // 		printf("%d networks: %d\n", __LINE__, gtk_aoa_graph_get_networks(GTK_AOA_GRAPH(aoa_graph_networks)));
//
//             }
//             else
//             {
//                 throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_name_auto, "(set automated_training flag?)");
//
//             }
//
// //             printf("%d networks: %d\n", __LINE__, gtk_aoa_graph_get_networks(GTK_AOA_GRAPH(aoa_graph_networks)));
//
//             gtk_aoa_graph_update_networks_final(GTK_AOA_GRAPH(aoa_graph_networks));
//
//
//             //     tyler_graphs_update();
//             //     gtk_tyler_graph_update_networks(GTK_TYLER_GRAPH(tyler_graph_networks));
//             //     gtk_tyler_graph_set_type(tyler_graph_networks);
//             //     gtk_tyler_graph_redraw(tyler_graph_networks);
//
//
//             //     char open_name_auto[128];
//             sprintf(open_name_auto, "%s", auto_name);
//
//             open_name_auto[strlen(open_name_auto)-1] = 0; //snip off the trailing '_'
//
//             for (i = 0; i < 6; i++) {
//                 gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_networks), i);
//
//                 gtk_aoa_graph_pdf(aoa_graph_networks, open_name_auto);
//             }
//
//             gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_networks), gtk_combo_box_get_active(GTK_COMBO_BOX(aoa_combo_box)));
//             gtk_aoa_graph_redraw(aoa_graph_networks);
//
//
//         } else {
//
//             sprintf(open_name_auto, "%s%d.tyler_task", auto_name, network);
//             if (file_exists(open_name_auto)) {
//                 //             printf("main: %s\n", open_name_auto);
//
//                 open_tyler_responses(open_name_auto);
//
//                 gtk_tyler_graph_update_networks_accumulate(GTK_TYLER_GRAPH(tyler_graph_networks), network);
//
//             }
//             else
//             {
//                 throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_name_auto, "(set automated_training flag?)");
//
//             }
//
//             gtk_tyler_graph_update_networks_final(GTK_TYLER_GRAPH(tyler_graph_networks));
//
//
//             //     tyler_graphs_update();
//             //     gtk_tyler_graph_update_networks(GTK_TYLER_GRAPH(tyler_graph_networks));
//             //     gtk_tyler_graph_set_type(tyler_graph_networks);
//             //     gtk_tyler_graph_redraw(tyler_graph_networks);
//
//
//             //     char open_name_auto[128];
//             sprintf(open_name_auto, "%s", auto_name);
//
//             open_name_auto[strlen(open_name_auto)-1] = 0; //snip off the trailing '_'
//
//             for (i = 0; i < 6; i++) {
//                 gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), i);
//
//                 gtk_tyler_graph_pdf(tyler_graph_networks, open_name_auto);
//             }
//
//             gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), gtk_combo_box_get_active(GTK_COMBO_BOX(tyler_combo_box)));
//             gtk_tyler_graph_redraw(tyler_graph_networks);
//
//         }
//
//
//
//     }
//
//     printf("Done!\n");
//     ghost_controls(0);
//
// }


void tyler_original_task(GtkButton *menu_button, GtkNotebook *notebook)
{

    GtkWindow *task_options = g_object_new(GTK_TYPE_WINDOW, "type", GTK_WINDOW_TOPLEVEL, NULL);
    gtk_widget_set_can_focus(GTK_WIDGET(task_options), TRUE);
    gtk_window_set_decorated(GTK_WINDOW(task_options), TRUE);
    gtk_window_set_type_hint(GTK_WINDOW(task_options), GDK_WINDOW_TYPE_HINT_POPUP_MENU);
    gtk_window_set_transient_for(task_options, GTK_WINDOW(window));
    gtk_window_set_modal(task_options, TRUE);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(task_options),TRUE);
    gtk_window_set_title(GTK_WINDOW(task_options), "Task Options");
    gtk_window_set_position(GTK_WINDOW(task_options), GTK_WIN_POS_CENTER);
    gtk_widget_set_app_paintable(GTK_WIDGET(task_options), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(task_options),200, 10);
    gtk_widget_show(GTK_WIDGET(task_options));

    GtkWidget *box = gtk_vbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER(task_options), box);

    GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show(hbox);
    int expand = 0; // expand controls whether the widgets are laid out in the box to fill in all the extra space in the box so the box is expanded to fill the area allotted to it
    int fill = 0; // fill controls whether the extra space is allocated to the widgets themselves
    int spacing = 5; // spacing is added between widgets..
    gtk_box_pack_start(GTK_BOX(box), hbox, expand, fill, spacing);


    GtkWidget *label = gtk_label_new("Test:");

    gtk_box_pack_start(GTK_BOX(hbox), label, expand, fill, spacing);
    gtk_widget_show(label);

    GtkWidget *test = gtk_check_button_new();

    gtk_box_pack_start(GTK_BOX(hbox), test, expand, fill, spacing);
    gtk_widget_show(test);


    hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show(hbox);

    gtk_box_pack_start(GTK_BOX(box), hbox, expand, fill, spacing);
    label = gtk_label_new("Train:");

    gtk_box_pack_start(GTK_BOX(hbox), label, expand, fill, spacing);
    gtk_widget_show(label);

    GtkWidget *train = gtk_check_button_new();

    gtk_box_pack_start(GTK_BOX(hbox), train, expand, fill, spacing);
    gtk_widget_show(train);

    hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show(hbox);

    gtk_box_pack_start(GTK_BOX(box), hbox, expand, fill, spacing);
    label = gtk_label_new("Runs:");

    gtk_box_pack_start(GTK_BOX(hbox), label, expand, fill, spacing);
    gtk_widget_show(label);

    GtkWidget *networks_spin_button = gtk_spin_button_new_with_range(0, 300, 10);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(networks_spin_button), 10);
//   g_signal_connect(G_OBJECT(networks_spin_button), "value-changed", G_CALLBACK(set_networks_spin_button), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), networks_spin_button, expand, fill, padding);

    gtk_widget_show(networks_spin_button);



    hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(box), hbox, expand, fill, spacing);

    label = gtk_label_new("Filename:");

    gtk_box_pack_start(GTK_BOX(hbox), label, expand, fill, spacing);
    gtk_widget_show(label);

    GtkEntryBuffer *buffer = gtk_entry_buffer_new("test", 4) ;
    GtkWidget *text_box = gtk_entry_new_with_buffer(buffer);
    gtk_box_pack_start(GTK_BOX(hbox), text_box, expand, fill, padding);

    gtk_widget_show(text_box);

    GtkWidget *button = gtk_button_new_with_label("Start");

    networks_struct *data = malloc(sizeof *data);
    data->train = train;
    data->test = test;
    data->networks = networks_spin_button;
    data->text = text_box;
    data->aoa = aoa;
    data->pop_up = GTK_WIDGET(task_options);

    g_signal_connect(G_OBJECT(button), "released", G_CALLBACK(tyler_original_task_), data);

    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);

    gtk_widget_show(button);

    gtk_widget_show(box);



}

void tyler_original_task_(GtkButton *button, networks_struct *data)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif


    int n = TEST_N_PATTERN;// ALERT
    int noise = TEST_NOISE;
//     int networks = gtk_spin_button_get_value(GTK_SPIN_BUTTON(((networks_struct*) data)->networks));



    ghost_controls(1);

    int networks = gtk_spin_button_get_value(GTK_SPIN_BUTTON(((networks_struct*) data)->networks));

    int automated_testing = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(((networks_struct*) data)->test));

    printf("networks = %d\n", networks) ;

    int automated_training = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(((networks_struct*) data)->train));

//     printf("automated_training%d\n",automated_training) ;
    int aoa = (int) ((networks_struct*) data)->aoa;

    printf("AOA = %d\n",aoa) ;
    // get text from the window with the task settings
    const char *auto_name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(((networks_struct*) data)->text)));



    if (aoa) {
        init_aoa_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));

    } else  {
        init_tyler_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));
    }

    //     int local_locus = lesion_locus;
    //           int tyler_network = 2;
//     int automated_training = 1;
//     int automated_testing = 1;
    double fabs_average_weight;
//     int tyler_network = networks; // How many networks to train, or have been trained (depending on automated_training flag).
//     int tyler_network = 10;


    int lesion_distribution, lesion_level, lesion_sample;

    if (aoa) {
        gtk_aoa_graph_reset(GTK_AOA_GRAPH(aoa_graph_networks));
        gtk_aoa_graph_networks(GTK_AOA_GRAPH(aoa_graph_networks), networks); // set number of networks that have been trained
        gtk_aoa_set_auto_name(GTK_AOA_GRAPH(aoa_graph_networks), &auto_name[0]);

        lesion_distribution = aoa_lesion_distribution;
        lesion_sample = aoa_sample;

        lesion_level = aoa_lesion_level;




//         printf("aoa_lesion_distribution = %d\n", aoa_lesion_distribution);
//         printf("tyler_lesion_distribution = %d\n", tyler_lesion_distribution);




    } else {
        gtk_tyler_graph_reset(GTK_TYLER_GRAPH(tyler_graph_networks));
        gtk_tyler_graph_networks(GTK_TYLER_GRAPH(tyler_graph_networks), networks); // set number of networks that have been trained
        gtk_tyler_set_auto_name(GTK_TYLER_GRAPH(tyler_graph_networks), &auto_name[0]);

        lesion_distribution = tyler_lesion_distribution;
        lesion_level = tyler_lesion_level;
        lesion_sample =    tyler_sample;


    }




    int p, sample, level, distribution, locus, network;          //iterators


    //ALERT//
    locus = TYLER_LESION_GLOBAL;

    if (net_type == TYLER_FEEDFORWARD) {


        if (automated_training) {
            for (network = 0; network < networks; network++) {
                ghost_controls(1);

                tyler_feedforward_train_networks_save(network, &auto_name[0]);


                printf("Trained network: %i of %i\n", network, networks);

            }
        }

        if (automated_testing) {
            ghost_controls(1);

            for (network = 0; network < networks; network++) {
                sprintf(open_filename, "%s%d.weight", auto_name, network);

//                 sprintf(open_filename, "%s%d.weight", aoa_auto_name[aoa], network);

                if (!file_exists(open_filename)) {
                    throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_filename, "(set automated_training training flag?)");
                }
                open_all_related_files();

                if (aoa) {
                    init_aoa_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));

                } else  {
                    init_tyler_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));
                }

                fabs_average_weight = 2*tyler_feedforward_fabs_average_weight(locus);

                tyler_feedforward_unclamp();

                tyler_feedforward_backup_weights();
                ghost_controls(1);

                for (distribution = 0; distribution < lesion_distribution; distribution++) { // distribution of lesion
                    for (level = 0; level < lesion_level; level++) { // level of leasioning


                        if (lesion_type == ZEROING) {
                            tyler_feedforward_lesion_weights_locus(level * 100.0 / (double) lesion_level, locus);

                        }
                        else if (lesion_type == NOISE) {
                            tyler_feedforward_disrupt_weights_locus(fabs_average_weight / (double) lesion_level * level, locus);
                        }



                        printf("TYLER FEEDFORWARD TASK: Network: %i/%i, Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", network+1, networks, locus+1, 3, distribution+1, lesion_distribution, level+1, lesion_level);
                        for (sample = 0; sample < lesion_sample; sample++) { // to ensure network in "right" attractor
                            for (p = 0; p < P; p++) { // pattern
                                tyler_feedforward_randomise_states();
                                tyler_feedforward_clamp_full_pattern(p);

                                tyler_feedforward_propagate_noise(noise, n); //for 3*4 timesteps
                                tyler_feedforward_unclamp();

                                if (aoa) {
                                    aoa_feedforward_save_response(p, sample, level, distribution, noise, n);

                                }
                                else {
                                    tyler_feedforward_save_response(p, sample, level, distribution, noise, n);
                                }

                                real_time_redraw();
                                gtkx_flush_events();

                            }
                        }


                        tyler_feedforward_undo_lesion();

                    }
                }



                if (autosave && is_file_open()) {
                    if (aoa) {

                        save_aoa_responses_locus(locus);

                    }
                    else {

                        save_tyler_responses_locus(locus);

                    }
                }
            }
        }
    }


    else if (net_type == TYLER_RECURRENT) {


        if (automated_training) {
            for (network = 0; network < networks; network++) {
                tyler_train_networks_save(network, &auto_name[0]);
            }
        }

        if (automated_testing) {

            for (network = 0; network < networks; network++) {

                sprintf(open_filename, "%s%d.weight", auto_name, network);

                if (!file_exists(open_filename)) {
                    throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_filename, "(set automated_training training flag?)");
                }
                open_all_related_files();
                init_tyler_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));

                fabs_average_weight = 2*tyler_feedforward_fabs_average_weight(locus);

                tyler_unclamp_all();

                tyler_backup_weights();
                ghost_controls(1);

                for (distribution = 0; distribution < lesion_distribution; distribution++) { // distribution of lesion
                    for (level = 0; level < lesion_level; level++) { // level of leasioning


                        if (lesion_type == ZEROING) {
                            tyler_lesion_weights_locus(level * 100.0 / (double) lesion_level, locus);

                        }
                        else if (lesion_type == NOISE) {
                            tyler_disrupt_weights(fabs_average_weight / (double) lesion_level * level);
                        }
                        printf("TYLER TASK: Network: %i/%i, Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", network+1, networks, locus+1, 3, distribution+1, lesion_distribution, level+1, lesion_level);
                        for (sample = 0; sample < lesion_sample; sample++) { // to ensure network in "right" attractor
                            for (p = 0; p < P; p++) { // pattern
                                tyler_randomise_states(tyler_t1);

                                tyler_clamp_full_pattern(p, tyler_t1);
                                tyler_real_time_settle_for(12); //for 3*4 timesteps
                                tyler_unclamp_time(tyler_t1);
                                tyler_real_time_unclamped_settle();
                                tyler_save_response(p, sample, level, distribution, noise, n);

                                real_time_redraw();
                                gtkx_flush_events();

                            }
                        }


                        tyler_undo_lesion();

                    }
                }



                if (autosave && is_file_open()) {

                    save_tyler_responses_locus(locus);
                }
            }
        }
    }
    char open_name_auto[128];
    for (network = 0; network < networks; network++) {
        redraw_current_page();

        if (aoa) {

            sprintf(open_name_auto, "%s%d.aoa_task", auto_name, network);
            if (file_exists(open_name_auto)) {
                printf("main: %s\n", open_name_auto);


                open_aoa_responses(open_name_auto);

                gtk_aoa_graph_update_networks_accumulate(GTK_AOA_GRAPH(aoa_graph_networks), network);
                printf("%d networks: %d\n", __LINE__, gtk_aoa_graph_get_networks(GTK_AOA_GRAPH(aoa_graph_networks)));

            }
            else
            {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_name_auto, "(set automated_training flag?)");

            }


        } else {

            sprintf(open_name_auto, "%s%d.tyler_task", auto_name, network);
            if (file_exists(open_name_auto)) {
                printf("main: %s\n", open_name_auto);


                open_tyler_responses(open_name_auto);

                gtk_tyler_graph_update_networks_accumulate(GTK_TYLER_GRAPH(tyler_graph_networks), network);


            }
            else
            {
                throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_name_auto, "(set automated_training flag?)");

            }

        }


    }
    if (aoa) {
        gtk_aoa_graph_update_networks_final(GTK_AOA_GRAPH(aoa_graph_networks));

    } else {
        gtk_tyler_graph_update_networks_final(GTK_TYLER_GRAPH(tyler_graph_networks));

    }


    //     tyler_graphs_update();
    //     gtk_tyler_graph_update_networks(GTK_TYLER_GRAPH(tyler_graph_networks));
    int i;

    if (aoa) {
        gtk_aoa_graph_redraw(aoa_graph_networks);

        //     char open_name_auto[128];
        sprintf(open_name_auto, "%s", auto_name);

//       open_name_auto[strlen(open_name_auto)-1] = 0; //snip off the trailing '_'
        /*
              for (i = 0; i < 6; i++) {
        	gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), i);

        	gtk_tyler_graph_pdf(tyler_graph_networks, open_name_auto);
              }

              gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), gtk_combo_box_get_active(GTK_COMBO_BOX(tyler_combo_box))); */
    } else  {

        gtk_tyler_graph_redraw(tyler_graph_networks);

        //     char open_name_auto[128];
        sprintf(open_name_auto, "%s", auto_name);

        open_name_auto[strlen(open_name_auto)-1] = 0; //snip off the trailing '_'

        for (i = 0; i < 6; i++) {
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), i);

            gtk_tyler_graph_pdf(tyler_graph_networks, open_name_auto);
        }

        gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), gtk_combo_box_get_active(GTK_COMBO_BOX(tyler_combo_box)));

    }

    printf("Done!\n");
    ghost_controls(0);

    gtk_widget_destroy(GTK_WIDGET(((networks_struct*) data)->pop_up));


}

// void tyler_task_networks_(GtkButton *button, GtkNotebook *notebook)
// {
// #if PRINT_FUNC
//     printf("%s\n", __FUNCTION__);
// #endif
//
//     gtk_combo_box_set_active(GTK_COMBO_BOX(net_type_combo_box), TYLER_FEEDFORWARD);
//     network_init(net_type);
//     redraw_current_page();
//
//
//     ghost_controls(1);
//     gtk_spin_button_set_value(GTK_SPIN_BUTTON(distribution_spin_button), TYLER_LESION_DISTRIBUTION);
//     gtk_spin_button_set_value(GTK_SPIN_BUTTON(level_spin_button), TYLER_LESION_LEVEL);
//     gtk_spin_button_set_value(GTK_SPIN_BUTTON(sample_spin_button), TYLER_SAMPLE);
//     char open_name_auto[128];
//
//     //     int local_locus = lesion_locus;
//     //           int tyler_network = 2;
//     int automated_training = 0;
//     int automated_testing = 0;
//     double fabs_average_weight;
//     int tyler_network = 1; // How many networks to train, or have been trained (depending on automated_training flag).
//
//
//
//     gtk_tyler_graph_reset(GTK_TYLER_GRAPH(tyler_graph_networks));
//
//     gtk_tyler_graph_networks(GTK_TYLER_GRAPH(tyler_graph_networks), tyler_network); // set number of networks that have been trained
// //     gtk_tyler_set_auto_name(GTK_TYLER_GRAPH(tyler_graph_networks), &auto_name[0]);
//
//     int p, sample, level, distribution, locus, network;
//
//     //ALERT
//     locus = TYLER_LESION_GLOBAL;
//
//
//     if (net_type == TYLER_FEEDFORWARD) {
//
//
//         if (automated_training) {
//             for (network = 0; network < tyler_network; network++) {
//                 tyler_feedforward_train_networks_save(network);
//             }
//         }
//
//         if (automated_testing) {
//
//             for (network = 0; network < tyler_network; network++) {
//
//                 sprintf(open_filename, "%s%d.weight", auto_name, network);
//
//                 if (!file_exists(open_filename)) {
//                     throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_filename, "(set automated_training training flag?)");
//                 }
//                 open_all_related_files();
//                 init_tyler_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));
//
//                 fabs_average_weight = 2*tyler_feedforward_fabs_average_weight(locus);
//
//                 tyler_feedforward_unclamp();
//
//                 tyler_feedforward_backup_weights();
//                 ghost_controls(1);
//
//                 for (distribution = 0; distribution < tyler_lesion_distribution; distribution++) { // distribution of lesion
//                     for (level = 0; level < tyler_lesion_level; level++) { // level of leasioning
//                         printf("TYLER FEEDFORWARD TASK: Network: %i/%i, Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", network+1, tyler_network, locus+1, 3, distribution+1, tyler_lesion_distribution, level+1, tyler_lesion_level);
//                         for (sample = 0; sample < tyler_sample; sample++) { // to ensure network in "right" attractor
//                             for (p = 0; p < P; p++) { // pattern
//                                 tyler_feedforward_randomise_states();
//
//                                 tyler_feedforward_clamp_full_pattern(p);
//                                 tyler_feedforward_propagate_gaussian(); //for 3*4 timesteps
//                                 tyler_feedforward_unclamp();
//                                 tyler_feedforward_save_response(p, sample, level, distribution);
//
//                                 real_time_redraw();
//                                 gtkx_flush_events();
//
//                             }
//                         }
//                         if (lesion_type == ZEROING) {
//                             tyler_feedforward_lesion_weights_locus(100.0 / (double) tyler_lesion_level, locus);
//
//                         }
//                         else if (lesion_type == NOISE) {
//                             tyler_feedforward_disrupt_weights_locus(fabs_average_weight / (double) tyler_lesion_level * level, locus);
//                         }
//                     }
//                     tyler_feedforward_undo_lesion();
//                 }
//
//
//
//                 if (autosave && is_file_open()) {
//
//                     save_tyler_responses_locus(locus);
//                 }
//             }
//         }
//     }
//
//
//
//     network = 0;
//     //     tyler_graphs_update();
//     sprintf(open_filename, "%s%d.weight", auto_name, network);
//
//     if (!file_exists(open_filename)) {
//         throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_filename, "(set automated_training training flag?)");
//     }
//     open_all_related_files();
//     init_tyler_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));
//
//
//     for (network = 0; network < tyler_network; network++) {
//         redraw_current_page();
//         sprintf(open_name_auto, "%s%d.tyler_task", auto_name, network);
//         if (file_exists(open_name_auto)) {
//             printf("%s\n", open_name_auto);
//
//             open_tyler_responses(open_name_auto);
//
//             gtk_tyler_graph_update_networks_accumulate(GTK_TYLER_GRAPH(tyler_graph_networks));
//
//         }
//         else
//         {
//             throw_warning(__FILE__,  __LINE__, __FUNCTION__,"Could not open file:", 2, open_name_auto, "(set automated_training flag?)");
//
//         }
//
//
//
//     }
//     gtk_tyler_graph_update_networks_final(GTK_TYLER_GRAPH(tyler_graph_networks));
//
//     gtk_tyler_graph_redraw(tyler_graph);
//
//
//     int i;
//     sprintf(open_name_auto, "%s", auto_name);
//
//     open_name_auto[strlen(open_name_auto)-1] = 0; //snip off the trailing '_'
//
//     for (i = 0; i < 6; i++) {
//         gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), i);
//
//         gtk_tyler_graph_pdf(tyler_graph_networks, open_name_auto);
//     }
//
//
//     printf("Done!\n");
//     ghost_controls(0);
//
// }


void tyler_task(GtkButton *button, GtkNotebook *notebook)
{

#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif


    int n = TEST_N_PATTERN; // ALERT FIX
    int noise = TEST_NOISE;


    int local_locus = lesion_locus;

    double fabs_average_weight = 0.0;

    if (aoa) {

        init_aoa_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));

    } else {
        init_tyler_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));

    }
    int lesion_distribution, lesion_level, lesion_sample;
    if (aoa) {
        lesion_distribution = aoa_lesion_distribution, lesion_level = aoa_lesion_level, lesion_sample = aoa_sample;

    }
    else {
        lesion_distribution = tyler_lesion_distribution, lesion_level = tyler_lesion_level, lesion_sample = tyler_sample;

    }

    ghost_controls(1);


    if (aoa) {
        aoa_graphs_reset();
    } else {
        tyler_graphs_reset();
    }

    int p, sample, level, distribution, locus;



    if (net_type == TYLER_RECURRENT) {
        tyler_unclamp_all();

        tyler_backup_weights();


        if (local_locus == 4) { //All
            for (locus = 0; locus < 4; locus++) {
                fabs_average_weight = 2*tyler_fabs_average_weight(locus);


                for (distribution = 0; distribution < lesion_distribution; distribution++) { // distribution of lesion
                    for (level = 0; level < lesion_level; level++) { // level of leasioning
                        if (aoa) {
                            printf("AOA TASK Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", locus+1, 4, distribution+1, lesion_distribution, level+1, lesion_level);
                        } else {
                            printf("TYLER TASK Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", locus+1, 4, distribution+1, lesion_distribution, level+1, lesion_level);
                        }
                        for (sample = 0; sample < lesion_sample; sample++) { // to ensure network in "right" attractor
                            for (p = 0; p < P; p++) { // pattern
                                tyler_randomise_states(tyler_t1);

                                tyler_clamp_full_pattern(p, tyler_t1);
                                tyler_real_time_settle_for(12); //for 3*4 timesteps
                                tyler_unclamp_time(tyler_t1);
                                tyler_real_time_unclamped_settle();
                                if (aoa) {
                                    aoa_save_response(p, sample, level, distribution, noise, n);
                                }
                                else {
                                    tyler_save_response(p, sample, level, distribution, noise, n);
                                }

                                real_time_redraw();
                                gtkx_flush_events();

                            }
                        }
                        if (lesion_type == ZEROING) {
                            tyler_lesion_weights_locus(100.0 / (double) lesion_level, locus);

                        }
                        else if (lesion_type == NOISE) {
                            tyler_disrupt_weights(fabs_average_weight / (double) lesion_level * level);
                        }
                    }
                    tyler_undo_lesion();
                }
            }

        }
        else {

            for (distribution = 0; distribution < lesion_distribution; distribution++) { // distribution of lesion
                for (level = 0; level < lesion_level; level++) { // level of leasioning
                    if (aoa) {
                        printf("AOA TASK Run: %i/%i, Level: %i/%i\n", distribution+1, lesion_distribution, level+1, lesion_level);
                    } else {
                        printf("TYLER TASK Run: %i/%i, Level: %i/%i\n", distribution+1, lesion_distribution, level+1, lesion_level);
                    }
                    for (sample = 0; sample < lesion_sample; sample++) { // to ensure network in "right" attractor
                        for (p = 0; p < P; p++) { // pattern
                            tyler_randomise_states(tyler_t1);

                            tyler_clamp_full_pattern(p, tyler_t1);
                            tyler_real_time_settle_for(12); //for 3*4 timesteps
                            tyler_unclamp_time(tyler_t1);
                            tyler_real_time_unclamped_settle();
                            if (aoa) {
                                aoa_save_response(p, sample, level, distribution, noise, n);
                            }
                            else {
                                tyler_save_response(p, sample, level, distribution, noise, n);
                            }
                            real_time_redraw();
                            gtkx_flush_events();

                        }
                    }
                    if (lesion_type == ZEROING) {
                        tyler_lesion_weights_locus(100.0 / (double) lesion_level, local_locus);

                    }
                    else if (lesion_type == NOISE) {
                        tyler_disrupt_weights(fabs_average_weight / (double) lesion_level * level);
                    }
                }
                tyler_undo_lesion();
            }
        }
    } else if (net_type == TYLER_FEEDFORWARD) {


        if (local_locus == TYLER_LESION_HIDDEN_HIDDEN) {
            return;
        }

        tyler_feedforward_unclamp();

        tyler_feedforward_backup_weights();


        if (local_locus == 4) {
            for (locus = 0; locus < 3; locus++) {
                fabs_average_weight = 2*tyler_feedforward_fabs_average_weight(local_locus);

                for (distribution = 0; distribution < lesion_distribution; distribution++) { // distribution of lesion
                    for (level = 0; level < lesion_level; level++) { // level of leasioning
                        if (aoa) {
                            printf("AOA FEEDFORWARD TASK Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", locus+1, 3, distribution+1, lesion_distribution, level+1, lesion_level);
                        } else {
                            printf("TYLER FEEDFORWARD TASK Lesion Locus: %i/%i, Run: %i/%i, Level: %i/%i\n", locus+1, 3, distribution+1, lesion_distribution, level+1, lesion_level);
                        }
                        for (sample = 0; sample < lesion_sample; sample++) { // to ensure network in "right" attractor
                            for (p = 0; p < P; p++) { // pattern
                                tyler_feedforward_randomise_states();

                                tyler_feedforward_clamp_full_pattern(p);
                                tyler_feedforward_propagate_noise(noise, n); //for 3*4 timesteps
                                tyler_feedforward_unclamp();
                                if (aoa) {
                                    aoa_feedforward_save_response(p, sample, level, distribution, noise, n);
                                } else {
                                    tyler_feedforward_save_response(p, sample, level, distribution, noise, n);
                                }

                                real_time_redraw();
                                gtkx_flush_events();

                            }
                        }
                        if (lesion_type == ZEROING) {
                            tyler_feedforward_lesion_weights_locus(100.0 / (double) lesion_level, locus);

                        }
                        else if (lesion_type == NOISE) {
                            tyler_feedforward_disrupt_weights(fabs_average_weight / (double) lesion_level * level);
                        }
                    }
                    tyler_feedforward_undo_lesion();
                }
            }
        }
        else {


            for (distribution = 0; distribution < lesion_distribution; distribution++) { // distribution of lesion
                for (level = 0; level < lesion_level; level++) { // level of leasioning
                    if (aoa) {
                        printf("AOA FEEDFORWARD TASK Run: %i/%i, Level: %i/%i\n", distribution+1, lesion_distribution, level+1, lesion_level);
                    } else {
                        printf("TYLER FEEDFORWARD TASK Run: %i/%i, Level: %i/%i\n", distribution+1, lesion_distribution, level+1, lesion_level);
                    }
                    for (sample = 0; sample < lesion_sample; sample++) { // to ensure network in "right" attractor
                        for (p = 0; p < P; p++) { // pattern
                            tyler_feedforward_randomise_states();

                            tyler_feedforward_clamp_full_pattern(p);
                            tyler_feedforward_propagate_noise(noise, n); //for 3*4 timesteps
                            tyler_feedforward_unclamp();

                            if (aoa) {
                                aoa_feedforward_save_response(p, sample, level, distribution, noise, n);
                            } else {
                                tyler_feedforward_save_response(p, sample, level, distribution, noise, n);
                            }
                            real_time_redraw();
                            gtkx_flush_events();

                        }
                    }
                    if (lesion_type == ZEROING) {
                        tyler_feedforward_lesion_weights_locus(100.0 / (double) lesion_level, local_locus);

                    }
                    else if (lesion_type == NOISE) {
                        tyler_feedforward_disrupt_weights(fabs_average_weight / (double) lesion_level * level);
                    }
                }
                tyler_feedforward_undo_lesion();
            }
        }
    }
    if (aoa) {
        aoa_graphs_update();
    } else {
        tyler_graphs_update();
    }

    if (autosave && is_file_open()) {
        if (aoa) {
            save_aoa_responses_locus(local_locus);
        } else {
            save_tyler_responses_locus(local_locus);
        }
    }
    //     }
    printf("Done!\n");
    ghost_controls(0);
}

void aoa_task(GtkButton *button, GtkNotebook *notebook)
{

    aoa = 1;
    tyler_task(button, notebook);
    aoa =0;

}


void aoa_original_task(GtkButton *button, GtkNotebook *notebook)
{
    aoa = 1;
    tyler_original_task(button, notebook);
    aoa =0;

}
void naming_task_alt(GtkButton *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif


    init_naming_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));

    ghost_controls(1);
    gtk_naming_graph_reset(GTK_NAMING_GRAPH(naming_graph));

    int p, sample, level, distribution;



    if (net_type == BPTT) {

        bptt_unclamp_all();

        bptt_backup_weights();

        for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
                printf("BPTT NAMING Run: %i/%i, Level: %i/%i\n", distribution+1, naming_lesion_distribution, level+1, naming_lesion_level);
                for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        bptt_randomise_states(bptt_t1);

                        bptt_clamp_visual(p, bptt_t1);
                        bptt_real_time_settle_for(12); //for 3*4 timesteps
                        bptt_unclamp_time(bptt_t1);
                        bptt_real_time_unclamped_settle();
                        bptt_save_naming_response_alt(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                    }
                }
                if (lesion_type == ZEROING) {
                    bptt_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
                }
                else if (lesion_type == NOISE) {
                    bptt_disrupt_weights(1.45 / (double) naming_lesion_level * level);
                }
            }
            bptt_undo_lesion();
        }
    }
//     else if (net_type == TYLER_RECURRENT) {
//
//       tyler_unclamp_all();
//
//       tyler_backup_weights();
//
//       for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
//             for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
//                 printf("BPTT NAMING Run: %i/%i, Level: %i/%i\n", distribution+1, naming_lesion_distribution, level+1, naming_lesion_level);
// 		for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
//                     for (p = 0; p < P; p++) { // pattern
//                         tyler_randomise_states(tyler_t1);
//
// 			tyler_clamp_visual(p, tyler_t1);
// 			tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			tyler_unclamp_time(tyler_t1);
// 			tyler_real_time_unclamped_settle();
// 			tyler_save_naming_response_alt(p, sample, level, distribution);
//
// 			real_time_redraw();
// 			gtkx_flush_events();
//
// 		    }
// 		}
// 		if (lesion_type == ZEROING) {
// 		  tyler_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
// 		}
// 		else if (lesion_type == NOISE) {
// 		  tyler_disrupt_weights(1.45 / (double) naming_lesion_level * level);
// 		}
// 	    }
// 	    tyler_undo_lesion();
//       }
//     }
    else if (net_type == PDPTOOL) {

        pdptool_unclamp();

        pdptool_backup_weights();

        for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
                printf("PDPTOOL NAMING Run: %i/%i, Level: %i/%i\n", distribution+1, naming_lesion_distribution, level+1, naming_lesion_level);
                for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        pdptool_randomise_states();

                        pdptool_clamp_visual(p);
                        pdptool_real_time_settle_for(12); //for 3*4 timesteps
                        pdptool_unclamp();
                        pdptool_real_time_unclamped_settle();
                        pdptool_save_naming_response_alt(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                    }
                }
                pdptool_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
            }
            pdptool_undo_lesion();
        }
    }
    else if (net_type == BM) {

        bm_unclamp();

        bm_backup_weights();

        for (distribution = 0; distribution < naming_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < naming_lesion_level; level++) { // level of leasioning
                for (sample = 0; sample < naming_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        printf("BM NAMING Run: %i/%i, Level: %i/%i, Sample:%i/%i, Pattern: %i/%i\n", distribution+1, naming_lesion_distribution, level+1, naming_lesion_level, sample+1, naming_sample, p+1, P);

                        bm_randomise_states();

                        bm_clamp_visual(p);
                        //                         real_time_settle_for(12); //for 3*4 timesteps
                        //                         unclamp_time(bptt_t1);
                        //                         real_time_unclamped_settle(notebook);
                        bm_real_time_semiclamped_anneal();
                        bm_save_naming_response_alt(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                    }
                }
                bm_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
            }
            bm_undo_lesion();
        }
    }

    gtk_naming_graph_update(GTK_NAMING_GRAPH(naming_graph));
    gtk_naming_graph_redraw(naming_graph);

    //     print_naming_responses();

    //     if (file_exists(naming_file_name)) {
    //       dialog = gtk_dialog_new_with_buttons("Overwrite ",
    // 					    GTK_WINDOW( window ),
    // 					    GTK_DIALOG_MODAL,
    // 					    GTK_STOCK_YES, 1,
    // 					    GTK_STOCK_NO,  2,
    // 					    NULL );
    //
    //     } else {
    if (autosave && is_file_open()) {
        save_naming_responses(naming_file_name);
    }
    //     }
    printf("Done!\n");
    ghost_controls(0);

}

void sorting_task(GtkButton *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    ghost_controls(1);
    gtk_sorting_graph_reset(GTK_SORTING_GRAPH(sorting_graph));


    int p, sample, level, distribution;

    init_sorting_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));


    if (net_type == BPTT) {
        bptt_unclamp_all();

        bptt_backup_weights();

        for (distribution = 0; distribution < sorting_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < sorting_lesion_level; level++) { // level of leasioning
                printf("BPTT SORTING Run: %i/%i, Level: %i/%i\n", distribution+1, sorting_lesion_distribution, level+1, sorting_lesion_level);
                for (sample = 0; sample < sorting_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern

                        bptt_randomise_states(bptt_t1);

                        bptt_clamp_visual(p, bptt_t1);
                        bptt_real_time_settle_for(12); //for 3*4 timesteps
                        bptt_unclamp_time(bptt_t1);
                        bptt_real_time_unclamped_settle();
                        bptt_save_sorting_response_given_visual(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                        bptt_randomise_states(bptt_t1);

                        bptt_clamp_name(p, bptt_t1);
                        bptt_real_time_settle_for(12); //for 3*4 timesteps
                        bptt_unclamp_time(bptt_t1);
                        bptt_real_time_unclamped_settle();
                        bptt_save_sorting_response_given_name(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();
                    }
                }
                if (lesion_type == ZEROING) {
                    bptt_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
                }
                else if (lesion_type == NOISE) {
                    bptt_disrupt_weights(1.45 / (double) naming_lesion_level * level);
                }
            }
            bptt_undo_lesion();
        }
    }
//     else if (net_type == TYLER_RECURRENT) {
//       tyler_unclamp_all();
//
//       tyler_backup_weights();
//
//       for (distribution = 0; distribution < sorting_lesion_distribution; distribution++) { // distribution of lesion
//             for (level = 0; level < sorting_lesion_level; level++) { // level of leasioning
//                 printf("BPTT SORTING Run: %i/%i, Level: %i/%i\n", distribution+1, sorting_lesion_distribution, level+1, sorting_lesion_level);
// 		for (sample = 0; sample < sorting_sample; sample++) { // to ensure network in "right" attractor
//                     for (p = 0; p < P; p++) { // pattern
//
//                         tyler_randomise_states(tyler_t1);
//
// 			tyler_clamp_visual(p, tyler_t1);
// 			tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			tyler_unclamp_time(tyler_t1);
// 			tyler_real_time_unclamped_settle();
// 			tyler_save_sorting_response_given_visual(p, sample, level, distribution);
//
// 			real_time_redraw();
// 			gtkx_flush_events();
//
// 			tyler_randomise_states(tyler_t1);
//
// 			tyler_clamp_distinctive(p, tyler_t1);
// 			tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			tyler_unclamp_time(tyler_t1);
// 			tyler_real_time_unclamped_settle();
// 			tyler_save_sorting_response_given_name(p, sample, level, distribution);
//
// 			real_time_redraw();
// 			gtkx_flush_events();
// 		    }
// 		}
// 		if (lesion_type == ZEROING) {
// 		  tyler_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
// 		}
// 		else if (lesion_type == NOISE) {
// 		  tyler_disrupt_weights(1.45 / (double) naming_lesion_level * level);
// 		}
// 	    }
// 	    tyler_undo_lesion();
//       }
//     }
    else if (net_type == PDPTOOL) {
        pdptool_unclamp();

        pdptool_backup_weights();

        for (distribution = 0; distribution < sorting_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < sorting_lesion_level; level++) { // level of leasioning
                printf("PDPTOOL SORTING Run: %i/%i, Level: %i/%i\n", distribution+1, sorting_lesion_distribution, level+1, sorting_lesion_level);
                for (sample = 0; sample < sorting_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern

                        pdptool_randomise_states();

                        pdptool_clamp_visual(p);
                        pdptool_real_time_settle_for(12); //for 3*4 timesteps
                        pdptool_unclamp();
                        pdptool_real_time_unclamped_settle();
                        pdptool_save_sorting_response_given_visual(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                        pdptool_randomise_states();

                        pdptool_clamp_name(p);
                        pdptool_real_time_settle_for(12); //for 3*4 timesteps
                        pdptool_unclamp();
                        pdptool_real_time_unclamped_settle();
                        pdptool_save_sorting_response_given_name(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();
                    }
                }
                pdptool_lesion_weights(100.0 / (double) sorting_lesion_level); //incrememnetal percentage of weights are zeroed
            }
            pdptool_undo_lesion();
        }
    }
    else if (net_type==BM) {
        bm_unclamp();

        bm_backup_weights();

        for (distribution = 0; distribution < sorting_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < sorting_lesion_level; level++) { // level of leasioning
                printf("BM SORTING Run: %i/%i, Level: %i/%i\n", distribution+1, sorting_lesion_distribution, level+1, sorting_lesion_level);
                for (sample = 0; sample < sorting_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern

                        printf("Run: %i/%i, Level: %i/%i, Sample: %i/%i, Pattern: %i/%i\n", distribution+1, sorting_lesion_distribution, level+1, sorting_lesion_level, sample+1, sorting_sample, p+1, P);


                        bm_randomise_states();

                        bm_clamp_visual(p);
// 				  real_time_settle_for(12); //for 3*4 timesteps
// 				  unclamp_time(bptt_t1);
//                         bm_real_time_semiclamped_anneal();
                        bm_real_time_anneal();

                        bm_save_sorting_response_given_visual(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();

                        bm_randomise_states();

                        bm_clamp_name(p);
// 				  real_time_settle_for(12); //for 3*4 timesteps
// 				  unclamp_time(bptt_t1);
// 				  real_time_unclamped_settle(notebook);
//                         bm_real_time_semiclamped_anneal();
                        bm_real_time_anneal();

                        bm_save_sorting_response_given_name(p, sample, level, distribution);

                        real_time_redraw();
                        gtkx_flush_events();
                    }
                }
                bm_lesion_weights(100.0 / (double) sorting_lesion_level); //incrememnetal percentage of weights are zeroed
            }
            bm_undo_lesion();
        }

    }


    gtk_sorting_graph_update(GTK_SORTING_GRAPH(sorting_graph));
    gtk_sorting_graph_redraw(sorting_graph);
    if (autosave && is_file_open()) {
        save_sorting_responses(sorting_file_name);
    }
    printf("Done!\n");

    ghost_controls(0);
}

void word_to_picture_task(GtkButton *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    ghost_controls(1);
    gtk_word_to_picture_graph_reset(GTK_WORD_TO_PICTURE_GRAPH(word_to_picture_graph));

    int distractor[P_MAX], i, n_distractor = 0, p, q = 0, sample, level, distribution;


    init_word_to_picture_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));


    if (net_type == BPTT) {

        bptt_unclamp_all();

        bptt_backup_weights();

        for (distribution = 0; distribution < word_to_picture_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < word_to_picture_lesion_level; level++) { // level of leasioning
                for (sample = 0; sample < word_to_picture_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++)  {
                        if (!is_general_name(p))  {
                            printf("BPTT WORD/PICTURE Run: %i/%i, Level: %i/%i, Sample: %i/%i, Pattern: %i/%i\n", distribution+1, word_to_picture_lesion_distribution, level+1, word_to_picture_lesion_level, sample+1, word_to_picture_sample, p+1, P);
                            bptt_randomise_states(bptt_t1);


                            // NAME target
                            bptt_clamp_name(p, bptt_t1);
                            bptt_real_time_settle_for(12); //for 3*4 timesteps
                            bptt_unclamp_time(bptt_t1);
                            bptt_real_time_unclamped_settle();
                            bptt_save_target_attractor(p, sample, level, distribution);

                            real_time_redraw();
                            gtkx_flush_events();
                            // 		    print_hidden_states();

                            bptt_randomise_states(bptt_t1);

                            // VISUAL target
                            bptt_clamp_visual(p, bptt_t1);
                            bptt_real_time_settle_for(12); //for 3*4 timesteps
                            bptt_unclamp_time(bptt_t1);
                            bptt_real_time_unclamped_settle();
                            bptt_save_word_to_picture_response_target(p, sample, level, distribution);

                            real_time_redraw();
                            gtkx_flush_events();


                            // CLOSE distractor i.e. both in same category
                            n_distractor = 0;
                            get_close_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {
                                bptt_randomise_states(bptt_t1);

                                q = distractor[i];
                                bptt_clamp_visual(q, bptt_t1);
                                // 				printf("pattern = %s close distractor = %s\n", get_name_string(p), get_name_string(q));
                                bptt_real_time_settle_for(12); //for 3*4 timesteps
                                bptt_unclamp_time(bptt_t1);
                                bptt_real_time_unclamped_settle();
                                // 		      print_hidden_states();

                                bptt_save_word_to_picture_response_close_distractor(p, q, sample, level, distribution);
                                //
                            }

                            real_time_redraw();
                            gtkx_flush_events();

                            // DISTANT distractor i.e. alternate categories in same domain
                            n_distractor = 0;
                            get_distant_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {
                                bptt_randomise_states(bptt_t1);

                                q = distractor[i];

                                // 				printf("pattern = %s distant distractor = %s\n", get_name_string(p), get_name_string(q));

                                bptt_clamp_visual(q, bptt_t1);
                                bptt_real_time_settle_for(12); //for 3*4 timesteps
                                bptt_unclamp_time(bptt_t1);
                                bptt_real_time_unclamped_settle();
                                bptt_save_word_to_picture_response_distant_distractor(p, q, sample, level, distribution);
                            }

                            real_time_redraw();
                            gtkx_flush_events();




                            // UNRELATED distractor i.e. different domain
                            n_distractor = 0;
                            get_unrelated_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {

                                bptt_randomise_states(bptt_t1);

                                q = distractor[i];

                                // 				printf("pattern = %s unrelated distractor = %s\n", get_name_string(p), get_name_string(q));

                                bptt_clamp_visual(q, bptt_t1);
                                bptt_real_time_settle_for(12); //for 3*4 timesteps
                                bptt_unclamp_time(bptt_t1);
                                bptt_real_time_unclamped_settle();
                                bptt_save_word_to_picture_response_unrelated_distractor(p, q, sample, level, distribution);

                            }

                            real_time_redraw();
                            gtkx_flush_events();
                        }

                    } //end of patterns loop

                } // end of sample loop


                if (lesion_type == ZEROING) {
                    bptt_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
                }
                else if (lesion_type == NOISE) {
                    bptt_disrupt_weights(1.45 / (double) naming_lesion_level * level);
                }

            } // end of lesion level loop
            bptt_undo_lesion();
        }   // end of lesion distribution loop
    }
//     else if (net_type == TYLER_RECURRENT) {
//
//       tyler_unclamp_all();
//
//       tyler_backup_weights();
//
//       for (distribution = 0; distribution < word_to_picture_lesion_distribution; distribution++) { // distribution of lesion
//             for (level = 0; level < word_to_picture_lesion_level; level++) { // level of leasioning
//                 for (sample = 0; sample < word_to_picture_sample; sample++) { // to ensure network in "right" attractor
//                     for (p = 0; p < P; p++)  {
// 		      if (!is_general_name(p))  {
// 			printf("BPTT WORD/PICTURE Run: %i/%i, Level: %i/%i, Sample: %i/%i, Pattern: %i/%i\n", distribution+1, word_to_picture_lesion_distribution, level+1, word_to_picture_lesion_level, sample+1, word_to_picture_sample, p+1, P);
// 			tyler_randomise_states(tyler_t1);
//
//
// 			// NAME target
// 			tyler_clamp_name(p, tyler_t1);
// 			tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			tyler_unclamp_time(tyler_t1);
// 			tyler_real_time_unclamped_settle();
// 			tyler_save_target_attractor(p, sample, level, distribution);
//
// 			real_time_redraw();
// 			gtkx_flush_events();
// 			// 		    print_hidden_states();
//
// 			tyler_randomise_states(tyler_t1);
//
// 			// VISUAL target
// 			tyler_clamp_visual(p, tyler_t1);
// 			tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			tyler_unclamp_time(tyler_t1);
// 			tyler_real_time_unclamped_settle();
// 			tyler_save_word_to_picture_response_target(p, sample, level, distribution);
//
// 			real_time_redraw();
// 			gtkx_flush_events();
//
//
// 			// CLOSE distractor i.e. both in same category
// 			n_distractor = 0;
// 			get_close_distractors(p, &distractor[0], &n_distractor);
// 			for (i = 0; i < n_distractor; i++) {
// 			  tyler_randomise_states(tyler_t1);
//
// 			  q = distractor[i];
// 			  tyler_clamp_visual(q, tyler_t1);
// 			  // 				printf("pattern = %s close distractor = %s\n", get_name_string(p), get_name_string(q));
// 			  tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			  tyler_unclamp_time(tyler_t1);
// 			  tyler_real_time_unclamped_settle();
// 			  // 		      print_hidden_states();
//
// 			  tyler_save_word_to_picture_response_close_distractor(p, q, sample, level, distribution);
// 			  //
// 			}
//
// 			real_time_redraw();
// 			gtkx_flush_events();
//
// 			// DISTANT distractor i.e. alternate categories in same domain
// 			n_distractor = 0;
// 			get_distant_distractors(p, &distractor[0], &n_distractor);
// 			for (i = 0; i < n_distractor; i++) {
// 			  tyler_randomise_states(tyler_t1);
//
// 			  q = distractor[i];
//
// 			  // 				printf("pattern = %s distant distractor = %s\n", get_name_string(p), get_name_string(q));
//
// 			  tyler_clamp_visual(q, tyler_t1);
// 			  tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			  tyler_unclamp_time(tyler_t1);
// 			  tyler_real_time_unclamped_settle();
// 			  tyler_save_word_to_picture_response_distant_distractor(p, q, sample, level, distribution);
// 			}
//
// 			real_time_redraw();
// 			gtkx_flush_events();
//
//
//
//
// 			// UNRELATED distractor i.e. different domain
// 			n_distractor = 0;
// 			get_unrelated_distractors(p, &distractor[0], &n_distractor);
// 			for (i = 0; i < n_distractor; i++) {
//
// 			  tyler_randomise_states(tyler_t1);
//
// 			  q = distractor[i];
//
// 			  // 				printf("pattern = %s unrelated distractor = %s\n", get_name_string(p), get_name_string(q));
//
// 			  tyler_clamp_visual(q, tyler_t1);
// 			  tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			  tyler_unclamp_time(tyler_t1);
// 			  tyler_real_time_unclamped_settle();
// 			  tyler_save_word_to_picture_response_unrelated_distractor(p, q, sample, level, distribution);
//
// 			}
//
// 			real_time_redraw();
// 			gtkx_flush_events();
// 		      }
//
// 		    } //end of patterns loop
//
// 		} // end of sample loop
//
//
// 		if (lesion_type == ZEROING) {
// 		  tyler_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
// 		}
// 		else if (lesion_type == NOISE) {
// 		  tyler_disrupt_weights(1.45 / (double) naming_lesion_level * level);
// 		}
//
// 	    } // end of lesion level loop
// 	    tyler_undo_lesion();
//       }   // end of lesion distribution loop
//     }
    else if (net_type == PDPTOOL) {

        pdptool_unclamp();

        pdptool_backup_weights();

        for (distribution = 0; distribution < word_to_picture_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < word_to_picture_lesion_level; level++) { // level of leasioning
                for (sample = 0; sample < word_to_picture_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++)  {

                        if (!is_general_name(p))  {

                            printf("PDPTOOL WORD/PICTURE Run: %i/%i, Level: %i/%i, Pattern: %i/%i\n", distribution+1, word_to_picture_lesion_distribution, level+1, word_to_picture_lesion_level, p+1, P);

                            pdptool_randomise_states();


                            // NAME target
                            pdptool_clamp_name(p);
                            pdptool_real_time_settle_for(12); //for 3*4 timesteps
                            pdptool_unclamp();
                            pdptool_real_time_unclamped_settle();
                            pdptool_save_target_attractor(p, sample, level, distribution);

                            real_time_redraw();
                            gtkx_flush_events();
                            // 		    print_hidden_states();

                            pdptool_randomise_states();

                            // VISUAL target
                            pdptool_clamp_visual(p);
                            pdptool_real_time_settle_for(12); //for 3*4 timesteps
                            pdptool_unclamp();
                            pdptool_real_time_unclamped_settle();
                            pdptool_save_word_to_picture_response_target(p, sample, level, distribution);

                            real_time_redraw();
                            gtkx_flush_events();


                            // CLOSE distractor i.e. both in same category
                            n_distractor = 0;
                            get_close_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {
                                pdptool_randomise_states();

                                q = distractor[i];
                                pdptool_clamp_visual(q);
                                pdptool_real_time_settle_for(12); //for 3*4 timesteps
                                pdptool_unclamp();
                                pdptool_real_time_unclamped_settle();

                                pdptool_save_word_to_picture_response_close_distractor(p, q, sample, level, distribution);

                            }

                            real_time_redraw();
                            gtkx_flush_events();


                            // DISTANT distractor i.e. alternate categories in same domain
                            n_distractor = 0;
                            get_distant_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {
                                pdptool_randomise_states();
                                q = distractor[i];
                                pdptool_clamp_visual(q);
                                pdptool_real_time_settle_for(12); //for 3*4 timesteps
                                pdptool_unclamp();
                                pdptool_real_time_unclamped_settle();
                                pdptool_save_word_to_picture_response_distant_distractor(p, q, sample, level, distribution);
                            }

                            real_time_redraw();
                            gtkx_flush_events();




                            // UNRELATED distractor i.e. different domain
                            n_distractor = 0;
                            get_unrelated_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {
                                pdptool_randomise_states();

                                q = distractor[i];
                                pdptool_clamp_visual(q);
                                pdptool_real_time_settle_for(12); //for 3*4 timesteps
                                pdptool_unclamp();
                                pdptool_real_time_unclamped_settle();
                                pdptool_save_word_to_picture_response_unrelated_distractor(p, q, sample, level, distribution);

                            }

                            real_time_redraw();
                            gtkx_flush_events();
                        }

                    } //end of patterns loop

                } // end of sample loop


                if (lesion_type == ZEROING) {
                    pdptool_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
                }
                else if (lesion_type == NOISE) {
                    pdptool_disrupt_weights(1.45 / (double) naming_lesion_level * level);
                }

            } // end of lesion level loop
            pdptool_undo_lesion();
        }   // end of lesion distribution loop
    }
    else if (net_type == BM) {

        bm_unclamp();

        bm_backup_weights();

        for (distribution = 0; distribution < word_to_picture_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < word_to_picture_lesion_level; level++) { // level of leasioning
                for (sample = 0; sample < word_to_picture_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++)  {
                        if (!is_general_name(p))  {

                            printf("BM WORD/PICTURE Run: %i/%i, Level: %i/%i, Sample: %i/%i, Pattern: %i/%i\n", distribution+1, word_to_picture_lesion_distribution, level+1, word_to_picture_lesion_level, sample+1, word_to_picture_sample, p+1, P);

                            bm_randomise_states();


                            // NAME target
                            bm_clamp_name(p);
                            bm_real_time_semiclamped_anneal();
                            bm_save_target_attractor(p, sample, level, distribution);

                            real_time_redraw();
                            gtkx_flush_events();

                            bm_randomise_states();

                            // VISUAL target
                            bm_clamp_visual(p);
                            bm_real_time_semiclamped_anneal();
                            bm_save_word_to_picture_response_target(p, sample, level, distribution);

                            real_time_redraw();
                            gtkx_flush_events();


                            // CLOSE distractor i.e. both in same category
                            n_distractor = 0;
                            get_close_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {
                                bm_randomise_states();

                                q = distractor[i];
                                bm_clamp_visual(q);
                                bm_real_time_semiclamped_anneal();
                                bm_save_word_to_picture_response_close_distractor(p, q, sample, level, distribution);
                            }

                            real_time_redraw();
                            gtkx_flush_events();



                            // DISTANT distractor i.e. alternate categories in same domain
                            n_distractor = 0;
                            get_distant_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {
                                bm_randomise_states();
                                q = distractor[i];

//                                 printf("pattern = %s distant distractor = %s\n", get_name_string(p), get_name_string(q));

                                bm_clamp_visual(q);
                                bm_real_time_semiclamped_anneal();
                                bm_save_word_to_picture_response_distant_distractor(p, q, sample, level, distribution);
                            }

                            real_time_redraw();
                            gtkx_flush_events();




                            // UNRELATED distractor i.e. different domain
                            n_distractor = 0;
                            get_unrelated_distractors(p, &distractor[0], &n_distractor);
                            for (i = 0; i < n_distractor; i++) {
                                bm_randomise_states();

                                q = distractor[i];

//                                 printf("pattern = %s unrelated distractor = %s\n", get_name_string(p), get_name_string(q));

                                bm_clamp_visual(q);
                                bm_real_time_semiclamped_anneal();
                                bm_save_word_to_picture_response_unrelated_distractor(p, q, sample, level, distribution);



                                real_time_redraw();
                                gtkx_flush_events();
                            }
                        }

                    } //end of patterns loop

                } // end of sample loop


                if (lesion_type == ZEROING) {
                    bm_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
                }
                else if (lesion_type == NOISE) {
                    bm_disrupt_weights(1.45 / (double) naming_lesion_level * level);
                }

            } // end of lesion level loop
            bm_undo_lesion();
        }   // end of lesion distribution loop
    }



    if (autosave && is_file_open()) {
        save_word_to_picture_responses(word_to_picture_file_name);
    }

    printf("Done!\n");
    /*    print_word_to_picture_responses();*/
    gtk_word_to_picture_graph_update(GTK_WORD_TO_PICTURE_GRAPH(word_to_picture_graph));
    gtk_word_to_picture_graph_redraw(word_to_picture_graph);
    ghost_controls(0);
}



void drawing_and_copying_task(GtkButton *button, GtkNotebook *notebook)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif
    ghost_controls(1);
    gtk_drawing_and_copying_graph_reset(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph));


    int p, sample, level, distribution;

    init_drawing_and_copying_response(gtk_spin_button_get_value(GTK_SPIN_BUTTON(distribution_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(level_spin_button)), gtk_spin_button_get_value(GTK_SPIN_BUTTON(sample_spin_button)));


    if (net_type == BPTT) {
        bptt_unclamp_all();

        bptt_backup_weights();

        for (distribution = 0; distribution < drawing_and_copying_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < drawing_and_copying_lesion_level; level++) { // level of leasioning
                printf("BPTT DRAW/COPY Run: %i/%i, Level: %i/%i\n", distribution+1, drawing_and_copying_lesion_distribution, level+1, drawing_and_copying_lesion_level);
                for (sample = 0; sample < drawing_and_copying_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern

                        bptt_randomise_states(bptt_t1);

                        bptt_clamp_name(p, bptt_t1);
                        bptt_real_time_settle_for(12); //for 3*4 timesteps
                        bptt_unclamp_time(bptt_t1);
                        bptt_real_time_unclamped_settle();
                        bptt_save_drawing_response(distribution, level, sample, p);

                        bptt_randomise_states(bptt_t1);

                        bptt_clamp_visual(p, bptt_t1);
                        bptt_real_time_settle_for(12); //for 3*4 timesteps
                        bptt_unclamp_time(bptt_t1);
                        bptt_real_time_unclamped_settle();
                        bptt_save_copying_response(distribution, level, sample, p);

                    }
                }
                if (lesion_type == ZEROING) {
                    bptt_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
                }
                else if (lesion_type == NOISE) {
                    bptt_disrupt_weights(1.45 / (double) naming_lesion_level * level);
                }
            }
            bptt_undo_lesion();
        }
    }
//     else if (net_type == TYLER_RECURRENT) {
//       tyler_unclamp_all();
//
//       tyler_backup_weights();
//
//       for (distribution = 0; distribution < drawing_and_copying_lesion_distribution; distribution++) { // distribution of lesion
//             for (level = 0; level < drawing_and_copying_lesion_level; level++) { // level of leasioning
//                 printf("BPTT DRAW/COPY Run: %i/%i, Level: %i/%i\n", distribution+1, drawing_and_copying_lesion_distribution, level+1, drawing_and_copying_lesion_level);
// 		for (sample = 0; sample < drawing_and_copying_sample; sample++) { // to ensure network in "right" attractor
//                     for (p = 0; p < P; p++) { // pattern
//
//                         tyler_randomise_states(tyler_t1);
//
// 			tyler_clamp_name(p, tyler_t1);
// 			tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			tyler_unclamp_time(tyler_t1);
// 			tyler_real_time_unclamped_settle();
// 			tyler_save_drawing_response(distribution, level, sample, p);
//
// 			tyler_randomise_states(tyler_t1);
//
// 			tyler_clamp_visual(p, tyler_t1);
// 			tyler_real_time_settle_for(12); //for 3*4 timesteps
// 			tyler_unclamp_time(tyler_t1);
// 			tyler_real_time_unclamped_settle();
// 			tyler_save_copying_response(distribution, level, sample, p);
//
// 		    }
// 		}
// 		if (lesion_type == ZEROING) {
// 		  tyler_lesion_weights(100.0 / (double) naming_lesion_level); //incrememnetal percentage of weights are zeroed
// 		}
// 		else if (lesion_type == NOISE) {
// 		  tyler_disrupt_weights(1.45 / (double) naming_lesion_level * level);
// 		}
// 	    }
// 	    tyler_undo_lesion();
//       }
//     }
    else if (net_type == PDPTOOL) {
        pdptool_unclamp();

        pdptool_backup_weights();

        for (distribution = 0; distribution < drawing_and_copying_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < drawing_and_copying_lesion_level; level++) { // level of leasioning
                printf("PDPTOOL DRAW/COPY Run: %i/%i, Level: %i/%i\n", distribution+1, drawing_and_copying_lesion_distribution, level+1, drawing_and_copying_lesion_level);
                for (sample = 0; sample < drawing_and_copying_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern

                        pdptool_randomise_states();

                        pdptool_clamp_name(p);
                        pdptool_real_time_settle_for(12); //for 3*4 timesteps
                        pdptool_unclamp();
                        pdptool_real_time_unclamped_settle();
                        pdptool_save_drawing_response(distribution, level, sample, p);

                        pdptool_randomise_states();

                        pdptool_clamp_visual(p);
                        pdptool_real_time_settle_for(12); //for 3*4 timesteps
                        pdptool_unclamp();
                        pdptool_real_time_unclamped_settle();
                        pdptool_save_copying_response(distribution, level, sample, p);

                    }
                }
                pdptool_lesion_weights(100.0 / (double) drawing_and_copying_lesion_level); //incrememnetal percentage of weights are zeroed
            }
            pdptool_undo_lesion();
        }
    }
    else if (net_type == BM) {

        bm_unclamp();

        bm_backup_weights();

        for (distribution = 0; distribution < drawing_and_copying_lesion_distribution; distribution++) { // distribution of lesion
            for (level = 0; level < drawing_and_copying_lesion_level; level++) { // level of leasioning
                for (sample = 0; sample < drawing_and_copying_sample; sample++) { // to ensure network in "right" attractor
                    for (p = 0; p < P; p++) { // pattern
                        printf("BM DRAW/COPY Run: %i/%i, Level: %i/%i, Sample: %i/%i, Pattern: %i/%i\n", distribution+1, drawing_and_copying_lesion_distribution, level+1, drawing_and_copying_lesion_level, sample+1, drawing_and_copying_sample, p+1, P);

                        bm_randomise_states();

                        bm_clamp_name(p);
// 			  real_time_settle_for(12); //for 3*4 timesteps
// // 			  unclamp_time(bptt_t1);
// 			  real_time_unclamped_settle(notebook);
                        bm_real_time_semiclamped_anneal();
                        bm_save_drawing_response(distribution, level, sample, p);

                        bm_randomise_states();

                        bm_clamp_visual(p);
// 			  real_time_settle_for(12); //for 3*4 timesteps
// 			  unclamp_time(bptt_t1);
// 			  real_time_unclamped_settle(notebook);

                        bm_real_time_semiclamped_anneal();

                        bm_save_copying_response(distribution, level, sample, p);

                    }
                }
                bm_lesion_weights(100.0 / (double) drawing_and_copying_lesion_level); //incrememnetal percentage of weights are zeroed
            }
            bm_undo_lesion();
        }
    }


    gtk_drawing_and_copying_graph_update(GTK_DRAWING_AND_COPYING_GRAPH(drawing_and_copying_graph));
    gtk_drawing_and_copying_graph_redraw(drawing_and_copying_graph);
    //     print_drawing_and_copying_responses();
    if (autosave && is_file_open()) {
        save_drawing_and_copying_responses(drawing_and_copying_file_name);
    }
    printf("Done!\n");
    ghost_controls(0);
}


void send_error_to_error_view(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

    //   if (net_type == BPTT) {
    gtk_error_view_set_errors(error_view, &Pattern_Error[NAME_ERROR][0], &Pattern_Error[VISUAL_ERROR][0], &Pattern_Error[VERBAL_ERROR][0], &Total_Stage_Error[0], &Total_Pattern_Error[0]);


    //   }
    //   else if (net_type == BM) {
    //     gtk_error_view_set_errors(error_view, &BM_Pattern_Error[STAGE_NAME][0], &BM_Pattern_Error[STAGE_VISUAL][0], &BM_Pattern_Error[STAGE_VERBAL][0], &BM_Total_Stage_Error[0], &BM_Total_Pattern_Error[0]);
    //
    //   }
}




void send_error_to_graph(void)
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif

    gtk_graph_set_ys(GTK_GRAPH(graph), &Total_Error[0], &Error[NAME_ERROR][0], &Error[VISUAL_ERROR][0], &Error[VERBAL_ERROR][0]);

    gtk_graph_set_max_iterations(GTK_GRAPH(graph), network_get_iterations_so_far(net_type));


}

void naming_graph_clear(void)
{
    gtk_naming_graph_clear(naming_graph);
    gtkx_flush_events();
}

void save_naming_graph_pdf(void)
{
    // char *naming_graph_name;
    // sprintf(naming_graph_name, "%s_naming", open_filename);
    if (is_file_open()) {

        gtk_naming_graph_pdf(naming_graph, open_name);
    }

}

void save_network_viewer_pdf(void)
{
    // char *naming_graph_name;
    // sprintf(naming_graph_name, "%s_naming", open_filename);
//   if (is_file_open()) {

    gtk_unrolled_network_viewer_pdf(patterns_viewer, open_name);
//   }

}



void sorting_graph_clear(void)
{
    gtk_sorting_graph_clear(sorting_graph);
    gtkx_flush_events();
}

void save_sorting_graph_pdf(void)
{
    // char *sorting_graph_name;
    // sprintf(sorting_graph_name, "%s_sorting", open_filename);
    gtk_sorting_graph_pdf(sorting_graph, open_name);

}
void drawing_and_copying_graph_clear(void)
{
    gtk_drawing_and_copying_graph_clear(drawing_and_copying_graph);
    gtkx_flush_events();
}


void tyler_graph_clear(void)
{
    gtk_tyler_graph_clear(tyler_graph);
    gtkx_flush_events();
}

void aoa_graph_clear(void)
{
    gtk_aoa_graph_clear(aoa_graph);
    gtkx_flush_events();
}

void save_drawing_and_copying_graph_pdf(void)
{
    // char *naming_graph_name;
    // sprintf(naming_graph_name, "%s_naming", open_filename);

    if (is_file_open()) {

        gtk_drawing_and_copying_graph_pdf(drawing_and_copying_graph, open_name);
    }
}


void word_to_picture_graph_clear(void)
{
    gtk_word_to_picture_graph_clear(word_to_picture_graph);
    gtkx_flush_events();
}

void save_word_to_picture_graph_pdf(void)
{
    if (is_file_open()) {

        // char *naming_graph_name;
        // sprintf(naming_graph_name, "%s_naming", open_filename);
        gtk_word_to_picture_graph_pdf(word_to_picture_graph, open_name);
    }

}

void save_tyler_graph_pdf(void)
{

    if (is_file_open()) {

        // char *naming_graph_name;
        // sprintf(naming_graph_name, "%s_naming", open_filename);
        char open_name_ih[128];
        char open_name_hh[128];
        char open_name_ho[128];
        char open_name_networks[128];

        int i;

        sprintf(open_name_ih, "%s_ih", open_name);
        sprintf(open_name_hh, "%s_hh", open_name);
        sprintf(open_name_ho, "%s_ho", open_name);
        sprintf(open_name_networks, "%s_networks", open_name);



        for (i = 0; i < 6; i++) {
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph), i);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_ih), i);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_hh), i);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_ho), i);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_ho), i);
            gtk_tyler_graph_set_type(GTK_TYLER_GRAPH(tyler_graph_networks), i);

            gtk_tyler_graph_pdf(tyler_graph, open_name);
            gtk_tyler_graph_pdf(tyler_graph_ih, open_name_ih);
            if (net_type == TYLER_RECURRENT) {
                gtk_tyler_graph_pdf(tyler_graph_hh, open_name_hh);
            }
            gtk_tyler_graph_pdf(tyler_graph_ho, open_name_ho);
            gtk_tyler_graph_pdf(tyler_graph_networks, open_name_networks);

        }






    }
}

void save_aoa_graph_pdf(void)
{

    if (is_file_open()) {

        // char *naming_graph_name;
        // sprintf(naming_graph_name, "%s_naming", open_filename);
        char open_name_ih[128];
        char open_name_hh[128];
        char open_name_ho[128];
        char open_name_networks[128];

        int i;

        sprintf(open_name_ih, "%s_ih", open_name);
        sprintf(open_name_hh, "%s_hh", open_name);
        sprintf(open_name_ho, "%s_ho", open_name);
        sprintf(open_name_networks, "%s_networks", open_name);


        for (i = 0; i < 6; i++) {
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph), i);
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_ih), i);
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_hh), i);
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_ho), i);
            gtk_aoa_graph_set_type(GTK_AOA_GRAPH(aoa_graph_networks), i);

            gtk_aoa_graph_pdf(aoa_graph, open_name);
            gtk_aoa_graph_pdf(aoa_graph_ih, open_name_ih);
            if (net_type == TYLER_RECURRENT) {
                gtk_aoa_graph_pdf(aoa_graph_hh, open_name_hh);
            }
            gtk_aoa_graph_pdf(aoa_graph_ho, open_name_ho);
            gtk_aoa_graph_pdf(aoa_graph_networks, open_name_networks);

        }






    }
}

/*
void real_time_network_run(int net_type)
{
if (net_type == BM) {

    bm_real_time_clamped_anneal();

}
else if (net_type == BPTT) {

    bptt_real_time_settle();

}
else if (net_type == TYLER_RECURRENT) {

    tyler_real_time_settle_noise();

}
else if (net_type == TYLER_FEEDFORWARD) {

    tyler_feedforward_propagate_noise(noise, random_int(0,3));

}
else if (net_type == PDPTOOL) {

    pdptool_real_time_settle();

}
}                */

// void create_clusters(GtkWidget widget, gpointer *data)
// {
//     int p, s;
//
//     s = 0;
//     clustering_init();
//     for (p = 0; p < P; p++) {
//
//         network_unclamp(net_type);
//         real_time_redraw();
//
//         network_randomise_states(net_type);
//         real_time_redraw();
//
//         network_clamp(net_type, p, s);
//         real_time_redraw();
//
//         real_time_network_run(net_type);
//
//         network_save_current_attractor_for_clustering(net_type);
//         real_time_redraw();
//
//     }
//
//     complete_linkage_clustering();
//
// }


int main(int argc, char *argv[])
{
#if PRINT_FUNC
    printf("%s\n", __FUNCTION__);
#endif



    global_argv = argv;


// 	GtkWidget *unrolled_network;
    GtkWidget *button;
    GtkWidget *combo_box;
//     GtkWidget *check_button;
    GtkWidget *box;
    GtkWidget *pattern_hbox;


    GtkWidget *label;
    GtkWidget *spin_button;
// 	GtkWidget *scale;

    GtkWidget *separator;

    // menu
    GtkWidget *file_menu;
    GtkWidget *view_menu;
    GtkWidget *semantic_tasks_menu;
    GtkWidget *new_item;
    GtkWidget *new_window_item;
    GtkWidget *open_item;
    GtkWidget *save_item;
    GtkWidget *save_as_item;
    GtkWidget *refresh_item;
    GtkWidget *real_time_item;
    GtkWidget *legend_item;
    //     GtkWidget *weights_item;
    GtkWidget *open_patterns_item;

    GtkWidget *quit_item;
    GtkWidget *file_item;
    GtkWidget *view_item;
    GtkWidget *autosave_item;
    GtkWidget *separator_item;
    GtkWidget *semantic_tasks_item;
    GtkWidget *menu_bar;
    GtkWidget *open_recent_item;


    gtk_init(&argc, &argv);



    W = 400;
    H = 300;




    // Initialise Patterns

//     init_patterns(net_type);


    g_object_set(gtk_settings_get_default(), "gtk-auto-mnemonics", FALSE, NULL);




    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    sprintf(window_title, "%s", open_name);
    gtk_window_set_title(GTK_WINDOW(window), window_title);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), W, H);
    gtk_widget_set_app_paintable(window, TRUE);
//     gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
//     gtk_widget_add_events(window, GDK_ENTER_NOTIFY_MASK);

//     g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), NULL);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(delete), NULL);
    gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("icon.png"));

//     table = gtk_table_new(8, 1, FALSE);
//     gtk_table_set_row_spacings(GTK_TABLE(table), 2);
    vertical_box = gtk_vbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vertical_box);

    file_menu = gtk_menu_new();
    view_menu = gtk_menu_new();
    semantic_tasks_menu = gtk_menu_new();

    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);


    new_item = gtk_menu_item_new_with_mnemonic("_New Weights");
    gtk_widget_add_accelerator(new_item, "activate", accel_group, GDK_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_item);
    g_signal_connect(GTK_OBJECT(new_item), "activate", G_CALLBACK(new_weights), GTK_WINDOW(window));
    gtk_widget_show(new_item);

    new_window_item = gtk_menu_item_new_with_mnemonic("New _Window");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_window_item);
    g_signal_connect(GTK_OBJECT(new_window_item), "activate", G_CALLBACK(new_window), GTK_WINDOW(window));
    gtk_widget_show(new_window_item);

    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator_item);
    gtk_widget_show(separator_item);


    open_item = gtk_menu_item_new_with_mnemonic("_Open Weights...");
    gtk_widget_add_accelerator(open_item, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
    g_signal_connect(GTK_OBJECT(open_item), "activate", G_CALLBACK(open_dialog), GTK_WINDOW(window));
    gtk_widget_show(open_item);

    open_recent_item = gtk_menu_item_new_with_mnemonic("Open Recent Weights");
//     gtk_widget_add_accelerator(open_recent_item, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_recent_item);
//     g_signal_connect(GTK_OBJECT(open_recent_item), "activate", G_CALLBACK(open_dialog), GTK_WINDOW(window));
    //     recent_manager = gtk_recent_manager_get_default();
    recent_chooser_menu = gtk_recent_chooser_menu_new();
    update_recent_chooser_filter();

    gtk_recent_chooser_set_show_tips(GTK_RECENT_CHOOSER(recent_chooser_menu), TRUE);
    gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(recent_chooser_menu), GTK_RECENT_SORT_MRU);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(open_recent_item), recent_chooser_menu);
    gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recent_chooser_menu), TRUE);
    g_signal_connect(GTK_OBJECT(recent_chooser_menu), "item-activated", G_CALLBACK(open_recent_item_activated), NULL);
    gtk_widget_show(open_recent_item);



    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator_item);
    gtk_widget_show(separator_item);

    open_patterns_item = gtk_menu_item_new_with_mnemonic("Open _Patterns...");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_patterns_item);
    g_signal_connect(GTK_OBJECT(open_patterns_item), "activate", G_CALLBACK(open_patterns_dialog), NULL);
    gtk_widget_show(open_patterns_item);

    refresh_item = gtk_menu_item_new_with_mnemonic("Patterns to Train with");
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), refresh_item);
    g_signal_connect(GTK_OBJECT(refresh_item), "activate", G_CALLBACK(patterns_to_train_with_dialogue), notebook);
    gtk_widget_show(refresh_item);

    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator_item);
    gtk_widget_show(separator_item);


    save_item = gtk_menu_item_new_with_mnemonic("_Save Weights");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_widget_add_accelerator(save_item, "activate", accel_group, GDK_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect(GTK_OBJECT(save_item), "activate", G_CALLBACK(save), GTK_WINDOW(window));
    gtk_widget_show(save_item);

    save_as_item = gtk_menu_item_new_with_mnemonic("Save Weights _As...");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);
    g_signal_connect(GTK_OBJECT(save_as_item), "activate", G_CALLBACK(save_as_dialog), GTK_WINDOW(window));
    gtk_widget_show(save_as_item);

    autosave_item = gtk_check_menu_item_new_with_mnemonic("A_utosave");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(autosave_item), autosave);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), autosave_item);
    g_signal_connect(GTK_OBJECT(autosave_item), "toggled", G_CALLBACK(set_autosave), NULL);
    gtk_widget_show(autosave_item);

    save_naming_item = gtk_menu_item_new_with_mnemonic("Save Current Network View");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_naming_item);
    g_signal_connect(GTK_OBJECT(save_naming_item), "activate", G_CALLBACK(save_network_viewer_pdf), unrolled_network);
    gtk_widget_show(save_naming_item);

    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator_item);
    gtk_widget_show(separator_item);

    quit_item = gtk_menu_item_new_with_mnemonic("_Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);
    gtk_widget_add_accelerator(quit_item, "activate", accel_group, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect(GTK_OBJECT(quit_item), "activate", G_CALLBACK(open_patterns_dialog), NULL);
    gtk_widget_show(quit_item);

    refresh_item = gtk_menu_item_new_with_mnemonic("_Refresh");
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), refresh_item);
    gtk_widget_add_accelerator(refresh_item, "activate", accel_group, GDK_F5, 0, GTK_ACCEL_VISIBLE);
    g_signal_connect(GTK_OBJECT(refresh_item), "activate", G_CALLBACK(refresh_button), notebook);
    gtk_widget_show(refresh_item);

    real_time_item = gtk_check_menu_item_new_with_mnemonic("Real-_time");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(real_time_item), real_time);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), real_time_item);
    g_signal_connect(GTK_OBJECT(real_time_item), "toggled", G_CALLBACK(set_real_time), NULL);
    gtk_widget_show(real_time_item);

//     weights_item = gtk_menu_item_new_with_mnemonic("_Weight Stats");
//     gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), weights_item);
//     g_signal_connect(GTK_OBJECT(weights_item), "activate", G_CALLBACK(print_weight_stats), NULL);
//     gtk_widget_show(weights_item);

    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), separator_item);
    gtk_widget_show(separator_item);

    patient_data_item = gtk_check_menu_item_new_with_mnemonic("_Patient Data from Rogers et al. (2004)");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(patient_data_item), data_type);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), patient_data_item);
    g_signal_connect(GTK_OBJECT(patient_data_item), "toggled", G_CALLBACK(set_data_type_patient), NULL);
    gtk_widget_show(patient_data_item);

    rogers_data_item = gtk_check_menu_item_new_with_mnemonic("_Model Data from Rogers et al. (2004)");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(rogers_data_item), data_type);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), rogers_data_item);
    g_signal_connect(GTK_OBJECT(rogers_data_item), "toggled", G_CALLBACK(set_data_type_rogers_model), NULL);
    gtk_widget_show(rogers_data_item);

    legend_item = gtk_check_menu_item_new_with_mnemonic("_Legend");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(legend_item), 1);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), legend_item);
    g_signal_connect(GTK_OBJECT(legend_item), "toggled", G_CALLBACK(set_legend), NULL);
    gtk_widget_show(legend_item);

    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), separator_item);
    gtk_widget_show(separator_item);

    refresh_item = gtk_menu_item_new_with_mnemonic("_Weight Statistics");
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), refresh_item);
    g_signal_connect(GTK_OBJECT(refresh_item), "activate", G_CALLBACK(weight_stats_dialogue), notebook);
    gtk_widget_show(refresh_item);




    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), separator_item);
    gtk_widget_show(separator_item);

    refresh_item = gtk_menu_item_new_with_mnemonic("Clear Recently _Used Items");
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), refresh_item);
    g_signal_connect(GTK_OBJECT(refresh_item), "activate", G_CALLBACK(clear_recent_items), notebook);
    gtk_widget_show(refresh_item);

    naming_item = gtk_menu_item_new_with_mnemonic("Run _Confrontation Naming");
    gtk_widget_add_accelerator(naming_item, "activate", accel_group, GDK_1, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), naming_item);
    g_signal_connect(GTK_OBJECT(naming_item), "activate", G_CALLBACK(naming_task), notebook);
    gtk_widget_show(naming_item);

    alt_naming_item = gtk_menu_item_new_with_mnemonic("Run Alternative Confrontation Naming");
//     gtk_widget_add_accelerator(naming_item, "activate", accel_group, GDK_1, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), alt_naming_item);
    g_signal_connect(GTK_OBJECT(alt_naming_item), "activate", G_CALLBACK(naming_task_alt), notebook);
    gtk_widget_show(alt_naming_item);

    clear_naming_item = gtk_menu_item_new_with_mnemonic("C_lear Confrontation Naming");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), clear_naming_item);
    g_signal_connect(GTK_OBJECT(clear_naming_item), "activate", G_CALLBACK(naming_graph_clear), NULL);
    gtk_widget_show(clear_naming_item);

    save_naming_item = gtk_menu_item_new_with_mnemonic("Save Graph for C_onfrontation Naming");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), save_naming_item);
    g_signal_connect(GTK_OBJECT(save_naming_item), "activate", G_CALLBACK(save_naming_graph_pdf), NULL);
    gtk_widget_show(save_naming_item);



    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), separator_item);
    gtk_widget_show(separator_item);

    sorting_item = gtk_menu_item_new_with_mnemonic("Run _Sorting Words and Pictures");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), sorting_item);
    gtk_widget_add_accelerator(sorting_item, "activate", accel_group, GDK_2, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect(GTK_OBJECT(sorting_item), "activate", G_CALLBACK(sorting_task), notebook);
    gtk_widget_show(sorting_item);

    clear_sorting_item = gtk_menu_item_new_with_mnemonic("Cl_ear Sorting Words and Pictures");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), clear_sorting_item);
    g_signal_connect(GTK_OBJECT(clear_sorting_item), "activate", G_CALLBACK(sorting_graph_clear), NULL);
    gtk_widget_show(clear_sorting_item);

    save_sorting_item = gtk_menu_item_new_with_mnemonic("Save Graph for Sorting _Words and Pictures");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), save_sorting_item);
    g_signal_connect(GTK_OBJECT(save_sorting_item), "activate", G_CALLBACK(save_sorting_graph_pdf), NULL);
    gtk_widget_show(save_sorting_item);

    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), separator_item);
    gtk_widget_show(separator_item);

    word_to_picture_item = gtk_menu_item_new_with_mnemonic("Run _Word-to-Picture Matching");
    gtk_widget_add_accelerator(word_to_picture_item, "activate", accel_group, GDK_3, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), word_to_picture_item);
    g_signal_connect(GTK_OBJECT(word_to_picture_item), "activate", G_CALLBACK(word_to_picture_task), notebook);
    gtk_widget_show(word_to_picture_item);

    clear_word_to_picture_item = gtk_menu_item_new_with_mnemonic("Cle_ar Word-to-Picture Matching");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), clear_word_to_picture_item);
    g_signal_connect(GTK_OBJECT(clear_word_to_picture_item), "activate", G_CALLBACK(word_to_picture_graph_clear), NULL);
    gtk_widget_show(clear_word_to_picture_item);

    save_word_to_picture_item = gtk_menu_item_new_with_mnemonic("Save Graph for Word-to-_Picture Matching");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), save_word_to_picture_item);
    g_signal_connect(GTK_OBJECT(save_word_to_picture_item), "activate", G_CALLBACK(save_word_to_picture_graph_pdf), NULL);
    gtk_widget_show(save_word_to_picture_item);

    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), separator_item);
    gtk_widget_show(separator_item);

    drawing_item = gtk_menu_item_new_with_mnemonic("Run _Drawing and Delayed Copying");
    gtk_widget_add_accelerator(drawing_item, "activate", accel_group, GDK_4, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), drawing_item);
    g_signal_connect(GTK_OBJECT(drawing_item), "activate", G_CALLBACK(drawing_and_copying_task), notebook);
    gtk_widget_show(drawing_item);

    clear_drawing_item = gtk_menu_item_new_with_mnemonic("Clea_r Drawing and Delayed Copying");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), clear_drawing_item);
    g_signal_connect(GTK_OBJECT(clear_drawing_item), "activate", G_CALLBACK(drawing_and_copying_graph_clear), NULL);
    gtk_widget_show(clear_drawing_item);

    save_drawing_item = gtk_menu_item_new_with_mnemonic("Save Graph for Drawing and Delayed Cop_ying");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), save_drawing_item);
    g_signal_connect(GTK_OBJECT(save_drawing_item), "activate", G_CALLBACK(save_drawing_and_copying_graph_pdf), NULL);
    gtk_widget_show(save_drawing_item);


    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), separator_item);
    gtk_widget_show(separator_item);

    tyler_item = gtk_menu_item_new_with_mnemonic("Run Tyler-like Task");
    gtk_widget_add_accelerator(tyler_item, "activate", accel_group, GDK_5, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), tyler_item);
    g_signal_connect(GTK_OBJECT(tyler_item), "activate", G_CALLBACK(tyler_task), notebook);
    gtk_widget_show(tyler_item);

    tyler_item = gtk_menu_item_new_with_mnemonic("Run Tyler et al. (2000) Task on 300 Networks");
    gtk_widget_add_accelerator(tyler_item, "activate", accel_group, GDK_5, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), tyler_item);
    g_signal_connect(GTK_OBJECT(tyler_item), "activate", G_CALLBACK(tyler_original_task), notebook);
    gtk_widget_show(tyler_item);

//     tyler_item = gtk_menu_item_new_with_mnemonic("Run Tyler-like Task on 300 Networks");
//     //     gtk_widget_add_accelerator(naming_item, "activate", accel_group, GDK_1, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
//     gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), tyler_item);
//     g_signal_connect(GTK_OBJECT(tyler_item), "activate", G_CALLBACK(tyler_task_networks), notebook);
//     gtk_widget_show(tyler_item);

    clear_tyler_item = gtk_menu_item_new_with_mnemonic("Clea_r Tyler (2000) Task Data");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), clear_tyler_item);
    g_signal_connect(GTK_OBJECT(clear_tyler_item), "activate", G_CALLBACK(tyler_graph_clear), NULL);
    gtk_widget_show(clear_tyler_item);

    save_tyler_item = gtk_menu_item_new_with_mnemonic("Save Graphs for Tyler (2000) Data");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), save_tyler_item);
    g_signal_connect(GTK_OBJECT(save_tyler_item), "activate", G_CALLBACK(save_tyler_graph_pdf), NULL);
    gtk_widget_show(save_tyler_item);


    separator_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), separator_item);
    gtk_widget_show(separator_item);

    aoa_item = gtk_menu_item_new_with_mnemonic("Run AoA Task");
    gtk_widget_add_accelerator(aoa_item, "activate", accel_group, GDK_5, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), aoa_item);
    g_signal_connect(GTK_OBJECT(aoa_item), "activate", G_CALLBACK(aoa_task), notebook);
    gtk_widget_show(aoa_item);

    aoa_item = gtk_menu_item_new_with_mnemonic("Run AoA et al. (2000) Task on 300 Networks");
    gtk_widget_add_accelerator(aoa_item, "activate", accel_group, GDK_5, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), aoa_item);
    g_signal_connect(GTK_OBJECT(aoa_item), "activate", G_CALLBACK(aoa_original_task), notebook);
    gtk_widget_show(aoa_item);

//     aoa_item = gtk_menu_item_new_with_mnemonic("Run AoA-like Task on 300 Networks");
//     //     gtk_widget_add_accelerator(naming_item, "activate", accel_group, GDK_1, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
//     gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), aoa_item);
//     g_signal_connect(GTK_OBJECT(aoa_item), "activate", G_CALLBACK(aoa_task_networks), notebook);
//     gtk_widget_show(aoa_item);

    clear_aoa_item = gtk_menu_item_new_with_mnemonic("Clea_r AoA (2000) Task Data");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), clear_aoa_item);
    g_signal_connect(GTK_OBJECT(clear_aoa_item), "activate", G_CALLBACK(aoa_graph_clear), NULL);
    gtk_widget_show(clear_aoa_item);

    save_aoa_item = gtk_menu_item_new_with_mnemonic("Save Graphs for AoA (2000) Data");
    gtk_menu_shell_append(GTK_MENU_SHELL(semantic_tasks_menu), save_aoa_item);
    g_signal_connect(GTK_OBJECT(save_aoa_item), "activate", G_CALLBACK(save_aoa_graph_pdf), NULL);
    gtk_widget_show(save_aoa_item);

    menu_bar = gtk_menu_bar_new();

    file_item = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), file_item);
    gtk_widget_show(file_item);

    view_item = gtk_menu_item_new_with_mnemonic("_View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), view_item);
    gtk_widget_show(view_item);


    semantic_tasks_item = gtk_menu_item_new_with_mnemonic("Semantic _Tasks");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(semantic_tasks_item), semantic_tasks_menu);
    gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), semantic_tasks_item);
    gtk_widget_show(semantic_tasks_item);

    gtk_box_pack_start(GTK_BOX(vertical_box), menu_bar, 0, 0, 0);
    gtk_widget_show(menu_bar);



    box = gtk_hbox_new(FALSE, spacing);
    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    gtk_box_pack_start(GTK_BOX(box), notebook, 1, 1, padding);
    gtk_box_pack_start(GTK_BOX(vertical_box), box, 1, 1, 0);
    gtk_widget_show(notebook);
    gtk_widget_show(box);
//                gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);


    unrolled_network = gtk_unrolled_network_viewer_new();
    gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), ROLLED_STATES);
    gtk_widget_show(GTK_WIDGET(unrolled_network));
    label = gtk_label_new("States");





    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(unrolled_network), label);
    unrolled_network_viewer_network_type_set(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), &net_type);
    unrolled_network_viewer_error_type_set(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), &error_type);
//     gtk_widget_add_events(unrolled_network, GDK_BUTTON_PRESS_MASK);
//     g_signal_connect(G_OBJECT(unrolled_network), "button-press-event", G_CALLBACK(clicked), NULL);


    graph = gtk_graph_new();
    gtk_graph_set_max_iterations(GTK_GRAPH(graph), 0);
    send_error_to_graph();
    gtk_widget_show(GTK_WIDGET(graph));
    label = gtk_label_new("Graph");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(graph), label);

    error_view = gtk_error_view_new();
    gtk_widget_show(GTK_WIDGET(error_view));
    send_error_to_error_view();
    label = gtk_label_new("Error");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(error_view), label);




// 	    gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network));
// 	    gtk_widget_show(GTK_WIDGET(unrolled_network));
// 	    label = gtk_label_new("Unrolled Network States");
// 	    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(unrolled_network), label);

    xbm_p = gtk_xbm_new();
    set_xbm_p_type(GTK_COMBO_BOX(xbm_p_combo_box), GTK_NOTEBOOK(notebook));
    gtk_widget_hide(xbm_p);

    if (net_type == BM) {
        gtk_widget_show(xbm_p);
        //gtk_xbm_redraw(GTK_XBM(widget));
    }
    label = gtk_label_new ("Probabilities");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(xbm_p), label);


// 	xbm_p_visual = gtk_xbm_new();
// 	gtk_xbm_prob(GTK_XBM(xbm_p_visual), VISIBLE, VISUAL);
// 	gtk_widget_show (GTK_WIDGET(xbm_p_visual));
// 	gtk_widget_set_name(xbm_p_visual, "p-visual");
// 	label = gtk_label_new ("P Visual");
// 	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(xbm_p_visual), label);
//
// 	xbm_p_verbal = gtk_xbm_new();
// 	gtk_xbm_prob(GTK_XBM(xbm_p_verbal), VISIBLE, VERBAL);
// 	gtk_widget_show (GTK_WIDGET(xbm_p_verbal));
// 	gtk_widget_set_name(xbm_p_verbal, "p-verbal");
// 	label = gtk_label_new ("P Verbal");
// 	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(xbm_p_verbal), label);

    patterns_viewer = gtk_unrolled_network_viewer_new();
    gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(patterns_viewer), PATTERNS);
    gtk_widget_show(GTK_WIDGET(patterns_viewer));
    label = gtk_label_new("Patterns");





    GtkWidget *fixed = gtk_fixed_new();


    GtkWidget *check_button[P_MAX*5];

    int p, y, x, i;

//     height = 800;
//     gtk_window_get_size (GTK_WINDOW(window), width, height);

//     printf
    //                g_signal_connect(G_OBJECT(check_button), "toggled", G_CALLBACK(set_pattern), NULL);
    pattern_hbox = gtk_hbox_new(FALSE, spacing);


//     x_space = (width - X_MARGIN * 2.0 - unit_width) / (double) (input_units);
//     x = X_MARGIN;
    GtkWidget *column_label[5];
    y = patterns_viewer->allocation.height ;
    char lbl[5][16]     = {"Early", "Familiar", "Frequent", "Train", "Test"};
    for (i = 0; i < 5; i++) {
        x = column_width * (i + 0.15);
        printf("x = %d\n", x);
        column_label[i] = gtk_label_new (lbl[i]);
        gtk_fixed_put(GTK_FIXED(fixed), column_label[i], x, y);
        gtk_widget_set_size_request(column_label[i], column_width, 50);
//       g_signal_connect(G_OBJECT(check_button[p*5+i]), "toggled", G_CALLBACK(set_pattern), NULL);
        gtk_widget_show(column_label[i]);
    }
    int check_button_coord[5*P_MAX+5][2] ;
    for (p = 0; p < P_MAX; p++) {
        y = Y_MARGIN + (patterns_viewer->allocation.height - 25 - Y_MARGIN - Y_MARGIN) * ((p+1) / (double) P_MAX) - 2;

        // because we want all of them to be put in order in the fixed

        for (i = 0; i < 5; i++) {
            x = column_width * (i + 0.5);
            check_button_coord[p*5+i][0] = p;
            check_button_coord[p*5+i][1] = i;
// 	  printf("x = %d\n", x);
            check_button[p*5+i] = gtk_check_button_new();
            gtk_fixed_put(GTK_FIXED(fixed), check_button[p*5+i], x, y);
            gtk_widget_set_size_request(check_button[p*5+i], 17, 13);
            g_signal_connect(G_OBJECT(check_button[p*5+i]), "toggled", G_CALLBACK(set_pattern_check_button), &check_button_coord[p*5+i][0]);
            gtk_widget_hide(check_button[p*5+i]);
        }

    }
    P = 16;
    for (p = 0; p < P; p++) {
//       y = Y_MARGIN + (patterns_viewer->allocation.height - 25 - Y_MARGIN - Y_MARGIN) * ((P - p) / (double) P);
//
//       gtk_fixed_put(GTK_FIXED(fixed), check_button[p], x, y);
//       gtk_widget_set_size_request(check_button[p], 17, 14);
//       g_signal_connect(G_OBJECT(check_button[p]), "toggled", G_CALLBACK(set_pattern), NULL);
        for (i = 0; i < 5; i++) {

            gtk_widget_show(check_button[p*5+i]);
        }

    }
    gtk_box_pack_start(GTK_BOX(pattern_hbox), GTK_WIDGET(fixed), expand,  fill, padding);
    gtk_widget_show(fixed);
    gtk_box_pack_start(GTK_BOX(pattern_hbox), GTK_WIDGET(patterns_viewer), 1,  1, padding);

    gtk_widget_show(GTK_WIDGET(patterns_viewer));
    gtk_widget_show(GTK_WIDGET(pattern_hbox));


    check_button_struct *data = malloc(sizeof *data);
    data->fixed = fixed;
    data->check_button = &check_button[0]; /* etc. */

    g_signal_connect(patterns_viewer, "size-allocate", G_CALLBACK(resize_check_buttons), data);
    pattern_check_button_init(&check_button[0]);

//     free(data);  must not free as is used continuously

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(pattern_hbox), label);

//     gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(patterns_viewer), label);
    unrolled_network_viewer_network_type_set(GTK_UNROLLED_NETWORK_VIEWER(patterns_viewer), &net_type);
    unrolled_network_viewer_error_type_set(GTK_UNROLLED_NETWORK_VIEWER(patterns_viewer), &error_type);


// 	unrolled_network = gtk_unrolled_network_viewer_new();
// 	gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), DELTAS);
// 	gtk_widget_show(GTK_WIDGET(unrolled_network));
// 	label = gtk_label_new("Unrolled Network Deltas");
// 	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(unrolled_network), label, UNROLLED_DELTAS);
//
// 	unrolled_network = gtk_unrolled_network_viewer_new();
// 	gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), ERRORS);
// 	gtk_widget_show(GTK_WIDGET(unrolled_network));
// 	label = gtk_label_new("Unrolled Network Errors");
// 	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(unrolled_network), label, UNROLLED_ERRORS);


//     unrolled_network = gtk_unrolled_network_viewer_new();
//     gtk_unrolled_network_viewer_type_of_view(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), UNROLLED_STATES);
//     gtk_widget_show(GTK_WIDGET(unrolled_network));
//     label = gtk_label_new("States");


//                gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);

    /*
        gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(unrolled_network), label);
        unrolled_network_viewer_network_type_set(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), &net_type);
        unrolled_network_viewer_error_type_set(GTK_UNROLLED_NETWORK_VIEWER(unrolled_network), &error_type);*/


    naming_graph = gtk_naming_graph_new();
    gtk_widget_show(GTK_WIDGET(naming_graph));
    label = gtk_label_new("Confrontation Naming");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(naming_graph), label);

    sorting_graph = gtk_sorting_graph_new();
    gtk_widget_show(GTK_WIDGET(sorting_graph));
    label = gtk_label_new("Sorting Words and Pictures");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(sorting_graph), label);

    word_to_picture_graph = gtk_word_to_picture_graph_new();
    gtk_widget_show(GTK_WIDGET(word_to_picture_graph));
    label = gtk_label_new("Word-to-Picture Matching");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(word_to_picture_graph), label);

    drawing_and_copying_graph = gtk_drawing_and_copying_graph_new();
    gtk_widget_show(GTK_WIDGET(drawing_and_copying_graph));
    label = gtk_label_new("Drawing and Delayed Copying");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(drawing_and_copying_graph), label);

    tyler_graph = gtk_tyler_graph_new();
    gtk_widget_show(GTK_WIDGET(tyler_graph));
    label = gtk_label_new("Tyler (2000) Graphs Global Lesion");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(tyler_graph), label);

    tyler_graph_ih = gtk_tyler_graph_new();
    gtk_widget_show(GTK_WIDGET(tyler_graph_ih));
    label = gtk_label_new("Tyler I-H");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(tyler_graph_ih), label);

    tyler_graph_ho = gtk_tyler_graph_new();
    gtk_widget_show(GTK_WIDGET(tyler_graph_ho));
    label = gtk_label_new("Tyler H-O");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(tyler_graph_ho), label);

    tyler_graph_hh = gtk_tyler_graph_new();
    gtk_widget_show(GTK_WIDGET(tyler_graph_hh));
    label = gtk_label_new("Tyler H-H");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(tyler_graph_hh), label);


    tyler_graph_networks = gtk_tyler_graph_new();
    gtk_widget_show(GTK_WIDGET(tyler_graph_networks));
    label = gtk_label_new("Tyler Networks");
    gtk_tyler_graph_networks(GTK_TYLER_GRAPH(tyler_graph_networks), TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(tyler_graph_networks), label);


    aoa_graph = gtk_aoa_graph_new();
    gtk_widget_show(GTK_WIDGET(aoa_graph));
    label = gtk_label_new("AoA Global");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(aoa_graph), label);

    aoa_graph_ih = gtk_aoa_graph_new();
    gtk_widget_show(GTK_WIDGET(aoa_graph_ih));
    label = gtk_label_new("AoA I-H");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(aoa_graph_ih), label);

    aoa_graph_ho = gtk_aoa_graph_new();
    gtk_widget_show(GTK_WIDGET(aoa_graph_ho));
    label = gtk_label_new("AoA H-O");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(aoa_graph_ho), label);

    aoa_graph_hh = gtk_aoa_graph_new();
    gtk_widget_show(GTK_WIDGET(aoa_graph_hh));
    label = gtk_label_new("AoA H-H");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(aoa_graph_hh), label);


    aoa_graph_networks = gtk_aoa_graph_new();
    gtk_widget_show(GTK_WIDGET(aoa_graph_networks));
    label = gtk_label_new("AoA Networks");
    gtk_aoa_graph_networks(GTK_AOA_GRAPH(aoa_graph_networks), TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(aoa_graph_networks), label);


    attractor_view = gtk_attractor_view_new();
    gtk_widget_show(GTK_WIDGET(attractor_view));
    label = gtk_label_new("Attractors");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), GTK_WIDGET(attractor_view), label);



    g_signal_connect(G_OBJECT(notebook), "switch-page", G_CALLBACK(notebook_redraw_current_page), NULL);





// 	button = gtk_button_new_with_label ("Init Weights");
//  	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(new_weights), notebook);
// 	gtk_table_attach(GTK_TABLE(table), button, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 2.5, 2.5);
// 	gtk_widget_show(button);
//
// 	button = gtk_button_new_with_label ("Init States");
//  	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(initialise_states), notebook);
// 	gtk_table_attach(GTK_TABLE(table), button, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 2.5, 2.5);
// 	gtk_widget_show(button);


    // General Controls


    box = gtk_hbox_new(FALSE, spacing);
    gtk_box_pack_start(GTK_BOX(vertical_box), box, expand, fill, vertical_padding);



    label = gtk_label_new_with_mnemonic("_Network Type:");
    gtk_box_pack_start(GTK_BOX(box), label, expand,  fill, padding);
    gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);

    gtk_widget_show(label);

    net_type_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(net_type_combo_box), "Select a network architecture.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(net_type_combo_box));
    gtk_combo_box_insert_text(GTK_COMBO_BOX(net_type_combo_box), BM, "BM");
    gtk_combo_box_insert_text(GTK_COMBO_BOX(net_type_combo_box), BPTT, "BPTT");
    gtk_combo_box_insert_text(GTK_COMBO_BOX(net_type_combo_box), PDPTOOL, "PDPTOOL");
    gtk_combo_box_insert_text(GTK_COMBO_BOX(net_type_combo_box), TYLER_RECURRENT, "TYLER Recurrent");
    gtk_combo_box_insert_text(GTK_COMBO_BOX(net_type_combo_box), TYLER_FEEDFORWARD, "TYLER Feedforward");
    g_signal_connect(G_OBJECT(net_type_combo_box), "changed", G_CALLBACK(set_net_type), NULL);
    gtk_box_pack_start(GTK_BOX(box), net_type_combo_box, expand, fill, padding);



    gtk_combo_box_set_active(GTK_COMBO_BOX(net_type_combo_box), net_type);
    gtk_widget_show(net_type_combo_box);


    label = gtk_label_new_with_mnemonic("Patt_erns:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.1, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    pattern_combo_box = gtk_combo_box_text_new();
    gtk_widget_set_tooltip_text(GTK_WIDGET(pattern_combo_box), "Select a training/testing set from presets.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(pattern_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "Base Names");
    gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "Category Names");
    gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "Category Names 2");

    gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "6 Patterns");
    gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "PDPTOOL patterns");

    gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "Original Rogers");

    gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "Tyler (2000)");
    gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "Tyler-like Orthogonal");
//     gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "Tyler-like 2");
//     gtk_combo_box_append_text(GTK_COMBO_BOX(pattern_combo_box), "Tyler-like 3");


    g_signal_connect(G_OBJECT(pattern_combo_box), "changed", G_CALLBACK(set_pattern_file), notebook);
    gtk_box_pack_start(GTK_BOX(box), pattern_combo_box, expand, fill, padding);
    if ((net_type == TYLER_RECURRENT) || (net_type == TYLER_FEEDFORWARD)) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(pattern_combo_box), 6);
//         widget_set_sensitive(pattern_combo_box, 0);

    }
    else {
        gtk_combo_box_set_active(GTK_COMBO_BOX(pattern_combo_box), 1);
        widget_set_sensitive(pattern_combo_box, 1);

    }
    widget_set_sensitive(pattern_combo_box, 1);

    reset_pattern_combo_box();
    set_pattern_file(GTK_COMBO_BOX(pattern_combo_box), GTK_NOTEBOOK(notebook));
    gtk_widget_show(pattern_combo_box);


    separator = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX(box), separator, expand, fill, padding);
    gtk_widget_show(separator);
    // Controls for Lesioning


    label = gtk_label_new("Lesioning:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    label = gtk_label_new_with_mnemonic("Distrib_utions:");
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);


    distribution_spin_button = gtk_spin_button_new_with_range(1, DRAWING_AND_COPYING_LESION_DISTRIBUTION, 1);
    gtk_widget_set_tooltip_text(GTK_WIDGET(distribution_spin_button), "Enter value for number of times the network is lesioned: starting from un-lesioned weights up to the number of lesioning levels.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(distribution_spin_button));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(distribution_spin_button), 1);
    g_signal_connect(G_OBJECT(distribution_spin_button), "value-changed", G_CALLBACK(set_lesion_distribution), NULL);
    gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(distribution_spin_button), TRUE);

    gtk_box_pack_start(GTK_BOX(box), distribution_spin_button, expand, fill, padding);

    gtk_widget_show(distribution_spin_button);

    label = gtk_label_new_with_mnemonic("Le_vels:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    level_spin_button = gtk_spin_button_new_with_range(1, DRAWING_AND_COPYING_LESION_LEVEL, 1);
    gtk_widget_set_tooltip_text(GTK_WIDGET(level_spin_button), "Enter value for number of times network is lesioned within each distribution, e.g., a 2 will produce a value at 0% and at 50% of weights zeroed.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(level_spin_button));

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(level_spin_button), 5);
    g_signal_connect(G_OBJECT(level_spin_button), "value-changed", G_CALLBACK(set_lesion_level), NULL);
    gtk_box_pack_start(GTK_BOX(box), level_spin_button, expand, fill, padding);
    gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(level_spin_button), TRUE);

    gtk_widget_show(level_spin_button);

    label = gtk_label_new_with_mnemonic("Sa_mples:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    sample_spin_button = gtk_spin_button_new_with_range(1, DRAWING_AND_COPYING_SAMPLE, 1);

    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(sample_spin_button));
    gtk_widget_set_tooltip_text(GTK_WIDGET(sample_spin_button), "Enter value for number of times network is sampled per sub-pattern, this is to avoid overestimating error.");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(sample_spin_button), 3);
    g_signal_connect(G_OBJECT(sample_spin_button), "value-changed", G_CALLBACK(set_sample), NULL);
    gtk_box_pack_start(GTK_BOX(box), sample_spin_button, expand, fill, padding);
    gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(sample_spin_button), TRUE);

    gtk_widget_show(sample_spin_button);

    set_task_spin_buttons();


    combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(combo_box), "Select a method of lesioning.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "Zeroing");
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "Noise");
    g_signal_connect(G_OBJECT(combo_box), "changed", G_CALLBACK(set_lesion_type), NULL);
    gtk_box_pack_start(GTK_BOX(box), combo_box, expand, fill, padding);

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), lesion_type);
    gtk_widget_show(combo_box);

    lesion_locus_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(lesion_locus_combo_box), "Select a locus of lesioning.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(lesion_locus_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(lesion_locus_combo_box), "Global");
    gtk_combo_box_append_text(GTK_COMBO_BOX(lesion_locus_combo_box), "In-Hidden");
    gtk_combo_box_append_text(GTK_COMBO_BOX(lesion_locus_combo_box), "Hidden-Out");
    gtk_combo_box_append_text(GTK_COMBO_BOX(lesion_locus_combo_box), "Hidden-Hidden");
    gtk_combo_box_append_text(GTK_COMBO_BOX(lesion_locus_combo_box), "All");
    g_signal_connect(G_OBJECT(lesion_locus_combo_box), "changed", G_CALLBACK(set_lesion_locus), NULL);
    gtk_box_pack_start(GTK_BOX(box), lesion_locus_combo_box, expand, fill, padding);
    gtk_combo_box_set_active(GTK_COMBO_BOX(lesion_locus_combo_box), lesion_locus);
    gtk_widget_show(lesion_locus_combo_box);

    separator = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX(box), separator, expand, fill, padding);
    gtk_widget_show(separator);


    // Controls for GUI
//     gtk_box_pack_start(GTK_BOX(vertical_box), box, expand, fill, vertical_padding);

    label = gtk_label_new("GUI:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.1, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    button = gtk_button_new_with_mnemonic("_Refresh");
    gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Update GUI.");
    gtk_widget_add_accelerator(button, "activate", accel_group, GDK_F5, 0, GTK_ACCEL_VISIBLE);
    g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(refresh_button), notebook);
    gtk_widget_set_size_request(GTK_WIDGET(button), -1, 30);
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);

    gtk_widget_show(button);

//     check_button = gtk_check_button_new_with_mnemonic("Real-t_ime");
//     gtk_widget_set_tooltip_text(GTK_WIDGET(check_button), "Toggle GUI updates.");
//     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), real_time);
//     g_signal_connect(G_OBJECT(check_button), "toggled", G_CALLBACK(set_real_time), NULL);
//     gtk_box_pack_start(GTK_BOX(box), check_button, expand, fill, padding);
//
//     gtk_widget_show(check_button);
    gtk_widget_show(box);




// Controls for Training


    box = gtk_hbox_new(FALSE, spacing);

    gtk_box_pack_start(GTK_BOX(vertical_box), box, expand, fill, vertical_padding);




    label = gtk_label_new("Training:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);
    gtk_widget_show(label);

//     int button_height, button_width;
    train_button = gtk_button_new_with_mnemonic("_Start");
    gtk_widget_set_tooltip_text(GTK_WIDGET(train_button), "Toggle training the network.");

    g_signal_connect(G_OBJECT(train_button), "clicked", G_CALLBACK(train_patterns), notebook);
    gtk_box_pack_start(GTK_BOX(box), train_button, expand, fill, padding);
//     GtkRequisition *requisition;
//     gtk_widget_size_request(GTK_WIDGET(button), requisition);
//
//     button_height = requisition->height;
//     button_width = requisition->width;
//     printf("%d %d\n", button_height, button_width);

    gtk_widget_show(train_button);

    label = gtk_label_new_with_mnemonic("_Algorithm:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);
    gtk_widget_hide(label);


    training_algo_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(training_algo_combo_box), "Select a training algorithm.");

    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(training_algo_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "Williams");
    gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "Williams' with TA");
    gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "McClelland");
    gtk_combo_box_append_text(GTK_COMBO_BOX(training_algo_combo_box), "~McClelland");
    g_signal_connect(G_OBJECT(training_algo_combo_box), "changed", G_CALLBACK(set_training_type), NULL);
    gtk_box_pack_start(GTK_BOX(box), training_algo_combo_box, expand, fill, padding);


    gtk_combo_box_set_active(GTK_COMBO_BOX(training_algo_combo_box), 3);
    gtk_widget_hide(training_algo_combo_box);


    label = gtk_label_new_with_mnemonic("_Learning Rate:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    learning_rate_spin_button = gtk_spin_button_new_with_range(0.0001, 1.0, 0.0001);
    gtk_widget_set_tooltip_text(GTK_WIDGET(learning_rate_spin_button), "Enter value for rate of learning during training, some networks allow for a decay of this value within a pre-specified range.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(learning_rate_spin_button));

    g_signal_connect(G_OBJECT(learning_rate_spin_button), "value-changed", G_CALLBACK(set_learning_rate), NULL);
    gtk_box_pack_start(GTK_BOX(box), learning_rate_spin_button, expand, fill, padding);
    gtk_widget_show(learning_rate_spin_button);
    update_learning_rate_spin_button();


    label = gtk_label_new("Momentum:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    momentum_spin_button = gtk_spin_button_new_with_range(0.0, 1.0, 0.0001);
    gtk_widget_set_tooltip_text(GTK_WIDGET(momentum_spin_button), "Enter value for weight momentum during training.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(momentum_spin_button));


    g_signal_connect(G_OBJECT(momentum_spin_button), "value-changed", G_CALLBACK(set_momentum), NULL);
    gtk_box_pack_start(GTK_BOX(box), momentum_spin_button, expand, fill, padding);


    gtk_widget_show(momentum_spin_button);
    update_momentum_spin_button();




    label = gtk_label_new_with_mnemonic("_Epochs:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.1, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);


    gtk_widget_show(label);

    iterations_spin_button = gtk_spin_button_new_with_range(0, BM_I_MAX, 1000);
    gtk_widget_set_tooltip_text(GTK_WIDGET(iterations_spin_button), "Enter value for number of training iterations, i.e., number of times the weights will have been updated.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(iterations_spin_button));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(iterations_spin_button), network_get_default_iterations(net_type));
    g_signal_connect(G_OBJECT(iterations_spin_button), "value-changed", G_CALLBACK(set_iterations), NULL);
    gtk_box_pack_start(GTK_BOX(box), iterations_spin_button, expand, fill, padding);

    gtk_widget_show(iterations_spin_button);

    t_clamped_label = gtk_label_new_with_mnemonic("Timesteps Clamped:");
    gtk_misc_set_alignment(GTK_MISC(t_clamped_label), 0.1, 0.5);
    gtk_box_pack_start(GTK_BOX(box), t_clamped_label, expand, fill, padding);


    gtk_widget_show(t_clamped_label);

    t_clamped_spin_button = gtk_spin_button_new_with_range(BPTT_T_CLAMPED_ROGERS, BPTT_T_CLAMPED_MAX, 1);
    gtk_widget_set_tooltip_text(GTK_WIDGET(t_clamped_spin_button), "Enter value for how many unrolled states should be clamped out of a maximum of 28.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(t_clamped_spin_button));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(t_clamped_spin_button), BPTT_T_CLAMPED);
    g_signal_connect(G_OBJECT(t_clamped_spin_button), "value-changed", G_CALLBACK(set_clamped_states), NULL);
    gtk_box_pack_start(GTK_BOX(box), t_clamped_spin_button, expand, fill, padding);
    set_clamped_states(GTK_SPIN_BUTTON(t_clamped_spin_button), NULL);


    gtk_widget_show(t_clamped_spin_button);

    gtk_widget_show(box);

    label = gtk_label_new_with_mnemonic("Weight Adjustments:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.1, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);
    gtk_widget_show(label);

    combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(combo_box), "Select how often weights should be updated.");

    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "Sub-pattern-wise");
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "Pattern-wise");
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "Batch");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), BPTT_WEIGHT_UPDATE_BATCH);
    g_signal_connect(G_OBJECT(combo_box), "changed", G_CALLBACK(set_weight_update_type), NULL);
    gtk_box_pack_start(GTK_BOX(box), combo_box, expand, fill, padding);
    gtk_widget_show(combo_box);


    /*
    label = gtk_label_new_with_mnemonic("Decelerate Learning Rate:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.1, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);
    gtk_widget_show(label);
    combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(combo_box), "Select by what factor the learning rate should decrease.");


    char decay[10];

    sprintf(decay, "%f", BM_R_RATE);           */




//     gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(combo_box));
//     gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), "1.0");
//     gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box), decay);
//     gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);
//     g_signal_connect(G_OBJECT(combo_box), "changed", G_CALLBACK(set_weight_decay), NULL);
//     gtk_box_pack_start(GTK_BOX(box), combo_box, expand, fill, padding);
//     gtk_widget_show(combo_box);


//      label = gtk_label_new("AoA:");
//      gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
//      gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);
//      gtk_widget_show(label);


    aoa_combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(aoa_combo_box), "AoA");
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(aoa_combo_box), "Just Test AoA");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(aoa_combo_box), "No AoA");
    gtk_box_pack_start(GTK_BOX(box), aoa_combo_box, expand, fill, padding);
    gtk_combo_box_set_active(GTK_COMBO_BOX(aoa_combo_box), 1);
    g_signal_connect(G_OBJECT(aoa_combo_box), "changed", G_CALLBACK(set_aoa), NULL);

    gtk_widget_show(aoa_combo_box);






    label = gtk_label_new("Gaussian Noise:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

//     noise_spin_button = gtk_spin_button_new_with_range(0.0, 1.0, 0.01);
//     gtk_widget_set_tooltip_text(GTK_WIDGET(noise_spin_button), "Enter value for pattern noise during training/testing.");
//     gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(noise_spin_button));
//     g_signal_connect(G_OBJECT(noise_spin_button), "value-changed", G_CALLBACK(set_noise), NULL);
//     gtk_box_pack_start(GTK_BOX(box), noise_spin_button, expand, fill, padding);
//     gtk_spin_button_set_value(GTK_SPIN_BUTTON(noise_spin_button), noise);

    gtk_widget_show(noise_spin_button);
    gtk_widget_show(box);

    // Controls for Testing


    box = gtk_hbox_new(FALSE, spacing);
    gtk_box_pack_start(GTK_BOX(vertical_box), box, expand, fill, vertical_padding);




    label = gtk_label_new("Input:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    button = gtk_button_new_with_mnemonic("Ran_domise");
    gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Reset network states to randomly generated numbers.");
    g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(reset_states_button), notebook);
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);

    gtk_widget_show(button);

    label = gtk_label_new_with_mnemonic("_Pattern:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    pattern_spin_button = gtk_spin_button_new_with_range(p_min, p_max-1, 1);
    gtk_widget_set_tooltip_text(GTK_WIDGET(pattern_spin_button), "Enter number representing a pattern to be clamped to input units.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(pattern_spin_button));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pattern_spin_button), 0);
    g_signal_connect(G_OBJECT(pattern_spin_button), "value-changed", G_CALLBACK(set_pattern), NULL);
    gtk_box_pack_start(GTK_BOX(box), pattern_spin_button, expand, fill, padding);

    gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(pattern_spin_button), TRUE);
    gtk_widget_show(pattern_spin_button);

    label = gtk_label_new_with_mnemonic("Sta_ge:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    stage_spin_button = gtk_spin_button_new_with_range(0, 2, 1);
    gtk_widget_set_tooltip_text(GTK_WIDGET(stage_spin_button), "Enter number representing a sub-pattern, i.e., a \"modality\" (name, verbal, or visual), to be clamped to input units.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(stage_spin_button));

    reset_stage_spin_button();
    g_signal_connect(G_OBJECT(stage_spin_button), "value-changed", G_CALLBACK(set_stage), NULL);
    gtk_box_pack_start(GTK_BOX(box), stage_spin_button, expand, fill, padding);

    gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(stage_spin_button), TRUE);
    gtk_widget_show(stage_spin_button);

    label = gtk_label_new("Settle:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);

    button = gtk_button_new_with_label (">");
    gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Update all states once, passing activations from input to hidden, and from hidden to output units.");
    g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(settle_step_button), notebook);
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);

    gtk_widget_show(button);

    button = gtk_button_new_with_mnemonic ("_>>");
    gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Propagate all activations until settling condition is met.");
    g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(settle_button), notebook);
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);

    gtk_widget_show(button);

    label = gtk_label_new_with_mnemonic("Settling T_ype:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);


    settle_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(settle_combo_box), "Select settling condition: when propagation of input to hidden to output activations stops.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(settle_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(settle_combo_box), "Equilibrium");
    gtk_combo_box_append_text(GTK_COMBO_BOX(settle_combo_box), "For 28");
    gtk_combo_box_append_text(GTK_COMBO_BOX(settle_combo_box), "Rogers'");
    gtk_combo_box_append_text(GTK_COMBO_BOX(settle_combo_box), "Semantic Task");

    g_signal_connect(G_OBJECT(settle_combo_box), "changed", G_CALLBACK(set_settling_type), NULL);
    gtk_box_pack_start(GTK_BOX(box), settle_combo_box, expand, fill, padding);

    gtk_combo_box_set_active(GTK_COMBO_BOX(settle_combo_box), settling_type);
    gtk_widget_hide(settle_combo_box);

    anneal_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(anneal_combo_box), "Select annealing type.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(anneal_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(anneal_combo_box), "Equilibrium");
    gtk_combo_box_append_text(GTK_COMBO_BOX(anneal_combo_box), "Clamped");
    gtk_combo_box_append_text(GTK_COMBO_BOX(anneal_combo_box), "Semi-clapmed");
    gtk_combo_box_append_text(GTK_COMBO_BOX(anneal_combo_box), "Unclamped");

    g_signal_connect(G_OBJECT(anneal_combo_box), "changed", G_CALLBACK(set_annealing_type), NULL);
    gtk_box_pack_start(GTK_BOX(box), anneal_combo_box, expand, fill, padding);

    gtk_combo_box_set_active(GTK_COMBO_BOX(anneal_combo_box), settling_type);
    gtk_widget_hide(anneal_combo_box);

    label = gtk_label_new_with_mnemonic("Err_or Measure:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);

    gtk_widget_show(label);


    error_combo_box = gtk_combo_box_new_text();
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(error_combo_box));

    gtk_widget_set_tooltip_text(GTK_WIDGET(error_combo_box), "Select error metric used to compare actual output to ideal output.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), GTK_WIDGET(error_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(error_combo_box), "Jaccard");
    gtk_combo_box_append_text(GTK_COMBO_BOX(error_combo_box), "RMSD");
    gtk_combo_box_append_text(GTK_COMBO_BOX(error_combo_box), "Sørensen");
    gtk_combo_box_append_text(GTK_COMBO_BOX(error_combo_box), "RSS");


//     gtk_combo_box_append_text(GTK_COMBO_BOX(error_combo_box), "Sørensen");

    g_signal_connect(G_OBJECT(error_combo_box), "changed", G_CALLBACK(set_error_type), NULL);
    gtk_box_pack_start(GTK_BOX(box), error_combo_box, expand, fill, padding);

    gtk_combo_box_set_active(GTK_COMBO_BOX(error_combo_box), error_type);
    gtk_widget_show(error_combo_box);

    button = gtk_button_new_with_mnemonic("Test _All");
    gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Calculate detailed error percentage per sub-pattern, shown in \"Error\" tab.");
    g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(test_button), notebook);
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);

    gtk_widget_show(button);

    gtk_widget_show(box);



    label = gtk_label_new_with_mnemonic("%:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
    gtk_box_pack_start(GTK_BOX(box), label, expand, fill, padding);
    gtk_widget_show(label);

    spin_button = gtk_spin_button_new_with_range(0, 100, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), lesion_percentage);
    g_signal_connect(G_OBJECT(spin_button), "value-changed", G_CALLBACK(set_lesion_percentage), NULL);
    gtk_box_pack_start(GTK_BOX(box), spin_button, expand, fill, padding);
    gtk_widget_show(spin_button);

    button = gtk_button_new_with_label ("Lesion");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(lesion_button), notebook);
    gtk_box_pack_start(GTK_BOX(box), button, expand, fill, padding);
    gtk_widget_show(button);


// 	GtkWidget *b_box =  gtk_hbox_new(FALSE, spacing);
// 	gtk_container_set_border_width (GTK_CONTAINER (b_box), 10);


// 	label = gtk_label_new(" ");
// 	gtk_misc_set_alignment(GTK_MISC(label), 0.1, 0.5);
// 	gtk_table_attach(GTK_TABLE(table), label, 0, 25, 5, 6, GTK_EXPAND, GTK_EXPAND, 0, 0);
// 	gtk_widget_show(label);


    box = gtk_hbox_new(FALSE, spacing);
    gtk_widget_set_name(box, "tab-specific-controls");

    gtk_box_pack_start(GTK_BOX(vertical_box), box, expand, fill, vertical_padding);


//     label = gtk_label_new_with_mnemonic("Tab-specific Controls:");
//     gtk_misc_set_alignment(GTK_MISC(label), 0.9, 0.5);
//     //     gtk_container_add (GTK_CONTAINER (b_box), unrolled_network_viewer_label);
//     gtk_box_pack_start(GTK_BOX(box),label, expand, fill, padding);
//     gtk_widget_show(label);


//     gtk_button_box_set_layout (GTK_BUTTON_BOX (b_box), GTK_BUTTONBOX_CENTER);
//     gtk_box_set_spacing(GTK_BOX(b_box), 0.5);
//     gtk_button_box_set_child_size (GTK_BUTTON_BOX (b_box), 5, 20);




// 	  label = gtk_label_new("Graph:");
// 	  gtk_misc_set_alignment(GTK_MISC(label), 0.05, 0.5);
// 	  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 7, 8, GTK_FILL, GTK_FILL, 2.5, 2.5);
// 	  gtk_container_add (GTK_CONTAINER (b_box), label);

// 	  gtk_box_pack_start_start(GTK_BOX(b_box), label, expand, fill, padding);
// 	  gtk_widget_show(label);

    attractor_view_label = gtk_label_new("Output of Attractors:");
    gtk_misc_set_alignment(GTK_MISC(attractor_view_label), 0.9, 0.5);
    gtk_box_pack_start(GTK_BOX(box), attractor_view_label, expand, fill, padding);
    gtk_widget_hide(attractor_view_label);

    attractor_view_button = gtk_button_new_with_label("Run");
    gtk_widget_set_tooltip_text(GTK_WIDGET(attractor_view_button), "Calculate attractor decay at specific lesioning levels.");
    g_signal_connect(GTK_OBJECT(attractor_view_button), "clicked", GTK_SIGNAL_FUNC(clicked_attractor_view_button), NULL);
    gtk_box_pack_start(GTK_BOX(box), attractor_view_button, expand, fill, padding);
    gtk_widget_hide(attractor_view_button);

    attractor_view_label2 = gtk_label_new("View:");
    gtk_misc_set_alignment(GTK_MISC(attractor_view_label2), 0.9, 0.5);
    gtk_box_pack_start(GTK_BOX(box), attractor_view_label2, expand, fill, padding);
    gtk_widget_hide(attractor_view_label2);

    attractor_view_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(attractor_view_combo_box), "Select input sub-pattern (name, verbal, visual) shown to network in order to produce outputs shown above.");
    gtk_combo_box_append_text(GTK_COMBO_BOX(attractor_view_combo_box), "Name");
    gtk_combo_box_append_text(GTK_COMBO_BOX(attractor_view_combo_box), "Visual");
    gtk_combo_box_append_text(GTK_COMBO_BOX(attractor_view_combo_box), "Verbal");
    gtk_combo_box_set_active(GTK_COMBO_BOX(attractor_view_combo_box), 0);
    g_signal_connect(G_OBJECT(attractor_view_combo_box), "changed", G_CALLBACK(set_attractor_view_type), NULL);
    gtk_box_pack_start(GTK_BOX(box), attractor_view_combo_box, expand, fill, padding);
    gtk_widget_hide(attractor_view_combo_box);
//    gtk_widget_show(box);
//     attractor_view_button2 = gtk_button_new_with_label("Save");
//     gtk_widget_set_tooltip_text(GTK_WIDGET(attractor_view_button2), "Save results.");
//     g_signal_connect(GTK_OBJECT(attractor_view_button2), "clicked", GTK_SIGNAL_FUNC(save_attractor_view), NULL);
//     gtk_box_pack_start(GTK_BOX(box), attractor_view_button2, expand, fill, padding);
//     gtk_widget_hide(attractor_view_button2);


    // Unrolled network viewer's settings

    unrolled_network_viewer_label = gtk_label_new_with_mnemonic("States _View:");
    gtk_misc_set_alignment(GTK_MISC(unrolled_network_viewer_label), 0.9, 0.5);
//     gtk_container_add (GTK_CONTAINER (b_box), unrolled_network_viewer_label);
    gtk_box_pack_start(GTK_BOX(box), unrolled_network_viewer_label, expand, fill, padding);
    gtk_widget_hide(unrolled_network_viewer_label);

    unrolled_network_viewer_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(unrolled_network_viewer_combo_box), "Select how to view unit states during training.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(unrolled_network_viewer_label), GTK_WIDGET(unrolled_network_viewer_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(unrolled_network_viewer_combo_box), "Unrolled");
    gtk_combo_box_append_text(GTK_COMBO_BOX(unrolled_network_viewer_combo_box), "Rolled");
    g_signal_connect(G_OBJECT(unrolled_network_viewer_combo_box), "changed", G_CALLBACK(set_unrolled_network_viewer_type), notebook);
    gtk_combo_box_set_active(GTK_COMBO_BOX(unrolled_network_viewer_combo_box), 1);
//     gtk_container_add (GTK_CONTAINER (b_box), );
    gtk_box_pack_start(GTK_BOX(box), unrolled_network_viewer_combo_box, expand, fill, padding);

    gtk_widget_hide(unrolled_network_viewer_combo_box);


    // Xbm's settings

    xbm_p_label = gtk_label_new_with_mnemonic("_Probability:");
    gtk_misc_set_alignment(GTK_MISC(xbm_p_label), 0.9, 0.5);
//     gtk_container_add (GTK_CONTAINER (b_box), xbm_p_label);
    gtk_box_pack_start(GTK_BOX(box), xbm_p_label, expand, fill, padding);

    gtk_widget_hide(xbm_p_label);

    xbm_p_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(unrolled_network_viewer_combo_box), "Select which minus probability to view in comparison to the plus probability.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(xbm_p_label), GTK_WIDGET(xbm_p_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(xbm_p_combo_box), "Name");
    gtk_combo_box_append_text(GTK_COMBO_BOX(xbm_p_combo_box), "Visual");
    gtk_combo_box_append_text(GTK_COMBO_BOX(xbm_p_combo_box), "Verbal");
    g_signal_connect(G_OBJECT(xbm_p_combo_box), "changed", G_CALLBACK(set_xbm_p_type), notebook);
    gtk_combo_box_set_active(GTK_COMBO_BOX(xbm_p_combo_box), 0);
//     gtk_container_add (GTK_CONTAINER (b_box), xbm_p_combo_box);
    gtk_box_pack_start(GTK_BOX(box), xbm_p_combo_box, expand, fill, padding);

    gtk_widget_hide(xbm_p_combo_box);



    // Naming Task Controls
    naming_label = gtk_label_new_with_mnemonic("Activation T_hreshold:");
    gtk_misc_set_alignment(GTK_MISC(naming_label), 0.0, 0.5);
//     gtk_container_add (GTK_CONTAINER (b_box), naming_label);
    gtk_box_pack_start(GTK_BOX(box), naming_label, expand, fill, padding);
    gtk_widget_hide(naming_label);

    char s[5];
    sprintf(s, "%1.2f", activation_threshold);
    naming_label3 = gtk_label_new(s);
//     gtk_label_set_text(GTK_LABEL(naming_label3), s);
    gtk_box_pack_start(GTK_BOX(box), naming_label3, expand, fill, padding);
    gtk_widget_hide(naming_label3);

    naming_scale = gtk_hscale_new_with_range(0.0, 1.0, 0.01);
    gtk_widget_set_tooltip_text(GTK_WIDGET(naming_scale), "Select the value of the threshold above which units are considered to be on.");
    gtk_widget_set_size_request(GTK_WIDGET(naming_scale), 150, -1);
    gtk_label_set_mnemonic_widget(GTK_LABEL(naming_label), GTK_WIDGET(naming_scale));
    gtk_scale_add_mark(GTK_SCALE(naming_scale), 0.5, GTK_POS_TOP, NULL);
    gtk_range_set_value(GTK_RANGE(naming_scale), ACTIVATION_THRESHOLD);
    gtk_scale_set_draw_value(GTK_SCALE(naming_scale), FALSE);
    gtk_scale_set_digits(GTK_SCALE(naming_scale), 3);
    g_signal_connect(G_OBJECT(naming_scale), "value-changed", G_CALLBACK(set_activation_threshold), naming_label3);
//     gtk_container_add (GTK_CONTAINER (b_box), naming_scale);
    gtk_box_pack_start(GTK_BOX(box), naming_scale, expand, fill, padding);

    gtk_widget_hide(naming_scale);


    naming_label2 = gtk_label_new_with_mnemonic("_Graph Type:");
    gtk_misc_set_alignment(GTK_MISC(naming_label2), 0.9, 0.5);
//     gtk_container_add (GTK_CONTAINER (b_box), naming_label2);
    gtk_box_pack_start(GTK_BOX(box), naming_label2, expand, fill, padding);

    gtk_widget_hide(naming_label2);

    naming_combo_box = gtk_combo_box_new_text();
    gtk_label_set_mnemonic_widget(GTK_LABEL(naming_label2), GTK_WIDGET(naming_combo_box));
    gtk_widget_set_tooltip_text(GTK_WIDGET(naming_combo_box), "Select the type of graph to view.");
    gtk_combo_box_append_text(GTK_COMBO_BOX(naming_combo_box), "General");
    gtk_combo_box_append_text(GTK_COMBO_BOX(naming_combo_box), "Category-Specific");
    g_signal_connect(G_OBJECT(naming_combo_box), "changed", G_CALLBACK(set_naming_graph_type), naming_graph);
    gtk_combo_box_set_active(GTK_COMBO_BOX(naming_combo_box), 0);
    //     gtk_container_add (GTK_CONTAINER (b_box), naming_combo_box);
    gtk_box_pack_start(GTK_BOX(box), naming_combo_box, expand, fill, padding);

    gtk_widget_hide(naming_combo_box);

    naming_combo_box_2 = gtk_combo_box_new_text();
    gtk_label_set_mnemonic_widget(GTK_LABEL(naming_label2), GTK_WIDGET(naming_combo_box_2));
    gtk_widget_set_tooltip_text(GTK_WIDGET(naming_combo_box_2), "Select the way to perform naming.");
    gtk_combo_box_append_text(GTK_COMBO_BOX(naming_combo_box_2), "Old");
    gtk_combo_box_append_text(GTK_COMBO_BOX(naming_combo_box_2), "New");
    gtk_combo_box_append_text(GTK_COMBO_BOX(naming_combo_box_2), "Alt");
    g_signal_connect(G_OBJECT(naming_combo_box_2), "changed", G_CALLBACK(set_naming_graph_way), naming_graph);
    gtk_combo_box_set_active(GTK_COMBO_BOX(naming_combo_box_2), 0);
    //     gtk_container_add (GTK_CONTAINER (b_box), naming_combo_box_2);
    gtk_box_pack_start(GTK_BOX(box), naming_combo_box_2, expand, fill, padding);

    gtk_widget_hide(naming_combo_box_2);





    // Word to Picture Task Controls
    word_to_picture_label2 = gtk_label_new_with_mnemonic("_Lesioning Percentage:");
    gtk_misc_set_alignment(GTK_MISC(word_to_picture_label2), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(box), word_to_picture_label2, expand, fill, padding);
    gtk_widget_hide(word_to_picture_label2);

    word_to_picture_combo_box2 = gtk_combo_box_text_new();
    gtk_widget_set_tooltip_text(GTK_WIDGET(word_to_picture_combo_box2), "Select the percentage of lesioning at which to view cross-sectional data.");

    gtk_label_set_mnemonic_widget(GTK_LABEL(word_to_picture_label2), GTK_WIDGET(word_to_picture_combo_box2));
    g_signal_connect(G_OBJECT(word_to_picture_combo_box2), "changed", G_CALLBACK(set_word_to_picture_current_lesion_level), notebook);
    set_word_to_picture_combo_box_lesion_levels();

//     gtk_container_add (GTK_CONTAINER (b_box), word_to_picture_combo_box);
    gtk_box_pack_start(GTK_BOX(box), word_to_picture_combo_box2, expand, fill, padding);

    gtk_widget_hide(word_to_picture_combo_box2);

    word_to_picture_label = gtk_label_new_with_mnemonic("_Graph Type:");
    gtk_misc_set_alignment(GTK_MISC(word_to_picture_label), 0.0, 0.5);
//     gtk_container_add(GTK_CONTAINER (b_box), word_to_picture_label);
    gtk_box_pack_start(GTK_BOX(box), word_to_picture_label, expand, fill, padding);

    gtk_widget_hide(word_to_picture_label);

    word_to_picture_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(word_to_picture_combo_box), "Select the type of graph to view.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(word_to_picture_label), GTK_WIDGET(word_to_picture_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(word_to_picture_combo_box), "Longitudinal");
    gtk_combo_box_append_text(GTK_COMBO_BOX(word_to_picture_combo_box), "Cross-section");
    g_signal_connect(G_OBJECT(word_to_picture_combo_box), "changed", G_CALLBACK(set_word_to_picture_graph_type), word_to_picture_graph);
    gtk_combo_box_set_active(GTK_COMBO_BOX(word_to_picture_combo_box), 0);
//     gtk_container_add (GTK_CONTAINER (b_box), word_to_picture_combo_box);
    gtk_box_pack_start(GTK_BOX(box), word_to_picture_combo_box, expand, fill, padding);

    gtk_widget_hide(word_to_picture_combo_box);



    // Drawing and Copying Task Controls
    drawing_and_copying_label = gtk_label_new_with_mnemonic("_Graph Type:");
    gtk_misc_set_alignment(GTK_MISC(drawing_and_copying_label), 0.9, 0.5);
//     gtk_container_add(GTK_CONTAINER (b_box), drawing_and_copying_label);
    gtk_box_pack_start(GTK_BOX(box), drawing_and_copying_label, expand, fill, padding);

    gtk_widget_hide(drawing_and_copying_label);


    drawing_and_copying_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(drawing_and_copying_combo_box), "Select the type of graph to view.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(drawing_and_copying_label), GTK_WIDGET(drawing_and_copying_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(drawing_and_copying_combo_box), "Overall Feature Errors");
    gtk_combo_box_append_text(GTK_COMBO_BOX(drawing_and_copying_combo_box), "Omission Errors");
    gtk_combo_box_append_text(GTK_COMBO_BOX(drawing_and_copying_combo_box), "Intrusion Errors");
    g_signal_connect(G_OBJECT(drawing_and_copying_combo_box), "changed", G_CALLBACK(set_drawing_and_copying_graph_type), drawing_and_copying_graph);
    gtk_combo_box_set_active(GTK_COMBO_BOX(drawing_and_copying_combo_box), 0);
//     gtk_container_add (GTK_CONTAINER (b_box), drawing_and_copying_combo_box);
    gtk_box_pack_start(GTK_BOX(box), drawing_and_copying_combo_box, expand, fill, padding);

//     gtk_widget_show(drawing_and_copying_combo_box);

    gtk_widget_hide(drawing_and_copying_combo_box);

//     gtk_widget_show(box);
    // Tyler Task Controls
    tyler_label = gtk_label_new_with_mnemonic("_Graph Type:");
    gtk_misc_set_alignment(GTK_MISC(tyler_label), 0.9, 0.5);
    //     gtk_container_add(GTK_CONTAINER (b_box), drawing_and_copying_label);
    gtk_box_pack_start(GTK_BOX(box), tyler_label, expand, fill, padding);

    gtk_widget_hide(tyler_label);


    tyler_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(tyler_combo_box), "Select the type of graph to view.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(tyler_label), GTK_WIDGET(tyler_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(tyler_combo_box), "Distinctive Features per Domain");
    gtk_combo_box_append_text(GTK_COMBO_BOX(tyler_combo_box), "Shared VS Distinctive Features for Living Domain");
    gtk_combo_box_append_text(GTK_COMBO_BOX(tyler_combo_box), "Shared Features per Domain");
    gtk_combo_box_append_text(GTK_COMBO_BOX(tyler_combo_box), "Functional Features per Domain");
    gtk_combo_box_append_text(GTK_COMBO_BOX(tyler_combo_box), "Percentage Correct per Domain");
    gtk_combo_box_append_text(GTK_COMBO_BOX(tyler_combo_box), "Percentage Incorrect for Between/Within Features per Domain");
    g_signal_connect(G_OBJECT(tyler_combo_box), "changed", G_CALLBACK(set_tyler_graph_type), tyler_graph);
    gtk_combo_box_set_active(GTK_COMBO_BOX(tyler_combo_box), 0);
    //     gtk_container_add (GTK_CONTAINER (b_box), tyler_combo_box);
    gtk_box_pack_start(GTK_BOX(box), tyler_combo_box, expand, fill, padding);

    //     gtk_widget_show(tyler_combo_box);

    gtk_widget_hide(tyler_combo_box);

    aoa_label = gtk_label_new_with_mnemonic("_Graph Type:");
    gtk_misc_set_alignment(GTK_MISC(aoa_label), 0.9, 0.5);
    //     gtk_container_add(GTK_CONTAINER (b_box), drawing_and_copying_label);
    gtk_box_pack_start(GTK_BOX(box), aoa_label, expand, fill, padding);
    gtk_widget_hide(aoa_label);
    aoa_combo_box = gtk_combo_box_new_text();
    gtk_widget_set_tooltip_text(GTK_WIDGET(aoa_combo_box), "Select the type of graph to view.");
    gtk_label_set_mnemonic_widget(GTK_LABEL(aoa_label), GTK_WIDGET(aoa_combo_box));
    gtk_combo_box_append_text(GTK_COMBO_BOX(aoa_combo_box), "Error per Category");
    g_signal_connect(G_OBJECT(aoa_combo_box), "changed", G_CALLBACK(set_aoa_graph_type), aoa_graph);
    gtk_combo_box_set_active(GTK_COMBO_BOX(aoa_combo_box), 0);
    //     gtk_container_add (GTK_CONTAINER (b_box), aoa_combo_box);
    gtk_box_pack_start(GTK_BOX(box), aoa_combo_box, expand, fill, padding);

    //     gtk_widget_show(aoa_combo_box);

    gtk_widget_hide(aoa_combo_box);



//     box = gtk_hbox_new(FALSE, spacing);
//     gtk_table_attach(GTK_TABLE(table), box, 0, 1, 7, 8, GTK_FILL, GTK_FILL, 0, 0);


//     button = gtk_button_new_with_mnemonic("Cluster");
//     g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(create_clusters), NULL);
//     gtk_box_pack_end(GTK_BOX(box), button, expand, fill, padding);
//     gtk_widget_show(button);

    button = gtk_button_new_with_mnemonic("E_xit");
    g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(delete), NULL);
    gtk_widget_set_size_request(GTK_WIDGET(button), -1, 30);
    gtk_box_pack_end(GTK_BOX(box), button, expand, fill, padding);
    gtk_widget_show(button);

    gtk_widget_show(box);



// 	  	gtk_widget_show_all(window);

// 	  word_to_picture_combo_box = gtk_combo_box_new_text();
// 	  gtk_combo_box_append_text(GTK_COMBO_BOX(word_to_picture_combo_box), "General");
// 	  gtk_combo_box_append_text(GTK_COMBO_BOX(word_to_picture_combo_box), "Category-Specific");
// 	  g_signal_connect(G_OBJECT(word_to_picture_combo_box), "changed", G_CALLBACK(set_word_to_picture_graph_type), word_to_picture_graph);
// 	  gtk_combo_box_set_active(GTK_COMBO_BOX(word_to_picture_combo_box), 0);
// 	  gtk_table_attach(GTK_TABLE(table), word_to_picture_combo_box, 12, 14, 3, 4, GTK_FILL, GTK_FILL, 2.5, 2.5);
// 	  gtk_widget_hide(word_to_picture_combo_box);



    //

    // General Controls

// 	label = gtk_label_new("Network:");
// 	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 7, 8, GTK_FILL, GTK_FILL, 2.5, 2.5);
//
// 	if (is_file_open()) {
// 	  weights_button = gtk_button_new_with_label(open_name);
// 	} else {
// 		weights_button = gtk_button_new_with_label ("Open Weights");
// 	}
//
// 	g_signal_connect(GTK_OBJECT(weights_button), "clicked", GTK_SIGNAL_FUNC(open_weights_button), window);
// 	gtk_table_attach(GTK_TABLE(table), weights_button, 1, 2, 7, 8, GTK_FILL, GTK_FILL, 2.5, 2.5);
// 	gtk_widget_show(weights_button);
//
// 	label = gtk_label_new("Patterns:");
// 	gtk_table_attach(GTK_TABLE(table), label, 2, 3, 7, 8, GTK_FILL, GTK_FILL, 2.5, 2.5);
//
// 	if (open_patterns_filename) {
// 	  patterns_button = gtk_button_new_with_label(open_patterns_filename);
// 	} else {
// 	 patterns_button = gtk_button_new_with_label ("Open Patterns");
// 	}

// 	g_signal_connect(GTK_OBJECT(patterns_button), "clicked", GTK_SIGNAL_FUNC(open_patterns_button), window);
// 	gtk_table_attach(GTK_TABLE(table), patterns_button, 3, 4, 7, 8, GTK_FILL, GTK_FILL, 2.5, 2.5);
// 	gtk_widget_show(patterns_button);

// 	button = gtk_button_new_with_label ("Open Patterns");
// 	g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(open_patterns_button), notebook);
// 	gtk_table_attach(GTK_TABLE(table), button, 2, 3, 7, 8, GTK_FILL, GTK_FILL, 2.5, 2.5);
// 	gtk_widget_show(button);



// 	button = gtk_button_new_with_label ("Save Graph");
// 	g_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(save_graph), window);
// 	gtk_table_attach(GTK_TABLE(table), button, 18, 19, 2, 3, GTK_FILL, GTK_FILL, 2.5, 2.5);
// 	gtk_widget_show(button);




    gtk_widget_show(vertical_box);
    gtk_widget_show(window);


    // Initialise Network
    new_weights(NULL);
    auto_open_file();
    real_time_redraw();

    bptt_unclamp_all();
    bm_unclamp();
    pdptool_unclamp();
    tyler_unclamp_all();



//     int i;

    char **new_argv;
    new_argv = malloc(sizeof(char *) * 3);
    char buffer[NAME_MAX];

    for (i = 0; i < 3; i++)
    {
        new_argv[i] = malloc(sizeof(buffer)); // create new string in the array
        strcpy(new_argv[i], buffer); // assign the string
    }



    if (argc > 1) {
        //first argc is the name of the program, the rest are taken to be filenames
        for(i = 1; i < argc; i++) {

            if (i > 1) {


                pid_t child;


                strncpy(new_argv[0],argv[0], NAME_MAX);
                strncpy(new_argv[1],argv[i],NAME_MAX);
                new_argv[2] = 0;

                child = fork();
                if (child == 0) { // child
                    setenv("proc_b", "", 1);
                    execvp(new_argv[0], new_argv);
                } else if (child < 0) // error
                    perror("fork");


            } else {

                if (file_exists(argv[i])) {
                    sprintf(open_filename, "%s", argv[i]);

                    open_all_related_files();
                } else {
                    printf("File cannot be opened.\n");

                }
            }
        }
    }

    gtk_main();


    gb_stop = TRUE;
    return 0;
}

