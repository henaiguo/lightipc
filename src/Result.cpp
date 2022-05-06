#include "Result.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace LightIPC {
///////////////////////////////////////////////////////////
/// @brief		Generate a result object (success)
///////////////////////////////////////////////////////////
Result Result::CreateSuccess()
{
	Result result(true);
	return result;
}

///////////////////////////////////////////////////////////
/// @brief		Generate a result object (error occurred)
/// @param[in]	_errorMessage Error message
///////////////////////////////////////////////////////////
Result Result::CreateError(const char *_errorMessage, ...)
{
	static const int TEXT_MAX_LEN = 1024;
	char text[TEXT_MAX_LEN];
	va_list ap;
	va_start(ap, _errorMessage);
	::vsnprintf(text, TEXT_MAX_LEN - 1, _errorMessage, ap);
	va_end(ap);
	std::string errorMessage(text);

	Result result(false, errorMessage);
	return result;
}

///////////////////////////////////////////////////////////
/// @brief	Constructor
/// @note	No error
///////////////////////////////////////////////////////////
Result::Result()
	: m_result(true),m_errorMessage("")
{
	// None
}

///////////////////////////////////////////////////////////
/// @brief	Constructor
/// @param[in]	_result Success or error
/// @param[in]	_errorMessage Error message
/// @note
///////////////////////////////////////////////////////////
Result::Result(bool _result, const std::string& _errorMessage)
	: m_result(_result)
{
	if (!m_result) m_errorMessage = _errorMessage;
}

///////////////////////////////////////////////////////////
/// @brief	Copy constructor
/// @param[in]	_result Result
/// @note
///////////////////////////////////////////////////////////
Result::Result(const Result& _result)
{
	m_result = _result.m_result;
	m_errorMessage = _result.m_errorMessage;
}

///////////////////////////////////////////////////////////
/// @brief	Destructor
/// @note
///////////////////////////////////////////////////////////
Result::~Result()
{
	// None
}

///////////////////////////////////////////////////////////
/// @brief	Get error message
/// @return	std::string
/// @note
///////////////////////////////////////////////////////////
const std::string &Result::ErrorMessage() const
{
	return m_errorMessage;
}

///////////////////////////////////////////////////////////
/// @brief	Check if there is an error
/// @note	True on success and false on error
///////////////////////////////////////////////////////////
Result::operator bool() const
{
	return IsSuccess();
}	

///////////////////////////////////////////////////////////
/// @brief	Success or not
/// @return bool
/// @note	True on success
///////////////////////////////////////////////////////////
bool Result::IsSuccess() const
{
	return m_result;
}

///////////////////////////////////////////////////////////
/// @brief	Error or not
/// @return bool
/// @note	True on error
///////////////////////////////////////////////////////////
bool Result::IsError() const
{
	return !m_result;
}

}