#include "huffmanTree.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PATH_SIZE 256

struct Info
{
	size_t thread_size;
	size_t word_size;
};

void   fireman(int);
void   error(std::string&& msg);
size_t read_file(huffman::priority& pq);

int main(int argc, char* argv[])
{
	pid_t       pid            = 0;
	int         socket_fd      = 0;
	int         new_socket_fd  = 0;
	int         port_number    = 0;
	int         n              = 0;
	socklen_t   client_length  = 0;
	size_t      thread_size    = 0;
	size_t      word_size      = 0;
	std::string word           = {};
	sockaddr_in server_addres  = {};
	sockaddr_in client_address = {};
	Info        info           = {};

	signal(SIGCHLD, fireman);

	if (argc < 2)
		error("ARGUMENT");

	port_number = std::stoi(argv[1]);

	huffman::priority pq = {};
	thread_size                 = read_file(pq);

	word_size = huffman::merge_tree(pq);
	huffman::preorder(pq.top());

	socket_fd                     = socket(AF_INET, SOCK_STREAM, 0);
	server_addres.sin_family      = AF_INET;
	server_addres.sin_addr.s_addr = INADDR_ANY;
	server_addres.sin_port        = htons(port_number);

	auto option = 1;

	n = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (n != 0)
		error("SOCKET OPTION");

	if (bind(socket_fd, (sockaddr*)&server_addres, sizeof(server_addres)) < 0)
		error("BINDING");

	listen(socket_fd, 20);

	new_socket_fd    = accept(socket_fd, (sockaddr*)&client_address, &client_length);
	info.thread_size = thread_size;
	info.word_size   = word_size;

	n = write(new_socket_fd, &info, sizeof(info));

	if (n < 0)
		error("WRITE");

	for (size_t i = 0; i < info.thread_size; i++)
	{
		new_socket_fd = accept(socket_fd, (sockaddr*)&client_address, &client_length);
		// std::cout << "CONNECTION SET" << std::endl;
		pid = fork();
		if (pid == 0)
		{
			if (new_socket_fd < 0)
				error("ACCEPT");

			char buffer[PATH_SIZE] = {'\0'};

			n = read(new_socket_fd, &buffer, sizeof(buffer));
			if (n < 0)
				error("READ");

			std::string path(buffer);

			char letter = huffman::decode(path, *pq.top());

			n = write(new_socket_fd, &letter, sizeof(char));

			close(new_socket_fd);
			_exit(0);
		}
	}

	close(socket_fd);
	huffman::deleteNodes(pq.top());
	return 0;
}
void fireman(int) { waitpid(-1, nullptr, WNOHANG); }

void error(std::string&& msg)
{
	std::cerr << std::strerror(errno) << " " << msg << std::endl;
	std::exit(EXIT_FAILURE);
}

size_t read_file(huffman::priority& pq)
{
	size_t      thead_size = 0;
	std::string line       = {};
	while (std::getline(std::cin, line))
	{
		thead_size++;
		auto freq   = 0;
		char symbol = '\0';

		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

		std::istringstream ss{line};

		if (line[0] == ' ') // for space character
			symbol = ' ';
		else
			ss >> symbol;

		ss >> freq;
		huffman::insert(pq, symbol, freq);
	}
	return thead_size;
}