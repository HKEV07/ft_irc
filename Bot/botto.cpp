#include "../includes/Server.hpp"

int bot_auth(int fd_bot, const std::string &cmd) {
    write(fd_bot,cmd.c_str(),cmd.length());
    return 0;
}

int check_msg(std::string str, std::string nick, int fd_bot) {
        std::string cmd;
        size_t indx = str.find(" "), i = 0;
        while(indx != str.npos) {
            i++;
            if(i == 3) {
                cmd = str.substr(indx + 1);
                if (cmd.at(0) == ':')
                    cmd = cmd.substr(1);
                if (cmd.at(cmd.length() - 1) == '\n')
                    cmd.erase(cmd.begin() + cmd.size() - 1);
                if (cmd.at(cmd.length() - 1) == '\r')
                    cmd.erase(cmd.begin() + cmd.size() - 1);
                break;
            }
            indx = str.find(" ", indx + 1);
        } if (cmd == "joke") {
            std::string line;
            int rand_val = std::rand() % 20 + 1, i = 0;
            std::ifstream f_joke("./Bot/Jokes.txt");
            if(f_joke.fail()) {
                return 1;
            } while (getline(f_joke, line)) {
                if(i == rand_val) {
                    std::string str1 = "PRIVMSG " + nick + " :" + line + " \r\n";
                    send(fd_bot, str1.c_str(),str1.length(),0);
                    f_joke.close();
                    break;
                }
                i++;
            }
        }
        else {
            std::string str1 = "PRIVMSG " + nick + " : Just \" joke \" ,Try Again..\r\n";
            send(fd_bot, str1.c_str(),str1.length(),0);
        }
        return 0;
}

void f()
{
	system("leaks ircserv");
}

int main(int ac, char *av[])
{
    if (ac != 5)
	{
		std::cerr << RED <<"Usage: " << av[0] << " <ip> <port> <nick> <password>" << RESET << std::endl;
        return 1;
    }
    int fd_bot = 0;
    std::string nick = av[3];
    std::string password = av[4];
    struct sockaddr_in _bot;
	std::srand(time(0));
	std::cout << GREEN << "Bot is Running..." << RESET << std::endl;
    fd_bot = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_bot < 0) {
        std::perror("socket");
        exit(1);
    }
    std::memset(&_bot, 0, sizeof(_bot));
    _bot.sin_family = AF_INET;
    _bot.sin_port = htons(std::atoi(av[2]));
    _bot.sin_addr.s_addr = inet_addr(av[1]);
	if (connect(fd_bot, (struct sockaddr*)&_bot, sizeof(_bot)) != 0) {
        close(fd_bot);
        std::perror("connect");
        return (1);
    }
    bot_auth(fd_bot, "PASS " + password + "\r\n");
    bot_auth(fd_bot, "USER bot bot bot bot\r\n");
    bot_auth(fd_bot, "NICK "+ nick +"\r\n");
    char buffer[512];
    std::string str, err;
    while (1)
	{
        std::memset(buffer, 0, 512);
        ssize_t buf = read(fd_bot, buffer, 511);
        if (buf <= 0)
            break;
        buffer[buf] = '\0';
        str = buffer;
        if (str.empty())
            continue;
        err = str.substr(str.find(" ") + 1, 3);
        if (err == "464" || err == "461") {
            std::cout << RED << "Incoerrect Password, Try Again.. :/" << RESET << std::endl;
            close(fd_bot);
            return 1;
        }
        if (err == "433" || err == "432" || err == "431") {
            std::cout << RED << "The Nick Name is already used, Try Again.. :/" << RESET << std::endl;
            close(fd_bot);
            return (1);
        }
        if (str.substr(0, 4) == "PING") {
            std::string num_pong = "PONG " + str.substr(6);
            send(fd_bot, num_pong.c_str(),num_pong.length(),0);
        }
        std::string test;
        test = str.substr(str.find(" ") + 1, 7);
        if (test == "PRIVMSG")
        {
            std::string nick = str.substr(1, str.find("!") - 1);
            if(check_msg(str, nick, fd_bot)) {
				std::string str1 = "PRIVMSG" + nick + " : Error in Jokes File, Try Again.. \r\n";
                send(fd_bot, str1.c_str(),str1.length(),0);
                continue;
            }
            nick.clear();
        }
    }
    close(fd_bot);
    return 0;
}
