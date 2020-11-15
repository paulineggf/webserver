/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/09 12:19:51 by trbonnes          #+#    #+#             */
/*   Updated: 2020/11/11 16:02:20 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include <iostream>
# include <list>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <vector>
# include <map>

# include "../HTTP/utils/utils.hpp"
# include "Compare.hpp"
# include "Location.hpp"

class	ConfigServer {
private:
	// Server Software
    std::string                 _httpVersion;
    std::string                 _serverSoftware;
    std::list<std::string>      _mimeTypes;
    std::string                 _configFilesRoot;
    std::string                 _errorFilesRoot;

	// Default server
    std::vector<int>            _port;
    int                         _defaultClientBodySize;
    std::map<int, std::string>  _errorPages;
	std::map<std::string, Location, Compare<std::string> > _locationList;
    std::string                 _defaultRoot;
    std::vector<std::string>    _defaultAllow; // == Accepted Methods
    std::vector<std::string>    _defaultServerName;
    std::vector<std::string>    _defaultIndex;
    std::string                 _defaultType;
    std::string                 _defaultCharset;
    std::vector<std::string>    _defaultLanguage;
    std::string                 _defaultAuth_basic;
    std::string                 _defaultAuth_basic_user_file;
    bool                        _defaultAutoindex;

	// CGI
	std::vector<std::string>	_defaultCgi;
	std::vector<std::string>	_defaultCgi_allow;
	std::string					_defaultCgi_root;

public:
	ConfigServer();
	ConfigServer(const ConfigServer &c);
	/*virtual*/ ~ConfigServer();
	ConfigServer &operator=(const ConfigServer &c);

    std::string                 getRoot(std::string _uri);
    std::string                 getHttpVersion(void); // NEW
    std::string                 getServerSoftware(void); // NEW
    std::vector<std::string>    getServerName(void);
    std::vector<std::string>    &getIndex(std::string location);
    std::string                 getType(std::string location);
    std::string                 getCharset(std::string location);
    std::vector<std::string>    &getLanguage(std::string location);
    std::vector<std::string>    &getAllow(std::string location);
    std::list<std::string>      &getMimeTypes();
	std::string                 getErrorFilesRoot(void);
    std::string                 getLocation(std::string location);
    std::string                 getAuth_basic(std::string location);
    std::string                 getAuth_basic_user_file(std::string location);
    bool                        getAutoindex(std::string location);
    std::string                 getAlias(std::string location);
    std::vector<std::string>    getCGI(std::string location);
    std::string                 getCGI_root(std::string location);
	std::vector<std::string>	&getCGI_allow(std::string location);
    int                         getClientBodySize(std::string location);
    std::vector<int>            getPort();
    std::map<int, std::string>  getErrorPages();
    std::string                 getHTMLErrorPage(int error);

    std::map<std::string, Location, Compare<std::string> > getLocationList();

    void					setCGI(std::vector<std::string> cgi);
	void					setCGI_allow(std::vector<std::string> cgi_allow);
	void					setCGI_root(std::string cgi_root);
    void                    setPort(int port);
    void                    setServer_name(std::vector<std::string> server_name);
    void                    setRoot(std::string root);
    void                    setIndex(std::vector<std::string> index);
    void                    setAutoIndex(bool autoIndex);
    void                    setClientBodySize(int clientBodySize);
    void                    setAllow(std::vector<std::string> allow);
    void                    setErrorPages(int error, std::string page);

    void                    insertLocation(std::string s, Location location);

    //void                    printServer(); -> used for local tests
};

#endif
