#ifndef PAPINST_MOCK_AST_CONSUMER_LISTENER_H
#define PAPINST_MOCK_AST_CONSUMER_LISTENER_H

// Local headers.
#include "papinst/ast_consumer_listener.h"

// Third-party headers.
#include <gmock/gmock.h>

namespace papinst {
class MockASTConsumerListener : public ASTConsumerListener {
public:
  MOCK_METHOD(void, ProcessTranslationUnit, (clang::ASTContext & context),
              (override));
  MOCK_METHOD(void, ProcessError, (const std::string &message), (override));
};
} // namespace papinst

#endif // PAPINST_MOCK_AST_CONSUMER_LISTENER_H
