#pragma once

#include <string>
#include <exception>

class AderException : public std::exception
{
public:
	AderException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	int getLine() const noexcept;
	const std::string& getFile() const noexcept;
	virtual const char* getType() const noexcept;
	std::string getSourceString() const noexcept;
private:
	int line;
	std::string file;
protected:
	mutable std::string whatBuffer;
};


