#include "HttpWorker.hpp"
#include <sys/sysinfo.h>

HttpWorker::HttpWorker(std::list<ListenSocket> &listen, Config* config) : Runnable(1, 1)
{
	_config = config;
    _listen_socket = listen;
}

HttpWorker::HttpWorker(const HttpWorker &w) : Runnable(w)
{
	_config = w._config;
    _listen_socket = w._listen_socket;
}

HttpWorker::~HttpWorker() {
}

int		equalRequest(httpRequest *newSocket, httpRequest *lastSocket)
{
	if (lastSocket == NULL)
		return 1;
;	if (newSocket->getMethod().compare(lastSocket->getMethod()))
		return 1;
	if (newSocket->getRequestURI().compare(lastSocket->getRequestURI()))
		return 1;
	if (newSocket->getHttpVersion().compare(lastSocket->getHttpVersion()))
		return 1;
	if (newSocket->getContentLength().compare(lastSocket->getContentLength()))
		return 1;
	if (newSocket->getContentLocation().compare(lastSocket->getContentLocation()))
		return 1;
	if (newSocket->getContentType().compare(lastSocket->getContentType()))
		return 1;
	if (newSocket->getTransferEncoding().compare(lastSocket->getTransferEncoding()))
		return 1;
	if (newSocket->getAuthorization().compare(lastSocket->getAuthorization()))
		return 1;
	if (newSocket->getHost().compare(lastSocket->getHost()))
		return 1;
	if (newSocket->getPort() != lastSocket->getPort())
		return 1;
	if (newSocket->getUserAgent().compare(lastSocket->getUserAgent()))
		return 1;
	if (newSocket->getReferer().compare(lastSocket->getReferer()))
		return 1;
	if (newSocket->getRemoteAddr().compare(lastSocket->getRemoteAddr()))
		return 1;
	if (newSocket->getXSecret().compare(lastSocket->getXSecret()))
		return 1;
	if  (newSocket->getAcceptCharset() != lastSocket->getAcceptCharset())
		return 1;	
	if  (newSocket->getAcceptLanguage() != lastSocket->getAcceptLanguage())
		return 1;
	if (newSocket->getBody().compare(lastSocket->getBody()))
		return 1;
	return 0;
}

void	HttpWorker::handleStreamRead(Connection *c)
{
	HTTP* method;
	Log::debug("CGI READ\n");

	method = c->getMethod();
	FD_CLR(method->get_cgi_out(), &_active_read);
	method->read_cgi_response();
	method->processResponse();
	// Log::debug(method->getResponse());
	FD_SET(c->getSock(), &_active_write);
}

void	HttpWorker::handleStreamWrite(Connection *c)
{
	Log::debug("CGI WRITE\n");
	HTTP* method = c->getMethod();
	
	// Remove cgi_in from 
	FD_CLR(method->get_cgi_in(), &_active_write);
    if (c->getMethod()->write_cgi_request() == -1)
	{
		// TO DO kill CGI process ?
		delete method;
		throw httpRequest::ConnectionClose();
	}
	FD_SET(method->get_cgi_out(), &_active_read);
}

void	HttpWorker::handleWrite(Connection *c)
{
	HTTP* method;
	char* response;
	size_t responseSize;

	Log::debug("About to write\n");
	method = c->getMethod();
	response = method->getResponse();
	responseSize = method->getResponseSize();
	if (write(c->getSock(), response, responseSize) == -1)
	{
		throw httpRequest::ConnectionClose();
	}
	Log::debug("Written\n");
	Log::debug(response);
	c->clearSocket();
	delete method;
	free(response);
	c->setMethod(NULL);
	FD_SET(c->getSock(), &_active_read);
	FD_CLR(c->getSock(), &_active_write);
}



void	HttpWorker::handleRead(Connection *c)
{
	Log::debug("About to read\n");
	HTTP *method;

	httpRequest *newSocket = httpRequestParser(c->getSock());
	c->setSocket(newSocket);
	ConfigServer *configServer = NULL;
	configServer = _config->getServerUnit(c->getSocket()->getPort(), c->getSocket()->getHost());
	if (configServer == NULL)
			throw httpRequest::ConnectionClose();
	method = new HTTP(c->getSocket(), configServer);
	c->setMethod(method);
	if (method->use_cgi())
	{
		Log::debug("Method uses CGI\n");
		// add cgi in to write set
		FD_SET(method->get_cgi_in(), &_active_write);
	}
	// if not cgi, directly add cgi to write
	else
	{
		method->processResponse();
		Log::debug("Get sock to write\n");
		FD_SET(c->getSock(), &_active_write);
	}
	// Remove socket from read set
	FD_CLR(c->getSock(), &_active_read);
}

void	HttpWorker::acceptConnection(int s)
{
	try
	{
		Connection *c = new Connection(s);
		_connections.push_front(c);
		FD_SET(c->getSock(), &_active_read);
		// std::cerr << "New connection " << c->getSock() << std::endl;
	}
	catch(const std::exception& e)
	{
	}
}

void	HttpWorker::run()
{
	// FD_SETS
	fd_set 	write_fs = { 0 };
	fd_set 	read_fs = { 0 };

	FD_ZERO(&_active_read);
	FD_ZERO(&_active_write);
	//List iterators;
	std::list<ListenSocket>::iterator il;
	std::list<Connection*>::iterator ic;

	// add Listen sockets in _active_read
	for (il = _listen_socket.begin(); il != _listen_socket.end(); il++)
		FD_SET(il->getSock(), &_active_read);

	// Main loop
	while (1)
	{
		read_fs = _active_read;
		write_fs = _active_write;
				// Log::debug("Looping");
		if (select(FD_SETSIZE, &read_fs, &write_fs, NULL, NULL) == -1)
		{
			if (g_server->getStatus() == HttpServer::STOPPING)
				break;
			else
				std::cerr << "Select error: " << strerror(errno) << std::endl;
		}

		if (g_server->getStatus() == HttpServer::STOPPING)
				break;
		// New connection
		for (il = _listen_socket.begin(); il != _listen_socket.end(); il++)
			if (FD_ISSET(il->getSock(), &read_fs))
				acceptConnection(il->getSock());

		ic = _connections.begin();
		while (ic != _connections.end())
		{
			Connection* c = *ic;
			try
			{
				if (FD_ISSET(c->getSock(), &read_fs))
					handleRead(c);
				// Write ready on active connection
				else if (FD_ISSET(c->getSock(), &write_fs))
					handleWrite(c);
				else if (c->getMethod() && c->getMethod()->get_cgi_out() != -1 && FD_ISSET(c->getMethod()->get_cgi_out(), &read_fs))
					handleStreamRead(c);
				else if (c->getMethod() && c->getMethod()->get_cgi_in() != -1 && FD_ISSET(c->getMethod()->get_cgi_in(), &write_fs))
					handleStreamWrite(c);
			}
			catch(const std::exception& e)
			{
				FD_CLR(c->getSock(), &_active_write);
				FD_CLR(c->getSock(), &_active_read);
				c->clearSocket();
				ic = _connections.erase(ic);
				delete c;
				continue;
			}
			ic++;
		}
	}
	closeConnections();
}

void		HttpWorker::closeConnections()
{
	std::list<Connection*>::iterator ic,next;

	ic = _connections.begin();
	while (ic != _connections.end())
	{
		next = ic;
		next++;
		delete *ic;
		_connections.erase(ic);
		ic = next;
	}
}

Runnable*	HttpWorker::clone() const
{
	return new HttpWorker(*this);
}
