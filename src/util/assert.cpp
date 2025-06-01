#include <cassert>

#include <iostream>
#include <fmt/format.h>
#include "assert.h"

void assertError(const char *assert, const char *file, int line, const std::string &msg) {
	std::cerr << fmt::format("\n!!! assert FAILURE ({}:{}): {}\n", file, line, !msg.empty() ? msg : assert);
	throw AssertFailureException();
}
