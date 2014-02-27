 CFLAGS =	`pkg-config --cflags gtk+-2.0` -Wall -g
#CFLAGS =	`pkg-config --cflags gtk+-2.0` -Wall -mcmodel=large -O3
# debug: -g	run: -O3
LIBS =		`pkg-config --libs gtk+-2.0` -lm
CC = 		gcc
XOBJECTS = 	main.o bptt.o bptt_internal.o bm.o bm_internal.o tyler.o tyler_internal.o tyler_feedforward.o lib_cairox.o lib_string.o unrolled_network_viewer.o graph.o naming_graph.o naming_bar_chart.o sorting_graph.o word_to_picture_graph.o drawing_and_copying_graph.o error_view.o patterns.o utils.o xbm.o error.o semantic_tasks.o bm_semantic_tasks.o bptt_semantic_tasks.o pdptool_semantic_tasks.o tyler_semantic_tasks.o pdptool.o network.o attractor_view.o tyler_graph.o aoa_graph.o


all:		 hub

create_patterns: create_patterns.o
		 gcc -o p create_patterns.o -lm

clean:
		rm -f hub *.o *~ *.orig

tidy:
		rm -f *~ *.orig
		astyle *.c *.h

hub:            $(XOBJECTS) Makefile 
		#$(RM) $@
		$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(XOBJECTS) $(LIBS)
		

test:		test.o lib_cairox.o Makefile
		$(CC) -o $@ $(CFLAGS) $(LDFLAGS) test.o lib_cairox.o $(LIBS)



kate:	
		kate *.c *.h
srctar:
		tar cvfz hub_model_src.tgz *.c *.h *.pat *.png Makefile

# G_SLICE=always-malloc G_DEBUG=gc-friendly  valgrind -v --tool=memcheck --leak-check=full --num-callers=40 --log-file=valgrind.log --main-stacksize=16777216 ./hub
#    valgrind --tool=callgrind -v --dump-every-bb=10000000 ./hub
