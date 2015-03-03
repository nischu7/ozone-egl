#include "ui/ozone/platform/test/test_window.h"

namespace ui {

/*
 eglWindow::eglWindow(PlatformWindowDelegate* delegate,
                              LibeglplatformShimLoader* eglplatform_shim,
                              EventFactoryEvdev* event_factory,
                              const gfx::Rect& bounds)
     : delegate_(delegate),
       eglplatform_shim_(eglplatform_shim),
       event_factory_(event_factory),
       bounds_(bounds),
       window_id_(SHIM_NO_WINDOW_ID) {
   window_id_ = eglplatform_shim_->ShimCreateWindow();
   delegate_->OnAcceleratedWidgetAvailable(window_id_);
   ui::PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
 }
*/

 eglWindow::eglWindow(SurfaceFactoryEgl* surface_factory)
     :surface_factory_(surface_factory){
   window_id_ = surface_factory_->CreateSingleWindow();
 }
 
 eglWindow::~eglWindow() {
   //ui::PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
   //if (window_id_ != SHIM_NO_WINDOW_ID)
   //  eglplatform_shim_->ShimDestroyWindow(window_id_);
 }
 
 //gfx::Rect eglWindow::GetBounds() {
 //  return bounds_;
 //}
 //
 //void eglWindow::SetBounds(const gfx::Rect& bounds) {
 //  bounds_ = bounds;
 //  delegate_->OnBoundsChanged(bounds);
 //}
 
 void eglWindow::Show() {
 }
 
 void eglWindow::Hide() {
 }
 
 void eglWindow::Close() {
 }
 
 void eglWindow::SetCapture() {
 }
 
 void eglWindow::ReleaseCapture() {
 }
 
 void eglWindow::ToggleFullscreen() {
 }
 
 void eglWindow::Maximize() {
 }
 
 void eglWindow::Minimize() {
 }
 
 void eglWindow::Restore() {
 }
 
 void eglWindow::SetCursor(PlatformCursor cursor) {
 }
 
 void eglWindow::MoveCursorTo(const gfx::Point& location) {
   event_factory_->WarpCursorTo(window_id_, location);
 }
 
 void eglWindow::ConfineCursorToBounds(const gfx::Rect& bounds) {
 }
 
 bool eglWindow::CanDispatchEvent(const ui::PlatformEvent& ne) {
   return true;
 }
 
 uint32_t eglWindow::DispatchEvent(const ui::PlatformEvent& native_event) {
   DCHECK(native_event);
   Event* event = static_cast<Event*>(native_event);
   if (event->IsTouchEvent())
     ScaleTouchEvent(static_cast<TouchEvent*>(event), bounds_.size());
 
   DispatchEventFromNativeUiEvent(
       native_event, base::Bind(&PlatformWindowDelegate::DispatchEvent,
                                base::Unretained(delegate_)));
 
   return ui::POST_DISPATCH_STOP_PROPAGATION;
 }
 
}
