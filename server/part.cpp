
#include "../includes/Server.hpp"

std::vector<std::string> splitChannelA(std::string msg)
{
	std::vector<std::string>	cmd;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
    while (std::getline(str, tm, ','))
	{
        if (!tm.empty())
		    cmd.push_back(tm);
	}
	return (cmd);
}
int    Server::PART(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(cmd);
    if(args.size() < 2)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"PART")),1;
    std::string reason = args.size() >= 3 ? args[2] :"";
    std::vector<std::string> vec = splitChannelA(args[1]);
    for (size_t i = 0; i < vec.size(); i++)
    {
        Channel *ch = getChannel(vec.at(i));
        if(!ch)
            c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),vec.at(i)));
        else if(ch->checkIfIsClient(c.getNickname()) == 0)
            c.reply(ERR_NOTONCHANNEL(c.getHost(),vec.at(i)));
        else
        {
            c.reply(PART_REPLY(c.getNickname(),c.getUsername(),c.getHost(),vec.at(i),reason));
            ch->sendReplyAll(PART_REPLY(c.getNickname(),c.getUsername(),c.getHost(),vec.at(i),reason),c.getNickname());
            ch->removeClientNickName(c.getNickname());
            if(ch->get_nbr_client() == 0)
            {
                delete _channels[vec.at(i)];
                _channels.erase(vec.at(i));
            }
        }
    }
    displayClient();
    return 0;
}