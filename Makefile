all: bin/wun lib/wunext.so

install:
	mkdir -p /usr/lib/wun
	cp lib/* /usr/lib/wun
	cp bin/wun /usr/bin

bin/wun: $(wildcard src/*.c src/*.h)
	gcc -o bin/wun src/wun.c src/wunrt.c $$(pkg-config --cflags --libs webkit2gtk-4.0)

lib/wunext.so: $(wildcard src/*.c src/*.h)
	gcc -shared -o lib/wunext.so -fPIC src/wunext.c $$(pkg-config --cflags --libs webkit2gtk-4.0)

clean:
	rm bin/* lib/wunext.so
