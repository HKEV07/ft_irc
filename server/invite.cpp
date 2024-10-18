
#include "../includes/Server.hpp"

int    Server::INVITE(std::string cmd, Client &c)
{
    if(c.getState() != REGISTERED)
        return c.reply(ERR_NOTREGISTERED(c.getNickname(),c.getHost())),1;
    std::vector<std::string> args = splitCommands(cmd);
    if(args.size() < 3)
        return c.reply(ERR_NEEDMOREPARAMS(c.getNickname(),c.getHost(),"INVITE")),1;
    std::string target = args[2];
    Channel *ch = getChannel(target);
    if(!ch || !ch->get_nbr_client())
        return c.reply(ERR_NOSUCHCHANNEL(c.getHost(),c.getNickname(),target)),1;
    if(ch->checkIfIsClient(c.getNickname()) == 0)
        return c.reply(ERR_NOTONCHANNEL(c.getHost(),target)),1;
    if(!ch->findClientRole(c.getNickname()) && ch->getInviteOnly())
        return c.reply(ERR_CHANOPRIVSNEEDED(c.getHost(),target)),1;
    std::string invite = args[1];
    if(ch->checkIfIsClient(invite))
        return c.reply(ERR_USERONCHANNEL(c.getHost(),invite,target)),1;
    Client *cl = getClientByNickNameS(invite);
    if(!cl || cl->getState() != REGISTERED)
        return c.reply(ERR_NOSUCHNICK(c.getHost(),c.getNickname(),invite)),1;
    ch->addIviteClient(cl->getFd(),cl->getNickname());
    c.reply(RPL_INVITING(c.getHost(),c.getNickname(),cl->getNickname(),target));
    cl->reply(RPL_INVITE(c.getNickname(),c.getUsername(),c.getHost(),target,cl->getNickname()));
    return 0;
}