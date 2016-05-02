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

#ifndef __desktop_drag_drop_client_egl_h__
#define __desktop_drag_drop_client_egl_h__

#include "base/compiler_specific.h"
#include "ui/views/views_export.h"
#include "ui/wm/public/drag_drop_client.h"

namespace views {

class VIEWS_EXPORT DesktopDragDropClientEgl : public aura::client::DragDropClient {
public:
    explicit DesktopDragDropClientEgl(aura::Window* root_window);
    virtual ~DesktopDragDropClientEgl();

    // Overridden from aura::client::DragDropClient:
    virtual int StartDragAndDrop(
        const ui::OSExchangeData& data,
        aura::Window* root_window,
        aura::Window* source_window,
        const gfx::Point& root_location,
        int operation,
        ui::DragDropTypes::DragEventSource source) OVERRIDE;
    virtual void DragUpdate(aura::Window* target,
                            const ui::LocatedEvent& event) OVERRIDE;
    virtual void Drop(aura::Window* target,
                    const ui::LocatedEvent& event) OVERRIDE;
    virtual void DragCancel() OVERRIDE;
    virtual bool IsDragDropInProgress() OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(DesktopDragDropClientEgl);
};

}  // namespace views

#endif  // __desktop_drag_drop_client_egl_h__
