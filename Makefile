all: bin/wun bin/wun-extension.so

bin/wun: $(wildcard src/*.c src/*.h)
	gcc $$(pkg-config --cflags --libs webkit2gtk-4.0) src/wun.c src/wunrt.c src/util.c -o bin/wun

bin/wun-extension.so: $(wildcard src/*.c src/*.h)
	gcc $$(pkg-config --cflags --libs webkit2gtk-4.0) -shared -o bin/wunext.so -fPIC src/wunext.c

clean:
	rm bin/*
