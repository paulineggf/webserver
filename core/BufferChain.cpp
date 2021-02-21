#include "BufferChain.hpp"

char g_read_medium[BUFFER_SIZE_MEDIUM] = {};
char g_read_large[BUFFER_SIZE_LARGE] = {};

BufferChain::BufferChain()
{
}

BufferChain::BufferChain(BufferChain&)
{
	// TO DO add deep copy
}

BufferChain& BufferChain::operator=(BufferChain&)
{
	// TO DO add flush then deep copy
	return (*this);
}

BufferChain::~BufferChain()
{
	flush();
}

void	BufferChain::pushBack(char* toadd, size_t size)
{
	if (size == 0)
		return;
	buffer_t pair;
	pair.data = toadd;
	pair.size = size;
	_chain.push_back(pair);
}

void	BufferChain::copyPushBack(char *tocopy, size_t size)
{
	if (size == 0)
		return;
	char* n = new char[size]();
	ft_memcpy(n, tocopy, std::min(size, (size_t)BUFFER_SIZE_LARGE));
	pushBack(n, size);
}

BufferChain::buffer_t* BufferChain::getFirst()
{
	if (_chain.empty())
		return NULL;
	return &_chain.front();
}

BufferChain::buffer_t* BufferChain::getLast()
{
	if (_chain.empty())
		return NULL;
	return &_chain.back();
}

void BufferChain::popFirst()
{
	_chain.pop_front();
}

BufferChain::iterator BufferChain::begin()
{
	return _chain.begin();
}

BufferChain::iterator BufferChain::end()
{
	return _chain.end();
}

size_t BufferChain::size()
{
	return _chain.size();
}

void	BufferChain::flush()
{
	buffer_t *tmp;

	while (_chain.size() > 0)
	{
		tmp = getFirst();
		delete[] tmp->data;
		popFirst();
	}
}

int		BufferChain::writeBufferToFd(BufferChain& chain, FD fd)
{
	int ret;
	BufferChain::buffer_t* buff;
	
	buff = chain.getFirst();
	ret = write(fd, buff->data, buff->size);
	if (ret == -1)
		throw IOError();
	return ret;
}

int		BufferChain::readToBuffer(BufferChain& chain, FD fd)
{
	int ret;
	
	ret = read(fd, g_read_large, BUFFER_SIZE_LARGE); // TO DO keep this size ?
	if (ret == -1)
		throw IOError();
	if (ret > 0)
		chain.copyPushBack(g_read_large, ret);
	return ret;
}

const char* IOError::what() const throw()
{
	return "An input or output eror has occurred";
}

std::ostream&	operator<<(std::ostream& out, BufferChain& chain)
{
	BufferChain::iterator it = chain.begin();

	out << "BufferChain: .size=" << chain.size() << std::endl;
	while (it != chain.end())
	{
		out << "|";
		for (size_t i = 0; i < it->size; i++)
			out << it->data[i]; // Not optimized but hey it's the easiest given the subject restricion on <cstring>
		// out.width(it->size);
		out << "| ";
		it++;
	}
	out << std::endl;
	return out;
}