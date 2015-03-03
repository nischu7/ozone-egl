#ifndef sghsdjfghfdjkxg
#define sghsdjfghfdjkxg


#include "ui/platform_window/platform_window.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/ozone/platform/egl/egl_surface_factory.h"

namespace ui {
class SurfaceFactoryEgl;

class eglWindow : public PlatformWindow {
 public:
  eglWindow(PlatformWindowDelegate* delegate,SurfaceFactoryEgl* surface_factory,const gfx::Rect& bounds);
  ~eglWindow() override;

  // PlatformWindow:
  gfx::Rect GetBounds() override;
  void SetBounds(const gfx::Rect& bounds) override;
  void Show() override;
  void Hide() override;
  void Close() override;
  void SetCapture() override;
  void ReleaseCapture() override;
  void ToggleFullscreen() override;
  void Maximize() override;
  void Minimize() override;
  void Restore() override;
  void SetCursor(PlatformCursor cursor) override;
  void MoveCursorTo(const gfx::Point& location) override;
  void ConfineCursorToBounds(const gfx::Rect& bounds) override;

 private:
  PlatformWindowDelegate* delegate_;
  //LibeglplatformShimLoader* eglplatform_shim_;
  //EventFactoryEvdev* event_factory_;
  gfx::Rect bounds_;
  //ShimNativeWindowId window_id_;
  SurfaceFactoryEgl* surface_factory_;
  intptr_t window_id_;


  DISALLOW_COPY_AND_ASSIGN(eglWindow);
};

} // namespace ui

#endif
