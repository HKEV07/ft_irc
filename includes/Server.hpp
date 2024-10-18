#ifndef IRC_HPP
# define IRC_HPP

#include <ctime>
# include <iostream>
# include <vector>
# include <fstream>
# include <sstream>
# include <sys/socket.h>
# include <algorithm>
# include <sys/types.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <fcntl.h>
# include <map>
# include <poll.h>
# include <unistd.h>
# include <cstdlib>
# include <string>
# include <strings.h>

# include <string.h>
# include <cstring>
# include <cerrno>
# include <signal.h>
#include <set>
#include <algorithm>

#include <stdlib.h>
#include "Channel.hpp"
#include "Client.hpp"
#include "Reply.hpp"

#define RED			"\033[1;91m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[1;93m"
#define BLUE		"\033[1;94m"
#define B_CYAN		"\033[1;96m"
#define B_BLACK		"\033[1;90m"
#define I_GREEN		"\033[1;92m"
#define RESET		"\033[0m"


extern bool g_interrupt;



class Server
{
	private:
		std::string					_cmd;
		int							_port;
		int							_sock;
		std::map<int,Client*>			_clients;
		std::map<std::string,Channel*>	_channels;
		std::vector<pollfd>			_pollfds;
		std::string					_password;


	public:
	Server(int port, std::string password);
	~Server();

	int									createSocket();
	void								launch();
	void								handleMessage(int fd);
	void								displayClient();
	void								replys(Client &c,std::string msg);
	void								parseCmd(std::string cmd, Client &cl,int fd);
	std::vector<std::string>			splitCommands(std::string msg);
	void								readMessage(int fd);
	Client								*getClientByNickNameS(std::string Nickname);
	int									checkIfChannelExist(std::string &target);
	Channel								*getChannel(std::string &target);
	void								joinZero(Client &c);
	void								clientDisconnect(int fd);
	void								eraseClient(int fd);
	void								newCl();

	bool								is_used(Client cl, std::string name);

	Client								*findClient(int fd);
	int									findClientInS(std::string nickname);
	std::map<int,Client*>::iterator		findClientIt(int fd);
    void								deletChannelA(std::string nickName);
	
	// IRC Commands...
	int		MODE(std::string args, Client &c);
	int		JOIN(std::string args, Client &c);
	int		TOPIC(std::string args, Client &c);
	int		KICK(std::string args, Client &c);
	int		INVITE(std::string args, Client &c);
	int		PART(std::string cmd, Client &c);
	int 	PRIVMSG(std::string args, Client& client);
	int 	cmdUser(std::string args, Client &cl);
	int 	cmdNick(std::string args, Client &cl);
	int 	cmdPass(std::string args, Client &cl);

};

// utils
long long	aatoi(const char * str);
std::string	del_break(std::string str);
std::string getTimeSc();
#endif
