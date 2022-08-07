#include <webkit2/webkit-web-extension.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <fcntl.h>
#include <gio/gunixfdlist.h>
#include "wunext.h"

static gboolean wunext_on_watch(GIOChannel *source, GIOCondition condition, gpointer data) {
	WUNEXT *wunext=data;
	WUNEXT_WATCH *watch=g_hash_table_lookup(wunext->watch_by_fd,GINT_TO_POINTER(g_io_channel_unix_get_fd(source)));
	if (!watch) {
		printf("warning: wunext_on_watch: Unknown file descriptor.\n");
		return TRUE;
	}

	JSCValue *cb=watch->cb;

	if (watch->source)
		g_source_remove(watch->source);

	watch->source=0;
	watch->cb=NULL;

	JSCValue *ret=jsc_value_function_call(cb,G_TYPE_NONE);
	g_object_unref(cb);

	return TRUE;
}

static void wunext_add_fildes(WUNEXT *wunext, int fd) {
	WUNEXT_WATCH *watch=g_malloc(sizeof(WUNEXT_WATCH));

	watch->fd=fd;
	watch->channel=g_io_channel_unix_new(watch->fd);
	watch->source=0;

	g_hash_table_insert(wunext->watch_by_fd,GINT_TO_POINTER(fd),watch);
}

static int sys_open(char *fn, int flags, WUNEXT *wunext) {
	int fildes=open(fn,O_RDONLY|O_NONBLOCK);

	wunext_add_fildes(wunext,fildes);

	return fildes;
}

static void sys_watch(int fd, int cond, JSCValue *cb, WUNEXT *wunext) {
	WUNEXT_WATCH *watch=g_hash_table_lookup(wunext->watch_by_fd,GINT_TO_POINTER(fd));
	if (!watch) {
		printf("warning: sys_watch: Unknown file descriptor.\n");
		return;
	}

	if (watch->source)
		g_source_remove(watch->source);

	watch->source=g_io_add_watch(watch->channel,G_IO_IN|G_IO_ERR,wunext_on_watch,wunext);
	watch->cb=cb;

	g_object_ref(cb);
}

static JSCValue *sys_read(int fd, int size, WUNEXT *wunext) {
	char *data=g_malloc(size);
	int actualsize=read(fd,data,size);
	data=g_realloc(data,actualsize);

	GBytes *bytes=g_bytes_new_take(data,actualsize);

	return jsc_value_new_string_from_bytes(wunext->context,bytes);
}

static void console_log(char *s) {
	printf("%s\n",s);
}

static void 
window_object_cleared_callback (WebKitScriptWorld *world, 
								WebKitWebPage     *web_page, 
								WebKitFrame       *frame, 
								gpointer           data)
{
	WUNEXT *wunext=data;

	JSCContext *context=webkit_frame_get_js_context_for_script_world(frame,world);
	wunext->context=context;

	JSCValue *sys=jsc_value_new_object(context,NULL,NULL);
	jsc_context_set_value(context,"sys",sys);

	jsc_value_object_set_property(sys,"open",
		jsc_value_new_function(context,"open",G_CALLBACK(sys_open),wunext,NULL,G_TYPE_INT,2,G_TYPE_STRING,G_TYPE_INT)
	);

	jsc_value_object_set_property(sys,"read",
		jsc_value_new_function(context,"read",G_CALLBACK(sys_read),wunext,NULL,JSC_TYPE_VALUE,2,G_TYPE_INT,G_TYPE_INT)
	);

	jsc_value_object_set_property(sys,"watch",
		jsc_value_new_function(context,"watch",G_CALLBACK(sys_watch),wunext,NULL,G_TYPE_NONE,3,G_TYPE_INT,G_TYPE_INT,JSC_TYPE_VALUE)
	);

	JSCValue *console=jsc_value_new_object(context,NULL,NULL);
	jsc_context_set_value(context,"console",console);

	jsc_value_object_set_property(console,"log",
		jsc_value_new_function(context,"log",G_CALLBACK(console_log),wunext,NULL,G_TYPE_NONE,1,G_TYPE_STRING)
	);
}

G_MODULE_EXPORT void
webkit_web_extension_initialize (WebKitWebExtension *extension)
{
	WUNEXT *wunext=g_malloc(sizeof(WUNEXT));

	wunext->extension=extension;
	wunext->watch_by_fd=g_hash_table_new(g_direct_hash,g_direct_equal);

	g_signal_connect(
		webkit_script_world_get_default(),
		"window-object-cleared",
		G_CALLBACK(window_object_cleared_callback),
		wunext
	);
}