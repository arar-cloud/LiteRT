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

#ifndef LITERT_C_INTERNAL_LITERT_PATH_VALIDATION_H_
#define LITERT_C_INTERNAL_LITERT_PATH_VALIDATION_H_

#include <string>

namespace litert {
namespace internal {

// Validates and canonicalizes a model file path
// Prevents directory traversal attacks by:
// - Resolving .. and . components
// - Ensuring path stays within allowed directories
// - Checking for symbolic link attacks
//
// Args:
//   model_path: The input model path to validate
//   allowed_base_dirs: Array of allowed base directories (e.g., /models, /cache)
//   num_allowed_dirs: Number of allowed base directories
//
// Returns:
//   Canonicalized path on success, empty string on validation failure
std::string ValidateAndCanonicalizePath(const std::string& model_path,
                                        const char** allowed_base_dirs,
                                        size_t num_allowed_dirs);

// Validates delegate plugin path against allowlist
// Args:
//   plugin_path: The input plugin path to validate
//   allowed_plugin_dirs: Array of allowed plugin directories
//   num_allowed_dirs: Number of allowed plugin directories
//
// Returns:
//   true if path is valid and within allowed directories, false otherwise
bool ValidateDelegatePluginPath(const std::string& plugin_path,
                                const char** allowed_plugin_dirs,
                                size_t num_allowed_dirs);

}  // namespace internal
}  // namespace litert

#endif  // LITERT_C_INTERNAL_LITERT_PATH_VALIDATION_H_
