@ECHO off
echo Permanently setting TBB_ROOT to the PATH user environment variable:

for /F "tokens=2* delims= " %%f IN ('reg query HKCU\Environment /v PATH ^| findstr /i path') do set OLD_SYSTEM_PATH=%%g
setx Path %~dp0lib\_deps\math-src\lib\tbb;%OLD_SYSTEM_PATH%

