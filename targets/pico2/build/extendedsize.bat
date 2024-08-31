@REM call :testFunction
for /R CMakeFiles %%f in (*.obj) do @call :printStats "%%f"
@REM call :printStats "D:\Sync\Compile\quake\startover\MCU-Quake\targets\pico2\build\CMakeFiles\quake.dir\D_\Sync\Compile\quake\startover\MCU-Quake\quake\common.c.obj"
@goto :eof


@REM D:\Sync\Compile\quake\startover\MCU-Quake\targets\pico2\build\CMakeFiles\quake.dir\D_\Sync\Compile\quake\startover\MCU-Quake\quake\common.c.obj

@REM D:\Sync\Compile\quake\startover\MCU-Quake\targets\pico2\build\CMakeFiles\quake.dir\D_\Sync\Compile\quake\startover\MCU-Quake\quake\render\r_main.c.obj

:printStats
@REM @echo %~1
@REM arm-none-eabi-size -A %~1
@arm-none-eabi-size -B %~1
@REM FOR /f %%f in ('"arm-none-eabi-size -A %~1"') do @echo "%%f"
@goto :eof



:testFunction
@echo "I got called"
@goto :eof







