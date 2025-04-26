# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/25 00:56:19 by bleow             #+#    #+#              #
#    Updated: 2025/04/26 15:23:36 by bleow            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo

CC = gcc
CFLAGS = -Wall -Wextra -Werror
THREAD_FLAGS = -pthread
DEBUG_FLAGS = -g
THREAD_DEBUG_FLAGS = -g -fsanitize=thread -fno-omit-frame-pointer
SANITIZE_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

PHILO_FILES = philo.c \
            initialise.c \
            sim_run_utils.c \
            sim_run.c \
            validate.c 
            
PHILO_SRCS = ./
PHILO_OBJS_DIR = objects

$(PHILO_OBJS_DIR):
	@mkdir -p $(PHILO_OBJS_DIR)

$(PHILO_OBJS_DIR)/%.o: $(PHILO_SRCS)%.c | $(PHILO_OBJS_DIR)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $(THREAD_FLAGS) -c $< -o $@

PHILO_OBJS = $(addprefix $(PHILO_OBJS_DIR)/, $(PHILO_FILES:.c=.o))

all: $(NAME)

$(NAME): $(PHILO_OBJS)
	@echo "Linking $(NAME)"
	@$(CC) $(CFLAGS) $(THREAD_FLAGS) $^ -o $@
	@echo "Build complete: $(NAME)"

clean:
	@echo "Cleaning up..."
	@rm -rf $(PHILO_OBJS_DIR)

fclean: clean
	@echo "Performing full clean..."
	@rm -f $(NAME)

re: fclean all

thread-debug: CFLAGS += $(THREAD_DEBUG_FLAGS)
thread-debug: re
	@echo "Built with thread sanitizer"

debug: CFLAGS += $(DEBUG_FLAGS)
debug: re
	@echo "Built with debug symbols"

sanitize: CFLAGS += $(SANITIZE_FLAGS)
sanitize: re
	@echo "Built with address sanitizer"

.PHONY: all clean fclean re debug thread-debug sanitize