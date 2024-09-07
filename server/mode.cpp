/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenaait <ibenaait@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 14:34:13 by ibenaait          #+#    #+#             */
/*   Updated: 2024/09/07 21:30:08 by ibenaait         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Channel.hpp"
#include "../includes/irc.hpp"
// #define MODE_USERMSG(client, mode) (":" + client + " MODE " + client + " :" + mode + "\r\n")
// #define ERR_UMODEUNKNOWNFLAG(client) (":localhost 501 " + client + " :Unknown MODE flag\r\n")
// #define ERR_USERSDONTMATCH(client) ("502 " + client + " :Cant change mode for other users\r\n")
// #define RPL_UMODEIS(client, mode) (":localhost 221 " + client + " " + mode + "\r\n")
// # define ERR_NEEDMOREPARAMS(Client, command) (":localhost 461 " + client + " " + command + " :Not enough parameters.\r\n")


int    Server::MODE(std::vector<std::string> args, Client &c)
{

    try
    {
        if(c.getState() != REGISTERED)
            return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
        if(args.size() < 3)
            throw(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"MODE #channel +-iokl xx"));
        std::string s = args[3];
        std::string target = args[1];
        std::string mode = args[2];
        if(!ft_check_if_channel_exist(target) && target[0] == '#')
            throw(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target));
        // else if(c.getNickname() != target)
        //     throw(ERR_USERSDONTMATCH(c.getNickname(),c.getHost()));
        // if(!findClientIn(target))
        //     throw(ERR_NOSUCHNICK(c.getHost(),c.getNickname(),target));
        bool flag = true;
        if(mode[0] == '-') flag = false;
        else if(mode[0] == '+') flag = true;
        std::string f = flag == true ? "+":"-";
        Channel &ch = ft_get_channel(target);
        std::string nick = c.getNickname();
        // if(args.size() >= 4)
        //     std::vector<std::string>::iterator it = args.begin();
        for(size_t i= 1; i < mode.size() ;i++)
        {
            
            if(mode[i] == 'i')
            {
                if(!ch.find_Client_role(c))
                    throw(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
                if(flag && ch.getInviteOnly())
                    c.reply(RPL_ALLINV(nick,c.getHost()));
                else if(!flag && !ch.getInviteOnly())
                    c.reply(RPL_NOTINV(nick,c.getHost()));
                else 
                {
                    ch.setInviteOnly(flag);
                    ch.sendReplyAll(RPL_MODE(ch.get_list_of_names(),nick,f+"i"),nick);
                }
            }
            else if(mode[i] == 'k')
            {
                if(args.size() < 4)
                    throw(ERR_NEEDMOREPARAMS(nick,c.getHost(),"MODE "+target+" "+f+"k KEY"));
                if(!ch.find_Client_role(c))
                    throw(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
                if(flag && ch.getKey())
                    throw(ERR_KEYSET(c.getHost(),nick,target));
                if(!flag && !ch.getKey())
                    throw(ERR_NOKEYSET(c.getHost(),nick,target));
                std::cout << "KEY===="<< args[3] <<" "<<args[3].size()<<std::endl;
                std::cout << "size == "<<args[3].size()<<std::endl;
                ch.set_password(args[3]);
                ch.setKey(flag);
                ch.sendReplyAll(RPL_MODE(ch.get_list_of_names(),nick,f+"k"),nick);

            }else if(mode[i]== 't')
            {
                if(!ch.find_Client_role(c))
                    throw(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            }
            else if(mode[i] == 'l')
            {
                if(args.size() < 4)
                    throw(ERR_NEEDMOREPARAMS(nick,c.getHost(),"MODE "+target+" "+f+"l limit"));
                if(!ch.find_Client_role(c))
                    throw(ERR_CHANOPRIVSNEEDED(c.getHost(),target));
            }
        }
    }
        catch(const std::string& e)
    {
        c.reply(e);
    }
    
    
    return 0;
}
