/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testMain.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/30 15:30:52 by trbonnes          #+#    #+#             */
/*   Updated: 2020/10/23 11:33:58 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <fcntl.h>
#include "socket.hpp"

int main() {
    size_t i = 0;
	int fd = open("./httpRequestTest", O_RDWR);
    Socket *socket = httpRequestParser(fd);

    std::cout << "Fd: " << socket->getFd() << std::endl;
    std::cout << "Method: " << socket->getMethod() << std::endl;
	std::cout << "URI: " << socket->getRequestURI() << std::endl;
    std::cout << "Version: " << socket->getHttpVersion() << std::endl;

    std::cout << "ContentType: " << socket->getContentType() << std::endl;
    std::cout << "getDate: " << socket->getDate() << std::endl;
    std::cout << "TransferEncoding: " << socket->getTransferEncoding() << std::endl;
    i = 0;
    std::cout << "AcceptCharset: " << std::endl;
    while (i < socket->getAcceptCharset().size()){
        std::cout << socket->getAcceptCharset()[i] << std::endl;
        i++;
    }
    i = 0;
    std::cout << "AcceptLanguage: " << std::endl;
    while (i < socket->getAcceptLanguage().size()){
        std::cout << socket->getAcceptLanguage()[i] << std::endl;
        i++;
    }

    std::cout << "Authorization: " << socket->getAuthorization() << std::endl;
    std::cout << "Host: " << socket->getHost() << std::endl;
    std::cout << "UserAgent: " << socket->getUserAgent() << std::endl;
	std::cout << "Referer: " << socket->getReferer() << std::endl;
    std::cout << "ContentLocation: " << socket->getContentLocation() << std::endl;
    std::cout << "ContentLength: " << socket->getContentLength() << std::endl;
    i = 0;
    std::cout << "Body: " << std::endl << socket->getBody() << std::endl;

	delete socket;
}