/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenaait <ibenaait@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 23:12:27 by ibenaait          #+#    #+#             */
/*   Updated: 2024/10/15 13:27:39 by ibenaait         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"

class Channel
{
    private:
        std::string name;
        std::string password;
        std::string topic;
        long long max_client;
        std::map<std::string,std::pair<bool,int> > client;////member
		std::map<std::string,int> invite;
		std::set<char> mode;
		std::string timeScTo;
		std::string timeScCh;
		std::string arg;
		bool isKey;
		bool islimit;
		bool isInviteOnly;
		bool isTopic;
		bool chTopicOp;
    public:
        Channel(std::string _name,std::string password);
		void clearTopic();
		int getModeSize();
        std::string get_name() const;
        std::string get_password() const;
        std::string getTopic() const;
		void clearInvite();
        long long get_nbr_client() const;
        long long get_max_client() const;
		void	changeNickName(std::string oldNickname,std::string newNickname);
		bool getLimit()const;
		bool getInviteOnly() const;
		bool getKey()const;
		void setMode(char s);
		void eraseMode(char s);
		std::string getMode(int i);
		bool hasMode(char mode) const;
		void setTime(std::string const &time);
		void setTimeTop(std::string const &time);
		std::string getTime();
		std::string getTimeTop();
		void setInviteOnly(bool in);
		void setLimit(bool limit);
        void setMaxClient(long long m);
        void setName(std::string &_name);
        void setPassword(std::string &_passord);
        void setTopic(std::string &_topic);
        void setClientRole(std::string const &nickname, bool role);
		void setKey(bool key);
		void setIsTopic(bool istopic);
		bool getIsTopic();
		void setChTopOp(bool istopic);
		bool getChTopOp();
		void	addClient(int fd, const std::string& nickname, bool isOperator);
		void	addIviteClient(int fd, const std::string& nickname);
		std::string get_list_of_names();
        int		checkIfIsClientNickName(std::string name);
        int		findClientRole(std::string nikname);
        int		checkIfIsClient(std::string const &nickname);
        void    removeClientNickName(std::string const &nickname);
		int		checkIfInviteToChannel(std::string nickName);
		void	sendReplyAll(const std::string &msg, std::string nickname);
};
#endif