#if !defined(LISTENSOCKET)
#define LISTENSOCKET

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/types.h>
#include <string>
#include <string.h>
#include <errno.h>

class ListenSocket
{
private:
	int	_sock;
	int _port;
	ListenSocket(const ListenSocket &);
	ListenSocket &operator=(const ListenSocket &);

public:
	ListenSocket(int port);
	~ListenSocket();
	int	getSock();
	int	getPort();

	class ListenSocketException : public std::exception
	{
		protected:
			std::string _msg;
		public:
			ListenSocketException(std::string msg, int errcode);
			virtual const char * what () const throw ();
    		virtual ~ListenSocketException() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT;

	};
	class BindingException : public ListenSocketException
	{
		public:
			BindingException(int errcode);
	};
	class ListenException : public ListenSocketException
	{
		public:
			ListenException(int errcode);
	};

};

#endif // LISTENSOCKET
