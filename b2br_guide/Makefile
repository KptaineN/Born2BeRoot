# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nkiefer <nkiefer@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/03 12:57:47 by nkiefer           #+#    #+#              #
#    Updated: 2024/12/03 18:18:26 by nkiefer          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = gcc 
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lncurses

SRC_DIR = src
SRCS = $(SRC_DIR)/b2br_main.c $(SRC_DIR)/get_next_line.c
OBJS = $(SRCS:.c=.o)
NAME = guide_b2br

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
