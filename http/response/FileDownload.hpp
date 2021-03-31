#if !defined(FILEDOWNLOAD_HPP)
#define FILEDOWNLOAD_HPP

#include "http/response/HttpResponse.hpp"

class FileDownload : public HttpResponse
{

public:
	//Coplien to do
	FileDownload(ResponseContext& ctx, BufferChain& writeChain, struct stat *file);
	~FileDownload();
};


#endif // FILEDOWNLOAD_HPP
