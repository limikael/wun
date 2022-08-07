#ifndef __WUNRT_H__
#define __WUNRT_H__

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

typedef struct {
	GtkWidget *main_window;
	WebKitWebView *web_view;
	char *uri;
} WUNRT;

WUNRT *wunrt_create();
void wunrt_run(WUNRT *wr);
void wunrt_set_uri(WUNRT *wr, char *uri);

#endif