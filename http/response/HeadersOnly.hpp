#if !defined(HEADERSONLY_HPP)
#define HEADERSONLY_HPP

#include "HttpResponse.hpp"

class HeadersOnly : public HttpResponse
{
private:
	BufferChain		_fakeWriteChain;
	HttpResponse*	_subResponse;
public:
	// HEAD
	HeadersOnly(ConfigServer*, HttpRequest*, BufferChain& writeChain, std::string route, std::string location, struct stat*);
	HeadersOnly(ConfigServer*, HttpRequest*, BufferChain& writeChain, std::string route, std::string location);
	// POST
	HeadersOnly(ConfigServer*, HttpRequest*, BufferChain& writeChain, std::string route);
	// OPTIONS
	// HeadersOnly(ConfigServer*, HttpRequest*, BufferChain& writeChain, std::string route, std::string location, );
	~HeadersOnly();
	void	handleRead(BufferChain& readChain, BufferChain& writeChain);
};
#endif // HEADERSONLY_HPP
