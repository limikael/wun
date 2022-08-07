#include "wunrt.h"
#include <gtk/gtk.h>

void usage() {
	printf("Usage: wun <script.js>\n");
	exit(1);
}

int main(int argc, char* argv[]) {
	gtk_init(&argc, &argv);

	if (argc!=2)
		usage();

	WUNRT *wunrt=wunrt_create();
	wunrt_set_uri(wunrt,argv[1]);
	wunrt_run(wunrt);

	gtk_main();

	return 0;
}
