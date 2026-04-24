# Dirs
SRC_DIR   = srcs
OBJ_DIR   = obj
INC_DIR   = includes
LIB_DIR   = libs

# Sources via wildcard
SRCS      = $(shell find $(SRC_DIR) -name "*.c")
OBJS      = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Libft
LIBFT_GIT    = https://github.com/LaGuibole/Libft_Boosted.git
LIBFT_DIR    = Libft_Boosted
LIBFT_PATH   = $(LIB_DIR)/$(LIBFT_DIR)
LIBFT_TARGET = libft.a
LIBFT_H      = libft.h
LIBFT_H_SRC  = $(LIBFT_PATH)/includes/$(LIBFT_H)
LIBFT_INC_H  = $(INC_DIR)/$(LIBFT_H)
LIBFT_AR     = $(LIBFT_PATH)/$(LIBFT_TARGET)

# Compiler
CC       = gcc
CFLAGS   = -Wall -Wextra -Werror -g
IFLAGS   = -I$(INC_DIR)
NAME     = ft_traceroute

all: $(NAME)

$(NAME): $(LIBFT_INC_H) $(LIBFT_TARGET) $(OBJS)
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $(OBJS) $(LIBFT_TARGET)
	@echo "✅ $(NAME) ready"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

$(LIBFT_TARGET): $(LIBFT_AR)
	@cp $(LIBFT_AR) ./

$(LIBFT_INC_H): $(LIBFT_AR)
	@cp $(LIBFT_H_SRC) $@

$(LIBFT_AR):
	@[ -d "$(LIBFT_PATH)/.git" ] || git clone $(LIBFT_GIT) $(LIBFT_PATH)
	@$(MAKE) --no-print-directory -C $(LIBFT_PATH)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME) $(LIBFT_INC_H) $(LIBFT_TARGET) 

re: fclean all

fcleanlibs: fclean
	@rm -rf $(LIBFT_PATH)

.PHONY: all clean fclean re fcleanlibs%     