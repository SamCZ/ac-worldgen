#pragma once

#include <string>

#include "wglinclude.h"

class WGLError : std::exception {

public:
	WGLError(const std::string &msg, antlr4::ParserRuleContext *ctx);

public:
	inline const std::string &message() const {
		return message_;
	}

private:
	std::string message_;

};

