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

#ifndef __desktop_screen_egl_h__
#define __desktop_screen_egl_h__

#include "ui/gfx/screen.h"
#include <vector>

namespace aura {
class Window;
}

namespace views {

class DesktopScreenEgl : public gfx::Screen {
public:
    DesktopScreenEgl();
    ~DesktopScreenEgl() override;

private:
    // Overridden from gfx::Screen:
    gfx::Point GetCursorScreenPoint() override;
    gfx::NativeWindow GetWindowUnderCursor() override;
    gfx::NativeWindow GetWindowAtScreenPoint(const gfx::Point& point) override;
    int GetNumDisplays() const override;
    std::vector<gfx::Display> GetAllDisplays() const override;
    gfx::Display GetDisplayNearestWindow(gfx::NativeView window) const override;
    gfx::Display GetDisplayNearestPoint(const gfx::Point& point) const override;
    gfx::Display GetDisplayMatching(const gfx::Rect& match_rect) const override;
    gfx::Display GetPrimaryDisplay() const override;
    void AddObserver(gfx::DisplayObserver* observer) override;
    void RemoveObserver(gfx::DisplayObserver* observer) override;

    void SetGeometry(const gfx::Rect& geometry);

    std::vector<gfx::Display> displays_;
    gfx::Rect rect_;

    DISALLOW_COPY_AND_ASSIGN(DesktopScreenEgl);
};

}  // namespace views

#endif  // __desktop_screen_egl_h__
