/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/30 15:45:46 by trbonnes          #+#    #+#             */
/*   Updated: 2021/01/14 17:54:17 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

int		httpRequestParseChunckedBody(std::string request, Socket *socket, size_t pos) {
	size_t chunkSize;
	std::string convert;
	std::string s = request.substr(pos, request.npos);
	std::string body;

	Log::debug("\033[0;32mCHUNCKED");
	try {
		char	c[8192];
		int		ret;
		
		while ((pos = s.find("\r\n")) == s.npos) {
			ft_bzero(c, 8192);
			ret = read(socket->getFd(), c, 1);
			s.append(c, ret);
		}
		pos = 0;
		while (s[pos] && s[pos] != '\r') {
			convert.append(s.substr(pos, 1));
			pos++;
		}
		chunkSize = strtol(convert.c_str(), NULL, 16);
		Log::debug("chunck Size: ");
		Log::debug(chunkSize);
		while (chunkSize > 0) {
			Log::debug("new chunk");
			socket->setContentLength(ft_itoa(atol(socket->getContentLength().c_str()) + chunkSize));
			s.clear();
			convert.clear();
			for (int i = 0; i < (int)chunkSize; i += ret) {
				if (chunkSize - i > 8192) {
					ft_bzero(c, 8192);
					ret = read(socket->getFd(), c, 8192);
					body.append(c, ret);
				}
				else {
					ft_bzero(c, 8192);
					ret = read(socket->getFd(), c, chunkSize - i);
					body.append(c, ret);
				}
			}
			ft_bzero(c, 8192);
			ret = read(socket->getFd(), c, 2);
			while ((pos = s.find("\r\n")) == s.npos) {
				ft_bzero(c, 2);
				ret = read(socket->getFd(), c, 1);
				s.append(c, ret);
			}
			pos = 0;
			while (s[pos] && s[pos] != '\r') {
				convert.append(s.substr(pos, 1));
				pos++;
			}
			chunkSize = strtol(convert.c_str(), NULL, 16);
			Log::debug("chunck Size: ");
			Log::debug(chunkSize);
		}
		Log::debug("All chunk read");
		socket->setBody(body);
		Log::debug("\033[0;32mchuncked content length: ");
		Log::debug(socket->getContentLength().c_str());
		body.clear();
	}
	catch (std::exception &e) {
		std::cerr << "Exception1: " << e.what() << std::endl;
	}

	// pos = s.find("\n0\r\n");
	// if (pos == s.npos) {
	// 	char	c[8192];
	// 	int		ret;

	// 	// Log::debug("\033[0;32mRead in chuncked body");
	// 	while ((pos = s.find("\n0\r\n")) >= s.npos) {
	// 		ft_bzero(c, 8192);
	// 		ret = read(socket->getFd(), c, 8192);
	// 		s.append(c, ret);
	// 	}
	// 	// Log::debug("\033[0;32mRead over");
	// 	// Log::debug(debug);
	// }
	// try {
	// 	chunkSize = 0;
	// 	pos = 0;
	// 	while (s[pos] && s[pos] != '\r') {
	// 		convert.append(s.substr(pos, 1));
	// 		pos++;
	// 	}
	// 	chunkSize = strtol(convert.c_str(), NULL, 16);
	// 	// Log::debug("chunck Size: ");
	// 	// Log::debug(chunkSize);
	// 	socket->setContentLength(ft_itoa(atol(socket->getContentLength().c_str()) + chunkSize));
	// 	while (chunkSize > 0) {
	// 		// Log::debug("new chunk");
	// 		body.append(s.substr(convert.length() + 3, chunkSize));
	// 		s.erase(0, convert.length() + 2 + chunkSize + 2);
	// 		convert.clear();
	// 		pos = 0;
	// 		while (s[pos] && s[pos] != '\r') {
	// 			convert.append(s.substr(pos, 1));
	// 			pos++;
	// 		}
	// 		chunkSize = strtol(convert.c_str(), NULL, 16);
	// 		// Log::debug("chunck Size: ");
	// 		// Log::debug(chunkSize);
	// 		socket->setContentLength(ft_itoa(atol(socket->getContentLength().c_str()) + chunkSize));
	// 	}
	// 	// Log::debug("All chunk read");
	// 	socket->setBody(body);
	// 	// Log::debug("\033[0;32mchuncked content length: ");
	// 	// Log::debug(socket->getContentLength().c_str());
	// }
	// catch (std::exception &e) {
	// 	std::cerr << "Exception1: " << e.what() << std::endl;
	// }
	
	return 0;
}

int		httpRequestParseBody(std::string request, Socket *socket) {
	std::string					content;
	std::string					body;
	size_t						chunkedPos = socket->getTransferEncoding().find("chunked");
	size_t						ret;

	Log::debug(request.c_str());

	if (!socket->getContentLength().size() && chunkedPos == std::string::npos){
		socket->setBody("");
		return 0;
	}
	
	size_t pos = request.find("\r\n\r\n");
	if (pos == request.npos) {
		pos = request.find("\n\n");
		pos += 2;
	}
	else 
		pos += 4;
	if (pos >= request.npos) {
		socket->setBody("");
		return 0;
	}
	if (chunkedPos != std::string::npos)
		return httpRequestParseChunckedBody(request, socket, pos);

	try {
		body = request.substr(pos, request.npos);
		size_t	contentLength = atol(socket->getContentLength().c_str());
		if (body.size() >= contentLength)
			socket->setBody(body);
		else {
			char	c[8192];
			
			//Log::debug("\033[0;32mRead if body missing");
			while (body.size() < contentLength) {
				ft_bzero(c, 8192);
				ret = read(socket->getFd(), c, 8192);
				body.append(c, ret);
			}
			if (body.size() > contentLength)
				body.erase(contentLength, body.npos);
			socket->setBody(body);
		}
	}
	catch (std::exception &e) {
		std::cerr << "Exception2: " << e.what() << std::endl;
	}
	
	return 0;
}

int		httpRequestParseHeaders(std::string request, Socket *socket) {
	char const *x[] = {
		"\nAccept-Charset",
		"\nAccept-Language",
		"\nAuthorization",
		"\nContent-Length",
		"\nContent-Location",
		"\nContent-Type",
		"\nDate",
		"\nHost",
		"\nReferer",
		"\nTransfer-Encoding",
		"\nUser-Agent",
		"\nX-Secret"
	};
	void (*f[])(Socket*, std::string, size_t) = {
		&ParseAcceptCharset,
		&ParseAcceptLanguage,
		&ParseAutorization,
		&ParseContentLength,
		&ParseContentLocation,
		&ParseContentType,
		&ParseDate,
		&ParseHost,
		&ParseReferer,
		&ParseTransferEncoding,
		&ParseUserAgent,
		&ParseXSecret
	};
	std::vector<std::string> headerVec(x, x + sizeof(x) / sizeof(*x));
	std::string	s2;
	size_t		pos;
	int			j = 0;

	for (std::vector<std::string>::iterator i = headerVec.begin(); i != headerVec.end(); i++) {
		pos = request.find(*i);
		if (pos != request.npos)
			f[j](socket, request, pos);
		j++;
	}

	return 0;
}

int		httpRequestParseRequestLine(std::string request, Socket *socket) {
	char const *x[] = {
	"GET",
	"HEAD",
	"POST",
	"PUT",
	"DELETE",
	"CONNECT",
	"OPTIONS",
	"TRACE"};
	std::vector<std::string> methodsVec(x, x + sizeof(x) / sizeof(*x));
	std::string s2;
	size_t pos;

	//GET METHOD FROM REQUEST
	for (std::vector<std::string>::iterator i = methodsVec.begin(); i != methodsVec.end(); i++) {
		pos = request.find(*i);
		if (pos != request.npos)
			break ;
	}
	if (pos == std::string::npos)
		return 400;
	while (request[pos] && request[pos] != ' ') {
		s2.push_back(request[pos++]);
	}
	socket->setMethod(s2);
	s2.clear();

	//GET REQUEST-URI
	while (request[pos] && request[pos] == ' ') { pos++ ;}
	while (request[pos] && request[pos] != ' ') {
		s2.push_back(request[pos++]);
	}
	socket->setRequestURI(s2);
	s2.clear();

	//GET HTTP VERSION
	while (request[pos] && request[pos] != 'H') { pos++; }
	while (request[pos] && request[pos] != '\n') {
		s2.push_back(request[pos++]);
	}
	socket->setHttpVersion(s2);

	return 0;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

Socket	*httpRequestParser(int fd, int p[2]) {

	Socket *socket;
	char	c[2];
	int		ret;
	std::string request;

	// Log::debug("\033[0;32mRequestParsing Reading");

	(void) p; // TO DO remove, it's just there to enable compilation until the deed is done

	while (request.find("\r\n\r\n") >= request.npos && request.find("\n\n") >= request.npos)
	{
		ft_bzero(c, 2);
		ret = read(fd, c, 2);
		if (ret == 0)
			throw Socket::ConnectionClose();
		if (ret == -1)
			throw Socket::BadReadException();
		request.append(c, ret);
	}
	// Log::debug("\033[0;32mRequestParsing Reading End");
	// Log::debug(request.c_str());
	socket = new Socket(fd);
	// Log::debug("\033[0;32mRequestParsing Creation");
	httpRequestParseRequestLine(request, socket);
	httpRequestParseHeaders(request, socket);
	// Log::debug("\033[0;32mHeaders Parsed");
	httpRequestParseBody(request, socket);
	// Log::debug("\033[0;32mBody Parsed");
	request.clear();
	return socket;
}
