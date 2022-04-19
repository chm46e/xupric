#ifndef _FRAME_HANDLE_H
#define _FRAME_HANDLE_H

#include "frame/frame.h"

void zoom_action(struct frame *f, int action);
void fullscreen_toggle(struct frame *f);
void view_navigate(struct frame *f, int back);
void window_close(void);
void debug_toggle(void);
void download_current_page(void);

#endif /* _FRAME_HANDLE_H */
