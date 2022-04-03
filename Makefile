CC = gcc 

OBJ = obj/mndlbrt.o obj/mandelbrot.o obj/logs.o obj/general.o

all: global

global: $(OBJ) 
	$(CC) $(OBJ) -lsfml-graphics -lsfml-window -lsfml-system -lstdc++  -lmvec -lm -o mandelbrot

obj/mndlbrt.o: src/mndlbrt.cpp src/global_conf.h
	$(CC) src/mndlbrt.cpp -c -o obj/mndlbrt.o 

obj/mandelbrot.o: src/global_conf.h src/mandelbrot/mandelbrot.cpp src/mandelbrot/mandelbrot.h src/mandelbrot/mandelbrot_conf.h
	$(CC) src/mandelbrot/mandelbrot.cpp -c -o obj/mandelbrot.o -O3 -mavx -mavx2 -msse4 -lm

obj/logs.o: src/global_conf.h src/logs/errors_and_logs.cpp src/logs/errors_and_logs.h src/logs/errors.h src/logs/log_definitions.h src/include/errors.txt
	$(CC) src/logs/errors_and_logs.cpp -c -o obj/logs.o 

obj/general.o: src/global_conf.h src/general/general.cpp src/general/general.h 
	$(CC) src/general/general.cpp -c -o obj/general.o

.PNONY: cleanup logfile logclean compile

cleanup:
	rm obj/*.o 

logfile:
	>> /tmp/mandelbrot.hmtl 

logclean:
	rm /tmp/mandelbrot.html

