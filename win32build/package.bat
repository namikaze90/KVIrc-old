@echo off
rem #####################################################
rem Usage:
rem ----------------------
rem Additional parameters:
rem 	--with-qt
rem 		The batch file tries to find the according Qt-Files automatically
rem	--build-doc
rem		The batch file will build the documentation too
rem	--build-translations
rem		The batch file will build the translations too
rem #####################################################

set PROJECTDIR=..
set DESTDIR=package
set DOCSDIR=..\kvirc-docs

set SEPERATOR=---------------------------------------

rem Only dirs which are needed before copying or which are empty
set SUBDIRS=pics modules\caps\toolbar qt-plugins locale

rem All needed Qt-files only used with --with-qt parameter
set QTDLLS=QtCore4.dll Qt3Support4.dll QtGui4.dll QtNetwork4.dll QtSql4.dll QtXml4.dll mingwm10.dll

:main
rem Creating dirs ...
if not exist %DESTDIR% mkdir %DESTDIR%
for %%D in (%SUBDIRS%) do if not exist %DESTDIR%\%%D mkdir %DESTDIR%\%%D

copy /y %PROJECTDIR%\README %DESTDIR%\README.txt
copy /y %PROJECTDIR%\ChangeLog %DESTDIR%\ChangeLog.txt

xcopy /y /s /exclude:bat_excludes.txt %PROJECTDIR%\doc\scriptexamples\*.* %DESTDIR%\doc\scriptexamples\

xcopy /y  %PROJECTDIR%\doc\*.txt %DESTDIR%\doc\

xcopy /y %PROJECTDIR%\INSTALL %DESTDIR%\doc\
xcopy /y %PROJECTDIR%\TODO %DESTDIR%\doc\
xcopy /y %PROJECTDIR%\FAQ %DESTDIR%\doc\
xcopy /y %PROJECTDIR%\data\msgcolors\*.msgclr %DESTDIR%\msgcolors\

xcopy /y %PROJECTDIR%\data\deftheme\silverirc\*.kvc %DESTDIR%\themes\silverirc\
xcopy /y %PROJECTDIR%\data\deftheme\silverirc\*.png %DESTDIR%\themes\silverirc\

xcopy /y %PROJECTDIR%\data\config\*.kvc %DESTDIR%\config\
xcopy /y %PROJECTDIR%\data\config\*.css %DESTDIR%\config\
xcopy /y %PROJECTDIR%\data\config\preinstalled.kvc.win32-example %DESTDIR%\config\
xcopy /y %PROJECTDIR%\src\modules\notifier\libkvinotifier.kvc %DESTDIR%\config\modules\

xcopy /y %PROJECTDIR%\data\defscript\default.kvs %DESTDIR%\defscript\
xcopy /y %PROJECTDIR%\data\defscript\theme-install.kvs %DESTDIR%\defscript\
xcopy /y %PROJECTDIR%\data\defscript\installer.kvs %DESTDIR%\defscript\

for /R %PROJECTDIR%\src\ %%I in (*.png) do copy /y %%I %DESTDIR%\pics\%%~nI%%~xI
xcopy /y /s %PROJECTDIR%\data\pics\*.png %DESTDIR%\pics\


xcopy /y %PROJECTDIR%\doc\COPYING %DESTDIR%\license\
xcopy /y %PROJECTDIR%\src\modules\tip\libkvitip*.kvc %DESTDIR%\config\modules\
xcopy /y %PROJECTDIR%\src\modules\logview\caps\logview %DESTDIR%\modules\caps\tool\
xcopy /y %PROJECTDIR%\src\modules\lamerizer\lamerizer %DESTDIR%\modules\caps\crypt\
xcopy /y %PROJECTDIR%\src\modules\rijndael\caps\rijndael %DESTDIR%\modules\caps\crypt\
xcopy /y %PROJECTDIR%\src\modules\filetransferwindow\caps\filetransferwindow %DESTDIR%\modules\caps\tool\
xcopy /y %PROJECTDIR%\src\modules\sharedfileswindow\caps\sharedfileswindow %DESTDIR%\modules\caps\tool\
xcopy /y %PROJECTDIR%\src\modules\mircimport\caps\mircimport %DESTDIR%\modules\caps\serverimport\
xcopy /y %PROJECTDIR%\src\modules\url\caps\url %DESTDIR%\modules\caps\action\


xcopy /y %PROJECTDIR%\data\helppics\helplogoleft.png %DESTDIR%\help\en\
xcopy /y %PROJECTDIR%\data\helppics\helplogoright.png %DESTDIR%\help\en\

rem nsis does not take empty dirs
echo "dummy" > %DESTDIR%\qt-plugins\dummy.txt

echo %SEPERATOR%
echo Finished main part. Checking for options.
echo %SEPERATOR%

rem Checking parameters
:nextpar
shift
if "%0"=="--with-qt" goto with-qt
if "%0"=="--release" goto release
if "%0"=="--debug" goto debug
if "%0"=="--build-doc" goto build-doc
if "%0"=="--build-translations" goto build-translations
if "%0"=="" goto finished
goto nextpar

:release
xcopy /y %PROJECTDIR%\bin\release\*.dll %DESTDIR%\
xcopy /y %PROJECTDIR%\bin\release\*.exe %DESTDIR%\
xcopy /y %PROJECTDIR%\bin\release\modules\*.dll %DESTDIR%\modules\
goto nextpar

:debug
xcopy /y %PROJECTDIR%\bin\debug\*.dll %DESTDIR%\
xcopy /y %PROJECTDIR%\bin\debug\*.exe %DESTDIR%\
xcopy /y %PROJECTDIR%\bin\debug\modules\*.dll %DESTDIR%\modules\
goto nextpar

:with-qt
echo --- Trying to autodetect additional Qt-files
for %%D in (%QTDLLS%) do (
	if exist %%~dp$PATH:D%%D (
		echo %%~dp$PATH:D%%D
		copy /y %%~dp$PATH:D%%D %DESTDIR%\
	) else echo "%%D" not found. You have to copy it manually.
)
goto nextpar

:build-doc
echo --- Generating documentation...
rem for /R %PROJECTDIR%\ %%I in (*.cpp) do echo %%I >> docfiles.tmp
rem for /R %PROJECTDIR%\data\doctemplates\ %%I in (*.template) do echo %%I >> docfiles.tmp
rem gendoc-helper.exe docfiles.tmp "%PROJECTDIR%\admin\gendoc.pl" "%DOCSDIR%\"
rem rm -f tmpdocfiles.txt
goto nextpar

:build-translations
echo --- Generating translations...
for /R %PROJECTDIR%\po\ %%I in (*.po) do (
	echo ------ Current file: %%~nI.po
	cmd.exe /c ""%MSGFMT%" -vo %%~pI%%~nI.mo %%I"
)
for /R %PROJECTDIR%\po\ %%I in (*.mo) do copy /y %%I %DESTDIR%\locale\%%~nI%%~xI
echo --- Done.
goto nextpar

:finished
echo %SEPERATOR%
echo Package complete.
echo %SEPERATOR%

:end
@echo on