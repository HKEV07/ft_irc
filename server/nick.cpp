#include "../includes/Server.hpp"

bool	Server::is_used(Client cl, std::string name)
{
	for (std::map<int,Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it->second->getNickname() == name && it->second->getFd() != cl.getFd())
			return (true);
	}
	return (false);
}

bool	isSpecial(char c)
{
	if (c >= 91 && c <= 96)
		return (true);
	else if (c >= 123 && c <= 125)
		return (true);
	else
		return (false);
}

bool	is_valid_nick(std::string name)
{
	if (!(isSpecial(name[0]) || isalpha(name[0])))
		return (false);
	for (size_t i = 1; i < name.size(); ++i)
	{
		if (!(isalnum(name[i]) || isSpecial(name[i]) || name[i] == '-'))
			return (false);
	}
	return (true);
}
std::string NICK_SUCCESS(std::string newNick)
{
	return ("\033[1;92mNICK " + newNick + "\033[0m\r\n");
}

int Server::cmdNick(std::string cmd, Client& client)
{
	std::vector<std::string> args = splitCommands(cmd);
	if (args.size() < 2)
		return (client.reply(ERR_NONICKNAMEGIVEN(client.getNickname(),client.getHost())), -1);
	std::string requestedNick = args[1];
	if (!is_valid_nick(requestedNick))
		return (client.reply(ERR_ERRONEUSNICKNAME(client.getNickname(),client.getHost(),requestedNick)), -1);
	if (is_used(client, requestedNick))
		return (client.reply(ERR_NICKNAMEINUSE(client.getNickname(),client.getHost())), -1);
	if(client.getState() == REGISTERED)
	{
		client.reply(RPL_NICKCHANGE(client.getNickname(),requestedNick,client.getUsername(),client.getHost()));
		std::map<std::string,Channel*>::iterator it = _channels.begin();
		while (it != _channels.end())
		{
			if(it->second->checkIfIsClient(client.getNickname()))
			{
				it->second->sendReplyAll(RPL_NICKCHANGE(client.getNickname(),requestedNick,client.getUsername(),client.getHost()),client.getNickname());
				it->second->changeNickName(client.getNickname(),requestedNick);
			}
			it++;
		}
	}
	else
	{
		if(client.getState() != LOGIN)
        	return client.reply(ERR_NOTREGISTERED(client.getNickname(),client.getHost())),-1;///bdloooh
		client.reply(NICK_SUCCESS(requestedNick));
	}
	client.setNickname(requestedNick);
	displayClient();
	client.welcome();
	return (0);
}
