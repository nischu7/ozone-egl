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

#include "desktop_window_tree_host_egl.h"

#include "ui/aura/client/focus_client.h"
#include "ui/aura/window.h"
#include "ui/aura/window_property.h"
#include "ui/base/ime/input_method_auralinux.h"
#include "ui/events/event_utils.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/screen_type_delegate.h"
#include "ui/native_theme/native_theme.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/platform_window/platform_window.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/views/corewm/tooltip_aura.h"
#include "ui/views/linux_ui/linux_ui.h"
#include "ui/views/views_delegate.h"
#include "ui/views/widget/desktop_aura/desktop_dispatcher_client.h"
#include "ui/views/widget/desktop_aura/desktop_native_widget_aura.h"
#include "ui/views/widget/desktop_aura/desktop_screen_position_client.h"
#include "ui/wm/core/window_util.h"
#include "ui/wm/public/window_move_client.h"

DECLARE_WINDOW_PROPERTY_TYPE(views::DesktopWindowTreeHostOzone*);

namespace views {

std::list<DesktopWindowTreeHostOzone*>* DesktopWindowTreeHostOzone::open_windows_ = nullptr;
DesktopWindowTreeHostOzone* DesktopWindowTreeHostOzone::active_window_ = nullptr;

DEFINE_WINDOW_PROPERTY_KEY(aura::Window*, kViewsWindowForRootWindow, nullptr);
DEFINE_WINDOW_PROPERTY_KEY(DesktopWindowTreeHostOzone*, kHostForRootWindow, nullptr);

DesktopWindowTreeHostOzone::DesktopWindowTreeHostOzone(internal::NativeWidgetDelegate* native_widget_delegate,
                                                       DesktopNativeWidgetAura* desktop_native_widget_aura)
    : aura::WindowTreeHostOzone(gfx::Rect(0, 0, 1280, 720))
    , native_widget_delegate_(native_widget_delegate)
    , desktop_native_widget_aura_(desktop_native_widget_aura)
    , content_window_(nullptr)
    , window_parent_(nullptr)
    , window_children_()
    , close_widget_factory_(this)
    , active_(false)
    , fullscreen_(false)
    , maximized_(false)
    , minimized_(false)
    , visible_(false)
    , restored_bounds_()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ctor() - this=" << this;
}

DesktopWindowTreeHostOzone::~DesktopWindowTreeHostOzone()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ~dtor() - this=" << this;
    window()->ClearProperty(kHostForRootWindow);
    aura::client::SetWindowMoveClient(window(), nullptr);
    desktop_native_widget_aura_->OnDesktopWindowTreeHostDestroyed(this);
    DestroyDispatcher();
}

DesktopWindowTreeHostOzone*
DesktopWindowTreeHostOzone::GetHostForAcceleratedWidget(gfx::AcceleratedWidget widget)
{
  aura::WindowTreeHost* host = aura::WindowTreeHost::GetForAcceleratedWidget(widget);
  return host ? host->window()->GetProperty(kHostForRootWindow) : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// DesktopWindowTreeHostOzone, DesktopWindowTreeHost implementation:

void DesktopWindowTreeHostOzone::Init(aura::Window* content_window, const Widget::InitParams& params)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] Init() - this=" << this << ", content_window=" << content_window;
    content_window_ = content_window;
    // In some situations, views tries to make a zero sized window, and that
    // makes us crash. Make sure we have valid sizes.
    Widget::InitParams sanitized_params = params;
    if (sanitized_params.bounds.width() == 0)
        sanitized_params.bounds.set_width(100);
    if (sanitized_params.bounds.height() == 0)
        sanitized_params.bounds.set_height(100);

    InitOzoneWindow(sanitized_params);
}

void DesktopWindowTreeHostOzone::OnNativeWidgetCreated(const Widget::InitParams& params)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnNativeWidgetCreated() - this=" << this;
    window()->SetProperty(kViewsWindowForRootWindow, content_window_);
    window()->SetProperty(kHostForRootWindow, this);

    // If we're given a parent, we need to mark ourselves as transient to another
    // window. Otherwise activation gets screwy.
    gfx::NativeView parent = params.parent;
    if (!params.child && params.parent)
        wm::AddTransientChild(parent, content_window_);

#if defined(ENABLE_HBBTV) && ENABLE_HBBTV && !defined(EGL_API_BRCM)
     compositor()->SetHostHasTransparentBackground(true);
     window()->SetTransparent(true);
     content_window_->SetTransparent(true);
#endif

    native_widget_delegate_->OnNativeWidgetCreated(true);
    open_windows().push_back(this);

    if (!params.parent)
        Activate();
}

scoped_ptr<corewm::Tooltip> DesktopWindowTreeHostOzone::CreateTooltip()
{
    return make_scoped_ptr(new corewm::TooltipAura);
}

scoped_ptr<aura::client::DragDropClient>
DesktopWindowTreeHostOzone::CreateDragDropClient(DesktopNativeCursorManager* cursor_manager)
{
    NOTIMPLEMENTED();
    return nullptr;
}

void DesktopWindowTreeHostOzone::Close()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] Close() - this=" << this;
    if (!close_widget_factory_.HasWeakPtrs()) {
        // And we delay the close so that if we are called from an ATL callback,
        // we don't destroy the window before the callback returned (as the caller
        // may delete ourselves on destroy and the ATL callback would still
        // dereference us when the callback returns).
        base::MessageLoop::current()->PostTask(
            FROM_HERE,
            base::Bind(&DesktopWindowTreeHostOzone::CloseNow,
            close_widget_factory_.GetWeakPtr()));
    }
}

void DesktopWindowTreeHostOzone::CloseNow()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] CloseNow() - this=" << this;
    if (!GetAcceleratedWidget())
        return;

    ReleaseCapture();
    native_widget_delegate_->OnNativeWidgetDestroying();

    // If we have children, close them. Use a copy for iteration because they'll
    // remove themselves.
    std::set<DesktopWindowTreeHostOzone*> window_children_copy = window_children_;
    for (std::set<DesktopWindowTreeHostOzone*>::iterator it = window_children_copy.begin(); it != window_children_copy.end(); ++it)
        (*it)->CloseNow();
    DCHECK(window_children_.empty());

    // If we have a parent, remove ourselves from its children list.
    if (window_parent_) {
        window_parent_->window_children_.erase(this);
        window_parent_ = nullptr;
    }

    open_windows().remove(this);
    // Actually free our native resources.
    platform_window()->Close();
    DesktopWindowTreeHostOzone* lastHost = nullptr;
    if (open_windows().empty()) {
        delete open_windows_;
        open_windows_ = nullptr;
    } else
        lastHost = open_windows().back();

    desktop_native_widget_aura_->OnHostClosed();

    if (lastHost) {
        LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] CloseNow() ---> Redrawing/Re-activating host: " << lastHost;
        lastHost->compositor()->ScheduleFullRedraw();
        lastHost->Activate();
    }
}

aura::WindowTreeHost* DesktopWindowTreeHostOzone::AsWindowTreeHost()
{
    return this;
}

void DesktopWindowTreeHostOzone::ShowWindowWithState(ui::WindowShowState show_state)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ShowWindowWithState() - this=" << this << ", show_state=" << show_state;
    visible_ = true;
    if (compositor())
        compositor()->SetVisible(true);

    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ShowWindowWithState() ------> has focus: " << (content_window_->HasFocus() ? "yes" : "no") << ", has capture: " << (content_window_->HasCapture() ? "yes" : "no");

    switch (show_state) {
    case ui::SHOW_STATE_MAXIMIZED:
        Maximize();
        break;
    case ui::SHOW_STATE_MINIMIZED:
        Minimize();
        break;
    case ui::SHOW_STATE_FULLSCREEN:
        SetFullscreen(true);
        break;
    default:
        break;
    }

    if (show_state != ui::SHOW_STATE_INACTIVE && show_state != ui::SHOW_STATE_MINIMIZED)
        Activate();

    native_widget_delegate_->AsWidget()->SetInitialFocus(show_state);
}

void DesktopWindowTreeHostOzone::ShowMaximizedWithBounds(const gfx::Rect& restored_bounds)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ShowMaximizedWithBounds()";
    Maximize();
    restored_bounds_ = restored_bounds;
    Show();
}

bool DesktopWindowTreeHostOzone::IsVisible() const
{
    return visible_;
}

void DesktopWindowTreeHostOzone::SetSize(const gfx::Size& requested_size)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] SetSize() - requested_size=" << requested_size.width() << ", " << requested_size.height();
    gfx::Rect new_bounds = platform_window()->GetBounds();
    new_bounds.set_size(AdjustSize(requested_size));
    platform_window()->SetBounds(new_bounds);
}

void DesktopWindowTreeHostOzone::StackAbove(aura::Window* window)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] StackAbove() - window=" << window;
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::StackAtTop()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] StackAtTop()";
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::CenterWindow(const gfx::Size& size)
{
    gfx::Size size_in_pixels = ToPixelRect(gfx::Rect(size)).size();
    gfx::Rect parent_bounds_in_pixels = GetWorkAreaBoundsInScreen();

    // If |window_|'s transient parent bounds are big enough to contain |size|,
    // use them instead.
    if (wm::GetTransientParent(content_window_)) {
    gfx::Rect transient_parent_rect =
        wm::GetTransientParent(content_window_)->GetBoundsInScreen();
    if (transient_parent_rect.height() >= size.height() &&
        transient_parent_rect.width() >= size.width()) {
      parent_bounds_in_pixels = ToPixelRect(transient_parent_rect);
    }
    }

    gfx::Rect window_bounds_in_pixels(
      parent_bounds_in_pixels.x() +
          (parent_bounds_in_pixels.width() - size_in_pixels.width()) / 2,
      parent_bounds_in_pixels.y() +
          (parent_bounds_in_pixels.height() - size_in_pixels.height()) / 2,
      size_in_pixels.width(), size_in_pixels.height());
    // Don't size the window bigger than the parent, otherwise the user may not be
    // able to close or move it.
    window_bounds_in_pixels.AdjustToFit(parent_bounds_in_pixels);
    platform_window()->SetBounds(window_bounds_in_pixels);
}

void DesktopWindowTreeHostOzone::GetWindowPlacement(gfx::Rect* bounds, ui::WindowShowState* show_state) const
{
    *bounds = GetRestoredBounds();
    if (IsMinimized())
        *show_state = ui::SHOW_STATE_MINIMIZED;
    else if (IsFullscreen())
        *show_state = ui::SHOW_STATE_FULLSCREEN;
    else if (IsMaximized())
        *show_state = ui::SHOW_STATE_MAXIMIZED;
    else if (!IsActive())
        *show_state = ui::SHOW_STATE_INACTIVE;
    else
        *show_state = ui::SHOW_STATE_NORMAL;
}

gfx::Rect DesktopWindowTreeHostOzone::GetWindowBoundsInScreen() const
{
    return aura::WindowTreeHostOzone::GetBounds();
}

gfx::Rect DesktopWindowTreeHostOzone::GetClientAreaBoundsInScreen() const
{
    return aura::WindowTreeHostOzone::GetBounds();
}

gfx::Rect DesktopWindowTreeHostOzone::GetRestoredBounds() const
{
    return (restored_bounds_.IsEmpty() ? GetWindowBoundsInScreen() : restored_bounds_);
}

gfx::Rect DesktopWindowTreeHostOzone::GetWorkAreaBoundsInScreen() const
{
    return ToDIPRect(GetWorkAreaBoundsInPixels());
}

void DesktopWindowTreeHostOzone::SetShape(SkRegion* native_region)
{
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::Activate()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] Activate() - this=" << this;
    if (IsActive())
        return;

    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] Activate()";
    active_ = true;
    if (active_window_)
        active_window_->Deactivate();
    SetCapture();
    if (IsVisible())
        OnActivationChanged(true);
    content_window_->Show();
    compositor()->SetVisible(true);
}

void DesktopWindowTreeHostOzone::Deactivate()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] Deactivate() - this=" << this;
    if (!IsActive())
        return;

    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] Deactivate()";
    active_ = false;
    ReleaseCapture();
    OnActivationChanged(false);
    compositor()->SetVisible(false);
    content_window_->Hide();
}

bool DesktopWindowTreeHostOzone::IsActive() const
{
    return active_;
}

void DesktopWindowTreeHostOzone::Maximize()
{
    if (maximized_)
        return;

    maximized_ = true;
    minimized_ = false;
    restored_bounds_ = platform_window()->GetBounds();
    platform_window()->Maximize();
    Relayout();
}

void DesktopWindowTreeHostOzone::Minimize()
{
    if (minimized_)
        return;

    maximized_ = false;
    minimized_ = true;
    platform_window()->Minimize();
    Deactivate();
}

void DesktopWindowTreeHostOzone::Restore()
{
    maximized_ = false;
    minimized_ = false;
    if (!IsActive())
        Activate();
}

bool DesktopWindowTreeHostOzone::IsMaximized() const
{
    return !IsFullscreen() && maximized_;
}

bool DesktopWindowTreeHostOzone::IsMinimized() const
{
    return minimized_;
}

bool DesktopWindowTreeHostOzone::HasCapture() const
{
    return active_window_ == this;
}

void DesktopWindowTreeHostOzone::SetAlwaysOnTop(bool always_on_top)
{
    NOTIMPLEMENTED();
}

bool DesktopWindowTreeHostOzone::IsAlwaysOnTop() const
{
    NOTIMPLEMENTED();
    return false;
}

void DesktopWindowTreeHostOzone::SetVisibleOnAllWorkspaces(bool always_visible)
{
    NOTIMPLEMENTED();
}

bool DesktopWindowTreeHostOzone::SetWindowTitle(const base::string16& title)
{
    NOTIMPLEMENTED();
    return false;
}

void DesktopWindowTreeHostOzone::ClearNativeFocus()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ClearNativeFocus() - this=" << this;
    // This method is weird and misnamed. Instead of clearing the native focus,
    // it sets the focus to our |content_window_|, which will trigger a cascade
    // of focus changes into views.
    if (content_window_ && aura::client::GetFocusClient(content_window_) &&
        content_window_->Contains(aura::client::GetFocusClient(content_window_)->GetFocusedWindow()))
        aura::client::GetFocusClient(content_window_)->FocusWindow(content_window_);
}

Widget::MoveLoopResult DesktopWindowTreeHostOzone::RunMoveLoop(const gfx::Vector2d& drag_offset,
                                                               Widget::MoveLoopSource source,
                                                               Widget::MoveLoopEscapeBehavior escape_behavior)
{
    NOTIMPLEMENTED();
    return Widget::MOVE_LOOP_SUCCESSFUL;
}

void DesktopWindowTreeHostOzone::EndMoveLoop()
{
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::SetVisibilityChangedAnimationsEnabled(bool value)
{
    NOTIMPLEMENTED();
}

bool DesktopWindowTreeHostOzone::ShouldUseNativeFrame() const
{
    NOTIMPLEMENTED();
    return false;
}

bool DesktopWindowTreeHostOzone::ShouldWindowContentsBeTransparent() const
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ShouldWindowContentsBeTransparent() - this=" << this;
#if defined ENABLE_HBBTV && ENABLE_HBBTV
    return true;
#else
    return false;
#endif
}

void DesktopWindowTreeHostOzone::FrameTypeChanged()
{
    Widget::FrameType new_type = native_widget_delegate_->AsWidget()->frame_type();
    // The default is determined by Widget::InitParams::remove_standard_frame
    // and does not change.
    if (new_type == Widget::FRAME_TYPE_DEFAULT)
        return;

    // Replace the frame and layout the contents. Even though we don't have a
    // swapable glass frame like on Windows, we still replace the frame because
    // the button assets don't update otherwise.
    native_widget_delegate_->AsWidget()->non_client_view()->UpdateFrame();
}

void DesktopWindowTreeHostOzone::SetFullscreen(bool fullscreen)
{
    fullscreen_ = fullscreen;
}

bool DesktopWindowTreeHostOzone::IsFullscreen() const
{
    return fullscreen_;
}

void DesktopWindowTreeHostOzone::SetOpacity(unsigned char opacity)
{
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::SetWindowIcons(const gfx::ImageSkia& window_icon, const gfx::ImageSkia& app_icon)
{
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::InitModalType(ui::ModalType modal_type)
{
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::FlashFrame(bool flash_frame)
{
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::OnRootViewLayout()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnRootViewLayout()";
    NOTIMPLEMENTED();
}

void DesktopWindowTreeHostOzone::OnNativeWidgetFocus()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnNativeWidgetFocus() ---------- this=" << this << ", content_window_=" << content_window_;
    native_widget_delegate_->AsWidget()->GetInputMethod()->OnFocus();
}

void DesktopWindowTreeHostOzone::OnNativeWidgetBlur()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnNativeWidgetBlur() ---------- this=" << this << ", content_window_=" << content_window_;
    if (GetAcceleratedWidget())
        native_widget_delegate_->AsWidget()->GetInputMethod()->OnBlur();
}

bool DesktopWindowTreeHostOzone::IsAnimatingClosed() const
{
    NOTIMPLEMENTED();
    return false;
}

bool DesktopWindowTreeHostOzone::IsTranslucentWindowOpacitySupported() const
{
    NOTIMPLEMENTED();
    return false;
}

void DesktopWindowTreeHostOzone::SizeConstraintsChanged()
{
    NOTIMPLEMENTED();
}

////////////////////////////////////////////////////////////////////////////////
// DesktopWindowTreeHostOzone, aura::WindowTreeHostOzone implementation:

void DesktopWindowTreeHostOzone::ShowImpl()
{
    if (IsVisible())
        return;

    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ShowImpl()";
    platform_window()->Show();
    ShowWindowWithState(ui::SHOW_STATE_NORMAL);
    native_widget_delegate_->OnNativeWidgetVisibilityChanged(true);
}

void DesktopWindowTreeHostOzone::HideImpl()
{
    if (!IsVisible())
        return;

    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] HideImpl()";
    visible_ = false;
    platform_window()->Hide();
    native_widget_delegate_->OnNativeWidgetVisibilityChanged(false);
}

void DesktopWindowTreeHostOzone::SetCapture()
{
    if (HasCapture())
        return;

    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] SetCapture() - this=" << this << ", active_window_=" << active_window_;
    DCHECK(this != active_window_);
    if (active_window_)
        active_window_->OnHostLostWindowCapture();
    active_window_ = this;
    platform_window()->SetCapture();
}

void DesktopWindowTreeHostOzone::ReleaseCapture()
{
    if (!HasCapture())
        return;

    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] ReleaseCapture() - this=" << this << ", active_window_=" << active_window_;
    DCHECK(this == active_window_);
    active_window_ = nullptr;
    OnHostLostWindowCapture();
    platform_window()->ReleaseCapture();
}

////////////////////////////////////////////////////////////////////////////////
// ui::PlatformWindowDelegate implementation:
void DesktopWindowTreeHostOzone::OnBoundsChanged(const gfx::Rect& new_bounds)
{
    native_widget_delegate_->AsWidget()->OnNativeWidgetMove();
    OnHostResized(new_bounds.size());
}

void DesktopWindowTreeHostOzone::OnDamageRect(const gfx::Rect& damaged_rect)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnDamageRect() - damaged_rect=" << damaged_rect.x() << ", " << damaged_rect.y() << ", " << damaged_rect.width() << ", " << damaged_rect.height();
    compositor()->ScheduleRedrawRect(damaged_rect);
}

void DesktopWindowTreeHostOzone::OnActivationChanged(bool active)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnActivationChanged() - this=" << this << ", active=" << (active ? "true" : "false");
    if (active == IsActive()) {
        LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnActivationChanged() - NOOP: state=" << (IsActive() ? "active" : "inactive");
        return;
    }

    if (active) {
        OnHostActivated();
        DCHECK(open_windows().size());
        open_windows().remove(this);
        open_windows().insert(open_windows().begin(), this);
    } else
        ReleaseCapture();

    desktop_native_widget_aura_->HandleActivationChanged(active);
    native_widget_delegate_->AsWidget()->GetRootView()->SchedulePaint();
}

void DesktopWindowTreeHostOzone::OnLostCapture()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnLostCapture() - this=" << this;
    ReleaseCapture();
}

void DesktopWindowTreeHostOzone::OnCloseRequest()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnCloseRequest() - this=" << this;
    Close();
}

void DesktopWindowTreeHostOzone::OnClosed()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnClosed() - this=" << this;
    CloseNow();
}

void DesktopWindowTreeHostOzone::OnWindowStateChanged(ui::PlatformWindowState new_state)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] OnWindowStateChanged() - this=" << this << ", new_state=" << new_state;

#if 0
    switch (new_state) {
    case ui::PLATFORM_WINDOW_STATE_MAXIMIZED: {
        if (IsMinimized()) {
            content_window_->Show();
            compositor()->SetVisible(true);
            maximized_ = true;
        }
        platform_window()->SetBounds(restored_bounds_);
        restored_bounds_ = gfx::Rect();
        Relayout();
        break;
    }
    default:
        break;
    }
#endif
}

void DesktopWindowTreeHostOzone::DispatchEvent(ui::Event* event)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] DispatchEvent(ui::Event*) - this=" << this << ", active_window_=" << active_window_;
    if (active_window_)
        active_window_->SendEventToProcessor(event);
    else
        SendEventToProcessor(event);
}

////////////////////////////////////////////////////////////////////////////////
// WindowTreeHostDelegateWayland, ui::PlatformEventDispatcher implementation:
bool DesktopWindowTreeHostOzone::CanDispatchEvent(const ui::PlatformEvent&)
{
    bool canDispatch = (active_window_ && active_window_ == this);
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] CanDispatchEvent() - this=" << this << ", canDispatch=" << (canDispatch ? "true" : "false");
    return canDispatch;
}

uint32_t DesktopWindowTreeHostOzone::DispatchEvent(const ui::PlatformEvent& ne)
{
    ui::EventType type = ui::EventTypeFromNative(ne);
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] DispatchEvent(const ui::PlatformEvent&) - this=" << this << ", active_window_=" << active_window_;

    switch (type) {
    case ui::ET_KEY_PRESSED:
    case ui::ET_KEY_RELEASED: {
        if (active_window_)
            active_window_->SendEventToProcessor(static_cast<ui::KeyEvent*>(ne));
        else
            SendEventToProcessor(static_cast<ui::KeyEvent*>(ne));
        break;
    }
    default:
        NOTIMPLEMENTED() << "DesktopWindowTreeHostOzone::DispatchEvent(): Unhandled event type";
    }
    return ui::POST_DISPATCH_STOP_PROPAGATION;
}

////////////////////////////////////////////////////////////////////////////////
// DesktopWindowTreeHostOzone, private:

// static
std::list<DesktopWindowTreeHostOzone*>& DesktopWindowTreeHostOzone::open_windows()
{
    if (!open_windows_)
        open_windows_ = new std::list<DesktopWindowTreeHostOzone*>();
    return *open_windows_;
}

// static
VIEWS_EXPORT ui::NativeTheme* DesktopWindowTreeHost::GetNativeTheme(aura::Window* window)
{
    const views::LinuxUI* linux_ui = views::LinuxUI::instance();
    if (linux_ui) {
        ui::NativeTheme* native_theme = linux_ui->GetNativeTheme(window);
        if (native_theme)
            return native_theme;
    }
    return ui::NativeTheme::instance();
}

void DesktopWindowTreeHostOzone::InitOzoneWindow(const Widget::InitParams& params)
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] InitOzoneWindow() - this=" << this;
    const gfx::Rect& bounds = gfx::Rect(params.bounds.origin(), AdjustSize(params.bounds.size()));
    DCHECK(platform_window());
    platform_window()->SetBounds(bounds);

    // Maintain parent child relation as done in X11 version.
    // If we have a parent, record the parent/child relationship. We use this
    // data during destruction to make sure that when we try to close a parent
    // window, we also destroy all child windows.
    DCHECK(GetAcceleratedWidget());
    gfx::AcceleratedWidget parent_window = 0;
    if (params.parent && params.parent->GetHost()) {
        parent_window = params.parent->GetHost()->GetAcceleratedWidget();
        window_parent_ = GetHostForAcceleratedWidget(parent_window);
        DCHECK(window_parent_);
        window_parent_->window_children_.insert(this);
    }

#if 0
    // If we have a delegate which is providing a default window icon, use that icon.
    gfx::ImageSkia* window_icon = ViewsDelegate::GetInstance() ? ViewsDelegate::GetInstance()->GetDefaultWindowIcon() : nullptr;
    if (window_icon)
        SetWindowIcons(gfx::ImageSkia(), *window_icon);
#endif
}

void DesktopWindowTreeHostOzone::Relayout()
{
    LOG(INFO) << "[Ozone::DesktopWindowTreeHostOzone] Relayout()";
    Widget* widget = native_widget_delegate_->AsWidget();
    NonClientView* non_client_view = widget->non_client_view();
    if (non_client_view) {
        non_client_view->client_view()->InvalidateLayout();
        non_client_view->InvalidateLayout();
    }
    widget->GetRootView()->Layout();
}

gfx::Size DesktopWindowTreeHostOzone::AdjustSize(const gfx::Size& requested_size)
{
    return requested_size;
}

gfx::Rect DesktopWindowTreeHostOzone::GetWorkAreaBoundsInPixels() const
{
    return aura::WindowTreeHostOzone::GetBounds();
}

gfx::Rect DesktopWindowTreeHostOzone::ToDIPRect(const gfx::Rect& rect_in_pixels) const
{
    gfx::RectF rect_in_dip = gfx::RectF(rect_in_pixels);
    GetRootTransform().TransformRectReverse(&rect_in_dip);
    return gfx::ToEnclosingRect(rect_in_dip);
}

gfx::Rect DesktopWindowTreeHostOzone::ToPixelRect(const gfx::Rect& rect_in_dip) const
{
    gfx::RectF rect_in_pixels = gfx::RectF(rect_in_dip);
    GetRootTransform().TransformRect(&rect_in_pixels);
    return gfx::ToEnclosingRect(rect_in_pixels);
}

}  // namespace views
