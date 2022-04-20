#ifndef _FRAME_STYLE_STYLE_H
#define _FRAME_STYLE_STYLE_H

void style_init(void);
void style_file_set(WebKitWebView *view, char *file);
void dark_mode_set(WebKitWebView *view);
void dark_mode_remove(WebKitWebView *view);

#endif /* _FRAME_STYLE_STYLE_H */
