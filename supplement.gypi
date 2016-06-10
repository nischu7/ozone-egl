{
  'conditions': [
    ['<(ozone_platform_egl) == 1', {
      'variables':  {
        'external_ozone_platform_deps': [
          '<(DEPTH)/ozone-egl/egl.gyp:ozone_platform_egl',
        ],
        'external_ozone_platforms': [
          'egl'
        ],
        'ozone_platform%': 'egl',
      },
    }],
  ],
}
