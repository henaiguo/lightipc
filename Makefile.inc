#---------------------------------------
# Include & Libraly
#---------------------------------------
INCLUDE = \
	-I./include	\

#---------------------------------------
# Sources
#---------------------------------------
SRCS += \
    src/ByteBuffer.cpp \
    src/Result.cpp \
    src/Mutex.cpp \
    src/MutexLock.cpp \
    src/Semaphore.cpp \
    src/SharedMemory.cpp \
    src/SharedMemoryContext.cpp \
    src/Thread.cpp \
    src/ThreadAbstract.cpp \
    src/UnixDomainSocket.cpp \
    src/UnixDomainSocketClient.cpp \
    src/UnixDomainSocketServer.cpp \
    src/MessageQueue.cpp \
    
