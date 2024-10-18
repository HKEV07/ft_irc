
#include "../includes/Server.hpp"

Server::Server(int port, std::string password) :_port(port), _password(password)
{
	_sock = createSocket();
}

Server::~Server() {

	std::map<std::string,Channel*>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if(it->second)
			delete it->second;
		it++;
	}
	_channels.clear();
	std::map<int,Client*>::iterator its = _clients.begin();
	while (its != _clients.end())
	{
		if(its->second)
			delete its->second;
		its++;
	}
	_clients.clear();
	
}

// getters

int		Client::getFd() const {
	return (sockfd);
}

std::string	Client::getNickname() const {
	return (nick);
}

std::string	Client::getUsername() const {
	return (username);
}

std::string	Client::getFullname() const {
	return (fullname);
}

std::string	Client::getMsg() const {
	return (msg);
}

std::string	Client::getHost() const {
	return (host);
}

State	Client::getState() const {
	return (state);
}


void	Client::setUsername(std::string _new) {
	username = _new;
}

void	Client::setNickname(std::string _new) {
	nick = _new;
}

void	Client::setFullname(std::string _new) {
	fullname = _new;
}

void	Client::setMsg(std::string Newmsg) {
	msg = Newmsg;
}

void	Client::setState(State Newstate) {
	state = Newstate;
}


void	Client::addMsg(std::string message) {
	msg += message;
}
void	Client::setHost(std::string _new) {
	host = _new;
}

int		Server::createSocket()
{
	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		throw std::runtime_error("\033[1;91mError while opening socket.\033[0m");
	int	val = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
		throw std::runtime_error("\033[1;91mError while setting socket options.\033[0m");

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("\033[1;91mError while setting socket to NON-BLOCKING.\033[0m");

	sockaddr_in serv_address = {};
	bzero((char *) &serv_address, sizeof(serv_address));
	serv_address.sin_port = htons(_port);
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		throw std::runtime_error("\033[1;91mError while binding socket.\033[0m");
	if (listen(sockfd, 1000) < 0)
		throw std::runtime_error("\033[1;91mError while listening on socket.\033[0m");
	return (sockfd);
}

void	Server::newCl()
{
	int	new_fd;
	char	buf[2048];
	sockaddr_in s_adress = {};
	socklen_t s_size = sizeof(s_adress);

	new_fd = accept(_sock, (sockaddr *)&s_adress, &s_size);
	if (new_fd < 0)
		throw std::runtime_error("\033[1;91mError while accepting new client.\033[0m");
	int flag = 1;
    if (setsockopt(new_fd, SOL_SOCKET, SO_NOSIGPIPE, &flag, sizeof(flag)) == -1)
    {
        std::cerr << "setsockopt() failed: " << std::endl;
        exit(1);
    }
	if (getnameinfo((struct sockaddr *) &s_adress, sizeof(s_adress), buf, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
		throw std::runtime_error("\033[1;91mError while getting hostname on new client.\033[0m");
	_clients[new_fd] = new Client(new_fd,buf);
	pollfd pollfd = {new_fd, POLLIN, 0};
	_pollfds.push_back(pollfd);
}

void	Server::displayClient()
{
	std::string state;
	size_t i = _clients.size();
	std::map<int,Client*>::iterator it = _clients.begin();
	std::cout << GREEN << "Clients connected: " << i << RESET << std::endl;
	int j = 0;
	while(it != _clients.end())
	{
		state = (it->second->getState() == REGISTERED) ? "yes" : "no";
		std::cout << BLUE << "client[" << j << "]:" << " registered:" << state
		<< "   nick:" << it->second->getNickname()
		<< "   user:" << it->second->getUsername()
		<< "   realname:" << it->second->getFullname() << RESET << std::endl;
		j++;
		it++;
	}
	std::cout << std::endl;
	return ;
}
void	Server::deletChannelA(std::string nickName)
{
	std::map<std::string,Channel*>::iterator it = _channels.begin();
	std::vector<std::string> channel;
	while (it != _channels.end())
	{
		if(it->second->checkIfIsClient(nickName)  && it->second->get_nbr_client() == 1)
			channel.push_back(it->first);
		else if(it->second->checkIfIsClient(nickName) || it->second->checkIfInviteToChannel(nickName))
			it->second->removeClientNickName(nickName);
		it++;
	}
	for (size_t i = 0; i < channel.size(); i++)
	{
		delete _channels[channel.at(i)];
		_channels.erase(channel.at(i));
	}
}
void	Server::eraseClient(int fd)
{
	std::map<int,Client*>::iterator it = _clients.begin();

	for (; it != _clients.end(); ++it)
	{
		if (it->second->getFd() == fd)
		{
			deletChannelA(it->second->getNickname());
			delete this->_clients[fd];
			_clients.erase(it);
			return ;
		}
	}
}

void	Server::clientDisconnect(int fd)
{
	std::vector<pollfd>::iterator it = _pollfds.begin();
	eraseClient(fd);
	while (it != _pollfds.end())
	{
		if (it->fd == fd)
		{
			_pollfds.erase(it);
			break ;
		}
		it++;
	}
	close(fd);
	std::cout << RED << "A client has been disconnected." << RESET << std::endl;
}

std::vector<std::string> Server::splitCommands(std::string msg)
{
    std::vector<std::string> cmd;
    std::string part;
    std::istringstream iss(msg);

    while (std::getline(iss, part, ' ')) 
	{
        if (part.find(':')  == 0) 
		{
            std::string restOfMessage = part.substr(1,part.size());
            std::string remaining;
            if (std::getline(iss, remaining)) 
                restOfMessage += " " + remaining;
            cmd.push_back(restOfMessage);
            return cmd;
        }
		if(!part.empty())
			cmd.push_back(part);
    }
    return cmd;
}
void Channel::clearTopic()
{
	topic.clear();
}
std::map<int,Client*>::iterator	Server::findClientIt(int fd)
{
	std::map<int,Client*>::iterator ret = _clients.find(fd);
	if(ret != _clients.end())
		return ret;
	throw (std::out_of_range("\033[1;91mError while searching for user3\033[0m"));
}

void	Server::readMessage(int fd)
{
	std::string	msg;
	char	buff[256];
	bzero(buff, 256);
	std::map<int,Client*>::iterator cl = findClientIt(fd);
	msg = cl->second->getMsg();
	while (!(std::strstr(buff, "\n")))
	{
		int	k = 0;
		bzero(buff, 256);
		if ((k = recv(fd, buff, 256, MSG_DONTWAIT)) < 0)
		{
			if (errno != EWOULDBLOCK)
				throw std::runtime_error("\033[1;91merror in reciving message\033[0m");
		}
		else if (k == 0)
			throw (std::out_of_range("\033[1;91mTEST\033[0m"));
		cl->second->setMsg(buff);
		msg += buff;
	}
	cl->second->setMsg("");
	std::string command ;
	std::istringstream is(msg);
	while (getline(is,command,'\n'))
	{
		_cmd = command;
		if(!del_break(_cmd).empty())
			parseCmd(_cmd, *findClient(fd),fd);
	}
}

void	Server::handleMessage(int fd)
{
	try
	{
		readMessage(fd);
	}
	catch (const std::exception& e)
	{
		clientDisconnect(fd);
		std::cerr << e.what() << '\n';
		return ;
	}
	displayClient();
}

void	Server::parseCmd(std::string str, Client &cl,int fd)
{
    std::string	tmp;
	std::stringstream	ss(str);
	std::getline(ss, tmp, ' ');
	std::string cmdu[10] = {"PASS", "NICK", "USER", "JOIN","MODE", "TOPIC", "KICK", "PART", "INVITE", "PRIVMSG"};
	std::string cmds[10] = {"pass", "nick", "user", "join","mode", "topic", "kick", "part", "invite", "privmsg"};

	int		(Server::*ptr[10])(std::string , Client &cl) = {
			&Server::cmdPass,
			&Server::cmdNick,
			&Server::cmdUser,
			&Server::JOIN,
			&Server::MODE,
			&Server::TOPIC,
			&Server::KICK,
			&Server::PART,
			&Server::INVITE,
			&Server::PRIVMSG
	};
	for (int i = 0; i <= 9; ++i)
	{
		if (del_break(tmp) == cmds[i] || del_break(tmp) == cmdu[i])
		{
			(this->*ptr[i])(del_break(_cmd), cl);
			return ;
		}
	}
	send(fd,(ERR_UNKNOWNCOMMAND(cl.getHost(),cl.getNickname(),del_break(tmp))).c_str(),strlen((ERR_UNKNOWNCOMMAND(cl.getHost(),cl.getNickname(),del_break(tmp))).c_str()),0);
}
 
int		Server::findClientInS(std::string nickname)
{
	std::map<int,Client*>::iterator cl = _clients.begin();
	while(cl != _clients.end())
	{
		if (cl->second->getNickname().compare(nickname) == 0)
			return (1);
		cl++;
	}
	return 0;
}

Client		*Server::findClient(int fd)
{
	std::map<int,Client*>::iterator cl = _clients.find(fd);
	if(cl != _clients.end())
		return cl->second;
	return NULL;
}

Client::Client(int sockfd, std::string hostname) : sockfd(sockfd), host(hostname)
{
	state = HANDSHAKE;
	msg = "";
}

Client::~Client() {}

std::string Client::getPrefix()
{
	std::string prefix = ":" + nick + (username.empty() ? "" : "!" + username) + (host.empty() ? "" : "@" + host);
	return (prefix);
}

void	Client::reply(std::string msg)
{
	if (send(sockfd, msg.c_str(), msg.length(), 0) < 0)
		throw (std::runtime_error("\033[1;91mError while sending message\033[0m"));
}
void			Server::joinZero(Client &c)
{
	std::map<std::string,Channel*>::iterator it  = _channels.begin();
    std::string cmd = "PART ";
    while (it != _channels.end())
    {
        if(it->second->checkIfIsClientNickName(c.getNickname()))
        {
            cmd += it->first+",";
        }
        it++;
    }
	PART(cmd,c);
}
void	Server::launch()
{
	pollfd fd_server = {_sock, POLLIN, 0};
	_pollfds.push_back(fd_server);


	std::cout << I_GREEN << "Server is started on port " << _port << RESET << std::endl;
	while (g_interrupt == false)
	{
		if (poll(_pollfds.begin().base(), _pollfds.size(), -1) < 0)
			break ;
		for (unsigned int i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents == 0)
				continue ;
			if ((_pollfds[i].revents & POLLIN) == POLLIN)
			{
				if (_pollfds[i].fd == _sock)
				{
					newCl();
					displayClient();
					break ;
				}
			}
			handleMessage(_pollfds[i].fd);
		}
	}
	for (unsigned int i = 0; i < _pollfds.size(); i++)
		close(_pollfds[i].fd);
}

void	Client::welcome()
{
	if (state != LOGIN || nick.empty() || username.empty())
	{
		std::cout << YELLOW << "Waiting registration... " << nick << std::endl;
		return ;
	}
	state = REGISTERED;
	reply(REPLY_WELCOME(nick,username,host));
	reply(REPLY_YOURHOST(nick,host));
	time_t		creatchannelTime;
	time(&creatchannelTime);
	struct tm *tm_time = gmtime(&creatchannelTime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S UTC", tm_time);
	reply(REPLY_CREATED(nick,host,buffer));
	std::cout << B_CYAN << nick << " is registered" << RESET << std::endl;
}

Channel	*Server::getChannel(std::string &target)
{
	std::map<std::string,Channel*>::iterator i = _channels.find(target);
	if(i != _channels.end())
		return i->second;
	return NULL;
}

int     Server::checkIfChannelExist(std::string &target)
{
    std::map<std::string,Channel*>::iterator i = _channels.find(target);
	if(i != _channels.end())
		return 1;
	return 0;
}
void    Channel::removeClientNickName(std::string const &nickname)
{
	this->client.erase(nickname);
	this->invite.erase(nickname);
}

std::string getTimeSc()
{
	time_t		creatchannelTime;
	std::stringstream		ss;
	time(&creatchannelTime);
	ss << creatchannelTime;
    return ss.str();

}

Channel::Channel(std::string _name,std::string password):name(_name),password(password)
{
	isKey = password.size() == 0 ? false : true;
	if(isKey)
		mode.insert('k');
	isInviteOnly = false;
	isTopic = false;
    timeScCh = getTimeSc();
	mode.insert('t');
	islimit = false;
	chTopicOp = true;
	topic = "";
	max_client = -1;
	
}
void Channel::clearInvite()
{
	this->invite.clear();
}
void	Channel::addIviteClient(int fd, const std::string& nickname)
{
	this->invite[nickname] = fd;
}
std::string Channel::getTime()
{
	return this->timeScCh;
}
std::string Channel::getTimeTop()
{
	return this->timeScTo;
}
int Channel::getModeSize()
{
	return this->mode.size();
}
void Channel::setIsTopic(bool istopic)
{
	this->isTopic = istopic;
}
bool Channel::getIsTopic()
{
	return this->isTopic;
}
void Channel::setChTopOp(bool chtopic)
{
	this->chTopicOp = chtopic;
}
bool Channel::getChTopOp()
{
	return this->chTopicOp;
}
void Channel::setTimeTop(std::string const &time)
{
			this->timeScTo = time;
}
void Channel::setTime(std::string const &time)
{
	this->timeScCh = time;
}


bool	Channel::getLimit() const
{
	return islimit;
}
std::string Channel::get_name() const
{
    return name;
}
bool Channel::getInviteOnly() const
{
	return isInviteOnly;
}
std::string Channel::get_password() const
{
    return password;
}
std::string Channel::getTopic() const
{
    return topic;
}
long long Channel::get_nbr_client() const
{
    return client.size();
}
bool Channel::getKey() const
{
	return isKey;
}
long long Channel::get_max_client() const
{
    return max_client;
}
void Channel::setMaxClient(long long m)
{
    max_client = m;
}
void Channel::setInviteOnly(bool in)
{
	isInviteOnly = in;
}
void Channel::setName(std::string &_name)
{
    name = _name;
}
void Channel::setPassword(std::string &_password)
{
    password = _password;
}
void Channel::setTopic(std::string &_topic)
{
    topic = _topic;
}
void Channel::setKey(bool b)
{
	this->isKey = b;
}
void Channel::setLimit(bool l)
{
	islimit  = l;
}

void    Channel::setClientRole(std::string const &nickname, bool role)
{
    std::map<std::string,std::pair<bool,int> >::iterator v = client.find(nickname);
	v->second.first = role;
}

void	Channel::changeNickName(std::string oldNickname,std::string newNickname)
{
	client[newNickname] = client[oldNickname];
	client.erase(oldNickname);
}
int Channel::checkIfIsClient(std::string const &nickname)
{
	if(client.find(nickname) != client.end())
        return 1;
    return 0;
}

int Channel::checkIfIsClientNickName(std::string name)
{
    std::map<std::string,std::pair<bool,int> >::iterator v = client.find(name);
	if(v != client.end())
		return 1;
    return 0;
}
int Channel::findClientRole(std::string nikname)
{
    std::map<std::string,std::pair<bool,int> >::iterator v = client.find(nikname);
	if(v != client.end())
		return v->second.first;
    return 0;
}

void Channel::addClient(int fd, const std::string& nickname, bool isOperator) {
        client[nickname] = std::pair<bool,int>(isOperator,fd);
    }

int	Channel::checkIfInviteToChannel(std::string nickName)
{
	std::map<std::string,int>::iterator v = invite.find(nickName);
	if(v != invite.end())
		return 1;
	return 0;
}


Client		*Server::getClientByNickNameS(std::string Nickname)
{
	std::map<int,Client*>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if(it->second->getNickname().compare(Nickname) == 0)
			return it->second;
		it++;
	}
	return(NULL);
}
std::string	Channel::get_list_of_names()
{
	std::string	names;
	std::map<std::string,std::pair<bool,int> >::iterator itr = client.begin();
	while (itr != client.end())
	{
		if (itr->second.first)
			names += "@" + itr->first + " ";
		else
			names += itr->first + " ";
		itr++;
	}
	return names;
}

void	Channel::sendReplyAll(const std::string &msg,std::string nickname)
{
		std::map<std::string,std::pair<bool,int> >::iterator it = client.begin();
		while (it != client.end())
		{
			if (it->first.compare(nickname) != 0 )
			{
				if(send(it->second.second, msg.c_str(), msg.length(), 0) < 0)
					return;
			}
			it++;
		}
}

void Channel::setMode(char s)
{
	this->mode.insert(s);
}
void Channel::eraseMode(char s)
{
	this->mode.erase(s);
}
std::string Channel::getMode(int i)
{
	std::string modeStr = "+";
	std::string arg = " ";
    for (std::set<char>::const_iterator it = this->mode.begin(); it != this->mode.end(); ++it) {
		if(*it == 'k' && !i)
			continue;
		else if(*it == 'k')
		{
			arg+= this->password;
			arg+=" ";
		}
		else if(*it == 'l')
		{
			std::ostringstream oss;
			oss << this->max_client;
			arg+=oss.str();
			arg+=" ";
		}
        modeStr += *it;
    }
    return std::string(modeStr + arg);
}
bool Channel::hasMode(char mode) const {
    return this->mode.find(mode) != this->mode.end();
}
