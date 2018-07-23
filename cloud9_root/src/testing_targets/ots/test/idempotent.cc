// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if !defined(_WIN32)
#ifdef __linux__
// Linux
#include <freetype/ftoutln.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#else
// Mac OS X
#include <ApplicationServices/ApplicationServices.h>  // g++ -framework Cocoa
#endif  // __linux__
#include <unistd.h>
#else
// Windows
#include <io.h>
#endif  // !defiend(_WIN32)

#include <fcntl.h>
#include <sys/stat.h>
/* C9: Needed to crash */
#include <assert.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "opentype-sanitiser.h"
#include "ots-memory-stream.h"

extern "C" {
void klee_begin_checked(void) __attribute__((weak));
void klee_end_checked(void) __attribute__((weak));
bool klee_memcmp(void *mem1, void *mem2, size_t size) __attribute__((weak));
int64_t klee_get_value_i64(int64_t expr) __attribute__((weak));
}

namespace {

int Usage(const char *argv0) {
  std::fprintf(stderr, "Usage: %s <ttf file>\n", argv0);
  return 1;
}

bool ReadFile(const char *file_name, uint8_t **data, size_t *file_size);
bool DumpResults(const uint8_t *result1, const size_t len1,
                 const uint8_t *result2, const size_t len2);

#if defined(_WIN32)
#define ADDITIONAL_OPEN_FLAGS O_BINARY
#else
#define ADDITIONAL_OPEN_FLAGS 0
#endif

bool ReadFile(const char *file_name, uint8_t **data, size_t *file_size) {
  const int fd = open(file_name, O_RDONLY | ADDITIONAL_OPEN_FLAGS);
  if (fd < 0) {
    return false;
  }

  struct stat st;
  fstat(fd, &st);

  *file_size = st.st_size;
  *data = new uint8_t[st.st_size];
  if (read(fd, *data, st.st_size) != st.st_size) {
    close(fd);
    return false;
  }
  close(fd);
  return true;
}

bool DumpResults(const uint8_t *result1, const size_t len1,
                 const uint8_t *result2, const size_t len2) {
  int fd1 = open("out1.ttf",
                 O_WRONLY | O_CREAT | O_TRUNC | ADDITIONAL_OPEN_FLAGS, 0600);
  int fd2 = open("out2.ttf",
                 O_WRONLY | O_CREAT | O_TRUNC | ADDITIONAL_OPEN_FLAGS, 0600);
  if (fd1 < 0 || fd2 < 0) {
    perror("opening output file");
    return false;
  }
  if ((write(fd1, result1, len1) < 0) ||
      (write(fd2, result2, len2) < 0)) {
    perror("writing output file");
    close(fd1);
    close(fd2);
    return false;
  }
  close(fd1);
  close(fd2);
  return true;
}

// Platform specific implementations.
bool VerifyTranscodedFont(uint8_t *result, const size_t len);

#if defined(__linux__)
// Linux
bool VerifyTranscodedFont(uint8_t *result, const size_t len) {
  FT_Library library;
  FT_Error error = ::FT_Init_FreeType(&library);
  if (error) {
    return false;
  }
  FT_Face dummy;
  error = ::FT_New_Memory_Face(library, result, len, 0, &dummy);
  if (error) {
    return false;
  }
  ::FT_Done_Face(dummy);
  return true;
}

#elif defined(__APPLE_CC__)
// Mac
bool VerifyTranscodedFont(uint8_t *result, const size_t len) {
  ATSFontContainerRef container_ref = 0;
  ATSFontActivateFromMemory(result, len, 3, kATSFontFormatUnspecified,
                            NULL, kATSOptionFlagsDefault, &container_ref);
  if (!container_ref) {
    return false;
  }

  ItemCount count;
  ATSFontFindFromContainer(
      container_ref, kATSOptionFlagsDefault, 0, NULL, &count);
  if (!count) {
    return false;
  }

  ATSFontRef ats_font_ref = 0;
  ATSFontFindFromContainer(
      container_ref, kATSOptionFlagsDefault, 1, &ats_font_ref, NULL);
  if (!ats_font_ref) {
    return false;
  }

  CGFontRef cg_font_ref = CGFontCreateWithPlatformFont(&ats_font_ref);
  if (!CGFontGetNumberOfGlyphs(cg_font_ref)) {
    return false;
  }
  return true;
}

#elif defined(_WIN32)
// Windows
bool VerifyTranscodedFont(uint8_t *result, const size_t len) {
  DWORD num_fonts = 0;
  HANDLE handle = AddFontMemResourceEx(result, len, 0, &num_fonts);
  if (!handle) {
    return false;
  }
  RemoveFontMemResourceEx(handle);
  return true;
}

#else
bool VerifyTranscodedFont(uint8_t *result, const size_t len) {
  std::fprintf(stderr, "Can't verify the transcoded font on this platform.\n");
  return false;
}

#endif

}  // namespace

int main(int argc, char **argv) {
  if (argc != 2) return Usage(argv[0]);

  size_t file_size = 0;
  uint8_t *data = 0;
  if (!ReadFile(argv[1], &data, &file_size)) {
    std::fprintf(stderr, "Failed to read file!\n");
    return 1;
  }

  // A transcoded font is usually smaller than an original font.
  // However, it can be slightly bigger than the original one due to
  // name table replacement and/or padding for glyf table.
  //
  // However, a WOFF font gets decompressed and so can be *much* larger than
  // the original.
  uint8_t *result = new uint8_t[file_size * 3/2];
  ots::MemoryStream output(result, file_size * 3/2);

  bool r = ots::Process(&output, data, file_size);
  if (!r) {
    std::fprintf(stderr, "Failed to sanitise file!\n");
    return 1;
  }
  const size_t result_len = output.Tell();
  delete[] data;

  uint8_t *result2 = new uint8_t[result_len];
  ots::MemoryStream output2(result2, result_len);
  r = ots::Process(&output2, result, result_len);
  if (!r) {
    std::fprintf(stderr, "Failed to sanitise previous output!\n");
    return 1;
  }
  const size_t result2_len = output2.Tell();

  klee_begin_checked();

  bool dump_results = false;
  if (result2_len != result_len) {
    std::fprintf(stderr, "Outputs differ in length\n");
    dump_results = true;
    assert(0 && "Files differ in size");
  } else {
#if 0
    // Do a shard comparison
    const int shard_size = 32;

    uint8_t **shard1 = new uint8_t*[(result_len-1)/shard_size + 1];
    uint8_t **shard2 = new uint8_t*[(result_len-1)/shard_size + 1];

    for (unsigned int i = 0; i < (result_len-1)/shard_size + 1; ++i) {
      if (i == (result_len-1)/shard_size) {
        shard1[i] = new uint8_t[result_len % shard_size];
        shard2[i] = new uint8_t[result_len % shard_size];
        std::memcpy(shard1[i], &result[i*shard_size], result_len % shard_size);
        std::memcpy(shard2[i], &result2[i*shard_size], result_len % shard_size);
      } else {
        shard1[i] = new uint8_t[shard_size];
        shard2[i] = new uint8_t[shard_size];
        std::memcpy(shard1[i], &result[i*shard_size], shard_size);
        std::memcpy(shard2[i], &result2[i*shard_size], shard_size);
      }
    }

    klee_begin_checked();

    for (unsigned int i = 0; i < (result_len-1)/shard_size + 1; ++i) {
      if (i == (result_len - 1)/shard_size) {
        if (std::memcmp(shard1[i], shard2[i], result_len % shard_size)) {
          std::fprintf(stderr, "Outputs differ in content\n");
          dump_results = true;
          assert(0 && "Files differ in content");
        }
      } else {
        if (std::memcmp(shard1[i], shard2[i], shard_size)) {
          std::fprintf(stderr, "Outputs differ in content\n");
          dump_results = true;
          assert(0 && "Files differ in content");
        }
      }
    }

    std::fprintf(stderr, "No output difference\n");

    klee_end_checked();
#else

    if (!klee_memcmp(result, result2, klee_get_value_i64(result_len))) {
      std::fprintf(stderr, "Outputs differ in content\n");
      dump_results = true;
      assert(0 && "Files differ in content");
    } else {
      std::fprintf(stderr, "No output difference\n");
    }
#endif
  }

  klee_end_checked();

  if (dump_results) {
    std::fprintf(stderr, "Dumping results to out1.tff and out2.tff\n");
    if (!DumpResults(result, result_len, result2, result2_len)) {
      std::fprintf(stderr, "Failed to dump output files.\n");
      return 1;
    }
  }

  /* C9: Check only idempotence */
#if 0
  // Verify that the transcoded font can be opened by the font renderer for
  // Linux (FreeType2), Mac OS X, or Windows.
  if (!VerifyTranscodedFont(result, result_len)) {
    std::fprintf(stderr, "Failed to verify the transcoded font\n");
    return 1;
  }
#endif

  return 0;
}
