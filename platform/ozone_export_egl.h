// Copyright 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OZONE_EXPORT_H_
#define OZONE_EXPORT_H_

#if defined(COMPONENT_BUILD) && defined(OZONE_EGL_IMPLEMENTATION)
  #define OZONE_EGL_EXPORT __attribute__ ((visibility ("default")))
#else
  #define OZONE_EGL_EXPORT
#endif

#endif  // OZONE_EXPORT_H_
