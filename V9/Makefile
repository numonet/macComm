VPATH:= SelfDetect
OBJ_DIR:= ./obj
COMPILE:=arm-linux-gnueabihf-
SRC:=Crc.c CommSocket.c Common.c tda_clock.c td_func.c tda_func.c tdt_func.c



all:$(SRC)
	$(COMPILE)gcc -Wall -Wextra -fno-stack-protector -g -o CommMain_A -pthread $(SRC) Main.c -lrt -lm


.PHONY : clean
clean:
	rm -rf parent CommMain_A fork_fifo $(OBJ_DIR)/*.o 
