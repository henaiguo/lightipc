///////////////////////////////////////////////////////////
/// @file	ByteBuffer.h
/// @brief	Byte buffer
/// @author	henaiguo
///////////////////////////////////////////////////////////

#ifndef __LIGHT_IPC_BYTE_BUFFER_
#define __LIGHT_IPC_BYTE_BUFFER_

#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @class ByteBuffer
/// @brief	Byte buffer
/// @note Primitive type, string, vector, map is managed by byte array
///////////////////////////////////////////////////////////
class ByteBuffer {
public:
	///////////////////////////////////////////////////////////
	/// @brief		Constructor
	/// @param[in]	_reserve Size when buffer is expanded
	/// @return		None
	/// @note
	///////////////////////////////////////////////////////////
	ByteBuffer(int _reserve = 2048);

	///////////////////////////////////////////////////////////
	/// @brief		Constructor
	/// @param[in]	_data Initial data
	/// @param[in]	_reserve Size when buffer is expanded
	/// @return		None
	/// @note		const std::string &Data() const std::string obtained in
	/// 			ByteBuffer can be restored by specifying
	///////////////////////////////////////////////////////////
	ByteBuffer(const std::string &_data, int _reserve = 2048);

	///////////////////////////////////////////////////////////
	/// @brief		Constructor
	/// @param[in]	_data Initial data
	/// @param[in]	_size Data size
	/// @param[in]	_reserve Size when buffer is expanded
	/// @return		None
	/// @note		const std::string &Data() const The char array of the value obtained in
	/// 			ByteBuffer can be restored by specifying
	///////////////////////////////////////////////////////////
	ByteBuffer(const char *_data, size_t _size, int _reserve = 2048);

	///////////////////////////////////////////////////////////
	/// @brief		Destructor
	/// @return		None
	/// @note
	///////////////////////////////////////////////////////////
	~ByteBuffer();

	///////////////////////////////////////////////////////////
	/// @brief		Check if the buffer is empty
	/// @return		True if empty
	/// @note
	///////////////////////////////////////////////////////////
	bool IsEmpty() const;

	///////////////////////////////////////////////////////////
	/// @brief		Get the current size of the buffer
	/// @return		Current size
	/// @note
	///////////////////////////////////////////////////////////
	size_t Size() const;

	///////////////////////////////////////////////////////////
	/// @brief		Clear the buffer
	/// @return		None
	/// @note
	///////////////////////////////////////////////////////////
	void Clear();

	///////////////////////////////////////////////////////////
	/// @brief		Get buffer contents as a string
	/// @return		Byte data
	/// @note		Serialized raw data
	/// 			ByteBuffer ByteBuffer can be restored by passing it to the constructor of
	///////////////////////////////////////////////////////////
	const std::string &Data() const;

	///////////////////////////////////////////////////////////
	/// @brief		ByteBuffer Add
	/// @param[in]	_data Data to write
	/// @return		ByteBuffer
	/// @note
	///////////////////////////////////////////////////////////
	ByteBuffer &Append(ByteBuffer &_data);

	///////////////////////////////////////////////////////////
	/// @brief		Add a string (std::string)
	/// @param[in]	_data Data to write
	/// @return		ByteBuffer
	/// @note
	///////////////////////////////////////////////////////////
	ByteBuffer &Append(const std::string &_data);

	///////////////////////////////////////////////////////////
	/// @brief		Add a string (char *)
	/// @param[in]	_data Data to write
	/// @return		ByteBuffer
	/// @note		null('\0') Terminated
	///////////////////////////////////////////////////////////
	ByteBuffer &Append(char *_data);

	///////////////////////////////////////////////////////////
	/// @brief		Add a formatted string
	/// @param[in]	_format format
	/// @param[in]	_args Variadic arguments
	/// @return		ByteBuffer
	/// @note		ex) buf.Append("[%04d] %s", lineNo, message.c_str());
	///////////////////////////////////////////////////////////
	ByteBuffer &Append(const char *_format, ...);

	///////////////////////////////////////////////////////////
	/// @brief		size_t Add
	/// @param[in]	_data Data to write
	/// @return		ByteBuffer
	/// @note		The data must be within the sizeof(int)
	///////////////////////////////////////////////////////////
	ByteBuffer &Append(size_t _data);

	///////////////////////////////////////////////////////////
	/// @brief		std::vector<T> Add
	/// @param[in]	_data Data to write
	/// @return		ByteBuffer
	/// @note
	/// Element T supports only primitive types or structures consisting of only primitive types
	///////////////////////////////////////////////////////////
	template <class T>
	ByteBuffer &Append(const std::vector<T> &_data)
	{
		// std::vector<T> Add size and elements in bulk
		int v = _data.size();
		Append(v);
		for (int i = 0; i < v; i++) {
			Append(_data.at(i));
		}
		return *this;
	}

	///////////////////////////////////////////////////////////
	/// @brief		std::map<K,V> Add
	/// @param[in]	_data Data to write
	/// @return		ByteBuffer
	/// Key K and element V are only supported for primitive types or structures composed of only primitive types
	///////////////////////////////////////////////////////////
	template <class K, class V>
	ByteBuffer &Append(const std::map<K,V> &_data)
	{
		// std::map<K,V> Adds the size and key, value together
		// Typename is required (probably) in template parameter
		const typename std::map<K,V> &m = _data;
		int v = m.size();
		Append(v);
		typename std::map<K,V>::const_iterator ite = _data.begin();
		typename std::map<K,V>::const_iterator end = _data.end();
		for (; ite != end; ite++) {
			Append(ite->first);
			Append(ite->second);
		}
		return *this;
	}

	///////////////////////////////////////////////////////////
	/// @brief		Add data type T
	/// @param[in]	_data Data to write
	/// @return		ByteBuffer
	/// @note
	/// std::vector, std::map, std::string Also adds the number of elements 
	/// sizeof(T) Primitive type or element is a primitive type because it cannot be calculated by 
	/// Use only configured structures
	///////////////////////////////////////////////////////////
	template <class T>
	ByteBuffer &Append(T _data)
	{
		m_buffer.append(reinterpret_cast<const char*>(&_data), sizeof(_data));
		return *this;
	}

	///////////////////////////////////////////////////////////
	/// @brief		Add data type T
	/// @param[in]	_data Data to write
	/// @return		ByteBuffer
	/// @note
	/// std::vector, std::map, std::string Also adds the number of elements 
	/// sizeof(T) Primitive type or element is a primitive type because it cannot be calculated by 
	/// Use only configured structures
	///////////////////////////////////////////////////////////
	template <class T>
	ByteBuffer &operator<<(T _data)
	{
		return Append(_data);
	}

	///////////////////////////////////////////////////////////
	/// @brief		ByteBuffer To get
	/// @param[out]	_out data
	/// @return		ByteBuffer
	/// @note		Take them out in the order they are added 
	///////////////////////////////////////////////////////////
	ByteBuffer &Value(ByteBuffer &_out);

	///////////////////////////////////////////////////////////
	/// @brief		Get string (std::string)
	/// @param[out]	_out data
	/// @return		ByteBuffer
	/// @note		Take them out in the order they are added 
	///////////////////////////////////////////////////////////
	ByteBuffer &Value(std::string &_out);

	///////////////////////////////////////////////////////////
	/// @brief		Get a string (char *)
	/// @param[out]	_out data
	/// @return		ByteBuffer
	/// @note		Take them out in the order they are added 
	///////////////////////////////////////////////////////////
	ByteBuffer &Value(char *_out);

	///////////////////////////////////////////////////////////
	/// @brief		size_t Get value by type
	/// @param[out]	_out data
	/// @return		ByteBuffer
	/// @note		Take them out in the order they are added 
	///////////////////////////////////////////////////////////
	ByteBuffer &Value(size_t &_out);

	///////////////////////////////////////////////////////////
	/// @brief		std::vector<T> To get
	/// @param[out]	_out data
	/// @return		ByteBuffer
	/// @note		Take them out in the order they are added 
	///////////////////////////////////////////////////////////
	template <class T>
	ByteBuffer &Value(std::vector<T> &_out)
	{
		// vector Get the size and elements of the
		int size = 0;
		Value(size);
		for (int i = 0; i < size; i++) {
			T value;
			Value(value);
			_out.push_back(value);
		}
		return *this;
	}

	///////////////////////////////////////////////////////////
	/// @brief		std::map<K,V> To get
	/// @param[out]	_out data
	/// @return		ByteBuffer
	/// @note		Take them out in the order they are added 
	///////////////////////////////////////////////////////////
	template <class K, class V>
	ByteBuffer &Value(std::map<K,V> &_out)
	{
		// std::map<K,V> Size of key and key, value are acquired collectively
		int size = 0;
		Value(size);
		for (int i = 0; i < size; i++) {
			K key;
			Value(key);
			V val;
			Value(val);
			_out.insert(std::make_pair(key,val));
		}
		return *this;
	}

	///////////////////////////////////////////////////////////
	/// @brief		Get data type T
	/// @param[out]	_out data
	/// @return		ByteBuffer
	/// @note		Take them out in the order they are added 
	///////////////////////////////////////////////////////////
	template <class T>
	ByteBuffer &Value(T &_out)
	{
		int size = sizeof(_out);
		::memcpy(&_out, m_buffer.data() + m_position, size);
		m_position += size;
		return *this;
	}

	///////////////////////////////////////////////////////////
	/// @brief		Get data type T
	/// @param[out]	_out data
	/// @return		ByteBuffer
	/// @note		Take them out in the order they are added 
	///////////////////////////////////////////////////////////
	template <class T>
	ByteBuffer &operator>>(T &_out)
	{
		return Value(_out);
	}

	///////////////////////////////////////////////////////////
	/// @brief		Get the position of the data pointer
	/// @return		The position of the data pointer
	/// @note		
	///////////////////////////////////////////////////////////
	unsigned int Position() const;

	///////////////////////////////////////////////////////////
	/// @brief		Move the data pointer position to the specified position
	/// @param[int]	pos position
	/// @note		
	///////////////////////////////////////////////////////////
	void SetPosition(unsigned int pos);

	///////////////////////////////////////////////////////////
	/// @brief		16 Get a hex dump
	/// @return		16 Susumu dump
	/// @note		
	///////////////////////////////////////////////////////////
	std::string Dump() const;

	///////////////////////////////////////////////////////////
	/// @brief		Hex dump to standard output
	/// @note		
	///////////////////////////////////////////////////////////
	void Print() const;

	///////////////////////////////////////////////////////////
	/// @brief		Hex dump to standard output
	/// @param[int]	title title
	/// @note		
	///////////////////////////////////////////////////////////
	void Print(const std::string &title) const;

private:
	/// Internal buffer
	std::string m_buffer;

	/// Data pointer position
	unsigned int m_position;
};
}

#endif
