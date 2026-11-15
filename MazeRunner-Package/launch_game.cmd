@echo off
set "INSTALL_DIR=%LOCALAPPDATA%\MazeRunner"

if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
if not exist "%INSTALL_DIR%\assets\audio" mkdir "%INSTALL_DIR%\assets\audio"

copy /Y "%~dp0maze_runner.exe" "%INSTALL_DIR%\" >nul
copy /Y "%~dp0libgcc_s_seh-1.dll" "%INSTALL_DIR%\" >nul
copy /Y "%~dp0libraylib.dll" "%INSTALL_DIR%\" >nul
copy /Y "%~dp0libstdc++-6.dll" "%INSTALL_DIR%\" >nul
copy /Y "%~dp0libwinpthread-1.dll" "%INSTALL_DIR%\" >nul
copy /Y "%~dp0glfw3.dll" "%INSTALL_DIR%\" >nul
copy /Y "%~dp0background_music.ogg" "%INSTALL_DIR%\assets\audio\" >nul

start "" /D "%INSTALL_DIR%" "%INSTALL_DIR%\maze_runner.exe"
exit /b


