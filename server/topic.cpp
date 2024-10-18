#include "../includes/Server.hpp"

std::vector<std::string> splitCommandsT(std::string msg)
{
    std::vector<std::string> cmd;
    std::string part;
    std::istringstream iss(msg);

    while (std::getline(iss, part, ' ')) 
	{
        if (part.find(':')  == 0) 
		{
            std::string restOfMessage = part.substr(0,part.size());
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

std::vector<std::string> splitCommandSpace(std::string msg)
{
	std::vector<std::string>	cmd;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
    while (std::getline(str, tm, ' '))
	{
        if (!tm.empty())
		    cmd.push_back(tm);
	}
	return (cmd);
}


int    Server::TOPIC(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;  
    std::vector<std::string> args = splitCommandsT(cmd);
    if(args.size() < 2)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"TOPIC")),1;
    std::string target = args[1];
    Channel *ch = getChannel(target);
    if(!ch)
        return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
    if(ch->checkIfIsClient(c.getNickname()) == 0)
        return c.reply(ERR_NOTONCHANNEL(c.getHost(),target)),1;
    if(args.size() == 2)
    {
        if(!ch->getIsTopic())
            c.reply(RPL_NOTOPIC(c.getHost(),c.getNickname(),target));
        else
        {
            c.reply(RPL_TOPICDISPLAY(c.getHost(),c.getNickname(),target,ch->getTopic()));
            c.reply(REPLY_TOPICWHOTIME(c.getUsername(),ch->getTimeTop(),c.getNickname(),c.getHost(),target));
        }
    }
    else
    {
        if(!ch->findClientRole(c.getNickname()) && ch->getChTopOp())
            c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
        else
        {
            std::vector<std::string> v = splitCommandSpace(args[2]);
            std::string str = args[2];
            if(v.size() == 1 && v.at(0)[0] == ':' && v.at(0).size() == 1)
            {
                ch->clearTopic();
                ch->setIsTopic(false);
                c.reply(RPL_TOPIC(c.getNickname(),c.getUsername(),c.getHost(),target,":"));
                ch->sendReplyAll(RPL_TOPIC(c.getNickname(),c.getUsername(),c.getHost(),target,":"),c.getNickname());
            }else
            {
                if(v.size() == 1 && v.at(0)[0] == ':')
                    str = args[2].substr(1,args[2].size()-1);
                ch->setTopic(str);
                ch->setIsTopic(true);
                ch->setTimeTop(getTimeSc());
                c.reply(RPL_TOPIC(c.getNickname(),c.getUsername(),c.getHost(),target,ch->getTopic()));
                ch->sendReplyAll(RPL_TOPIC(c.getNickname(),c.getUsername(),c.getHost(),target,ch->getTopic()),c.getNickname());
            }
        }
    }
    return 0;
}
