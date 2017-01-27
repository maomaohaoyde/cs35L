all: randmain randlibhw.so randlibsw.so
	$(CC) -o randmain -ldl -Wl,-rpath=$PWD

randmain:randmain.c randcpuid.c
	$(CC) randmain.c -I . -L . -ldl -Wl,-rpath=. randcpuid.c -o randmain

randlibhw.so:randlibhw.c
	$(CC) -shared -mrdrnd -fPIC randlibhw.c -o randlibhw.so

randlibsw.so:randlibsw.c
	$(CC) -shared -fPIC randlibsw.c -o randlibsw.so

