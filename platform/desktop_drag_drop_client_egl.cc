/*
 * ---------------------------------------------------------------------------------
 * Copyright (C) 2015  STMicroelectronics - All Rights Reserved
 *
 * May be copied or modified under the terms of the LGPL v2.1.
 *
 * ST makes no warranty express or implied including but not limited to,
 * any warranty of
 *
 *   (i)  merchantability or fitness for a particular purpose and/or
 *   (ii) requirements, for a particular purpose in relation to the LICENSED
 *        MATERIALS, which is provided AS IS, WITH ALL FAULTS. ST does not
 *        represent or warrant that the LICENSED MATERIALS provided here
 *        under is free of infringement of any third party patents,
 *        copyrights, trade secrets or other intellectual property rights.
 *        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE
 *        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW
 *
 * ---------------------------------------------------------------------------------
 */

#include "ozone/ui/desktop_aura/desktop_drag_drop_client_egl.h"

#include "ui/base/dragdrop/drag_drop_types.h"
#include "ui/base/dragdrop/drop_target_event.h"

namespace views {

DesktopDragDropClientEgl::DesktopDragDropClientEgl(
    aura::Window* root_window) {
    NOTIMPLEMENTED();
}

DesktopDragDropClientEgl::~DesktopDragDropClientEgl() {
    NOTIMPLEMENTED();
}

int DesktopDragDropClientEgl::StartDragAndDrop(
    const ui::OSExchangeData& data,
    aura::Window* root_window,
    aura::Window* source_window,
    const gfx::Point& root_location,
    int operation,
    ui::DragDropTypes::DragEventSource source) {
    NOTIMPLEMENTED();
    return false;
}

void DesktopDragDropClientEgl::DragUpdate(aura::Window* target,
                                          const ui::LocatedEvent& event) {
    NOTIMPLEMENTED();
}

void DesktopDragDropClientEgl::Drop(aura::Window* target,
                                    const ui::LocatedEvent& event) {
    NOTIMPLEMENTED();
}

void DesktopDragDropClientEgl::DragCancel() {
    NOTIMPLEMENTED();
}

bool DesktopDragDropClientEgl::IsDragDropInProgress() {
    return false;
}

}  // namespace views
