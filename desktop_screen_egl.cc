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

#include "desktop_screen_egl.h"

#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"

namespace views {

DesktopScreenEgl::DesktopScreenEgl()
    : rect_(0, 0, 1280, 720)
{
    LOG(INFO) << "[Ozone::DesktopScreenEgl] ctor() - this=" << this;
    SetGeometry(rect_);
}

DesktopScreenEgl::~DesktopScreenEgl()
{
    LOG(INFO) << "[Ozone::DesktopScreenEgl] ~dtor() - this=" << this;
}

gfx::Point DesktopScreenEgl::GetCursorScreenPoint()
{
    NOTIMPLEMENTED();
    return gfx::Point();
}

gfx::NativeWindow DesktopScreenEgl::GetWindowUnderCursor()
{
    LOG(INFO) << "[Ozone::DesktopScreenEgl] GetWindowUnderCursor()";
    return nullptr;
}

gfx::NativeWindow DesktopScreenEgl::GetWindowAtScreenPoint(const gfx::Point& point)
{
    LOG(INFO) << "[Ozone::DesktopScreenEgl] GetWindowAtScreenPoint()";
    NOTIMPLEMENTED();
    return nullptr;
}

int DesktopScreenEgl::GetNumDisplays() const
{
    return displays_.size();
}

std::vector<gfx::Display> DesktopScreenEgl::GetAllDisplays() const
{
    return displays_;
}

gfx::Display DesktopScreenEgl::GetDisplayNearestWindow(gfx::NativeView window) const
{
//    LOG(INFO) << "[Ozone::DesktopScreenEgl] GetDisplayNearestWindow()";
    DCHECK(!rect_.IsEmpty());
    if (displays_.size() == 1)
        return displays_.front();

    NOTIMPLEMENTED();
    return GetPrimaryDisplay();
}

gfx::Display DesktopScreenEgl::GetDisplayNearestPoint(const gfx::Point& point) const
{
    LOG(INFO) << "[Ozone::DesktopScreenEgl] GetDisplayNearestPoint()";
    if (displays_.size() == 1)
        return displays_.front();

    for (std::vector<gfx::Display>::const_iterator it = displays_.begin(); it != displays_.end(); ++it) {
        if (it->bounds().Contains(point))
            return *it;
    }

    return GetPrimaryDisplay();
}

gfx::Display DesktopScreenEgl::GetDisplayMatching(const gfx::Rect& match_rect) const
{
    LOG(INFO) << "[Ozone::DesktopScreenEgl] GetDisplayMatching()";
    if (displays_.size() == 1)
        return displays_.front();

    DCHECK(!rect_.IsEmpty());
    int max_area = 0;
    const gfx::Display* matching = nullptr;
    for (std::vector<gfx::Display>::const_iterator it = displays_.begin(); it != displays_.end(); ++it) {
        gfx::Rect intersect = gfx::IntersectRects(it->bounds(), match_rect);
        int area = intersect.width() * intersect.height();
        if (area > max_area) {
            max_area = area;
            matching = &*it;
        }
    }

    // Fallback to the primary display if there is no matching display.
    return matching ? *matching : GetPrimaryDisplay();
}

gfx::Display DesktopScreenEgl::GetPrimaryDisplay() const
{
    LOG(INFO) << "[Ozone::DesktopScreenEgl] GetPrimaryDisplay()";
    DCHECK(!rect_.IsEmpty());
    return displays_.front();
}

void DesktopScreenEgl::AddObserver(gfx::DisplayObserver* observer)
{
    NOTIMPLEMENTED();
}

void DesktopScreenEgl::RemoveObserver(gfx::DisplayObserver* observer)
{
    NOTIMPLEMENTED();
}

void DesktopScreenEgl::SetGeometry(const gfx::Rect& geometry)
{
    LOG(INFO) << "[Ozone::DesktopScreenEgl] SetGeometry() - rect=" << geometry.x() << ", " << geometry.y() << ", " << geometry.width() << ", " << geometry.height();
    rect_ = geometry;
    int max_area = 0;
    const gfx::Display* matching = nullptr;
    for (std::vector<gfx::Display>::const_iterator it = displays_.begin(); it != displays_.end(); ++it) {
        gfx::Rect intersect = gfx::IntersectRects(it->bounds(), rect_);
        int area = intersect.width() * intersect.height();
        if (area > max_area) {
            max_area = area;
            matching = &*it;
        }
    }

    if (!matching)
        displays_.push_back(gfx::Display(displays_.size(), rect_));
}

}  // namespace views
