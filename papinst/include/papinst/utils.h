#ifndef PAPINST_UTILS_H
#define PAPINST_UTILS_H

// Local headers.
#include "papinst/logger.h"

// C++ standard library headers.
#include <string>
#include <vector>

namespace papinst {
namespace utils {
std::string ToString(const std::vector<std::string> &v, const char sep);

bool IsSupportedCompiler(const std::string &compiler);

bool IsGNU(const std::string &compiler);

bool IsSourceFile(const std::string &filepath);

std::string GetFileContents(const std::string &filepath);

std::string CreateInstFile(std::shared_ptr<Logger> logger,
                           const std::string &filepath);

void RemoveInstFile(std::shared_ptr<Logger> logger,
                    const std::string &filepath);

void SetDryRun(bool dry_run);

bool GetDryRun(void);
} // namespace utils
} // namespace papinst

#endif // PAPINST_UTILS_H
