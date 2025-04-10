@echo off
chcp 65001
setlocal EnableDelayedExpansion

:: Set project paths
set PROJECT_ROOT=%~dp0..
set BUILD_DIR=%PROJECT_ROOT%\..\build

:: Read version from config.ini
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "Major="') do set VERSION_MAJOR=%%a
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "Minor="') do set VERSION_MINOR=%%a
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "Patch="') do set VERSION_PATCH=%%a
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "Build="') do set VERSION_BUILD=%%a

:: Set version string
set VERSION=%VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_PATCH%.%VERSION_BUILD%

:: Read paths from config.ini
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "Qt="') do set QT_DIR=%%a
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "OpenCV="') do set OPENCV_DIR=%%a
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "ONNX="') do set ONNX_DIR=%%a
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "FFmpeg="') do set FFMPEG_DIR=%%a
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "MySQL="') do set MYSQL_DIR=%%a
for /f "tokens=2 delims==" %%a in ('type "%~dp0config.ini" ^| findstr "InnoSetup="') do set INNO_DIR=%%a

:: Parse command line arguments
set BUILD_TYPE=Debug
set CLEAN=0

:parse_args
if "%1"=="" goto end_parse_args
if "%1"=="clean" set CLEAN=1
if "%1"=="release" set BUILD_TYPE=Release
if "%1"=="debug" set BUILD_TYPE=Debug
shift
goto parse_args
:end_parse_args

:: Clean if requested
if %CLEAN%==1 (
    echo Cleaning build directory...
    if exist %BUILD_DIR% (
        rd /s /q %BUILD_DIR%
        echo Build directory cleaned
    )
    goto :eof
)

:: Create version.h file
echo Generating version information...
(
echo #pragma once
echo #define VERSION_MAJOR %VERSION_MAJOR%
echo #define VERSION_MINOR %VERSION_MINOR%
echo #define VERSION_PATCH %VERSION_PATCH%
echo #define VERSION_BUILD %VERSION_BUILD%
echo #define VERSION_STRING "%VERSION%"
) > "%PROJECT_ROOT%\version.h"

cd %BUILD_DIR% || mkdir %BUILD_DIR% && cd %BUILD_DIR%

:: Configure and build project
echo Configuring CMake...
cmake -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DQt5_DIR=%QT_DIR%\lib\cmake\Qt5 ^
    -DOpenCV_DIR=%OPENCV_DIR% ^
    -DFFMPEG_PREFIX_PATH=%FFMPEG_DIR% ^
    -DonnxDir=%ONNX_DIR% ^
    -DsqlDir="%MYSQL_DIR%" ^
    %PROJECT_ROOT% || goto :error

echo Building project...
cmake --build . --config %BUILD_TYPE% || goto :error

:: Copy dependency files
cd bin\%BUILD_TYPE% || goto :error
if not exist platforms mkdir platforms

echo Copying dependency files...

:: Copy Qt DLLs
echo Copying Qt DLLs...
copy "%QT_DIR%\bin\Qt5Core.dll" . || goto :error
copy "%QT_DIR%\bin\Qt5Gui.dll" . || goto :error
copy "%QT_DIR%\bin\Qt5Widgets.dll" . || goto :error
copy "%QT_DIR%\bin\Qt5Charts.dll" . || goto :error
copy "%QT_DIR%\bin\Qt5PrintSupport.dll" . || goto :error
copy "%QT_DIR%\plugins\platforms\qwindows.dll" .\platforms\ || goto :error

:: Copy OpenCV DLL
echo Copying OpenCV DLL...
if exist "%OPENCV_DIR%\bin\opencv_world451d.dll" (
    echo Found debug version of OpenCV DLL
    copy "%OPENCV_DIR%\bin\opencv_world451d.dll" . || goto :error
) else (
    echo Debug version not found, using release version
    copy "%OPENCV_DIR%\bin\opencv_world451.dll" . || goto :error
)

:: Copy ONNX Runtime DLL
echo Copying ONNX Runtime DLL...
copy "%ONNX_DIR%\lib\onnxruntime.dll" . || goto :error

:: Copy FFmpeg DLLs
echo Copying FFmpeg DLLs...
copy "%FFMPEG_DIR%\bin\avcodec-*.dll" . || goto :error
copy "%FFMPEG_DIR%\bin\avformat-*.dll" . || goto :error
copy "%FFMPEG_DIR%\bin\avutil-*.dll" . || goto :error
copy "%FFMPEG_DIR%\bin\swscale-*.dll" . || goto :error
copy "%FFMPEG_DIR%\bin\swresample-*.dll" . || goto :error

:: Copy MySQL DLL
echo Copying MySQL DLL...
copy "%MYSQL_DIR%\lib\libmysql.dll" . || goto :error

:: Copy model and resource files
echo Copying model and resource files...
if not exist models\facenet mkdir models\facenet
if not exist models\yolo mkdir models\yolo
if not exist resources mkdir resources

xcopy /E /I /Y "%PROJECT_ROOT%\Facenet\ONNX_model\*" "models\facenet\" || goto :error
xcopy /E /I /Y "%PROJECT_ROOT%\Yolov8\sourse\*" "models\yolo\" || goto :error
xcopy /E /I /Y "%PROJECT_ROOT%\UI\Resources\*" "resources\" || goto :error

:: Copy config file
echo Copying config file...
copy "%~dp0config.ini" . || goto :error

cd %PROJECT_ROOT%\installer

:: Create installer output directory
:: if not exist "%PROJECT_ROOT%\..\..\installer_output" mkdir "%PROJECT_ROOT%\..\..\installer_output"

:: Generate setup.iss from template
echo Generating setup.iss...
(
echo #define MyAppVersion "%VERSION%"
echo #define MyAppName "FaceRecognition"
echo #define MyAppPublisher "Your Company Name"
echo #define MyAppDescription "Face Recognition Application"
echo.
for /f "tokens=* delims=" %%a in (setup.iss.template) do (
    set "line=%%a"
    setlocal enabledelayedexpansion
    set "line=!line:@VERSION@=%VERSION%!"
    set "line=!line:@APP_NAME@=FaceRecognition!"
    set "line=!line:@COMPANY_NAME@=Your Company Name!"
    set "line=!line:@APP_DESCRIPTION@=Face Recognition Application!"
    set "line=!line:@BUILD_TYPE@=%BUILD_TYPE%!"
    echo !line!
    endlocal
)
) > setup.iss

:: Run Inno Setup compiler
echo Generating installer...
"%INNO_DIR%\ISCC.exe" setup.iss || goto :error

echo Deployment completed!
goto :end

:error
echo.
echo Error: Deployment process failed!
echo Error code: %errorlevel%
cd %PROJECT_ROOT%\installer
goto :end

:end
echo.
echo Press any key to continue...
pause > nul
exit /b %errorlevel% 