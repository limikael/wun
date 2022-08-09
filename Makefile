all: bin/wun lib/wunext.so

install:
	mkdir -p /usr/lib/wun
	cp lib/* /usr/lib/wun
	cp bin/wun /usr/bin

bin/wun: $(wildcard src/*.c src/*.h)
	gcc $$(pkg-config --cflags --libs webkit2gtk-4.0) src/wun.c src/wunrt.c src/util.c -o bin/wun

lib/wunext.so: $(wildcard src/*.c src/*.h)
	gcc $$(pkg-config --cflags --libs webkit2gtk-4.0) -shared -o lib/wunext.so -fPIC src/wunext.c

clean:
	rm bin/* lib/wunext.so
