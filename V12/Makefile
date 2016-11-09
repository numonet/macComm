OBJ_DIR:= ./obj
COMPILE:=arm-linux-gnueabihf-
SRC:=Common.c tda_clock.c td_func.c tda_func.c tdt_func.c



all:$(SRC)
	$(COMPILE)gcc -Wall -Wextra -fno-stack-protector -g -o macComm -pthread $(SRC) Main.c -lrt -lm


.PHONY : clean
clean:
	rm -rf macComm fork_fifo $(OBJ_DIR)/*.o 
