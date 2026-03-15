# TFLite Failing-Path Diagnostics Guide

## Overview
This document provides guidance for identifying and diagnosing common failure patterns in TFLite builds, particularly in the tflite module area.

## Common Failure Patterns

### 1. Missing Delegate Plugin Headers
**Symptoms**:
- Compilation error: `fatal error: 'tflite/core/delegates/gpu/public/delegate.h' file not found`
- Build fails in:
  - litert/c/internal/litert_delegate_wrapper.cc
  - litert/cc/internal/litert_dispatch_delegate.h

**Root Causes**:
- TFLite core headers not present in workspace
- Delegate implementation modules not included in build
- Incorrect include path configuration

**Diagnostic Steps**:
```bash
# Check if tflite/core directory exists
find . -type d -name "tflite" | head -20

# Verify delegate headers are present
ls -la tflite/core/delegates/gpu/public/
ls -la tflite/core/delegates/nnapi/

# Check BUILD file dependencies
grep -r "delegate" litert/c/internal/BUILD
```

### 2. Circular Include Dependencies
**Symptoms**:
- Compilation warning: `#include cycle detected`
- Linker error: `undefined reference to` (with multiple definitions)

**Root Causes**:
- Wrapper headers including each other indirectly
- Missing forward declarations
- Incorrect include order

**Diagnostic Steps**:
```bash
# Analyze include dependencies (requires clang tools)
include-what-you-use litert/c/internal/litert_delegate_wrapper.h

# Manual inspection
grep -n "#include" litert/c/internal/litert_delegate_wrapper.h
```

### 3. Type Compatibility Issues
**Symptoms**:
- Compilation error: `no matching function for call to`
- Type mismatch between delegate implementations
- ABI compatibility warnings

**Root Causes**:
- Incompatible type definitions across modules
- Version mismatch between TFLite and LiteRT
- Missing type conversions

**Diagnostic Steps**:
```bash
# Check type definitions
grep -r "typedef.*delegate" litert/c/
grep -r "typedef.*delegate" tflite/core/

# Compare function signatures
nm -g litert/c/internal/litert_delegate_wrapper.cc.o
```

### 4. Build Configuration Inconsistencies
**Symptoms**:
- Build succeeds with Bazel but fails with CMake (or vice versa)
- Platform-specific build failures (Android/iOS/Web)
- Inconsistent symbol definitions across build systems

**Root Causes**:
- Divergent build configurations
- Missing preprocessor flags
- Incomplete platform-specific includes

**Diagnostic Steps**:
```bash
# Compare Bazel and CMake build flags
bazel build --verbose_failures litert/c:litert_delegate_wrapper
cmake --build . --verbose 2>&1 | grep -i delegate

# Check preprocessor flags
bazel query 'attr(copts, litert_delegate_wrapper)'
```

## Debugging Workflow

1. **Identify the Failure Point**
   - Note the exact file and line number
   - Determine if it's compilation or linking

2. **Isolate the Issue**
   - Build only the failing target in isolation
   - Strip down to minimal reproduction

3. **Review Dependencies**
   - Check BUILD/CMakeLists.txt for completeness
   - Verify all required headers are available

4. **Validate Include Paths**
   - Confirm include directory configuration
   - Test header includes individually

5. **Compare Reference Builds**
   - Check TFLite reference implementations
   - Compare with successful historical builds

## Performance Hotspots

The following files are identified as quality hotspots with potential build optimization opportunities:

- `litert/c/internal/litert_delegate_wrapper.cc` - Delegate registration overhead
- `litert/cc/internal/litert_dispatch_delegate.h` - Template instantiation overhead
- `litert/core/model/model.h` - Model loading path optimization
- `litert/ats/capture_common.h` - Test capture overhead
- `litert/ats/compile_fixture.h` - Compilation fixture setup

## Build System Integration

The following build configuration files control tflite module builds:

- `litert/BUILD` - Primary Bazel build rules
- `litert/CMakeLists.txt` - CMake configuration
- `.bazeliskrc` - Bazel version and configuration
- `litert/build_common/litert_build_defs.bzl` - Build definitions

## Verification Commands

```bash
# Full tflite module validation
bazel build //litert/c:all --verbose_failures

# CMake configuration and build
cmake --preset=default
cmake --build ./cmake_build --verbose

# Test suite execution
bazel test //litert/c:all

# Static analysis
clang-tidy litert/c/internal/*.cc -- -I. -Iexternal/*
```
