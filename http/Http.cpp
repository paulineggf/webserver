#include "Http.hpp"

Http::Http(Connection &c) :
_connection(c),
_read_chain(c.getReadChain()),
_write_chain(c.getWriteChain())
{
	_req = NULL;
	_resp = NULL;
	_config = NULL;
}

Http::Http(const Http &c) :
_connection(c._connection),
_read_chain(c._connection.getReadChain()),
_write_chain(c._connection.getWriteChain())
{
	(void) c; // TO DO implement copy and operator=
}

Http& Http::operator=(const Http &c)
{
	(void) c; // TO DO implement copy and operator=
	return *this;
}

// This methods is called each time there's a socket read
// The bytes have already been loaded into _read_chain by ths point
void Http::handleRead()
{
	if (_resp == NULL)
		handleNewRequest();
	
	if (_resp && _read_chain.getFirst())
	{
		try
		{
			_resp->handleRead(_read_chain, _write_chain);
		}
		catch(const HttpResponse::HttpError& e)
		{
			_resp->abort();
			delete _resp;
			// TO DO Throw error
			// _resp = new Error(_config, _req, _write_chain, e.getStatusCode());
		}
	}
	checkState();
}

void Http::handleWrite()
{
		try
		{
			if (_resp)
				_resp->handleWrite(_read_chain, _write_chain);
		}
		catch(const HttpResponse::HttpError& e)
		{
			_resp->abort();
			delete _resp;
			// _resp = new Error(_config, _req, _write_chain, e.getStatusCode());
		}
		catch(const HttpResponse::ConnectionClose& e)
		{
			std::cout << "===== AN error has been thrown" << std::endl;
			throw;
		}
	checkState();
}


void Http::handleStreamRead()
{
	Log::debug("handleStreamRead()");
	try
	{
		_resp->handleStreamRead(_read_chain, _write_chain);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	checkState();
}

void Http::handleStreamWrite()
{
	try
	{
		_resp->handleStreamWrite(_read_chain, _write_chain);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	checkState();
}

void	Http::checkState()
{
	std::cout << " _read_chain: " << _read_chain;
	if (_resp)
	{
		std::cout << " _stream_write_chain: " << _resp->getStreamWriteChain();
		std::cout << " _stream_read_chain: " << _resp->getStreamReadChain();
	}
	std::cout << " _write_chain: " <<_write_chain;


	if (_resp)
	{
		char* states[5];

		states[HttpResponse::NONE] = (char*)"NONE";
		states[HttpResponse::WAITING] = (char*)"WAITING";
		states[HttpResponse::IGNORE] = (char*)"IGNORE";
		states[HttpResponse::READY] = (char*)"READY";
		states[HttpResponse::DONE] = (char*)"DONE"; 

		std::cout << " status _read: " << states[_resp->_state.read] << std::endl;
		std::cout << " status _stream_write: " << states[_resp->_state.writeStream] << std::endl;
		std::cout << " status _stream_read: " << states[_resp->_state.readStream] << std::endl;
		std::cout << " status _write: " << states[_resp->_state.write] << std::endl;
	}
	
	
	if (_resp)
	{
		// Unsub
		if (_resp->_state.read == HttpResponse::DONE)
			_connection.unsubRead();
		if (_resp->_state.readStream == HttpResponse::DONE)
			_connection.unsubStreamRead();
		if (_resp->_state.writeStream == HttpResponse::DONE || _resp->_state.writeStream == HttpResponse::WAITING)
			_connection.unsubStreamWrite();

		// Sub
		if (_resp->_state.readStream == HttpResponse::READY)
			_connection.subStreamRead();
		if (_resp->_state.writeStream == HttpResponse::READY)
			_connection.subStreamWrite();
		
		// End of request
		if (_resp->_state.read == HttpResponse::DONE && _write_chain.getFirst() == NULL && 
			(_resp->_state.readStream == HttpResponse::DONE || _resp->_state.readStream == HttpResponse::NONE) &&
			(_resp->_state.writeStream == HttpResponse::DONE || _resp->_state.writeStream == HttpResponse::NONE))
		{
			std::cout << "======================================= REQUEST END" << std::endl;
			reset();
			if (_read_chain.size() > 0)
				handleRead();
		}
	}
	if (_write_chain.getFirst() &&  (!_resp || _resp->_state.write == HttpResponse::READY))
		_connection.subWrite();
	// if there's a buffer, try init a new request
}

void	Http::setConfig(Config* c)
{
	_config = c;
}

void	Http::reset()
{
		_connection.subRead();
		_connection.unsubStreamRead();
		_connection.unsubStreamWrite();
		_connection.setStreamRead(-1);
		_connection.setStreamWrite(-1);
		if (_req)
		{
			delete _req;
			_req = NULL;
		}
		if (_resp)
		{
			delete _resp;
			_resp = NULL;
		}
		_requestBuffer.clear();
		_streamBuffer.clear();
}

Http::~Http()
{
	reset();
	_connection.unsubRead();
}
