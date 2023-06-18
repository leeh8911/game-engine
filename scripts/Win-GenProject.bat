@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
pushd ..\
call 3rdparty\premake\premake5.exe vs2022 premake5.lua
popd
PAUSE
