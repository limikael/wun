#include "wun_runner.h"
#include <gtk/gtk.h>

void usage() {
	printf("Usage: wun <script.js|page.html>\n");
	exit(1);
}

int main(int argc, char* argv[]) {
	gtk_init(&argc, &argv);

	if (argc!=2)
		usage();

	WUN_RUNNER *wr=wun_runner_create();
	wun_runner_set_uri(wr,argv[1]);
	wun_runner_run(wr);

	gtk_main();

	return 0;
}
