#ifndef _FUN_FUN_H
#define _FUN_FUN_H

#include "frame/frame.h"

void zoom_action(struct frame *f, int action);
void zoom_label_update(struct frame *f);
void fullscreen_toggle(struct frame *f);
void fullscreen_action(struct frame *f, int action);
void view_navigate(struct frame *f, int back);
void window_close(void);
void debug_toggle(void);
void download_current_page(void);
void clear_focus_entry(void);
void clear_focus_secondary_entry(void);
void workspace_load(int c);
void new_window_spawn(char *uri);
void toggle_webinspector(void);

#endif /* _FUN_FUN_H */
