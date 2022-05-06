#include "SharedMemoryContext.h"

namespace LightIPC {

SharedMemoryContext::SharedMemoryContext()
{
}

SharedMemoryContext::~SharedMemoryContext()
{
	std::map<const std::string, SharedMemory *>::iterator ite = m_sharedMemories.begin();
	std::map<const std::string, SharedMemory *>::iterator end = m_sharedMemories.end();
	for (; ite != end; ite++) {
		delete ite->second;
	}

	m_sharedMemories.clear();
}

SharedMemory *SharedMemoryContext::bindSharedMemory(const std::string &name, size_t size, bool isOwner)
{
	SharedMemory *sm = NULL;
	if (m_sharedMemories.count(name) == 0) {
		if (isOwner) {
			sm = new SharedMemory(name, size, isOwner);
		} else {
			Result result = SharedMemory::Exist(name);
			if (result.IsSuccess()) {
				sm = new SharedMemory(name, size, isOwner);
			}
		}
		m_sharedMemories.insert(std::make_pair(name, sm));
	} else {
		sm = m_sharedMemories[name];
	}
	return sm;
}

}
