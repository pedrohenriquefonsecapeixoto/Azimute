build:
	g++ -Isrc -Iinclude src/*.cpp -o bin/app.exe

run: build
	.\bin\app.exe

build_unit:
	g++ -Isrc -Iinclude test/unit/*.cpp src/bib.cpp -o bin/unit_testes.exe

run_unit: build_unit
	.\bin\unit_testes.exe

build_func:
	g++ -Isrc -Iinclude test/*.cpp src/bib.cpp -o bin/testeRegressivo.exe

run_func: build_func
	.\bin\testeRegressivo.exe

build_dll:
	g++ -Isrc -Iinclude -shared src/bib.cpp -Wl,--export-all-symbols -Wl,--out-implib,bin/libmodulo.dll.a -o bin/modulo.dll

build_dll_app: build_dll
	g++ -Isrc -Iinclude src/main.cpp -Lbin -lmodulo -o bin/app.exe

clean:
	-del /Q .\bin\*.exe
	-del /Q .\bin\*.dll
	-del /Q .\bin\*.o
	-del /Q .\bin\*.a
	-del /Q .\bin\*.so
