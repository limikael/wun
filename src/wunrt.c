#include "wunrt.h"
#include "util.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <fcntl.h>
#include <gio/gunixinputstream.h>

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
	gtk_main_quit();
}

static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window)
{
	gtk_widget_destroy(window);
	return TRUE;
}

static gboolean wunwrt_on_load_failed (WebKitWebView  *web_view,
               WebKitLoadEvent load_event,
               char           *failing_uri,
               GError         *error,
               gpointer        user_data) {
	printf("load error\n");
}

static void wunrt_on_uri_scheme_request(WebKitURISchemeRequest *request, gpointer data) {
	WUNRT *wunrt=data;
	
	const gchar *request_path=webkit_uri_scheme_request_get_path(request);
	char *full_path=g_strdup_printf("%s/%s.js",wunrt->library_path,request_path);

	//printf("trying: %s\n",full_path);

	int fildes=open(full_path,O_RDONLY);
	if (fildes==-1) {
		perror(g_strdup_printf("Importing module: %s",request_path));
		exit(1);
	}

	g_free(full_path);
	GInputStream *stream=g_unix_input_stream_new(fildes,true);

	webkit_uri_scheme_request_finish(request,stream,-1,"application/javascript");
	g_object_unref(stream);
}

WUNRT *wunrt_create() {
	WUNRT *wunrt=g_malloc(sizeof(WUNRT));

	wunrt->library_path=NULL;

	wunrt->main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(wunrt->main_window), 800, 600);

	wunrt->web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_container_add(GTK_CONTAINER(wunrt->main_window), GTK_WIDGET(wunrt->web_view));

	g_signal_connect(wunrt->main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
	g_signal_connect(wunrt->web_view, "close", G_CALLBACK(closeWebViewCb), wunrt->main_window);

	g_signal_connect(wunrt->web_view,"load-failed",G_CALLBACK(wunwrt_on_load_failed),wunrt);

	WebKitSettings *settings=webkit_web_view_get_settings(wunrt->web_view);
	webkit_settings_set_enable_write_console_messages_to_stdout(settings,TRUE);
	webkit_settings_set_allow_file_access_from_file_urls(settings,TRUE);
	webkit_settings_set_allow_universal_access_from_file_urls(settings,TRUE);

	WebKitWebContext *context=webkit_web_view_get_context(wunrt->web_view);
	webkit_web_context_register_uri_scheme(context,"wun",wunrt_on_uri_scheme_request,wunrt,NULL);

	return wunrt;
}

void wunrt_set_library_path(WUNRT *wunrt, char *path) {
	wunrt->library_path=path;

	WebKitWebContext *context=webkit_web_view_get_context(wunrt->web_view);
	webkit_web_context_set_web_extensions_directory(context,path);
}

void wunrt_set_uri(WUNRT *wunrt, char *uri) {
	wunrt->uri=uri;
}

void wunrt_load_url(WUNRT *wunrt) {
	char *abspath,*absurl,*html;
	abspath=realpath(wunrt->uri,NULL);
	asprintf(&absurl,"file://%s",abspath);

	asprintf(&html,
		"<html>"
		"<body></body>"
		"<script src=\"%s\" type=\"module\"></script>"
		"</html>",absurl);
	webkit_web_view_load_html(wunrt->web_view,html,"file:///");

	free(html);
	free(abspath);
	free(absurl);
}

void wunrt_run(WUNRT *wunrt) {
	if (!wunrt->library_path) {
		printf("Library path not set...\n");
		exit(1);
	}

	char *extfn=g_strdup_printf("%s/%s",wunrt->library_path,"wunext.so");
	if (!g_file_test(extfn,G_FILE_TEST_EXISTS)) {
		printf("wunext.so not found in %s\n",wunrt->library_path);
		exit(1);
	}

	g_free(extfn);
	wunrt_load_url(wunrt);

	gtk_widget_grab_focus(GTK_WIDGET(wunrt->web_view));
	gtk_widget_show_all(wunrt->main_window);
}