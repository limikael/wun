#ifndef __WUN_RUNNER_H__
#define __WUN_RUNNER_H__

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

typedef struct {
	GIOChannel *channel;
	int source;
	int mainfd;
	int extfd;
} WUN_WATCH;

typedef struct {
	GtkWidget *main_window;
	WebKitWebView *web_view;
	char *uri;
	GHashTable *watch_by_mainfd;
	GHashTable *watch_by_extfd;
} WUN_RUNNER;

WUN_RUNNER *wun_runner_create();
void wun_runner_run(WUN_RUNNER *wr);
void wun_runner_set_uri(WUN_RUNNER *wr, char *uri);

#endif