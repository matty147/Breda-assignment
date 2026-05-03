@echo off
REM Build and package distribution folder
REM Run this after building in Visual Studio

if exist "dist" (
    rmdir /S /Q dist
)

mkdir dist
mkdir dist\assets
mkdir dist\levels

copy /Y tmpl_2022.exe dist\tmpl_2022.exe
xcopy /E /I /Y tmpl8\assets\* dist\assets\
xcopy /E /I /Y tmpl8\levels\* dist\levels\
copy /Y dlls_x64\* dist\
copy /Y readme.txt dist\

echo Distribution package ready in dist/
