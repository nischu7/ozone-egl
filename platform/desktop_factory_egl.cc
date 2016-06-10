/* Copyright (c) 2016, Voxtok Technologies
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include "desktop_factory_egl.h"
#include "desktop_screen_egl.h"
#include "desktop_window_tree_host_egl.h"

#include "ui/ozone/public/ozone_platform.h"
#include "ui/platform_window/platform_window.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace views {

DesktopFactoryEgl::DesktopFactoryEgl()
    : views::DesktopFactoryOzone()
    , desktop_screen_(nullptr)
{
    LOG(INFO) << "[Ozone::DesktopFactoryEgl] ctor() - this=" << this;
    DesktopFactoryOzone::SetInstance(this);
}

DesktopFactoryEgl::~DesktopFactoryEgl()
{
    LOG(INFO) << "[Ozone::DesktopFactoryEgl] ~dtor() - this=" << this;
    DesktopFactoryOzone::SetInstance(nullptr);
    delete desktop_screen_;
}

DesktopWindowTreeHost*
DesktopFactoryEgl::CreateWindowTreeHost(internal::NativeWidgetDelegate* native_widget_delegate, DesktopNativeWidgetAura* desktop_native_widget_aura)
{
    LOG(INFO) << "[Ozone::DesktopFactoryEgl] CreateWindowTreeHost() - this=" << this;
    return new DesktopWindowTreeHostOzone(native_widget_delegate, desktop_native_widget_aura);
}

gfx::Screen* DesktopFactoryEgl::CreateDesktopScreen()
{
    LOG(INFO) << "[Ozone::DesktopFactoryEgl] CreateDesktopScreen() - this=" << this;
    if (!desktop_screen_)
        desktop_screen_ = new DesktopScreenEgl();
    return desktop_screen_;
}

}  // namespace views
