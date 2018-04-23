make WIN32=1

\path\to\pin.exe -t aflpin.dll -entry 0xADDR -exit 0xADDR -- prog.exe
__AFL_SHM_ID=1337 AFL_NO_FORKSRV=1 ./afl-fuzz.exe -i in/ -o out/ -N -- ./wrap.exe 8888 @@