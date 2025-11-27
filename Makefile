# ---------------------------------------------------------------------------- #
#                                    ft_ping                                   #
# ---------------------------------------------------------------------------- #

include Vars.mk

# -------------------------------- Compilation ------------------------------- #

$(NAME): $(OBJS)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)
	@echo "[$(TARGET_NAME)] Compiled successfully!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# -------------------------------- Phony rules ------------------------------- #

.PHONY: all clean fclean re

all: $(NAME)

clean:
	@rm -f $(OBJS)
	@echo "[$(TARGET_NAME)] Object files removed!"

fclean: clean
	@rm -f $(NAME)
	@echo "[$(TARGET_NAME)] Executable removed!"

re: fclean all
	@echo "[$(TARGET_NAME)] Recompiled successfully!"

