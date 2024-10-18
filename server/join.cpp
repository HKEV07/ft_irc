
#include "../includes/Server.hpp"

std::vector<std::string> splitChannelandKey(std::string msg)
{
	std::vector<std::string>	cmd;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
    while (std::getline(str, tm, ','))
	{
        if(!tm.empty())
		    cmd.push_back(tm);
	}
	return (cmd);
}

int Server::JOIN(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(cmd);
    if (args.size() < 2) 
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"JOIN")),1;
    if(!args[1].compare("0"))
        return joinZero(c),1;
    std::vector<std::string> channels = splitChannelandKey(args[1]);
    std::vector<std::string> key;
    if(args.size() >= 3)
        key = splitChannelandKey(args[2]);
    std::string keys;
    for (size_t i = 0;i < channels.size();i++)
    {
        if (channels[i][0] != '#' || channels[i].find(' ') != std::string::npos || channels[i].size() == 1)
        {
            c.reply(ERR_BADCHANNELNAME(c.getNickname(),c.getHost(),channels[i]));
            continue;
        }
        keys = i < key.size() && !key[i].empty() && (key[i].find(' ') == std::string::npos)  ? key[i]:"";
        if (!checkIfChannelExist(channels[i]))
        {
            Channel *ch = new Channel(channels[i],keys);
            ch->addClient(c.getFd(),c.getNickname(),true);
            _channels[channels[i]] = ch;
            c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels[i],c.getHost()));
            c.reply(RPL_NAMREPLY(c.getHost(),ch->get_list_of_names(),channels[i],c.getNickname()));
            c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels[i]));
        }else
        {
            Channel *cn = getChannel(channels[i]);
            if(cn->checkIfIsClientNickName(c.getNickname()))
                c.reply(ERR_USERONCHANNEL(c.getHost(),c.getNickname(),channels[i]));
            else if(cn->getLimit() && cn->get_max_client() <= cn->get_nbr_client())
                c.reply(ERR_CHANNELISFULL(c.getHost(),c.getNickname(),channels[i]));
            else if(cn->getInviteOnly() && !cn->checkIfInviteToChannel(c.getNickname()))
                c.reply(ERR_INVITEONLY(c.getHost(),c.getNickname(),channels[i]));
            else if(cn->getKey() && cn->get_password().compare(keys) != 0 && !cn->checkIfInviteToChannel(c.getNickname()))
                c.reply(ERR_BADCHANNELKEY(c.getNickname(),c.getHost(),channels[i]));
            else 
            {
                cn->addClient(c.getFd(),c.getNickname(),false);
                c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels[i],c.getHost()));
                if(cn->getIsTopic())
                {
                    c.reply(RPL_TOPICDISPLAY(c.getHost(),c.getNickname(),channels[i],cn->getTopic()));
                    c.reply(REPLY_TOPICWHOTIME(c.getUsername(),cn->getTimeTop(),c.getNickname(),c.getHost(),channels[i]));
                }
                else
                    c.reply(RPL_NOTOPIC(c.getHost(),c.getNickname(),channels[i]));
                cn->sendReplyAll(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getHost() + " JOIN " + channels[i] + "\r\n", c.getNickname());
                c.reply(RPL_NAMREPLY(c.getHost(),cn->get_list_of_names(),channels[i],c.getNickname()));
                c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels[i]));
            }
        }
    }
    return 0;
}