OBJ := $(patsubst %.c, %.o, $(wildcard *.c))
Steg.out : $(OBJ)
	gcc -o  $@ $^
clean :
	rm *.out *.o