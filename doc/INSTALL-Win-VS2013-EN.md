# Compilation Guide for VS 2013 CE 32/64bit
***! INCOMPLETE !***

## Description:
This Guide will give a detailed explanation on how to compile KVIrc yourself

## Requirements:

* [Microsoft Windows Operation System](http://www.microsoft.com/de-de/windows)
* [Microsoft Visual Studio 2013 CE](https://www.visualstudio.com/visual-studio-community-vs)
* [Qt5](http://www.qt.io/download-open-source/)
* [OpenSSL](http://openssl.org/source/)
* [ActivePerl](http://www.activestate.com/activeperl) or [Strawberry Perl](http://strawberryperl.com/)
* [Python](https://www.python.org/downloads/windows/)
* [CMake](http://www.cmake.org/)
* [zlib](http://www.zlib.net/)
* [NSIS - Nullsoft Scriptable Install System](http://nsis.sourceforge.net/Download)
* [Git Client](https://git-scm.com/downloads)
* [GNU sed](http://gnuwin32.sourceforge.net/packages/sed.htm)
* [GNU GetText](http://gnuwin32.sourceforge.net/packages/gettext.htm)
* [Doxygen](http://www.stack.nl/~dimitri/doxygen/download.html) *optional*
* [WindowsSDK](https://msdn.microsoft.com/de-de/windows/desktop/bg162891.aspx) *optional*

## Install (& and compile) Requirements:

1.  Microsoft Visual Studio 2013 CE:
    * Download Visual Studio from the link above, the Installation will take about  
      8 GB of Space, though only if you don't select a product from the extras.  
      You probably wouldn't need them anyway, except if you want to develop  
      Windows Apps or ASP.NET Pages and so on.
2.  Qt5:
    * Download the Qt Online Installer; run it. On the Screen to select the Products  
      you want to install deselect everything, Qt Creator can't be deselected so you  
      have to stick with it. Now choose only the point for the VS 2013 32bit Sources  
      (msvc2013 32bit).
3.  Perl:
    * ActivePerl:
    * Strawberry Perl:
4.  OpenSSL:  
    * ***Install first Perl, otherwise you won't be able to compile OpenSSL!***
    * Download the Source Package, because we are going to compile OpenSSL ourself.  
     + Extract the Archive to something like `C:\projects\src\openssl`
     + Open a VS2013 x86 Native Prompt, Windows + R:  
       `%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"" x86`  
       `perl Configure VC-WIN32 --prefix=C:\projects\openssl_x86_32`  
       `ms\do_ms`  
       `nmake -f ms\nt.mak`  
       `nmake -f ms\nt.mak install`
5.  Python:
6.  CMake:
7.  zlib:
8.  NSIS:
9.  Git Client
10. GNU sed:
11. GNU GetText:
12. Doxygen:
13. WindowsSDK:

## KVIrc Compilation:

1. Open a VS2013 32bit shell:
2. Choose a Directory where you will clone the Sourcecode:  
   `mkdir C:\projects cd C:\projects`  
   `git clone https://github.com/kvirc/KVIrc.git`  
   `cd KVIrc`
3. Change to the new Directory and create a folder named `build`  
   `mkdir build`  
   `cd build`
4. Copy the `win_vs_cmake.bat` to your `build`-Directory  
   `copy ..\doc\win_vs_cmake.bat .`
5. Edit the copied `win_vs_cmake.bat` with your preferred editor  
   *(Notepad, Notepad++, Wordpad, Sublime, Visual Code, gVim)*
