#include "wglerror.h"

#include <fmt/format.h>

WGLError::WGLError(const std::string &msg, antlr4::ParserRuleContext *ctx) {
	message_ = fmt::format("[{}] {}",
		ctx ? std::to_string(ctx->getStart()->getLine()) : std::string{},
		msg
	);
}
