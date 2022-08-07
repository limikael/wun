#include "wun_runner.h"
#include "util.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <gio/gunixfdlist.h>

static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
	gtk_main_quit();
}

static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window)
{
	gtk_widget_destroy(window);
	return TRUE;
}

WUN_RUNNER *wun_runner_create() {
	WUN_RUNNER *wr=malloc(sizeof(WUN_RUNNER));

	wr->main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(wr->main_window), 800, 600);

	wr->web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_container_add(GTK_CONTAINER(wr->main_window), GTK_WIDGET(wr->web_view));

	g_signal_connect(wr->main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
	g_signal_connect(wr->web_view, "close", G_CALLBACK(closeWebViewCb), wr->main_window);

	WebKitSettings *settings=webkit_web_view_get_settings(wr->web_view);
	webkit_settings_set_enable_write_console_messages_to_stdout(settings,1);

	wr->watch_by_mainfd=g_hash_table_new(g_direct_hash,g_direct_equal);
	wr->watch_by_extfd=g_hash_table_new(g_direct_hash,g_direct_equal);

	return wr;
}

void wun_runner_set_uri(WUN_RUNNER *wr, char *uri) {
	wr->uri=uri;
}

void wun_runner_load_url(WUN_RUNNER *wr) {
	char *abspath,*absurl,*html;
	abspath=realpath(wr->uri,NULL);
	asprintf(&absurl,"file://%s",abspath);

	char *ext=strrchr(wr->uri,'.');
	if (!strcmp(ext,".js")) {
		asprintf(&html,
			"<html>"
			"<body></body>"
			"<script src=\"%s\"></script>"
			"</html>",absurl);
		webkit_web_view_load_html(wr->web_view,html,"file:///");
		free(html);
	}

	else if (!strcmp(ext,".html")) {
		webkit_web_view_load_uri(wr->web_view,absurl);
	}

	else {
		printf("Unknown file extension: %s\n",ext);
		exit(1);
	}

	free(abspath);
	free(absurl);
}

static gboolean wun_runner_on_watch(GIOChannel *source, GIOCondition condition, gpointer data) {
	WUN_RUNNER *wr=(WUN_RUNNER *)data;
	int fildes=g_io_channel_unix_get_fd(source);
	WUN_WATCH *watch=g_hash_table_lookup(wr->watch_by_mainfd,GINT_TO_POINTER(fildes));

	if (watch->source) {
		g_source_remove(watch->source);
		watch->source=0;
	}

	//printf("watch fired, fildes=%d, source=%d\n",fildes,watch->source);

	webkit_web_view_run_javascript(
		wr->web_view,
		"window.dispatchEvent(new Event('watch'));",
		NULL,NULL,NULL
	);

	//printf("the js was run..\n");

	return TRUE;
}

static gboolean wun_runner_on_message(WebKitWebContext *context, WebKitUserMessage *message, gpointer data) {
	WUN_RUNNER *wr=(WUN_RUNNER *)data;
	const char *name=webkit_user_message_get_name(message);
	GVariant *var=webkit_user_message_get_parameters(message);

	if (!strcmp(name,"register")) {
		GUnixFDList *list=webkit_user_message_get_fd_list(message);

		WUN_WATCH *watch=g_malloc(sizeof(WUN_WATCH));
		watch->mainfd=g_unix_fd_list_get(list,0,NULL);
		watch->extfd=g_variant_get_int32(var);
		watch->channel=g_io_channel_unix_new(watch->mainfd);
		watch->source=0;

		g_hash_table_insert(wr->watch_by_mainfd,GINT_TO_POINTER(watch->mainfd),watch);
		g_hash_table_insert(wr->watch_by_extfd,GINT_TO_POINTER(watch->extfd),watch);

		//printf("channel registered, mainfd=%d, extfd=%d\n",watch->mainfd,watch->extfd);
	}

	if (!strcmp(name,"watch")) {
		int extfd=g_variant_get_int32(var);
		WUN_WATCH *watch=g_hash_table_lookup(wr->watch_by_extfd,GINT_TO_POINTER(extfd));

		if (watch->source)
			g_source_remove(watch->source);

		watch->source=g_io_add_watch(watch->channel,G_IO_IN,wun_runner_on_watch,wr);

		//printf("watching, mainfd=%d, extfd=%d\n",watch->mainfd,watch->extfd);
	}
}

void wun_runner_run(WUN_RUNNER *wr) {
	//printf("%p\n",webkit_web_view_get_javascript_global_context(wr->web_view));

	WebKitWebContext *context=webkit_web_view_get_context(wr->web_view);
	webkit_web_context_set_web_extensions_directory(context,"/home/micke/Repo.lab/wun/bin");

	g_signal_connect(context,"user-message-received",G_CALLBACK(wun_runner_on_message),wr);

	wun_runner_load_url(wr);

	gtk_widget_grab_focus(GTK_WIDGET(wr->web_view));
	gtk_widget_show_all(wr->main_window);
}