#ifndef PATHINST_INSTRUMENTER_H
#define PATHINST_INSTRUMENTER_H

#include <string>

namespace pathinst {
class Instrumenter {
public:
  const std::string GetLibraryIncludeLine(void) const;
};
} // namespace pathinst

#endif // PATHINST_INSTRUMENTER_H
