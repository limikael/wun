all: bin/wun bin/wun-extension.so

bin/wun: $(wildcard src/*.c src/*.h)
	gcc $$(pkg-config --cflags --libs webkit2gtk-4.0) src/wun.c src/wun_runner.c src/util.c -o bin/wun

bin/wun-extension.so: $(wildcard src/*.c src/*.h)
	gcc $$(pkg-config --cflags --libs webkit2gtk-4.0) -shared -o bin/wun-extension.so -fPIC src/wun_extension.c

clean:
	rm bin/*
