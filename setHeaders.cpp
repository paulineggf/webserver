#include "Methods.hpp"

//** absolute location route for the server **
void        Methods::setLocation()
{
  _location = _config.getLocation(_uri);
}

//** absolute location route for the user agent **
void        Methods::setContentLocation()
{
  _contentLocation.assign("http://").append(_config.getServerName()).append(_route);
  if (_config.getAlias(_location).length() > 0)
    _contentLocation.replace(_contentLocation.find(_location), _location.length(), _config.getAlias(_location));
}

//** Copy file into body string **
void        Methods::setBody(int fd)
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
    else
        close(fd);
}

void        Methods::setCharset(void)
{
  _charset = _config.getCharset(_location);
}

void        Methods::setServerName()
{
    _server = _config.getServerName();
}

void        Methods::setContentLength()
{
    _contentLength = _stat.st_size;
}

void        Methods::setStat()
{
    stat(_route.c_str(), &_stat);
}

void        Methods::setLastModified()
{
    struct tm *timeinfo;

    timeinfo = localtime(&(_stat.st_mtim.tv_sec)); // st_mtimespec = apple ; st_mtime = linux
    strftime(_lastModified, 100, "%a %d %b 20%y %OH:%OM:%OS GMT", timeinfo);
}

void        Methods::setDate()
{
    struct timeval  tv;
    struct tm       *timeinfo;

    if (gettimeofday(&tv, NULL) == 0)
    {
        timeinfo = localtime(&(tv.tv_sec));
        strftime(_date, 100, "%a %d %b 20%y %OH:%OM:%OS GMT", timeinfo);
    }
}

void        Methods::setContentType()
{
    int                     find;
    int                     length;
    std::string             line;
    std::list<std::string>  mimeTypes;
    std::list<std::string>::iterator it;

    find = _route.find('.', 0);
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

int         Methods::setRoot()
{
    int         fd;
    int         find;
    std::string str;
    struct stat file;

    if (_uri.compare(0, 4, "http") == 0)
    {
        //** Absolute path **

        find = _route.append(_socket.getRequestURI()).find(_config.getServerName());
        _route.erase(0, find + _config.getServerName().length());
        _route.insert(0, _config.getRoot(_location));
        _route.insert(_config.getRoot(_location).length(), acceptLanguage());
    }
    else
    {
        //** Relative path **

        _route.assign(_config.getRoot(_location));
        _route.append(_socket.getRequestURI());
        stat(_route.c_str(), &file);
        if ((file.st_mode & S_IFMT) == S_IFREG)
        {
            fd = open(_route.c_str(), O_RDONLY);
            _statusCode = OK;
            return (fd);
        }
        _route.assign(_config.getRoot(_location));
        _route.append(acceptLanguage());
        _route.append(str.assign(_socket.getRequestURI()).erase(0, _location.length()));
    }
    //** Open and test if the file exist **
    fd = openFile();
    return (fd);
}
