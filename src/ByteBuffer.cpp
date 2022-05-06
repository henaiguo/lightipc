#include "ByteBuffer.h"

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @brief		Constructor
/// @param[in]	_reserve Size when buffer is expanded
/// @return		None
/// @note
///////////////////////////////////////////////////////////
ByteBuffer::ByteBuffer(int _reserve)
{
	m_buffer.reserve(_reserve);
	m_position = 0;
}

///////////////////////////////////////////////////////////
/// @brief		Constructor
/// @param[in]	_data Initial data
/// @param[in]	_reserve Size when buffer is expanded
/// @return		None
/// @note
///////////////////////////////////////////////////////////
ByteBuffer::ByteBuffer(const std::string &_data, int _reserve)
{
	m_buffer.reserve(_reserve);
	m_buffer.assign(_data);
	m_position = 0;
}

///////////////////////////////////////////////////////////
/// @brief		Constructor
/// @param[in]	_data Initial data
/// @param[in]	_size Data size
/// @param[in]	_reserve Size when buffer is expanded
/// @return		None
/// @note
///////////////////////////////////////////////////////////
ByteBuffer::ByteBuffer(const char *_data, size_t _size, int _reserve)
{
	m_buffer.reserve(_reserve);
	m_buffer.assign(_data, _size);
	m_position = 0;
}

///////////////////////////////////////////////////////////
/// @brief		Destructor
/// @return		None
/// @note
///////////////////////////////////////////////////////////
ByteBuffer::~ByteBuffer()
{
}

///////////////////////////////////////////////////////////
/// @brief		Check if the buffer is empty
/// @return		True if empty
/// @note
///////////////////////////////////////////////////////////
bool ByteBuffer::IsEmpty() const
{
	return m_buffer.empty();
}

///////////////////////////////////////////////////////////
/// @brief		Get the current size of the buffer
/// @return		Current size
/// @note
///////////////////////////////////////////////////////////
size_t ByteBuffer::Size() const
{
	return m_buffer.size();
}

///////////////////////////////////////////////////////////
/// @brief		Clear the buffer
/// @return		None
/// @note
///////////////////////////////////////////////////////////
void ByteBuffer::Clear()
{
	m_buffer.clear();
	m_position = 0;
}

///////////////////////////////////////////////////////////
/// @brief		Get buffer contents as a string
/// @return		Byte data
/// @note
///////////////////////////////////////////////////////////
const std::string &ByteBuffer::Data() const
{
	return m_buffer;
}

///////////////////////////////////////////////////////////
/// @brief		ByteBuffer Add
/// @param[in]	_data Data to write
/// @return		ByteBuffer
/// @note
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Append(ByteBuffer &_data)
{
	return Append(_data.Data());
}

///////////////////////////////////////////////////////////
/// @brief		Add a string (std::string)
/// @param[in]	_data Data to write
/// @return		ByteBuffer
/// @note
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Append(const std::string &_data)
{
	// Order of calling template and non-template functions
	//  If the non-template function matches the template function equivalently,
	//  Non-template functions are selected unless template arguments are explicitly specified

	// std::string Add size and string together
	Append(_data.size());
	m_buffer.append(_data);
	return *this;
}

///////////////////////////////////////////////////////////
/// @brief		Add a string (char *)
/// @param[in]	_data Data to write
/// @return		ByteBuffer
/// @note
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Append(char *_data)
{
	// char * Add size and string together
	Append(strlen(_data));
	m_buffer.append(_data);
	return *this;
}

///////////////////////////////////////////////////////////
/// @brief		Add a formatted string
/// @param[in]	_format format
/// @param[in]	_args Variadic arguments
/// @return		ByteBuffer
/// @note		ex) buf.Append("[%04d] %s", lineNo, message.c_str());
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Append(const char *_format, ...)
{
	static const int LOG_TEXT_MAX_LEN = 1024*4;
	char text[LOG_TEXT_MAX_LEN];
    va_list ap;
    va_start(ap, _format);
	::vsnprintf(text, sizeof(text), _format, ap);
    va_end(ap);
	return Append(text);
}


///////////////////////////////////////////////////////////
/// @brief		size_t Add
/// @param[in]	_data Data to write
/// @return		ByteBuffer
/// @note
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Append(size_t _data)
{
	// sizeof(size_t) Is
	// 32bit 4 bytes (32 bits) in the environment
	// 64bit 8 bytes (64 bits) in the environment
	// So Append as int
	return Append(static_cast<int>(_data));
}

///////////////////////////////////////////////////////////
/// @brief		ByteBuffer To get
/// @param[out]	_out data
/// @return		ByteBuffer
/// @note		Take them out in the order they are added 
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Value(ByteBuffer &_out)
{
	int size = 0;
	Value(size);
    _out = ByteBuffer(static_cast<const char *>(m_buffer.data() + m_position), size, size);
	m_position += size;
	return *this;
}

///////////////////////////////////////////////////////////
/// @brief		Get string (std::string)
/// @param[out]	_out data
/// @return		ByteBuffer
/// @note		Take them out in the order they are added 
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Value(std::string &_out)
{
	// std::string Gets the size and string together
	int size = 0;
	Value(size);
	_out.assign(m_buffer.data() + m_position, size);
	m_position += size;
	return *this;
}

///////////////////////////////////////////////////////////
/// @brief		Get a string (char *)
/// @param[out]	_out data
/// @return		ByteBuffer
/// @note		Take them out in the order they are added 
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Value(char *_out)
{
	int size = 0;
	Value(size);
	::strncpy(_out, m_buffer.data() + m_position, size);
    _out[size] = '\0';
	m_position += size;
	return *this;
}

///////////////////////////////////////////////////////////
/// @brief		size_t Get value by type
/// @param[out]	_out data
/// @return		ByteBuffer
/// @note		Take them out in the order they are added 
///////////////////////////////////////////////////////////
ByteBuffer &ByteBuffer::Value(size_t &_out)
{
	// int To extract and store in size_t (supports 64-bit environment)
	int size = 0;
	Value(size);
	_out = size;
	return *this;
}

///////////////////////////////////////////////////////////
/// @brief		Get the position of the data pointer
/// @return		The position of the data pointer
/// @note		
///////////////////////////////////////////////////////////
unsigned int ByteBuffer::Position() const
{
    return m_position;
}

///////////////////////////////////////////////////////////
/// @brief		Move the data pointer position to the specified position
/// @param[int]	pos position
/// @note		
///////////////////////////////////////////////////////////
void ByteBuffer::SetPosition(unsigned int pos)
{
    m_position = pos;
}

///////////////////////////////////////////////////////////
/// @brief		16 Get a hex dump
/// @return		16 Susumu dump
/// @note		
///////////////////////////////////////////////////////////
std::string ByteBuffer::Dump() const
{
	char buffer[1024];
    size_t bufSize = sizeof(buffer);

    std::string dump = "";
	static const unsigned long COL_PER_ROW = 16UL;
	unsigned long pos = 0;
	unsigned long size = Size();
	const std::string &data = Data();

	::snprintf(buffer, bufSize, "Size=%lu\n", size);
	dump += std::string(buffer);
	int row = 0;
	while (pos < size) {
        std::string hex, ascii;
		// 16 Hexadecimal notation + ASCII notation
		for (unsigned long i = 0; i < COL_PER_ROW; i++) {
			if (pos < size) {
				unsigned char d = data[pos];
                ::snprintf(buffer, bufSize, "%02x ", d & 0xff);
				hex += std::string(buffer);
				if (::isprint(d)) {
                    ::snprintf(buffer, bufSize, "%c", d & 0xff);
                    ascii += std::string(buffer);
				}
				else {
					ascii += ".";
				}
				pos++;
			}
			else {
				hex += "   ";
				ascii += " ";
			}
		}
        ::snprintf(buffer, bufSize, "%03d0 | %s| %s\n", row, hex.c_str(), ascii.c_str());
        dump += std::string(buffer);
		row++;
	}
    return dump;
}

///////////////////////////////////////////////////////////
/// @brief		Hex dump to standard output
/// @note		
///////////////////////////////////////////////////////////
void ByteBuffer::Print() const
{
    std::string dump = Dump();
    ::printf("%s\n",dump.c_str());
}

///////////////////////////////////////////////////////////
/// @brief		Hex dump to standard output
/// @param[int]	title title
/// @note		
///////////////////////////////////////////////////////////
void ByteBuffer::Print(const std::string &title) const
{
    std::string dump = Dump();
    ::printf("%s\n%s\n",title.c_str(), dump.c_str());
}

}
