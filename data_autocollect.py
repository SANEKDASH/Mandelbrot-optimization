import subprocess
import os

prog_name = "Mandelbrot"

default_flags = "-c -march=native -O3 -DSPEED_TEST "

compile_flags = ["",                                #по точкам.     буфер с цветами
                 "-DAVX_IMPLEMENT ",                #авикс инстр.   буфер с цветами
                 "-DSFML_BUFFER ",                  #по точкам.     буфер сфмл
                 "-DSFML_BUFFER -DAVX_IMPLEMENT "]  #avx инструкции.буфер сфмл

for test_counter in range(4):
    os.system("rm -rf *.o")

    with open("MakeFile" + str(test_counter), "w") as makefile:
        makefile.write("CC = g++\n\n")
        makefile.write("CFLAGS = " + default_flags + compile_flags[test_counter] + "\n\n")
        makefile.write("LDFLAGS=\n\n"
                        "SOURCES=main.cpp mandelbrot.cpp\n\n"
                        "LIBRARIES=-lsfml-graphics -lsfml-window -lsfml-system\n\n"
                        "DEFAULT_OBJECTS=$(SOURCES:.cpp=.o)\n\n"
                        "EXECUTABLE=" + prog_name + "\n\n"

                        "all: $(SOURCES) $(EXECUTABLE)\n"
                        "$(EXECUTABLE): $(DEFAULT_OBJECTS)\n"
                            "\t$(CC) $(LDFLAGS) $(DEFAULT_OBJECTS) -o $@ $(LIBRARIES)\n\n"

                        ".cpp.o:\n"
                            "\t$(CC) $(CFLAGS) $< -o $@\n\n"

                        "clean:\n"
                            "\trm -rf *.o\n\n")

    os.system("make -f MakeFile" + str(test_counter))

    programm_process = subprocess.Popen("./Mandelbrot")

    programm_process.wait()

    print("\e[1;32m Test passed! \e[0m")

    os.system("cp data.txt data" + str(test_counter) + ".txt")

