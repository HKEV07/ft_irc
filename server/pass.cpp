#include "../includes/Server.hpp"

int Server::cmdPass(std::string arg, Client &cl)
{
	std::vector<std::string> pswrd = splitCommands(del_break(arg));
	if (pswrd.size() < 2)
		return cl.reply(ERR_NEEDMOREPARAMS(cl.getNickname(),cl.getHost(),"PASS")),-1;
	if(cl.getState() == REGISTERED)
        return cl.reply(ERR_ALREADYREGISTERED(cl.getHost(),cl.getNickname())),-1;
	std::string mdp = del_break(pswrd[1]);
	if (mdp != _password)
		return cl.reply(ERR_PASSWDMISMATCH(cl.getNickname(),cl.getHost())),-1;
	cl.setState(LOGIN);
	displayClient();
	cl.welcome();
	return (0);
}
