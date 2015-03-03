// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/egl/ozone_platform_egl.h"
#include "ui/ozone/platform/egl/egl_surface_factory.h"

#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/public/gpu_platform_support.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/ozone/public/ozone_platform.h"
#include "egl_wrapper.h"

#include "ui/ozone/common/native_display_delegate_ozone.h"

namespace ui {

namespace {

class OzonePlatformEgl : public OzonePlatform {
 public:
  OzonePlatformEgl()
  {
  }
  virtual ~OzonePlatformEgl() {
  }

  // OzonePlatform:
  virtual ui::SurfaceFactoryOzone* GetSurfaceFactoryOzone() override {
    return surface_factory_ozone_.get();
  }
  //virtual EventFactoryOzone* GetEventFactoryOzone() override {
  //  return event_factory_ozone_.get();
  //}
  virtual CursorFactoryOzone* GetCursorFactoryOzone() override {
    return cursor_factory_ozone_.get();
  }
  virtual GpuPlatformSupport* GetGpuPlatformSupport() override {
    return gpu_platform_support_.get();
  }
  virtual GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return gpu_platform_support_host_.get();
  }

/////
  scoped_ptr<SystemInputInjector> CreateSystemInputInjector() override {
    return nullptr;  // no input injection support.
  }
  InputController* GetInputController() override {
    return input_controller_.get(); // as in test
  }
  scoped_ptr<NativeDisplayDelegate> CreateNativeDisplayDelegate() override {
    return scoped_ptr<NativeDisplayDelegate>(new NativeDisplayDelegateOzone());
  }
  scoped_ptr<PlatformWindow> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      const gfx::Rect& bounds) override {
    return nullptr;  // no window support ;)
 }
/////

#if defined(OS_CHROMEOS)
  virtual scoped_ptr<NativeDisplayDelegate> CreateNativeDisplayDelegate()
      override {
    return scoped_ptr<NativeDisplayDelegate>(new NativeDisplayDelegateOzone());
  }
#endif
  virtual void InitializeUI() override {
   device_manager_ = CreateDeviceManager();
   event_factory_ozone_.reset(
        new EventFactoryEvdev(NULL, device_manager_.get()));
    surface_factory_ozone_.reset(new SurfaceFactoryEgl());
    cursor_factory_ozone_.reset(new CursorFactoryOzone());
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
    input_controller_ = CreateStubInputController();
  }

  virtual void InitializeGPU() override {
    surface_factory_ozone_.reset(new SurfaceFactoryEgl());
    cursor_factory_ozone_.reset(new CursorFactoryOzone());
    gpu_platform_support_.reset(CreateStubGpuPlatformSupport());
 }

 private:
  scoped_ptr<DeviceManager> device_manager_;
  scoped_ptr<EventFactoryEvdev> event_factory_ozone_;
  scoped_ptr<SurfaceFactoryEgl> surface_factory_ozone_;
  scoped_ptr<CursorFactoryOzone> cursor_factory_ozone_;

  scoped_ptr<GpuPlatformSupport> gpu_platform_support_;
  scoped_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;

  scoped_ptr<InputController> input_controller_;
  DISALLOW_COPY_AND_ASSIGN(OzonePlatformEgl);
};

}  // namespace

OzonePlatform* CreateOzonePlatformEgl() {
  OzonePlatformEgl* platform = new OzonePlatformEgl;
  return platform;
}
}  // namespace ui
