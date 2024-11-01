# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hskrzypi <hskrzypi@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/12 13:49:32 by hskrzypi          #+#    #+#              #
#    Updated: 2024/10/31 19:31:24 by hskrzypi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= pipex

CFLAGS	:= -Wextra -Wall -Werror

LIBLIBFT	:= ./complete_Libft

HEADERS	:= -I ./include -I $(LIBLIBFT)

LIBS	:= -L$(LIBLIBFT) -lft -ldl -lglfw -pthread

SRCS	:= main2.c paths.c error_handling.c utils.c helper_pipex.c input_checks.c paths_utils.c

OBJS	:= ${SRCS:.c=.o}

RM = rm -rf

all: libft $(NAME)

libft:
	@$(MAKE) -C $(LIBLIBFT)

%.o: %.c
	@$(CC) $(CFLAGS) $< -c -o $@ $(HEADERS) 
	@printf "Compiling: $(notdir $<)\n"

$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(LIBS) $(HEADERS) -o $(NAME)
	@echo "$(NAME) compiled successfully.\n"

clean:
	$(RM) $(OBJS)
	@$(MAKE) clean -C $(LIBLIBFT)

fclean: clean
	$(RM) $(NAME) #$(NAME_BONUS)
	@$(MAKE) fclean -C $(LIBLIBFT)

re: fclean all

.PHONY: all clean fclean re libft
