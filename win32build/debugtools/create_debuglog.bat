@echo off
echo Starting KVIrc in debug mode ...
gdb -x kvirc.gdb --args kvirc.exe > debug_stdout.txt 2> debug_stderr.txt
echo Finished debbugging.
@echo on