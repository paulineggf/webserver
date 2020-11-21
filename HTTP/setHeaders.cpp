#include "HTTP.hpp"

//** absolute location route for the user agent **
void        HTTP::setContentLocation()
{
    _contentLocation.assign("http://").append(_config.getServerName()[0]).append(_route); // TO DO quick fix
    if (_config.getAlias(_location).length() > 0)
        _contentLocation.replace(_contentLocation.find(_location), _location.length(), _config.getAlias(_location));
}

//** Copy file into body string **
void        HTTP::setBody(int fd)
{
    int     ret;
    char    buf[1024 + 1];

    while ((ret = read(fd, buf, 1024)) > 0)
    {
        buf[ret] = '\0';
        _body.append(buf);
    }
    if (ret == -1)
        _statusCode = INTERNAL_SERVER_ERROR;
    close(fd);
}

void        HTTP::setCharset(void)
{
    if (_contentType.compare("text/html") == 0)
        _charset = _config.getCharset(_location);
}

void        HTTP::setServerName()
{
    _server = _config.getServerName()[0]; // TO DO quick fix
}

void        HTTP::setContentLength()
{
   _contentLength = _stat.st_size + 2;
}

void        HTTP::setLastModified()
{
    struct tm *timeinfo;

	#if defined(TARGET_OS_MAC)
            	timeinfo = localtime(&(directory.st_mtimespec.tv_sec)); // st_mtimespec.tv_sec = macos ; st_mtim = linux
	#else
    		timeinfo = localtime(&(_stat.st_mtim.tv_sec));
	#endif // TARGET_OS_MAC
    strftime(_lastModified, 100, "%a %d %b 20%y %OH:%OM:%OS GMT", timeinfo);
}

void        HTTP::setDate()
{
    struct timeval  tv;
    struct tm       *timeinfo;

    if (gettimeofday(&tv, NULL) == 0)
    {
        timeinfo = localtime(&(tv.tv_sec));
        strftime(_date, 100, "%a %d %b 20%y %OH:%OM:%OS GMT", timeinfo);
    }
}

void        HTTP::setContentType()
{
    int                     find;
    int                     length;
    std::string             line;
    std::list<std::string>  mimeTypes;
    std::list<std::string>::iterator it;

    find = _route.find_last_of('.');
    find += 1;
    length = _route.length() - find;
    _contentType = _route.substr(find, length);
    find = -1;
    it = _config.getMimeTypes().begin();
    while (it != _config.getMimeTypes().end())
    {
        if ((find = (*it).find(_contentType)) >= 0)
            break ;
        it++;
    }
    if (find >= 0)
        _contentType = (*it).substr(0, (*it).find(" "));
    else
        _contentType = _config.getType(_location);
}
