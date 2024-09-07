
#include "../includes/irc.hpp"
std::vector<std::string> splitChannelandKey(std::string msg)
{
	// int					i = 0;
	std::vector<std::string>	cmd;
	std::stringstream	str(msg);
	std::string				tm;
	if (msg == "\n")
		return (cmd);
	while (std::getline(str, tm, ','))
	{
		cmd.push_back(tm);
		// std::cout << cmd.at(i++) << std::endl;
	}
	return (cmd);
}
int    replyAllError(int i,Client &c,std::string channel)
{
    switch (i)
        {
            case 1:
                c.reply(":is Member\r\n");
                return 1;
            case 2:
                c.reply(ERR_CHANNELISFULL(c.getHost(),c.getNickname(),channel));
                return 2;
            case 3:
                c.reply(ERR_INVITEONLY(c.getHost(),c.getNickname(),channel));
                return 3;
            case 4:
                c.reply(ERR_BADCHANNELKEY(c.getNickname(),c.getHost(),channel));
                return 4;
            case 5:
                c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),channel));
            default:
                return 0;
        }
}

void REPLY_A(int a,int topic,Client &c,Channel &ch,std::string channels,std::string listname)
{
    (void)topic;
    c.reply(RPL_NOTOPIC(c.getNickname(),channels,c.getHost()));
	c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels,c.getHost()));
    if(a)
        ch.sendReplyAll(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getHost() + " JOIN " +channels + "\r\n", c.getNickname());
	c.reply(RPL_NAMREPLY(c.getHost(),listname,channels,c.getNickname()));
	c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels));
}
int Server::JOIN(std::vector<std::string> args, Client &c)
{

    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    if (args.size() == 1) 
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"JOIN")),1;
    std::vector<std::string> channels = splitChannelandKey(args[1]);
    std::vector<std::string> key = splitChannelandKey(args[2]);
    std::string keys;
    for (size_t i = 0;i < channels.size();i++)
    {
        keys = i < key.size() ? key[i]:"";
        if (channels[i][0] != '#')
        {
            c.reply(ERR_BADCHANNELNAME(c.getHost(),c.getNickname(),channels[i]));
            break;
        }
        if (!ft_check_if_channel_exist(channels[i]))
        {
            Channel ch(channels[i], keys);
            ch.ft_add_Member(c);
            set_Channel(ch);
            // c.reply(RPL_NOTOPIC(c.getNickname(),channels[i],c.getHost()));
			// c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels[i],c.getHost()));
			// c.reply(RPL_NAMREPLY(c.getHost(),ch.get_list_of_names(),channels[i],c.getNickname()));
			// c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels[i]));
            REPLY_A(0,0,c,ch,channels[i],ch.get_list_of_names());
        
        }else
        {
            try
            {
                Channel &cn = ft_get_channel(channels[i]);
                cn.ft_invite_channel(c, keys);
                // c.reply(RPL_NOTOPIC(c.getNickname(),channels[i],c.getHost()));
                // c.reply(RPL_JOIN(c.getNickname(),c.getUsername(),channels[i],c.getHost()));
                // cn.sendReplyAll(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getHost() + " JOIN " +channels[i] + "\r\n", c.getNickname());
                // c.reply(RPL_NAMREPLY(c.getHost(),cn.get_list_of_names(),channels[i],c.getNickname()));
                // c.reply(RPL_ENDOFNAMES(c.getHost(),c.getNickname(),channels[i]));
                REPLY_A(1,1,c,cn,channels[i],cn.get_list_of_names());

            }
            catch(const int & i)
            {
                if(replyAllError(i,c,channels[i]))
                    break;
            }
        }
    }
    return 0;
}
