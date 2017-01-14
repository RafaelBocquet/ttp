main: main.cpp build/header.h.pch instance.h sa.h tour.h ttour.h
	mkdir -p build
	time clang++ -include-pch build/header.h.pch -Wall -Wextra -std=c++1z -g -O3 -march=native main.cpp -o build/main
	rm -f main
	ln -s build/main main
build/header.h.pch: header.h
	mkdir -p build
	clang++ -x c++-header -Wall -Wextra -std=c++1z -g -O3 -march=native header.h -o build/header.h.pch
.PHONY: clean
clean:
	rm -rf build
