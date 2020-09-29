#include "AderException.h"
#include <sstream>


AderException::AderException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* AderException::what() const noexcept
{
	std::ostringstream oss;
	oss << getType() << std::endl
		<< getSourceString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}


int AderException::getLine() const noexcept
{
	return line;
}

const std::string& AderException::getFile() const noexcept
{
	return file;
}

const char* AderException::getType() const noexcept
{
	return "Ader Exception";
}

std::string AderException::getSourceString() const noexcept
{
	std::ostringstream oss;
	oss << "File :: " << file << std::endl << "Line :: " << line;
	return oss.str();
}