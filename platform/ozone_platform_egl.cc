// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ozone-egl/platform/ozone_platform_egl.h"
#include "ozone-egl/platform/egl_surface_factory.h"

#include "ui/ozone/common/native_display_delegate_ozone.h"
#include "ui/ozone/common/stub_overlay_manager.h"
#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/public/gpu_platform_support.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/system_input_injector.h"
#include "ui/platform_window/platform_window.h"
#include "egl_window.h"
#include "egl_wrapper.h"

#if defined(AURA_VIEWS)
#include "desktop_factory_egl.h"
#endif

namespace ui {

namespace {

class OzonePlatformEgl : public OzonePlatform {
 public:
  OzonePlatformEgl()
  {
  }
  ~OzonePlatformEgl() override {
  }

  // OzonePlatform:
  ui::SurfaceFactoryOzone* GetSurfaceFactoryOzone() override {
    return surface_factory_ozone_.get();
  }

  OverlayManagerOzone* GetOverlayManager() override {
    return overlay_manager_.get();
  }

  InputController* GetInputController() override {
    return event_factory_ozone_->input_controller();
  }
  CursorFactoryOzone* GetCursorFactoryOzone() override {
    return cursor_factory_ozone_.get();
  }
  GpuPlatformSupport* GetGpuPlatformSupport() override {
    return gpu_platform_support_.get();
  }
  GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return gpu_platform_support_host_.get();
  }

/////
  scoped_ptr<SystemInputInjector> CreateSystemInputInjector() override {
    return event_factory_ozone_->CreateSystemInputInjector();
  }
  scoped_ptr<NativeDisplayDelegate> CreateNativeDisplayDelegate() override {
    return scoped_ptr<NativeDisplayDelegate>(new NativeDisplayDelegateOzone());
  }
  scoped_ptr<PlatformWindow> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      const gfx::Rect& bounds) override {
      scoped_ptr<eglWindow> platformWindow(
        new eglWindow(delegate, surface_factory_ozone_.get(),
           event_factory_ozone_.get(), bounds));
      platformWindow->Initialize();
      return std::move(platformWindow);
  }
  base::ScopedFD OpenClientNativePixmapDevice() const override {
    return base::ScopedFD();
  }
  void InitializeUI() override {
   device_manager_ = CreateDeviceManager();
   overlay_manager_.reset(new StubOverlayManager());
    KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(
        make_scoped_ptr(new StubKeyboardLayoutEngine()));
    event_factory_ozone_.reset(new EventFactoryEvdev(
        nullptr, device_manager_.get(),
        KeyboardLayoutEngineManager::GetKeyboardLayoutEngine()));
    if(!surface_factory_ozone_)
     surface_factory_ozone_.reset(new SurfaceFactoryEgl());
    if (!cursor_factory_ozone_)
        cursor_factory_ozone_.reset(new CursorFactoryOzone());
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
  }

  void InitializeGPU() override {
    if(!surface_factory_ozone_)
     surface_factory_ozone_.reset(new SurfaceFactoryEgl());
    if (!cursor_factory_ozone_)
        cursor_factory_ozone_.reset(new CursorFactoryOzone());
    gpu_platform_support_.reset(CreateStubGpuPlatformSupport());
 }

 private:
#if defined(AURA_VIEWS)
  views::DesktopFactoryEgl desktop_factory_ozone_;
#endif
  scoped_ptr<DeviceManager> device_manager_;
  scoped_ptr<EventFactoryEvdev> event_factory_ozone_;
  scoped_ptr<SurfaceFactoryEgl> surface_factory_ozone_;
  scoped_ptr<CursorFactoryOzone> cursor_factory_ozone_;

  scoped_ptr<GpuPlatformSupport> gpu_platform_support_;
  scoped_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;
  scoped_ptr<OverlayManagerOzone> overlay_manager_;
  DISALLOW_COPY_AND_ASSIGN(OzonePlatformEgl);
};

}  // namespace

OzonePlatform* CreateOzonePlatformEgl() {
  OzonePlatformEgl* platform = new OzonePlatformEgl;
  return platform;
}
}  // namespace ui
