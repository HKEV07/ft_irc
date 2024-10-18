
#include "../includes/Server.hpp"

std::vector<std::string> splitNickName(std::string msg)
{
	std::vector<std::string>	cmd;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
    while (std::getline(str, tm, ','))
	{
        if (tm.empty())
		    return (cmd);
		cmd.push_back(tm);
	}
	return (cmd);
}
int    Server::KICK(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(cmd);
    if(args.size() < 3)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"KCIK")),1;
    std::string target = args[1];
    Channel *ch = getChannel(target);
    if(!ch)
        return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
    if(ch->checkIfIsClient(c.getNickname()) == 0)
        return c.reply(ERR_NOTONCHANNEL(c.getHost(),target)),1;
    if(!ch->findClientRole(c.getNickname()))
        return c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target)),1;
    std::vector<std::string> vec = splitNickName(args[2]);
    std::string reason = args.size() >= 4 ? args[3] : ":Kicked by the channel's operator";
    for(size_t i = 0;i < vec.size();i++)
    {
        if(ch->findClientRole(c.getNickname()) == 0)
            c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
        else if(ch->checkIfIsClient(vec.at(i)) == 0)
            c.reply(ERROR_USERNOTINCHANNEL(c.getHost(),target,vec.at(i)));
        else
        {
            c.reply(RPL_KICK(c.getNickname(),c.getUsername(),c.getHost(),target,vec.at(i),reason));
            ch->sendReplyAll(RPL_KICK(c.getNickname(),c.getUsername(),c.getHost(),target,vec.at(i),reason),c.getNickname());
            ch->removeClientNickName(vec.at(i));
        }
    }
    if(ch->get_nbr_client() == 0)
    {
        delete _channels[target];
        _channels.erase(target);
    }
    return 0;
}