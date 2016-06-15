# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'use_bcm_host%': 0,
  },
  'targets': [
    {
      'target_name': 'ozone_platform_egl',
      'type': 'static_library',
      'defines': [
        'OZONE_EGL_IMPLEMENTATION',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/ui/events/events.gyp:events',
        '<(DEPTH)/ui/events/ozone/events_ozone.gyp:events_ozone_evdev',
        '<(DEPTH)/ui/gfx/gfx.gyp:gfx',
      ],
      'sources': [
        'media/media_ozone_platform_egl.cc',
        'media/media_ozone_platform_egl.h',
        'platform/ozone_export_egl.h',
        'platform/client_native_pixmap_factory_egl.cc',
        'platform/client_native_pixmap_factory_egl.h',
        'platform/egl_surface_factory.cc',
        'platform/egl_surface_factory.h',
        'platform/egl_window.cc',
        'platform/egl_window.h',
        'platform/egl_wrapper.cc',
        'platform/egl_wrapper.h',
        'platform/ozone_platform_egl.cc',
        'platform/ozone_platform_egl.h',
        'platform/ozone_platform_egl.cc',
        'platform/ozone_platform_egl.h',
      ],
      'link_settings': {
        'libraries': [
          '-lEGL',
          '-lGLESv2',
        ],
      },
      'conditions': [
        ['use_aura==1 and toolkit_views==1', {
          'defines': [ 'AURA_VIEWS' ],
          'sources': [
            'platform/desktop_factory_egl.cc',
            'platform/desktop_factory_egl.h',
            'platform/desktop_screen_egl.cc',
            'platform/desktop_screen_egl.h',
            'platform/desktop_window_tree_host_egl.cc',
            'platform/desktop_window_tree_host_egl.h',
          ],
        }],
        ['<(use_bcm_host) == 1', {
          'defines': [
            'EGL_API_BRCM',
          ],
          'cflags': [
            '<!@(pkg-config --cflags bcm_host)',
          ],
          'link_settings': {
            'ldflags': [
              '<!@(pkg-config --libs-only-L --libs-only-other bcm_host)',
            ],
            'libraries': [
              '<!@(pkg-config --libs-only-l bcm_host)',
            ],
          },
        }],
      ],
    },
  ],
}
