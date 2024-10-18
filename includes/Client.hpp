#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "Channel.hpp"

enum State
{
	HANDSHAKE,
	LOGIN,
	REGISTERED
};

class Client
{
	private:
		int				sockfd;
		std::string		msg;
		std::string		nick;
		std::string		host;
		std::string		username;
		std::string		fullname;
		State			state;
	public:
		Client(int fd, std::string host);
		~Client();
		void	reply(std::string msg);
		void	welcome();

	// Getters

		std::string		getMsg() const;
		std::string		getNickname() const;
		std::string		getHost() const;
		std::string		getUsername() const;
		std::string		getFullname() const;
		std::string		getPrefix();
		State			getState() const;
		int				getFd() const;
	// Setters

		void		setMsg(std::string msg);
		void		setNickname(std::string nickname);
		void		setUsername(std::string username);
		void		setFullname(std::string realname);
		void		setHost(std::string hostname);
		void		setState(State newState);
		void		addMsg(std::string msg);
};

#endif
