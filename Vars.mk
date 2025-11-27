# ---------------------------------------------------------------------------- #
#                               ft_ping Variables                              #
# ---------------------------------------------------------------------------- #

TARGET_NAME		:= ft_traceroute

# -------------------------------- Directories ------------------------------- #

SRC_DIR			:= srcs
LIB_DIR			:= includes
BIN_DIR			:= bin
OBJ_DIR			:= objs

# ----------------------------------- Flags ---------------------------------- #

INCLUDES		:= -I./$(LIB_DIR)
CC				:= gcc
CFLAGS			:= -Wall -Wextra -Werror -g $(INCLUDES)
LFLAGS			:= -lm

# ----------------------------------- Files ---------------------------------- #

SRCS			:= $(wildcard $(SRC_DIR)/*.c)
OBJS			:= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
NAME			:= $(BIN_DIR)/$(TARGET_NAME)