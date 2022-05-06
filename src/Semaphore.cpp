#include "Semaphore.h"

#include <fcntl.h>

#include <cstdio>
#include <cerrno>
#include <cassert>
#include <cstring>

namespace LightIPC {

Result Semaphore::Exist(const std::string &name)
{
	if (name.size() == 0) {
		return Result::CreateError("semaphore invalid name [%s]\n","empty name");
	}

	if (name[0] != '/') {
		return Result::CreateError("semaphore invalid name [%s]\n","it does not begin with '/'");
	}

	const char *named = name.c_str();

	// open POSIX semaphore
	sem_t *semaphore = ::sem_open(named, O_RDONLY, S_IRUSR);
	if (semaphore == SEM_FAILED) {
		return Result::CreateError("semaphore open error [%s]\n", std::strerror(errno));
	}
	::sem_close(semaphore);

	return Result::CreateSuccess();
}

Semaphore::Semaphore(const std::string &name, bool isOwner)
	: m_name(name)
	, m_isOwner(isOwner)
	, m_semaphore(NULL)
{
	if (name.size() == 0) {
		std::fprintf(stderr, "semaphore creation error [%s]\n","empty name");
		return;
	}

	if (name[0] != '/') {
		std::fprintf(stderr, "semaphore creation error [%s]\n","it does not begin with '/'");
		return;
	}

	const char *named = m_name.c_str();

	// remove old semaphore
	if (m_isOwner) {
		::sem_unlink(named);
	}

	// create semaphore
	if (m_isOwner) {
		m_semaphore = ::sem_open(named, O_CREAT|O_RDWR|O_EXCL, S_IRUSR|S_IWUSR, 1);
	} else {
		m_semaphore = ::sem_open(named, O_RDWR, S_IRUSR|S_IWUSR);
	}

	if (m_semaphore == SEM_FAILED) {
		m_semaphore = NULL;
		std::fprintf(stderr, "semaphore creation error [%s]\n", std::strerror(errno));
		return;
	}
}

Semaphore::~Semaphore()
{
	if (m_semaphore) {
		::sem_close(m_semaphore);
		if (m_isOwner) {
			const char *named = m_name.c_str();
			::sem_unlink(named);
		}
	}
}

const std::string &Semaphore::Name() const
{
	return m_name;
}

void Semaphore::Wait()
{
	if (m_semaphore) {
		::sem_wait(m_semaphore);
	}
}

void Semaphore::Post()
{
	if (m_semaphore) {
		int v = 0;
		::sem_getvalue(m_semaphore, &v);
		if (v < 1) {
			::sem_post(m_semaphore);
		}
	}
}

}
