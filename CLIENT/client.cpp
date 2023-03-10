#include "huffmanTree.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

struct Info
{
	size_t thread_size;
	size_t word_size;
};

struct Connection
{
	std::string  line;
	std::string  host;
	std::string  port;
	std::string* word;
};

void  error(std::string&& msg);
void* handler(void* void_ptr);
Info  get_sizes(const std::string& port, const std::string& address);

int main(int argc, char* argv[])
{
	std::string line = {};
	std::string word = {};
	size_t      i    = 0;
	Info        info = {};

	if (argc < 3)
		error("ARGUMENT COUNT");

	info = get_sizes(argv[2], argv[1]); //// rename function
	word = std::string(info.word_size, '\0');

	std::vector<Connection> connections(info.thread_size);
	std::vector<pthread_t>  ids(info.thread_size);

	while (std::getline(std::cin, line))
	{
		connections.insert(connections.begin() + i, Connection());
		connections[i].line = line;
		connections[i].host = argv[1];
		connections[i].port = argv[2];
		connections[i].word = &word;
		pthread_create(&ids[i], nullptr, handler, &connections[i]);
		i++;
	}
	for (size_t j = 0; j < info.thread_size; j++)
	{
		pthread_join(ids[j], nullptr);
	}
	std::cout << "Original message: " << word << std::endl;
	return 0;
}

void error(std::string&& msg)
{
	std::cerr << std::strerror(errno) << " " << msg << std::endl;
	std::exit(EXIT_FAILURE);
}

void* handler(void* void_ptr)
{
	Connection* con = (Connection*)void_ptr;

	sockaddr_in server_address = {};
	hostent*    server         = {};
	int         port_number    = 0;
	int         socket_fd      = 0;
	int         n              = 0;
	int         pos            = 0;
	char        letter         = '\0';
	std::string path           = {};

	server = gethostbyname(con->host.data());
	if (server == nullptr)
		error("NO SUCH HOST");

	port_number               = std::stoi(con->port);
	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(port_number);
	std::memcpy(server->h_addr, &server_address.sin_addr.s_addr, server->h_length);

	std::string line = con->line;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		error(std::strerror(errno));

	if (connect(socket_fd, (sockaddr*)&server_address, sizeof(server_address)) < 0)
		error(std::strerror(errno));

	std::istringstream ss(line);
	ss >> path;

	n = write(socket_fd, path.c_str(), path.size());
	if (n < 0)
		error("WRITE");

	n = read(socket_fd, &letter, sizeof(char));
	while (ss >> pos)
	{
		(*con->word)[pos] = letter;
	}

	return nullptr;
}

Info get_sizes(const std::string& port, const std::string& address)
{
	Info        info           = {};
	sockaddr_in server_address = {};
	hostent*    server         = {};
	int         socket_fd      = 0;
	int         n              = 0;

	server = gethostbyname(address.c_str());
	if (server == nullptr)
		error("NO SUCH HOST");

	server_address.sin_port   = htons(std::stoi(port));
	server_address.sin_family = AF_INET;

	std::memcpy(server->h_addr, &server_address.sin_addr.s_addr, server->h_length);

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		error("SOCKET");

	n = connect(socket_fd, (sockaddr*)&server_address, sizeof(server_address));
	if (n < 0)
		error("CONNECTION");

	n = read(socket_fd, &info, sizeof(Info));
	if (n < 0)
		error("READ");

	close(socket_fd);
	return info;
}
