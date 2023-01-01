@echo off

setlocal

set old_dir=%CD%

if "%1"=="/?" goto :usage
if "%1"=="-h" goto :usage
if "%1"=="clean" goto :make_clean

:: Set default values
set "clean="
set "major_ver=%1"
if "%major_ver%"=="" set "major_ver=999" 
set "minor_ver=%2" 
if "%minor_ver%"=="" set "minor_ver=unknown" 
set "dist=%3" 

:: Check arguments
set /a "arith_major_ver=%major_ver%"
if "%major_ver%"=="%arith_major_ver%" goto :check_arg_major_ver_max
echo ERROR: The argument ^<major_version^> must be a number
goto :usage
:check_arg_major_ver_max
if "%major_ver%"=="%major_ver:~0,3%" goto :check_arg_minor_ver
echo ERROR: The argument ^<major_version^> can't be greater than 999
goto :usage

:check_arg_minor_ver
if "%minor_ver%"=="%minor_ver:~0,15%" goto :check_arg_dist
echo ERROR: The argument ^<minor_version^> can't be longer than 15 characters
goto :usage

:check_arg_dist
if "%dist%"=="" goto :valid_args 
if /I "%dist%"=="dist" goto :valid_args 
echo ERROR: The 3rd argument must be "dist" or empty
goto :usage


:valid_args
set "d2x_build=d2x-v%major_ver%-%minor_ver%"

echo -----------------------------
echo Building %d2x_build%
echo -----------------------------
if not exist "%~dp0\build\%d2x_build%" md "%~dp0\build\%d2x_build%"
goto :make_modules

:make_clean
:: Set clean var then remove build and dist folders 
set "clean=clean"
if exist "%~dp0\build" rd /s /q "%~dp0\build" 
if exist "%~dp0\dist"  rd /s /q "%~dp0\dist"  

:make_modules
:: Build or clean library, plugins and modules
call :make cios-lib     ""                %clean% 
call :make dip-plugin   %d2x_build%\DIPP  %clean% 
call :make ehci-module  %d2x_build%\EHCI  %clean% 
call :make es-plugin    %d2x_build%\ES    %clean% 
call :make fat-module   %d2x_build%\FAT   %clean% 
call :make ffs-plugin   %d2x_build%\FFSP  %clean% 
call :make mload-module %d2x_build%\MLOAD %clean% 
call :make usb-module   %d2x_build%\USBS  %clean% 

if "%clean%"=="clean" goto :done

:: Replace variables in some files 
call :replace_vars ciosmaps.xml      "build"
call :replace_vars ciosmaps-vWii.xml "build"
call :replace_vars d2x-beta.bat      "build\%d2x_build%"
call :replace_vars ReadMe.txt        "build"

:: Copy Changelog.txt to the build directory 
copy "%~dp0\data\Changelog.txt" "%~dp0\build" > NUL


if "%dist%"=="" goto :done
echo.
echo Creating distribution package...
:: Copy files to ModMii
if exist "%MODMII%\Support\d2x-beta" rd /S /Q "%MODMII%\Support\d2x-beta"
md "%MODMII%\Support\d2x-beta"
copy "%~dp0\build\%d2x_build%\*.*"   "%MODMII%\Support\d2x-beta" > NUL
copy "%~dp0\build\ciosmaps.xml"      "%MODMII%\Support\d2x-beta" > NUL
copy "%~dp0\build\ciosmaps-vWii.xml" "%MODMII%\Support\d2x-beta" > NUL
copy "%~dp0\build\ReadMe.txt"        "%MODMII%\Support\d2x-beta" > NUL
copy "%~dp0\build\Changelog.txt"     "%MODMII%\Support\d2x-beta" > NUL
:: Launch ModMii to build the wad files, calculate their md5 and generate the zip file
cd /D "%MODMII%"
start "d2x Distribution Process" /wait "%MODMII%\Support\d2x-beta-md5-updater.bat"
:: Copy the generated files back to build and dist folders
copy /Y "%MODMII%\Support\d2x-beta\d2x-beta.bat" "%~dp0\build\%d2x_build%\d2x-beta.bat" > NUL
if not exist "%~dp0\dist" md "%~dp0\dist"
copy /Y "%MODMII%\%d2x_build%.zip" "%~dp0\dist" > NUL
if "%dist%"=="DIST" goto :done
:: Clean ModMii's files and folders  
del "%MODMII%\%d2x_build%.zip" > NUL
rd /S /Q "%MODMII%\Support\d2x-beta"
rd /S /Q "%MODMII%\Support\More-cIOSs\%d2x_build%"

:done
echo.
echo Done!
echo.
pause

:exit
cd /D "%old_dir%"
endlocal
goto :EOF


:usage
echo.
echo Usage 1: %~n0 [^<major_version^> [^<minor_version^> [dist ^| DIST]]] 
echo   It builds d2x with the specified major and minor version. 
echo   Default values are "999" and "unknown" respectively.
echo   If option dist or DIST is specified then a zip file is generated, i.e. the
echo   distribution package. Be aware that:
echo     - it may take several minutes  
echo     - ModMii is required --^> http://gbatemp.net/topic/207126-modmii-for-windows
echo     - the MODMII environment variable must be set to ModMii install directory
echo     - internet connection is required 
echo   Contrary to DIST, the dist option removes the generated files from ModMii,
echo   allowing you to keep it clean. 
echo   Examples:
echo     %~n0  
echo     %~n0 1 
echo     %~n0 1 final 
echo     %~n0 1 final dist 
echo     %~n0 1 final DIST 
echo.
echo Usage 2: %~n0 clean
echo   It permanently deletes any previous build and dist. 
echo.
goto :exit


:make
echo.
echo Making %1 %3...
cd /D "%~dp0\source\%1"
make %3 STRIP=../../stripios
if errorlevel 0 goto :make_ok
echo Build failed!!!
goto :exit
:make_ok
if "%3"=="clean" goto :make_end
if not "%2"=="" copy "%1.elf" "..\..\build\%2.app" > NUL
:make_end
cd /D "%~dp0"  
goto :EOF

:replace_vars
echo.
echo Replacing variables in %1...
cd /D "%~dp0"
awk -v major_ver=%major_ver% -v minor_ver=%minor_ver% -f replace.awk "%~dp0\data\%1" > "%~dp0\%2\%1" 
goto :EOF


