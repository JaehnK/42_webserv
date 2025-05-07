CPP = c++
CPPFLAGS = -std=c++98 -Wall -Wextra -Werror -g
CPPFLAGS_DEBUG = -fsanitize=address
NAME = webserv

INCLUDE_DIR = ./includes
HEADR = $(addprefix $(INCLUDE_DIR)/, ServerManager.hpp)

SRCS =	webserv.cpp \
		ServerManager.cpp

SRCS_DIR = ./srcs
OBJS_DIR = ./objs

SRCS := $(addprefix $(SRCS_DIR)/, $(SRCS))
OBJS := $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))
OBJS_DEBUG := $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%_debug.o,$(SRCS))

all: $(OBJS_DIR) $(NAME)

debug: CPPFLAGS += $(CPPFLAGS_DEBUG)
debug: $(OBJS_DIR) $(NAME)_debug

$(NAME)_debug: $(OBJS_DEBUG)
	$(CPP) $(CPPFLAGS) $(OBJS_DEBUG) -o $(NAME)_debug
	@echo "Debug version is ready"

$(NAME): $(OBJS)
	@$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "webserv is ready"

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(HEADR) | $(OBJS_DIR)
	@mkdir -p $(dir $@)
	@$(CPP) $(CPPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJS_DIR)/%_debug.o: $(SRCS_DIR)/%.cpp $(HEADR) | $(OBJS_DIR)
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

clean:
	@rm -rf $(OBJS_DIR)
	@echo "Cleaning..."

fclean: clean
	@rm -f $(NAME) $(NAME)_debug
	@echo "Full cleaning..."

re: fclean all

.PHONY: all clean fclean re debug