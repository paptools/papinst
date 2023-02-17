#include "pathinst/transformer.h"

#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/AST/ASTContext.h>

Transformer::Transformer(clang::ASTContext &context, clang::Rewriter &rewriter)
    : context(context), rewriter(rewriter) {}
