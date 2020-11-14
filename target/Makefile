NAME = demo

all: $(NAME)

$(NAME):
	gcc -g main.c -o $(NAME)

dump: $(NAME)
	 gdb -q --batch --command=dump.gdb --args $(NAME) > dump.txt
