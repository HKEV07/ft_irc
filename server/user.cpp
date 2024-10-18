#include "../includes/Server.hpp"

int	Server::cmdUser(std::string cmds, Client &cl)
{
	
	std::vector<std::string> args = splitCommands(del_break(cmds));
	if (args.size() < 5)
		return cl.reply(ERR_NEEDMOREPARAMS(cl.getNickname(),cl.getHost(), "USER")),-1;
	if(cl.getState() == REGISTERED)
        return cl.reply(ERR_ALREADYREGISTERED(cl.getHost(),cl.getNickname())),-1;
	if(cl.getState() != LOGIN)
        return cl.reply(ERR_NOTREGISTERED(cl.getNickname(),cl.getHost())),-1;
	if (args.size() >= 5)
	{
		std::string	tmp;
		size_t i;
		cl.setUsername(args.at(1));
		for (i = 1; i < args.size() - 1; i++)
		{
			if (args.at(1)[0] == ':')
				break ;
		}
		if (args.at(1)[0] == ':')
			tmp = args.at(i).substr(1);
		else
			tmp = args.at(i);
		if (i + 1 == args.size())
		{
			cl.setFullname(tmp);
			cl.welcome();
			return (0);
		}
		i++;
		tmp += " ";
		for (; i < args.size() - 1; i++)
			tmp += args.at(i) + " ";
		tmp += args.at(i);
		cl.setFullname(tmp);
	}
	displayClient();
	cl.welcome();
	return (0);
}
