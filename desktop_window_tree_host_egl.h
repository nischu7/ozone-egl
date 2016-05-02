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

#ifndef __desktop_window_tree_host_egl_h__
#define __desktop_window_tree_host_egl_h__

#include "base/basictypes.h"
#include "ui/aura/window_tree_host_ozone.h"
#include "ui/events/event_source.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/views/widget/desktop_aura/desktop_window_tree_host.h"
#include "ui/wm/public/drag_drop_client.h"

#include <list>
#include <set>

namespace ui {
class PlatformWindow;
}

namespace views {

namespace corewm {
class Tooltip;
}

class VIEWS_EXPORT DesktopWindowTreeHostOzone : public DesktopWindowTreeHost
                                              , public aura::WindowTreeHostOzone
                                              , public ui::PlatformEventDispatcher {
public:
    DesktopWindowTreeHostOzone(internal::NativeWidgetDelegate*, DesktopNativeWidgetAura*);
    ~DesktopWindowTreeHostOzone() override;

    // Accepts a opaque handle widget and returns associated DesktopWindowTreeHostOzone.
    static DesktopWindowTreeHostOzone* GetHostForAcceleratedWidget(gfx::AcceleratedWidget);

protected:
    // Overridden from DesktopWindowTreeHost:
    void Init(aura::Window*, const views::Widget::InitParams&) override;
    void OnNativeWidgetCreated(const views::Widget::InitParams&) override;
    scoped_ptr<corewm::Tooltip> CreateTooltip() override;
    scoped_ptr<aura::client::DragDropClient> CreateDragDropClient(DesktopNativeCursorManager*) override;
    void Close() override;
    void CloseNow() override;
    aura::WindowTreeHost* AsWindowTreeHost() override;
    void ShowWindowWithState(ui::WindowShowState) override;
    void ShowMaximizedWithBounds(const gfx::Rect&) override;
    bool IsVisible() const override;
    void SetSize(const gfx::Size&) override;
    void StackAbove(aura::Window*) override;
    void StackAtTop() override;
    void CenterWindow(const gfx::Size&) override;
    void GetWindowPlacement(gfx::Rect*, ui::WindowShowState*) const override;
    gfx::Rect GetWindowBoundsInScreen() const override;
    gfx::Rect GetClientAreaBoundsInScreen() const override;
    gfx::Rect GetRestoredBounds() const override;
    gfx::Rect GetWorkAreaBoundsInScreen() const override;
    void SetShape(SkRegion*) override;
    void Activate() override;
    void Deactivate() override;
    bool IsActive() const override;
    void Maximize() override;
    void Minimize() override;
    void Restore() override;
    bool IsMaximized() const override;
    bool IsMinimized() const override;
    bool HasCapture() const override;
    void SetAlwaysOnTop(bool always_on_top) override;
    bool IsAlwaysOnTop() const override;
    void SetVisibleOnAllWorkspaces(bool) override;
    bool SetWindowTitle(const base::string16&) override;
    void ClearNativeFocus() override;
    views::Widget::MoveLoopResult RunMoveLoop(const gfx::Vector2d&, views::Widget::MoveLoopSource, views::Widget::MoveLoopEscapeBehavior) override;
    void EndMoveLoop() override;
    void SetVisibilityChangedAnimationsEnabled(bool) override;
    bool ShouldUseNativeFrame() const override;
    bool ShouldWindowContentsBeTransparent() const override;
    void FrameTypeChanged() override;
    void SetFullscreen(bool) override;
    bool IsFullscreen() const override;
    void SetOpacity(unsigned char) override;
    void SetWindowIcons(const gfx::ImageSkia&, const gfx::ImageSkia&) override;
    void InitModalType(ui::ModalType) override;
    void FlashFrame(bool) override;
    void OnRootViewLayout() override;
    void OnNativeWidgetFocus() override;
    void OnNativeWidgetBlur() override;
    bool IsAnimatingClosed() const override;
    bool IsTranslucentWindowOpacitySupported() const override;
    void SizeConstraintsChanged() override;

    // Overridden from aura::WindowTreeHostOzone:
    void ShowImpl() override;
    void HideImpl() override;
    void SetCapture() override;
    void ReleaseCapture() override;

    // Overridden from aura::WindowTreeHostOzone:
    void OnBoundsChanged(const gfx::Rect&) override;
    void OnDamageRect(const gfx::Rect&) override;
    void DispatchEvent(ui::Event*) override;
    void OnCloseRequest() override;
    void OnClosed() override;
    void OnWindowStateChanged(ui::PlatformWindowState) override;
    void OnLostCapture() override;
    void OnActivationChanged(bool active) override;

    // Overridden from ui::PlatformEventDispatcher:
    bool CanDispatchEvent(const ui::PlatformEvent&) override;
    uint32_t DispatchEvent(const ui::PlatformEvent&) override;

private:
    // Initializes our Ozone surface to draw on. This method performs all
    // initialization related to talking to the Ozone server.
    void InitOzoneWindow(const views::Widget::InitParams& params);

    void Relayout();
    gfx::Size AdjustSize(const gfx::Size& requested_size);

    gfx::Rect GetWorkAreaBoundsInPixels() const;
    gfx::Rect ToDIPRect(const gfx::Rect& rect_in_pixels) const;
    gfx::Rect ToPixelRect(const gfx::Rect& rect_in_dip) const;

    static std::list<DesktopWindowTreeHostOzone*>& open_windows();

    // A list of all (top-level) windows that have been created but not yet destroyed.
    static std::list<DesktopWindowTreeHostOzone*>* open_windows_;
    static DesktopWindowTreeHostOzone* active_window_;

    // Owned by DesktopNativeWidgetAura.
    views::internal::NativeWidgetDelegate* native_widget_delegate_;
    views::DesktopNativeWidgetAura* desktop_native_widget_aura_;
    aura::Window* content_window_;

    // We can optionally have a parent which can order us to close, or own
    // children who we're responsible for closing when we CloseNow().
    DesktopWindowTreeHostOzone* window_parent_;
    std::set<DesktopWindowTreeHostOzone*> window_children_;

    base::WeakPtrFactory<DesktopWindowTreeHostOzone> close_widget_factory_;

    bool active_;
    bool fullscreen_;
    bool maximized_;
    bool minimized_;
    bool visible_;

    gfx::Rect restored_bounds_;

    DISALLOW_COPY_AND_ASSIGN(DesktopWindowTreeHostOzone);
};

}  // namespace views

#endif  // __desktop_window_tree_host_egl_h__
