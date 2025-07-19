if "%BUILD_VS_YEAR%"=="2017" set BUILD_VS_LIBDIRPOST=vc15
if "%BUILD_VS_YEAR%"=="2019" set BUILD_VS_LIBDIRPOST=vc15
if "%BUILD_VS_YEAR%"=="2022" set BUILD_VS_LIBDIRPOST=vc15

set BUILD_VS_SVNDIR=win64_%BUILD_VS_LIBDIRPOST%
set BUILD_VS_LIBDIR="%BLENDER_DIR%..\lib\%BUILD_VS_SVNDIR%"

if NOT "%verbose%" == "" (
	echo Library Directory = "%BUILD_VS_LIBDIR%"
)
@REM if NOT EXIST %BUILD_VS_LIBDIR% (
@REM 	rem libs not found, but svn is on the system
@REM 	echo 
@REM 	if not "%SVN%"=="" (
@REM 		echo.
@REM 		echo The required external libraries in %BUILD_VS_LIBDIR% are missing
@REM 		echo.
@REM 		set /p GetLibs= "Would you like to download them? (y/n)"
@REM 		if /I "!GetLibs!"=="Y" (
@REM 			echo.
@REM 			echo Downloading %BUILD_VS_SVNDIR% libraries, please wait.
@REM 			echo.
@REM :RETRY			
@REM 			"%SVN%" checkout https://svn.blender.org/svnroot/bf-blender/trunk/lib/%BUILD_VS_SVNDIR% %BUILD_VS_LIBDIR%
@REM 			if errorlevel 1 (
@REM 				set /p LibRetry= "Error during download, retry? y/n"
@REM 				if /I "!LibRetry!"=="Y" (
@REM 					cd %BUILD_VS_LIBDIR%
@REM 					"%SVN%" cleanup 
@REM 					cd %BLENDER_DIR%
@REM 					goto RETRY
@REM 				)
@REM 				echo.
@REM 				echo Error: Download of external libraries failed. 
@REM 				echo This is needed for building, please manually run 'svn cleanup' and 'svn update' in
@REM 				echo %BUILD_VS_LIBDIR% , until this is resolved you CANNOT make a successful blender build
@REM 				echo.
@REM 				exit /b 1
@REM 			)
@REM 		)
@REM 	)
@REM )

if NOT EXIST %BUILD_VS_LIBDIR% (
	echo.
	echo Error: Required external libraries in %BUILD_VS_LIBDIR% are missing, please download them and try to compile again.
	echo.
	exit /b 1
)
