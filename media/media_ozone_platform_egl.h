#ifndef OZONE_MEDIA_PLATFORM_EGL_H_
#define OZONE_MEDIA_PLATFORM_EGL_H_

#include "ozone-egl/platform/ozone_export_egl.h"

namespace media {

class MediaOzonePlatform;

OZONE_EGL_EXPORT MediaOzonePlatform* CreateMediaOzonePlatformEgl();

}  // namespace media

#endif  // OZONE_MEDIA_PLATFORM_EGL_H_
