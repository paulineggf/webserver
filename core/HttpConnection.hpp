#if !defined(HTTPCONNECTION)
#define HTTPCONNECTION

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "ListenSocket.hpp"

class HttpConnection
{
public:
	HttpConnection(ListenSocket& listen_sock);
	HttpConnection(HttpConnection &&) = default;
	HttpConnection(const HttpConnection &) = default;
	HttpConnection &operator=(HttpConnection &&) = default;
	HttpConnection &operator=(const HttpConnection &) = default;
	~HttpConnection() = default;

private:
	int				_sock;
	ListenSocket&	_listen_sock;
	struct sockaddr	_client_name;

public:
	void	accept();
	void	write(char *data, size_t size);
	void	read();
	int		getSock();
	int		getPort();
	
	class ConnectionClose : public std::exception
	{
		public:
			const char * what () const throw ()
			{
				return "Connection closed"; // Maybe add Port or something
			}
	};
};

#endif // HTTPCONNECTION
