#include <webkit2/webkit-web-extension.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <fcntl.h>
#include <gio/gunixfdlist.h>
#include "wunext.h"

static gboolean wunext_on_watch(GIOChannel *source, GIOCondition cond, gpointer data) {
	WUNEXT *wunext=data;
	int fd=g_io_channel_unix_get_fd(source);
	WUNEXT_WATCH *watch=g_hash_table_lookup(wunext->watch_by_fd,GINT_TO_POINTER(fd));
	if (!watch) {
		printf("warning: wunext_on_watch: Unknown file descriptor.\n");
		return TRUE;
	}

	for (int i=0; i<WUNEXT_WATCH_NCOND; i++) {
		if ((1<<i)&cond) {
			if (watch->cond[i].source && watch->cond[i].cb) {
				JSCValue *cb=watch->cond[i].cb;

				g_source_remove(watch->cond[i].source);
				watch->cond[i].source=0;
				watch->cond[i].cb=NULL;

				JSCValue *ret=jsc_value_function_call(cb,G_TYPE_INT,fd,G_TYPE_INT,1<<i,G_TYPE_NONE);
				g_object_unref(cb);
			}

			else {
				printf("warning: wunext_on_watch: Triggered but no cb.\n");
			}
		}
	}

	return TRUE;
}

static void wunext_add_fildes(WUNEXT *wunext, int fd) {
	WUNEXT_WATCH *watch=g_malloc(sizeof(WUNEXT_WATCH));

	watch->fd=fd;
	watch->channel=g_io_channel_unix_new(watch->fd);

	for (int i=0; i<WUNEXT_WATCH_NCOND; i++) {
		watch->cond[i].cb=NULL;
		watch->cond[i].source=0;
	}

	g_hash_table_insert(wunext->watch_by_fd,GINT_TO_POINTER(fd),watch);
}

static void wunext_remove_fildes(WUNEXT *wunext, int fd) {
	WUNEXT_WATCH *watch=g_hash_table_lookup(wunext->watch_by_fd,GINT_TO_POINTER(fd));
	if (!watch) {
		printf("warning: wunext_remove_fildes: Unknown file descriptor.\n");
		return;
	}

	for (int i=0; i<WUNEXT_WATCH_NCOND; i++) {
		if (watch->cond[i].source)
			g_source_remove(watch->cond[i].source);

		if (watch->cond[i].cb)
			g_object_unref(watch->cond[i].cb);
	}

	g_hash_table_remove(wunext->watch_by_fd,GINT_TO_POINTER(fd));
}

static void wunext_throw(WUNEXT *wunext, char *func) {
	JSCException *ex=jsc_exception_new_printf(wunext->context,
		"%s: %s",func,strerror(errno)
	);

	jsc_context_throw_exception(wunext->context,ex);
}

static int sys_open(char *fn, int flags, WUNEXT *wunext) {
	int fildes=open(fn,O_RDONLY|O_NONBLOCK);
	if (fildes==-1) {
		wunext_throw(wunext,"open");
		return 0;
	}

	wunext_add_fildes(wunext,fildes);

	return fildes;
}

static void sys_close(int fildes, WUNEXT *wunext) {
	wunext_remove_fildes(wunext,fildes);
	if (close(fildes)==-1)
		wunext_throw(wunext,"close");
}

static void sys_watch(int fd, int cond, JSCValue *cb, WUNEXT *wunext) {
	WUNEXT_WATCH *watch=g_hash_table_lookup(wunext->watch_by_fd,GINT_TO_POINTER(fd));
	if (!watch) {
		jsc_context_throw_exception(wunext->context,jsc_exception_new_printf(
			wunext->context,
			"watch: Unknown file descriptor"
		));
		return;
	}

	if (!cond) {
		jsc_context_throw_exception(wunext->context,jsc_exception_new_printf(
			wunext->context,
			"watch: Unknown condition"
		));
		return;
	}

	for (int i=0; i<WUNEXT_WATCH_NCOND; i++) {
		if ((1<<i)&cond) {
			if (watch->cond[i].source) {
				g_source_remove(watch->cond[i].source);
				g_object_unref(watch->cond[i].cb);
			}

			if (cb) {
				watch->cond[i].source=g_io_add_watch(watch->channel,1<<i,wunext_on_watch,wunext);
				watch->cond[i].cb=cb;
				g_object_ref(cb);
			}
		}
	}
}

static JSCValue *sys_read(int fd, int size, WUNEXT *wunext) {
	char *data=g_malloc(size);
	int actualsize=read(fd,data,size);
	if (actualsize==-1) {
		wunext_throw(wunext,"read");
		g_free(data);
		return NULL;
	}

	if (actualsize==0) {
		g_free(data);
		return jsc_value_new_null(wunext->context);
	}

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

	jsc_value_object_set_property(sys,"G_IO_IN",jsc_value_new_number(context,G_IO_IN));
	jsc_value_object_set_property(sys,"G_IO_OUT",jsc_value_new_number(context,G_IO_OUT));
	jsc_value_object_set_property(sys,"G_IO_PRI",jsc_value_new_number(context,G_IO_PRI));
	jsc_value_object_set_property(sys,"G_IO_ERR",jsc_value_new_number(context,G_IO_ERR));
	jsc_value_object_set_property(sys,"G_IO_HUP",jsc_value_new_number(context,G_IO_HUP));
	jsc_value_object_set_property(sys,"G_IO_NVAL",jsc_value_new_number(context,G_IO_NVAL));

	jsc_value_object_set_property(sys,"open",
		jsc_value_new_function(context,"open",G_CALLBACK(sys_open),wunext,NULL,G_TYPE_INT,2,G_TYPE_STRING,G_TYPE_INT)
	);

	jsc_value_object_set_property(sys,"close",
		jsc_value_new_function(context,"close",G_CALLBACK(sys_close),wunext,NULL,G_TYPE_NONE,1,G_TYPE_INT)
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