NAME := ft_ping
CC := cc
CFLAGS := -Wall -Wextra -Werror -Iincludes
LDFLAGS := -lm
SRC_DIR := src
OBJ_DIR := obj
SRCS := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/options.c \
	$(SRC_DIR)/resolve.c \
	$(SRC_DIR)/checksum.c \
	$(SRC_DIR)/stats.c \
	$(SRC_DIR)/socket.c \
	$(SRC_DIR)/icmp_errors.c \
	$(SRC_DIR)/ping.c
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c includes/ft_ping.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
