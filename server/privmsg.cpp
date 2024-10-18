
#include "../includes/Server.hpp"

std::vector<std::string> splitByComa(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str); 
    std::string item;

    while (std::getline(ss, item, ',')) {
        if(!item.empty())
            result.push_back(item);
    }
    return result;
}

int Server::PRIVMSG(std::string cmd, Client& client)
{
    if(client.getState() != REGISTERED)
        return client.reply(ERR_NOTREGISTERED(client.getNickname(),client.getHost())),1;
    std::vector<std::string> receiver;
    std::vector<std::string> args = splitCommands(cmd);
    int num = args.size();
    if (num < 2)
        return(client.reply(ERR_NORECIPIENT(client.getHost(),client.getNickname(),"PRIVMSG #channle Message")),1);
    else if (num < 3)
        return(client.reply(ERR_NOTEXTTOSEND(client.getHost(),client.getNickname())),1);
    else
    {
        receiver = splitByComa(args[1]);
        for(size_t i = 0 ; i < receiver.size();i++)
        {
            if(receiver.at(i)[0] == '#')
            {
                Channel* ch = this->getChannel(receiver.at(i));
                if(ch == NULL)
                    client.reply(ERR_NOSUCHCHANNEL(client.getHost(), client.getNickname(), receiver.at(i)));
                else if (ch->checkIfIsClientNickName(client.getNickname()) == 0)
                    client.reply(ERR_CANNOTSENDTOCHAN(client.getHost(), client.getNickname(),receiver.at(i)));
                else
                    ch->sendReplyAll(RPL_PRIVMSG(client.getNickname(),client.getUsername(),client.getHost(),receiver.at(i),args[2]),client.getNickname());
            }
            else
            {
                Client *cl = getClientByNickNameS(receiver.at(i));
                if(!cl || cl->getState() != REGISTERED)
                {
                    client.reply(ERR_NOSUCHNICK(client.getHost(),client.getNickname(),receiver.at(i)));
                    continue;
                }
                cl->reply(RPL_PRIVMSG(client.getNickname(),client.getUsername(),client.getHost(),receiver.at(i),args[2]));
            }
        }
    }
    return(0);
}