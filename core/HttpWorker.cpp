#include "HttpWorker.hpp"


//TO DO The runnable arguments might depend from the configuration file
HttpWorker::HttpWorker(std::vector<ListenSocket> &listen, Config* config) : Runnable(1, 1)
{
	std::cout << "Worker Initializing" << std::endl;
	_config = config;
    _listen_socket = listen;
}

HttpWorker::~HttpWorker() {
}


// TO DO Check config var regarding max connections and max port/server block
void	HttpWorker::run()
{
	fd_set active_fs;
	fd_set read_fs;
	int i;
	HttpConnection *new_connection; // temp pointer 
	HttpConnection* connections[FD_SETSIZE]; // array of connections
	ListenSocket* listening[FD_SETSIZE]; // array of pointers

	std::cout << "Running a worker" << std::endl;
	// Important zeroing of the values
	bzero(connections, FD_SETSIZE * sizeof(ListenSocket*)); // TO DO forbidden function
	bzero(listening, FD_SETSIZE * sizeof(ListenSocket*));
	// Transforming list in fdset and Listensocket hashmap (not sure about this name)
	FD_ZERO(&active_fs);
	for (unsigned int i = 0; i < _listen_socket.size(); i++)
	{
		listening[_listen_socket[i].getSock()] = &_listen_socket[i];
		FD_SET(_listen_socket[i].getSock(), &active_fs);
	}
	// Main loop of the worker
	while (1)
	{
		//read fs is going to be modified by select call, so we must reattribute the set there
		read_fs = active_fs;
		// Waiting for an event on listen socket
		if (select(FD_SETSIZE, &read_fs, NULL, NULL, NULL) == -1) // TO DO check if 0
		{
			std::cout << "Select error " << strerror(errno) << std::endl;
			continue; // TO DO throw something ?
		}
		i = 0;
		for (i = 0; i < FD_SETSIZE; i++) // TO DO optimization ?
		{
			// if the fd is not set then there's no event on that fd, next
			if (!FD_ISSET(i, &read_fs))
				continue ;
			// if it is on a listening socket, create a new connection
			if (listening[i])
			{
				std::cout << "New connection" << std::endl; // TO DO remove or change log
				try
				{
					new_connection = new HttpConnection(*listening[i]);
					new_connection->accept();
					connections[new_connection->getSock()] = new_connection;
					FD_SET(new_connection->getSock(), &active_fs);
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
					delete new_connection;
				}
				
			}
			// If it is a connection socket, do the job
			else if (connections[i])
			{
				std::cout << "Event on connection" << std::endl;
				FD_CLR(i, &read_fs);
				Socket *socket;
				try
				{
					socket = httpRequestParser(connections[i]->getSock()); // TO DO why would it return a socket class and not an httpRequest object ? 
				}
				catch(const HttpConnection::ConnectionClose& e)
				{
					std::cerr << e.what() << '\n';
					delete connections[i];
					connections[i] = 0;
					FD_CLR(i, &active_fs);
					continue ;
				}
				
				// ConfigServer *ptr = _config->getServerUnit(connections[i]->getSock(), socket->getHost()); // TO DO check if null ?
				// std::cout << "PASSED THIS POINT, value of ptr : " << ptr << socket->getHost() << std::endl;
				
				ConfigServer &ptr2 = _config->getServerList()[0];
				HTTP method(socket, ptr2);
				std::string response;
				response = method.getResponse(); // TO DO make code more modulare and clean up names

				connections[i]->write((char*)response.c_str(), response.length()); // TO DO ugly
			}
			else
			{
				// TO DO or not ?
			}
		}
	// TO DO timeout for http
	}
	// TO DO delete connections
}
