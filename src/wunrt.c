#include "wunrt.h"
#include "util.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
	gtk_main_quit();
}

static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window)
{
	gtk_widget_destroy(window);
	return TRUE;
}

WUNRT *wunrt_create() {
	WUNRT *wunrt=g_malloc(sizeof(WUNRT));

	wunrt->main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(wunrt->main_window), 800, 600);

	wunrt->web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_container_add(GTK_CONTAINER(wunrt->main_window), GTK_WIDGET(wunrt->web_view));

	g_signal_connect(wunrt->main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
	g_signal_connect(wunrt->web_view, "close", G_CALLBACK(closeWebViewCb), wunrt->main_window);

	WebKitSettings *settings=webkit_web_view_get_settings(wunrt->web_view);
	webkit_settings_set_enable_write_console_messages_to_stdout(settings,1);
	webkit_settings_set_allow_file_access_from_file_urls(settings,TRUE);

	return wunrt;
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
	WebKitWebContext *context=webkit_web_view_get_context(wunrt->web_view);
	webkit_web_context_set_web_extensions_directory(context,"/home/micke/Repo.lab/wun/bin");

	wunrt_load_url(wunrt);

	gtk_widget_grab_focus(GTK_WIDGET(wunrt->web_view));
	gtk_widget_show_all(wunrt->main_window);
}