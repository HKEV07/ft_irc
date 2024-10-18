#include "./includes/Server.hpp"

bool	g_interrupt = false;

std::string	del_break(std::string str)
{
	if (str.empty())
		return (str);
	while (1)
	{
		if (str[str.size() - 1] == '\r')
			str = str.substr(0, (str.size() - 1));
		else if(str[str.size() - 1] == '\n')
			str = str.substr(0, (str.size() - 1));
		else if(str[str.size() - 1] == '\v')
			str = str.substr(0, (str.size() - 1));
		else if(str[str.size() - 1] == '\t')
			str = str.substr(0, (str.size() - 1));
		else if(str[str.size() - 1] == '\f')
			str = str.substr(0, (str.size() - 1));
		else
			break;
	}
	return (str);
}

std::string	ERROR_NEED_MORE_PARAMETERS(Client &client, std::string cmd)
{
	return ("461 " + client.getNickname() + " " + cmd + " :Not enough parameters\r\n");
}

void	signalHandler(int const signal)
{
	(void)signal;
	g_interrupt = true;
}

int	getport(std::string sport)
{
	if (sport.size() < 4 || sport.size() > 5)
	{
		return (-1);
	}
	for (size_t i = 0; i < sport.size(); i++)
	{
		if (sport[i] < '0' || sport[i] > '9')
		{
			return (-1);
		}
	}
	return (std::atoi(sport.c_str()));
}
int main(int ac, char **av)
{
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	if (ac != 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	const std::string password = av[2];
	for (size_t i = 0; i < password.size(); i++)
	{
		if (password[i] <= ' ' || password[i] > '~')
		{
			std::cout << "Error input: password must contain only printable characters" << std::endl;
			return (1);
		}
	}
	if ((password.size() < 2) || (password.size() > 8))
	{
		std::cout << "Error input: password must contain at least 2 characters" << std::endl;
		return (1);
	}
	const int port = getport(av[1]);
	if (port < 1024 || port > 65535)
	{
		std::cout << "Error input: port must be between 1024 and 65535" << std::endl;
		return (-1);
	}
	try
	{
		Server server(port, password);
		server.launch();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
