CC = arm-linux-gcc

all:
	$(CC) -Wall -Wextra -Werror -o GPS2 GPS2.c
clean:
	rm -fr GPS *~
