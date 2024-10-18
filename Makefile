NAME = ircserv
BONUS_NAME = botto

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 #-g -fsanitize=address

HEADER = ./includes/Server.hpp ./includes/Reply.hpp ./includes/Client.hpp ./includes/Channel.hpp

RED = \033[1;91m
GREEN = \033[1;92m
YELLOW = \033[1;93m
RESET = \033[0m

SRCS = ./server/server.cpp \
       ./server/pass.cpp \
       ./server/user.cpp \
       ./server/nick.cpp \
       ./server/mode.cpp \
       ./server/join.cpp \
       ./server/topic.cpp \
       ./server/kick.cpp \
       ./server/part.cpp \
       ./server/invite.cpp \
       ./server/privmsg.cpp \
       ./main.cpp

BONUS_SRC = ./Bot/botto.cpp

OBJS_DIR = ./objs/
OBJS = $(SRCS:%.cpp=$(OBJS_DIR)%.o)
BONUS_OBJS = $(BONUS_SRC:%.cpp=$(OBJS_DIR)%.o)

all: $(NAME)

$(NAME): $(OBJS) $(HEADER)
	@echo "$(GREEN)ボクはアルベド、西風騎士団の首席錬金術師だ。\n\
	キミから感じる星海の気配に興味があってね、近くで観察したいと思ったんだ。\n\
	きっとこれから一緒に行動する機会が増えると思うよ。いい景色だね。休憩中にこの景色を絵にしようか。\n\
	I am ALBEDO, Chief Alchemist of the Knights of Favonius. You carry the aura of the stars, interesting...\n\
	I would like to study you, if you do not mind.\n\
	I'm certain we will have many opportunities to be alone in the future.\n\
	⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣤⠤⠶⢖⡒⠒⠚⠛⠻⢦⣄⡀⠀⠀⠈⠙⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⢠⡏⠙⠦⣄⡀⠀⠀⠀⢀⣀⡤⠖⣛⡉⠀⠀⠀⠀⠀⠈⠳⣄⠀⠀⠀⠀⠉⠳⣄⠀⠀⠀⢹⡄⠀⠀⠀⠀⠀⠀⠀\n\
⠀⢸⡇⠀⠀⠈⠉⠛⠓⠚⠛⠛⠉⠉⠉⠉⠙⠓⠦⣄⠀⢀⠀⠈⠂⠀⠀⢀⣠⣤⣌⠳⡄⠀⠈⠻⢤⣀⠀⠀⠀⠀⠀\n\
⠀⠈⢧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠴⠚⠉⠉⠉⠓⢄⣀⠔⠉⠀⠀⠀⠑⢅⢀⣀⣀⠀⠻⡇⠀⠀⠀⠀\n\
⠀⠀⠈⠳⣄⠀⠀⠀⠀⠀⠀⠀⣀⣀⣀⠔⠋⠀⠀⠀⠀⠀⠀⢀⠈⢁⡾⠓⣄⠀⠀⠀⠈⢧⠀⠉⠙⠲⣽⣄⠀⠀⠀\n\
⠀⠀⠀⠀⣨⠟⢒⣦⠴⠒⠋⠉⢁⠜⠁⠀⠀⠀⠀⠀⠀⣠⣾⡽⠷⠿⠗⠶⠿⡄⠀⠀⠀⠘⣧⠀⠀⠀⠀⠹⣆⠀⠀\n\
⣴⡶⠶⠶⠛⠛⠉⠀⠀⠀⠀⡴⠃⠀⠀⠀⠀⠀⠀⢀⣾⠃⠀⠀⠀⠀⠀⠀⠸⣿⠀⠀⠀⠀⢿⢧⠀⠀⠀⠀⠹⣆⠀\n\
⠈⠣⡀⠀⠀⠀⠀⠀⠀⣠⠞⠀⠀⠀⠀⠀⣠⠀⢠⡟⠀⠀⠀⠀⠀⠀⠀⠀⢀⢸⣷⠀⠀⠀⢸⠈⣧⠀⠀⠀⠀⢻⠀\n\
⠀⠀⠈⡷⠲⠤⣤⡠⠞⠁⠀⠀⠀⠀⠀⢠⠏⢀⡞⢀⣀⡤⠤⠤⠀⠀⠀⠀⢸⢀⡿⠀⠀⠀⣾⣄⡸⡆⠀⠀⠀⢸⡇\n\
⠀⠀⠀⣧⣤⠖⠋⠀⠀⣠⠆⠀⠀⠀⢀⣼⠀⡾⠀⠀⠀⠀⠀⣀⣠⠀⠀⠀⡟⢸⡇⠀⠀⢰⠃⠀⠀⢷⠀⠀⠀⢸⡇\n\
⠀⠀⠀⣿⠈⠙⠓⠒⢛⡏⠀⠀⠀⠀⠀⡟⣾⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⢸⠇⢸⠇⠀⢠⣿⣿⣿⣿⣿⠀⠀⠀⣼⠁\n\
⠀⢰⠻⣿⣇⠀⠀⡴⣿⡇⠀⠀⠀⠀⠀⣿⡟⡇⣿⠤⠄⣤⣦⢚⡇⠀⢀⡟⢀⣾⠀⣠⣟⠛⣯⢹⡏⣿⠄⠀⢠⡿⠀\n\
⠀⢸⠀⠈⠛⣶⢶⡇⢻⣇⢷⠀⠀⠀⠀⣿⠇⠀⠙⢞⢳⠎⠁⣸⠃⠀⢼⠗⢫⣧⠔⠁⢿⣲⠋⣸⠁⡼⠀⣠⠟⠀⠀\n\
⠀⠈⡗⠀⠀⡏⣸⠇⣸⠛⢿⣷⡀⠀⠀⠸⡄⠀⠀⠈⠛⠒⠚⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠚⠁⠼⢷⣾⠁⠀⠀⠀\n\
⠀⠘⣇⣠⣶⣿⢦⣘⣾⡞⠉⠉⠉⠢⣀⠀⠹⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣀⣀⣀⠀\n\
⠀⠀⣘⠿⠋⠀⠀⣠⠸⡇⠀⠀⠲⠦⠈⠙⠒⣚⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⢀⡼⠁⠉⢉⣽⠃\n\
⢿⡋⠁⠀⠀⢀⠔⠁⠀⠹⣄⣀⠀⠀⠀⣀⡶⢤⣉⡲⢄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⡤⠖⠉⠐⢶⣾⣉⣀⠀\n\
⠀⠉⢓⣲⠞⠁⠀⠀⠀⡇⠀⠉⠉⠉⠉⠙⡆⠀⠙⢿⣿⣿⣷⣶⣶⣶⣶⣶⣶⣶⡚⠛⠉⠁⢦⡀⠀⠀⠀⣠⠴⠋⠀\n\
⠀⠀⠘⠯⠤⣤⣤⡒⠀⣧⠀⠀⠀⠀⠀⠀⠹⣦⣄⣀⣉⣻⣿⣿⣹⣿⣿⣿⣿⣿⡿⣆⣀⠀⠀⣙⣳⠶⠋⠁⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠙⠲⢼⠗⠲⠴⠦⣤⣄⣀⣨⣿⣿⣾⠿⢦⣼⣿⣿⣿⣿⣩⣿⣷⣿⡍⠉⠉⠉⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣾⣿⡿⠋⠁⢠⠏⣰⣿⣿⣿⣸⣿⣿⣧⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⡇⠀⠀⠸⡄⠹⣿⣿⣿⣿⣿⣿⢿⣿⣿⣿⣺⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⠹⣿⣷⣤⣤⣴⡿⣶⡿⣿⡁⣿⢻⢿⣆⣼⠟⢸⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠟⠀⢺⣿⡿⢉⣻⣿⣿⣧⡈⠻⢃⣼⣆⠙⠁⢀⣾⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠞⣠⣶⡟⠁⠀⣾⣿⣿⣿⣿⣿⣶⣾⣿⣿⣧⣴⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀$(RESET)"
	@echo "$(RED)Compiling...$(RESET)⏳"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Executable is ready$(RESET)✅"

$(OBJS_DIR)%.o: %.cpp $(HEADER)
	@mkdir -p $(OBJS_DIR)server $(OBJS_DIR)Bot
	@$(CC) $(CFLAGS) -c $< -o $@

bonus: $(BONUS_OBJS)
	@echo "$(RED)Compiling Bot...$(RESET)⏳"
	@$(CC) $(CFLAGS) $(BONUS_OBJS) -o $(BONUS_NAME)
	@echo "$(GREEN)Bot is ready$(RESET)✅"

clean:
	@echo "$(YELLOW)Cleaning up object files$(RESET)❌"
	@rm -rf $(OBJS_DIR)

fclean: clean
	@echo "$(YELLOW)Executable removed$(RESET)❌"
	@rm -f $(NAME) $(BONUS_NAME)

re: fclean all

.PHONY: all clean fclean re bonus
