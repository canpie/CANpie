@echo off

:--------------------------------------------------------------------
: The setlocal enabledelayedexpansion enableextensions with the !var! 
: rather than %var% is necessary to prevent odd behavior, so that 
: variables behave properly when within a FOR or an IF. It's an 
: oddity with batch files. See set /? where that is mentioned further.
:
setlocal enabledelayedexpansion enableextensions

:--------------------------------------------------------------------
: Create symbolic links for some QCan source files
:
: The variable SYM_DIRS holds all directories which need to be linked.
: 
set SYM_FILES="qcan_filter.cpp qcan_filter_list.cpp qcan_frame.cpp qcan_socket.cpp qcan_timestamp.cpp"

     
:--------------------------------------------------------------------
: Create sym-links
:
echo Create sym-links for selected files

set i=-1
for %%f in ("%SYM_FILES%") do (
 set /a i=!i!+1
 set names[!i!]=%%f
)
set lastindex=!i!

for /L %%f in (0,1,!lastindex!) do ( 
  if not exist .\src\!names[%%f]! (
    mklink .\src\!names[%%f]! .\..\..\!names[%%f]!
  ) else (
    echo  - File .\src\!names[%%f]! already exists
  ) 
)

:--------------------------------------------------------------------
echo Create a moc file for the QCanSocket class header

moc ./../qcan_socket.hpp -o ./src/qcan_socket_moc.cpp
if %errorlevel%==9009 (
    echo.
    echo Make sure binary path, e.g. Qt5.9.8/5.9.8/mingw53_32/bin is within your PATH variable 
)

:--------------------------------------------------------------------
: check if the 'node_modules' directory is already present
:
if not exist .\node_modules (
    echo Install node_modules...
    md .\node_modules
    npm install node-addon-api
)

echo.
echo Start compilation by running: 'npm run build'
echo.