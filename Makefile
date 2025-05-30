CPP = c++
CPPFLAGS = -std=c++98 -Wall -Wextra -Werror -g
CPPFLAGS_DEBUG = -fsanitize=address
NAME = webserv
INCLUDE_DIR = ./includes
# HEADERS = $(INCLUDE_DIR)/00_ServerManager.hpp \
#          $(INCLUDE_DIR)/10_Config.hpp \
#          $(INCLUDE_DIR)/11_ConfigBlock.hpp \
#          $(INCLUDE_DIR)/webserv.hpp

HEADERS = webserv.hpp \
		  00_ServerManager.hpp \
		  10_Config.hpp \
		  11_Server.hpp \
		  12_Location.hpp \
		  12-1_LocationAPI.hpp \
		  12-2_LocationDownload.hpp \
		  12-3_LocationUpload.hpp \
		  12-4_LocationCGI.hpp \
		  12-5_LocationDefault.hpp \
		  13_FileParser.hpp \
		  20_HttpServer.hpp \
		  21_HttpRequest.hpp \
		  22_HttpResponse.hpp \

SRCS = webserv.cpp \
       00_ServerManager.cpp \
       10_Config.cpp \
       11_Server.cpp \
	   12_Location.cpp \
	   12-1_LocationAPI.cpp \
	   12-2_LocationDownload.cpp \
	   12-3_LocationUpload.cpp \
	   12-4_LocationCGI.cpp \
	   12-5_LocationDefault.cpp \
	   13_FileParser.cpp \
	   20_HttpServer.cpp \
	   21_HttpRequest.cpp \


SRCS_DIR = ./srcs
OBJS_DIR = ./objs
HEADERS := $(addprefix $(INCLUDE_DIR)/, $(HEADERS))
SRCS := $(addprefix $(SRCS_DIR)/, $(SRCS))
OBJS := $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))
OBJS_DEBUG := $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%_debug.o,$(SRCS))

all: $(OBJS_DIR) $(NAME)

debug: CPPFLAGS += $(CPPFLAGS_DEBUG)
debug: $(OBJS_DIR) $(NAME)_debug

$(NAME)_debug: $(OBJS_DEBUG)
	@$(CPP) $(CPPFLAGS) $(OBJS_DEBUG) -o $(NAME)_debug
	@echo "Debug version is ready"

$(NAME): $(OBJS)
	@$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "webserv is ready"

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(HEADERS) | $(OBJS_DIR)
	@mkdir -p $(dir $@)
	@$(CPP) $(CPPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJS_DIR)/%_debug.o: $(SRCS_DIR)/%.cpp $(HEADERS) | $(OBJS_DIR)
	@mkdir -p $(dir $@)
	@$(CPP) $(CPPFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

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