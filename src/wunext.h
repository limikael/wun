#ifndef __WUNEXT_H__
#define __WUNEXT_H__

#include <gtk/gtk.h>
#include <webkit2/webkit-web-extension.h>

typedef struct {
	int fd;
	GIOChannel *channel;
//	struct {
		JSCValue *cb;
		int source;
//	} conditions[6];
} WUNEXT_WATCH;

typedef struct {
	WebKitWebExtension *extension;
	JSCContext *context;
	GHashTable *watch_by_fd;
} WUNEXT;

#endif