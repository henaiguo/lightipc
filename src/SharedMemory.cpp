#include "SharedMemory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <cstdio>
#include <cstring>

#include <string>
#include <cerrno>
#include <cassert>

namespace LightIPC {

Result SharedMemory::Exist(const std::string &name)
{
	if (name.size() == 0) {
		return Result::CreateError("shared memory invalid name [%s]\n","empty name");
	}

	if (name[0] != '/') {
		return Result::CreateError("shared memory invalid name [%s]\n","it does not begin with '/'");
	}

	// open POSIX shared memory
	int fd = ::shm_open(name.c_str(), O_RDONLY, S_IRUSR);
	if (fd == -1) {
		return Result::CreateError("shared memory open error [%s]\n", std::strerror(errno));
	}
	::close(fd);

	return Result::CreateSuccess();
}

SharedMemory::SharedMemory(const std::string &name, size_t size, bool isOwner)
	: m_name(name)
	, m_isOwner(isOwner)
	, m_memoryMap(NULL)
	, m_size(0UL)
	, m_semaphore(NULL)
{
	if (name.size() == 0) {
		std::fprintf(stderr, "shared memory invalid name [%s]\n","empty name");
		return;
	}

	if (name[0] != '/') {
		std::fprintf(stderr, "shared memory invalid name [%s]\n","it does not begin with '/'");
		return;
	}

	if (size == 0) {
		std::fprintf(stderr, "shared memory creation error [%s]\n", "size is 0");
		return;
	}

	const char *named = m_name.c_str();

	// create/open POSIX shared memory
	{
		int fd = -1;
		if (m_isOwner) {
			// remove old shared memory
			::shm_unlink(named);
			// create shared memory
			fd = ::shm_open(named, O_CREAT|O_RDWR|O_EXCL, S_IRUSR|S_IWUSR);
		} else {
			// open shared memory
			fd = ::shm_open(named, O_RDWR, S_IRUSR|S_IWUSR);
		}

		if (fd == -1) {
			std::fprintf(stderr, "shared memory creation error [%s]\n", std::strerror(errno));
			return;
		}


		// set shared memory size
		m_size = size;
		if (isOwner) {
		   int result = ::ftruncate(fd, m_size);
		   if (result != 0) {
				std::fprintf(stderr, "shared memory data truncate error [%s]\n", std::strerror(errno));
		   }
		}

		// map shared memory to user defined struct
		m_memoryMap = ::mmap(NULL, m_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		// close shared memory file discriptor
		::close(fd);
	}

	// create/open named semaphore
	{
		m_semaphore = new Semaphore(m_name, m_isOwner);
	}
}

SharedMemory::~SharedMemory()
{
	if (m_memoryMap) {
		::munmap(m_memoryMap, m_size);
		if (m_isOwner) {
			const char *named = m_name.c_str();
			::shm_unlink(named);
		}
		delete m_semaphore;
	}
}

const std::string &SharedMemory::Name() const
{
	return m_name;
}

void SharedMemory::Wait()
{
	if (m_memoryMap) {
		m_semaphore->Wait();
	}
}

void SharedMemory::Post()
{
	if (m_memoryMap) {
		m_semaphore->Post();
	}
}

}
