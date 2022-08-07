#include <webkit2/webkit-web-extension.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <fcntl.h>
#include <gio/gunixfdlist.h>

WebKitWebExtension *global_extension;
JSCContext *global_context;

static void register_fd_in_context(int fd) {
	int fds[1]={fd};
	GUnixFDList *fdlist=g_unix_fd_list_new_from_array(fds,1);
	GVariant *var=g_variant_new_int32(fd);
	WebKitUserMessage *msg=webkit_user_message_new_with_fd_list("register",var,fdlist);

	webkit_web_extension_send_message_to_context(
		global_extension,
		msg,
		NULL,NULL,NULL);
}

static int sys_open(char *fn) {
	int fildes=open(fn,O_RDONLY|O_NONBLOCK);

	register_fd_in_context(fildes);

	return fildes;
}

//	GVariant *var=g_variant_new_strv(strv,-1);

static void sys_watch(int fd) {
	GVariant *var=g_variant_new_int32(fd);
	WebKitUserMessage *msg=webkit_user_message_new("watch",var);

	//printf("registering watch...\n");
	webkit_web_extension_send_message_to_context(
		global_extension,
		msg,
		NULL,NULL,NULL);
}

static JSCValue *sys_read(int fd, int size) {
	char *data=g_malloc(size);
	int actualsize=read(fd,data,size);
	data=g_realloc(data,actualsize);

	GBytes *bytes=g_bytes_new_take(data,actualsize);

	return jsc_value_new_string_from_bytes(global_context,bytes);
}

void console_log(char *s) {
	printf("%s\n",s);
}

static void 
window_object_cleared_callback (WebKitScriptWorld *world, 
								WebKitWebPage     *web_page, 
								WebKitFrame       *frame, 
								gpointer           user_data)
{
	JSCContext *context=webkit_frame_get_js_context_for_script_world(frame,world);
	global_context=context;

	JSCValue *sys=jsc_value_new_object(context,NULL,NULL);
	jsc_context_set_value(context,"sys",sys);

	jsc_value_object_set_property(sys,"open",
		jsc_value_new_function(context,"open",G_CALLBACK(sys_open),NULL,NULL,G_TYPE_INT,1,G_TYPE_STRING)
	);

	jsc_value_object_set_property(sys,"read",
		jsc_value_new_function(context,"read",G_CALLBACK(sys_read),NULL,NULL,JSC_TYPE_VALUE,2,G_TYPE_INT,G_TYPE_INT)
	);

	jsc_value_object_set_property(sys,"watch",
		jsc_value_new_function(context,"watch",G_CALLBACK(sys_watch),NULL,NULL,G_TYPE_NONE,1,G_TYPE_INT)
	);

	JSCValue *console=jsc_value_new_object(context,NULL,NULL);
	jsc_context_set_value(context,"console",console);

	jsc_value_object_set_property(console,"log",
		jsc_value_new_function(context,"log",G_CALLBACK(console_log),NULL,NULL,G_TYPE_NONE,1,G_TYPE_STRING)
	);
}

G_MODULE_EXPORT void
webkit_web_extension_initialize (WebKitWebExtension *extension)
{
	global_extension=extension;

	g_signal_connect(
		webkit_script_world_get_default(),
		"window-object-cleared",
		G_CALLBACK(window_object_cleared_callback),
		NULL
	);
}