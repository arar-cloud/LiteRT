# Delegate Plugin Header Indirection Validation

## Overview
This document describes the delegate plugin wrapper headers and their include path resolution patterns to prevent circular dependencies and missing implementations.

## Header Mapping

### delegate_plugin.h
- **Purpose**: Base delegate plugin interface and registration mechanism
- **Expected Location**: `tflite/core/c/common.h` or equivalent
- **Dependencies**: Core TFLite C API definitions
- **Include Pattern**: `#include "tflite/core/c/common.h"`

### gpu_plugin.h
- **Purpose**: GPU-specific delegate plugin implementation
- **Expected Location**: `tflite/core/delegates/gpu/public/delegate.h`
- **Dependencies**: GPU delegate infrastructure
- **Include Pattern**: `#include "tflite/core/delegates/gpu/public/delegate.h"`

### nnapi_plugin.h
- **Purpose**: NNAPI-specific delegate plugin implementation
- **Expected Location**: `tflite/core/delegates/nnapi/nnapi_delegate.h`
- **Dependencies**: NNAPI delegate infrastructure
- **Include Pattern**: `#include "tflite/core/delegates/nnapi/nnapi_delegate.h"`

## Build Target Compatibility

### Backend Builds
- All three headers should compile and link successfully
- CPU fallback for GPU/NNAPI when delegates unavailable

### Mobile Builds (Android/iOS)
- NNAPI delegate available on Android only
- GPU delegate conditionally available
- Headers must gracefully handle missing delegate implementations

### Web Builds
- GPU delegate typically available via WebGPU
- NNAPI not applicable
- Headers must provide no-op implementations for unavailable delegates

## Circular Dependency Prevention

1. **Header Guards**: All wrapper headers include proper `#ifndef`/`#define` guards
2. **Forward Declarations**: Use forward declarations for types when possible
3. **Include Order**: Follow strict include order: system → external → internal
4. **No Cross-Inclusion**: Wrapper headers should not include each other

## Validation Checklist

- [ ] All delegate_plugin.h redirects resolve to valid paths
- [ ] No circular include chains detected
- [ ] GPU plugin headers compile on supported platforms
- [ ] NNAPI headers compile on Android platforms
- [ ] All includes have corresponding BUILD/CMakeLists dependencies
- [ ] Type definitions compatible across delegate implementations

## Troubleshooting

### Missing Include Error: "file not found"
- Verify `tflite/core` paths exist in build output
- Check BUILD/CMakeLists.txt dependency declarations
- Ensure delegate implementation modules are enabled

### Symbol Conflict Error
- Check for duplicate type definitions across headers
- Verify namespace scoping (e.g., `tflite::delegates::*`)
- Review header guard naming for collisions

### Linking Error: "undefined reference"
- Verify delegate implementation libraries linked in target
- Check CMake target_link_libraries() declarations
- Review Bazel cc_library() dependencies
