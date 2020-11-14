NAME = fuzzer
D_SRCS = srcs
D_OBJS = objs
D_INCS = incs

SRCS = $(wildcard $(D_SRCS)/*.c)
OBJS = $(patsubst $(D_SRCS)/%.c,$(D_OBJS)/%.o,$(SRCS))
DEPS = $(patsubst $(D_SRCS)/%.c,$(D_OBJS)/%.d,$(SRCS))
INCS = $(wildcard $(D_INCS)/*.h)

CFLAGS = -Wall -Wextra -Wpedantic
CFLAGS += -I $(D_INCS)

LDFLAGS =

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJS)
	@echo "\033[38;5;10m=== $(NAME): Compiled !\033[0m"

$(D_OBJS)/%.o: $(D_SRCS)/%.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

include $(DEPS)

$(DEPS): ;

clean:
	find -name "*~" -delete -o -name "#*#" -delete
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) -d $(D_OBJS)


fclean: clean
	$(RM) $(NAME)

re: fclean all

debug: CFLAGS += -g3 -O0 -D DEBUG
debug: re
	@echo "\033[1;49;36m=== $(NAME): Debug mode\033[0m"

hardcore: CC = clang -Weverything
hardcore: re
	@echo "\033[1;49;31m=== $(NAME): H4rdc0r3 mode\033[0m"

.PHONY: all clean fclean re debug hardcore
