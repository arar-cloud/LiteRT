// Copyright 2024 The AI Edge LiteRT Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ==============================================================================

#include "litert/c/internal/litert_path_validation.h"

#include <algorithm>
#include <cstring>
#include <limits.h>
#include <stdlib.h>

namespace litert {
namespace internal {

std::string ValidateAndCanonicalizePath(const std::string& model_path,
                                        const char** allowed_base_dirs,
                                        size_t num_allowed_dirs) {
  if (model_path.empty() || !allowed_base_dirs || num_allowed_dirs == 0) {
    return "";
  }

  // Use realpath to resolve . and .. components and symlinks
  char resolved_path[PATH_MAX];
  if (realpath(model_path.c_str(), resolved_path) == nullptr) {
    // If realpath fails, the path doesn't exist or is invalid
    return "";
  }

  std::string canonical_path(resolved_path);

  // Verify the canonical path is within an allowed base directory
  for (size_t i = 0; i < num_allowed_dirs; ++i) {
    const std::string& allowed_dir = allowed_base_dirs[i];
    
    // Ensure allowed_dir doesn't end with / for consistent comparison
    std::string base = allowed_dir;
    if (!base.empty() && base.back() == '/') {
      base.pop_back();
    }
    
    // Check if canonical path starts with allowed directory
    if (canonical_path == base || 
        (canonical_path.find(base + "/") == 0)) {
      return canonical_path;
    }
  }

  // Path is outside allowed directories
  return "";
}

bool ValidateDelegatePluginPath(const std::string& plugin_path,
                                const char** allowed_plugin_dirs,
                                size_t num_allowed_dirs) {
  if (plugin_path.empty() || !allowed_plugin_dirs || num_allowed_dirs == 0) {
    return false;
  }

  // Check for suspicious patterns
  if (plugin_path.find("..") != std::string::npos ||
      plugin_path.find("~") != std::string::npos) {
    return false;
  }

  // Use realpath to resolve and validate path
  char resolved_path[PATH_MAX];
  if (realpath(plugin_path.c_str(), resolved_path) == nullptr) {
    return false;
  }

  std::string canonical_path(resolved_path);

  // Verify plugin path is within an allowed directory
  for (size_t i = 0; i < num_allowed_dirs; ++i) {
    const std::string& allowed_dir = allowed_plugin_dirs[i];
    
    std::string base = allowed_dir;
    if (!base.empty() && base.back() == '/') {
      base.pop_back();
    }
    
    if (canonical_path == base || 
        (canonical_path.find(base + "/") == 0)) {
      // Additional check: ensure it's a shared library file
      if (canonical_path.find(".so") != std::string::npos ||
          canonical_path.find(".dylib") != std::string::npos ||
          canonical_path.find(".dll") != std::string::npos) {
        return true;
      }
    }
  }

  return false;
}

}  // namespace internal
}  // namespace litert
