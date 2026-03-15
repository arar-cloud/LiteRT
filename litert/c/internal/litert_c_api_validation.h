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

#ifndef LITERT_C_INTERNAL_LITERT_C_API_VALIDATION_H_
#define LITERT_C_INTERNAL_LITERT_C_API_VALIDATION_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Maximum allowed tensor shape dimension value (prevents integer overflow in buffer size calculations)
#define LITERT_MAX_TENSOR_SHAPE_DIM UINT32_MAX

// Maximum allowed buffer size (256 MB safety limit for single buffer)
#define LITERT_MAX_BUFFER_SIZE (256 * 1024 * 1024)

// Maximum allowed tensor rank
#define LITERT_MAX_TENSOR_RANK 16

// Validates a tensor shape array for bounds and overflow risks
// Returns: non-zero on success, zero on validation failure
inline int LiteRtValidateTensorShape(const uint32_t* shape, size_t shape_len) {
  if (shape == NULL && shape_len > 0) return 0;
  if (shape_len > LITERT_MAX_TENSOR_RANK) return 0;
  
  uint64_t total_size = 1;
  for (size_t i = 0; i < shape_len; ++i) {
    if (shape[i] > LITERT_MAX_TENSOR_SHAPE_DIM) return 0;
    // Check for multiplication overflow
    if (total_size > LITERT_MAX_BUFFER_SIZE / shape[i]) return 0;
    total_size *= shape[i];
  }
  return 1;
}

// Validates a buffer pointer and size for safety
// Returns: non-zero on success, zero on validation failure
inline int LiteRtValidateBuffer(const void* buffer, size_t buffer_size) {
  if (buffer == NULL && buffer_size > 0) return 0;
  if (buffer_size > LITERT_MAX_BUFFER_SIZE) return 0;
  return 1;
}

// Validates pointer is non-null
// Returns: non-zero on success, zero on validation failure
inline int LiteRtValidateNonNullPtr(const void* ptr) {
  return ptr != NULL ? 1 : 0;
}

#ifdef __cplusplus
}
#endif

#endif  // LITERT_C_INTERNAL_LITERT_C_API_VALIDATION_H_
