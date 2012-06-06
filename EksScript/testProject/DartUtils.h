// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#ifndef BIN_DARTUTILS_H_
#define BIN_DARTUTILS_H_

#include "dart_api.h"

class DartUtils {
 public:
  static bool IsDartSchemeURL(const char* url_name);
  static bool IsDartExtensionSchemeURL(const char* url_name);
  static bool IsDartCryptoLibURL(const char* url_name);
  static bool IsDartIOLibURL(const char* url_name);
  static bool IsDartJsonLibURL(const char* url_name);
  static bool IsDartUriLibURL(const char* url_name);
  static bool IsDartUtfLibURL(const char* url_name);
  static Dart_Handle CanonicalizeURL(Dart_Handle library,
                                     const char* url_str);
  static Dart_Handle ReadStringFromFile(const char* filename);
  static Dart_Handle LoadSource(Dart_Handle library,
                                Dart_Handle url,
                                Dart_LibraryTag tag,
                                const char* filename);

  static const char* kDartScheme;
  static const char* kDartExtensionScheme;
  static const char* kBuiltinLibURL;
  static const char* kCoreLibURL;
  static const char* kCoreImplLibURL;
  static const char* kCryptoLibURL;
  static const char* kIOLibURL;
  static const char* kJsonLibURL;
  static const char* kUriLibURL;
  static const char* kUtfLibURL;
  static const char* kIsolateLibURL;

  static const char* kIdFieldName;

 private:
  static const char* GetCanonicalPath(const char* reference_dir,
                                      const char* filename);
};

#endif  // BIN_DARTUTILS_H_