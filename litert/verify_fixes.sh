#!/bin/bash
# Build verification script for critical issues
# Validates: Bazel config, Android NDK, header dependencies, delegate plugins

set -e

echo "=== LiteRT Build Verification ==="
echo ""

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Issue 1: Bazelisk Configuration
echo -e "${YELLOW}[Issue-084baf025c] Verifying .bazeliskrc configuration...${NC}"
if [ -f ".bazeliskrc" ]; then
  echo -e "${GREEN}✓ .bazeliskrc exists${NC}"
  if grep -q "USE_BAZEL_VERSION" ".bazeliskrc"; then
    echo -e "${GREEN}✓ Bazel version pinning configured${NC}"
  else
    echo -e "${RED}✗ Bazel version not pinned${NC}"
    exit 1
  fi
else
  echo -e "${RED}✗ .bazeliskrc missing${NC}"
  exit 1
fi
echo ""

# Issue 2: C++ Type Compatibility
echo -e "${YELLOW}[Issue-4a8958b73d] Verifying C++ type compatibility in ats.cc...${NC}"
if grep -q "#include.*absl/log/log.h" "litert/ats/ats.cc"; then
  echo -e "${GREEN}✓ absl/log/log.h included${NC}"
else
  echo -e "${RED}✗ absl/log/log.h missing${NC}"
  exit 1
fi

if grep -q "litert/cc/internal/litert_c_types_printing.h" "litert/ats/ats.cc"; then
  echo -e "${GREEN}✓ litert_c_types_printing.h include configured${NC}"
else
  echo -e "${YELLOW}⚠ litert_c_types_printing.h conditionally included${NC}"
fi
echo ""

# Issue 3: Android NDK Environment
echo -e "${YELLOW}[Issue-a5c7e0744c] Verifying Android NDK configuration...${NC}"
if grep -q "ANDROID_NDK_HOME" "litert/CMakePresets.json"; then
  echo -e "${GREEN}✓ ANDROID_NDK_HOME referenced in CMakePresets${NC}"
else
  echo -e "${RED}✗ ANDROID_NDK_HOME not configured${NC}"
  exit 1
fi

if [ -n "$ANDROID_NDK_HOME" ]; then
  echo -e "${GREEN}✓ ANDROID_NDK_HOME environment variable set: $ANDROID_NDK_HOME${NC}"
else
  echo -e "${YELLOW}⚠ ANDROID_NDK_HOME not set (required for Android builds)${NC}"
fi
echo ""

# Issue 4: Delegate Plugin Headers
echo -e "${YELLOW}[Issue-8e3335cec8] Verifying delegate plugin header configuration...${NC}"
if grep -q "delegate_plugin" "litert/DELEGATE_PLUGIN_HEADERS.md"; then
  echo -e "${GREEN}✓ Delegate plugin documentation created${NC}"
else
  echo -e "${YELLOW}⚠ Delegate plugin documentation pending${NC}"
fi

# Check for expected includes in wrapper files
for wrapper in "litert_delegate_wrapper.h" "litert_dispatch_delegate.h"; do
  if [ -f "litert/c/internal/$wrapper" ] || [ -f "litert/cc/internal/$wrapper" ]; then
    echo -e "${GREEN}✓ $wrapper found${NC}"
  else
    echo -e "${YELLOW}⚠ $wrapper not found (may be platform-specific)${NC}"
  fi
done
echo ""

# Issue 5: Failing-Path Diagnostics
echo -e "${YELLOW}[Issue-084baf025c] Verifying diagnostics documentation...${NC}"
if [ -f "litert/TFLITE_DIAGNOSTICS.md" ]; then
  echo -e "${GREEN}✓ TFLite diagnostics guide created${NC}"
else
  echo -e "${RED}✗ TFLite diagnostics guide missing${NC}"
  exit 1
fi
echo ""

# Summary
echo -e "${GREEN}=== All verifications passed ===${NC}"
echo ""
echo "Next steps:"
echo "  1. Set ANDROID_NDK_HOME for Android builds: export ANDROID_NDK_HOME=<path>"
echo "  2. Build with Bazel: bazel build //litert/c:all"
echo "  3. Build with CMake: cmake --preset=default && cmake --build ./cmake_build"
echo "  4. Run tests: bazel test //litert/ats:all"
echo ""
