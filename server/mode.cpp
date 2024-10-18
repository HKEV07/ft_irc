
#include "../includes/Server.hpp"

long long	aatoi(const char * str)
{
	size_t	i;
	long long	s;

	i = 0;
	s = 0;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			return -1;
		i++;
	}
	while (str[i])
	{
		if (std::isalpha(str[i]))
			return (-2);
		s = (s * 10) + (str[i] - '0');
        s = s > INT_MAX ? INT_MAX : s;
		i++;
	}
	return (s);
}
int     invalidMode(std::string mode)
{
    bool flag = false;
    bool flag1 = false;
    for (size_t i = 0; i < mode.size(); i++)
    {
        if(mode[i] == '-' || mode[i] == '+')
            flag = true;
        else if(mode[i] == 'i' || mode[i]=='k' || mode[i] == 't' || mode[i] == 'l'|| mode[i] == 'o')
            flag1 = true;
    }
    if((flag == true && flag1 == true) || flag1 == true)
        return 1;
    return 0;
}
std::string makeMode(std::string modeStr)
{
    int n = modeStr[0] == '+' ? 1 : 0;
    std::string resulta = n == 1 ? "+" : "-";
    for (size_t i = 1; i < modeStr.size(); i++)
    {
        if(modeStr[i] == '+' || modeStr[i] == '-')
        {
            int m = modeStr[i] == '+' ? 1 : 0;
            if(n == m)
                continue;
            else
            {
                n = m;
                resulta += modeStr[i];
            }
        }else
            resulta += modeStr[i];
    }
    return resulta;
}
int    Server::MODE(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(cmd);
    if(args.size() == 1)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"MODE #channel +-iotkl xx")),1;
    std::string target = args[1];
    Channel *ch = getChannel(target);
    if(!ch)
    {
        if(target[0] == '#')
            return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
        if(c.getNickname().compare(target) == 0)
            return c.reply(RPL_UMODEIS(c.getHost(),c.getNickname(),"+i")),1;
        if(getClientByNickNameS(target))
            return c.reply(ERR_USERSDONTMATCH(c.getHost(),c.getNickname())),1;
        return c.reply(ERR_NOSUCHNICK(c.getHost(),c.getNickname(),target)),1;
    }
    
    if(args.size() == 2 || (args.size() == 3 && !invalidMode(args[2])))
    {
        if(ch->checkIfIsClient(c.getNickname()))
            c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),ch->get_name(),ch->getMode(1)));
        else
            c.reply(RPL_CHANNELMODEIS(c.getHost(),c.getNickname(),ch->get_name(),ch->getMode(0)));
        c.reply(RPL_CREATIONTIME(c.getHost(),c.getNickname(),ch->get_name(),ch->getTime()));
        return 1;
    }
    std::string mode = args[2];
    std::string as,m;
    bool flag  = true;
    if(!ch->checkIfIsClientNickName(c.getNickname()))
        return c.reply(ERR_NOTONCHANNEL(c.getHost(),c.getNickname())),1;
    if(!ch->findClientRole(c.getNickname()))
        return c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target)),1;
    std::string nick = c.getNickname();
    std::vector<std::string>::iterator it = args.begin()+3;
    for(size_t i= 0; i < mode.size() ;i++)
    {
        if(mode[i] == '-') flag = false;
        else if(mode[i] == '+') flag = true;
        else if(mode[i] == 'i')
        {
            if(flag && ch->getInviteOnly())
                continue;
            else if(!flag && !ch->getInviteOnly())
                continue;
            else 
            {
                std::string f = flag == true ? "+":"-";
                as+=f+"i";
                ch->setInviteOnly(flag);
                ch->clearInvite();
                if(flag)
                    ch->setMode('i');
                else
                    ch->eraseMode('i');
            }
        }
        else if(mode[i] == 'k')
        {
            if(it == args.end())
                c.reply("Reply(696): "+target+" k * You must specify a parameter for the key mode. Syntax: <key>.\r\n");
            else
            {
                if(!flag)
                {
                    ch->setKey(flag);
                    ch->eraseMode('k');
                    m += "* ";
                }
                else 
                {
                    if(it->find(' ') != std::string::npos)
                    {
                        c.reply(ERROR_INVALIDMODEPARAM__KEY(c.getNickname(),target,c.getHost(),*it));
                        it++;
                        continue;
                    }
                    ch->setPassword(*it);
                    ch->setKey(flag);
                    m += *it+" ";
                    ch->setMode('k');
                    
                }
                it++;
                std::string f = flag == true ? "+":"-";
                as+=f+"k";
            } 
        }else if(mode[i]== 't')
        {
            if(ch->getChTopOp() && flag)
                continue;
            else if(!ch->getChTopOp() && !flag)
                continue;
            else
            {
                std::string f = flag == true ? "+":"-";
                as+=f+"t";
                ch->setChTopOp(flag);
                if(flag)
                    ch->setMode('t');
                else
                    ch->eraseMode('t');
            }
        }
        else if(mode[i] == 'l')
        {
            if(it == args.end() && flag)
                c.reply(ERROR_INVALIDMODEPARAM_LIMIT(target,c.getHost(),"*"));
            else if(!ch->getLimit() && !flag)
                continue;
            else 
            {
                if(!flag)
                {
                    ch->setLimit(flag);
                    ch->setMaxClient(-1);
                    ch->eraseMode('l');
                }
                else
                {
                    if(aatoi(it->c_str()) >= INT_MAX)
                    {
                        it++;
                        continue;
                    }if(aatoi(it->c_str()) <= -1)
                    {
                        c.reply(ERROR_INVALIDMODEPARAM_LIMIT(target,c.getHost(),*it));
                        it++;
                        continue;
                    }
                    else
                    {
                        ch->setLimit(flag);
                        ch->setMaxClient(aatoi(it->c_str()));
                        ch->setMode('l');
                        m += *it+" ";
                    }
                    it++;
                }
                std::string f = flag == true ? "+":"-";
                as+=f+"l";
            }
        }
        else if(mode[i] ==  'o')
        {
            if(it == args.end())
                c.reply(ERROR_INVALIDMODEPARAM_OP(target,c.getHost()));
            else if(!ch->checkIfIsClientNickName(*it))
                c.reply(ERR_NOSUCHNICK(c.getHost(),c.getNickname(),*it));
            else
            {
                if((ch->findClientRole(*it) && flag) || (!ch->findClientRole(*it) && !flag))
                {
                    it++;
                    continue;
                }
                else
                {
                    std::string f = flag == true ? "+":"-";
                    as+=f+"o";
                    m += *it+" ";
                    ch->setClientRole(*it,flag);
                    it++;
                }
            }
            
        }else
            c.reply(ERR_UNKNOWNMODE(c.getNickname(),c.getHost(),target,mode[i]));
    }
    if(!as.empty())
    {
        c.reply(RPL_MODE(c.getNickname(),c.getUsername(),c.getHost(),target,makeMode(as),m));
        ch->sendReplyAll(RPL_MODE(c.getNickname(),c.getUsername(),c.getHost(),target,makeMode(as),m),c.getNickname());
    }
    return 0;
}
