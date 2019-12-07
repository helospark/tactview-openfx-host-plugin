@echo off
setlocal ENABLEDELAYEDEXPANSION
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

del /S *.obj
del /S *.dll

"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\bin\HostX64\x64\CL.exe" /c /Zi /nologo /diagnostics:classic /O2 /Oi /GL /D _WINDLL /D _MBCS /Gm- /EHsc /MD /GS /Gy /fp:precise /permissive- /Zc:wchar_t /Zc:forScope /Zc:inline /DDEBUG_LOGGING=true /Gd /TP /FC /errorReport:prompt  /Zc:strictStrings- *.cpp
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\bin\HostX64\x64\link.exe" /ERRORREPORT:PROMPT /OUT:openfxplugin.dll /NOLOGO /LIBPATH:"C:\lib\ffmpeg-3.4.2-win64-dev\lib" /LIBPATH:C:\lib\opencv\build\x64\vc15\lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /MANIFEST /LTCG /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /OPT:REF /OPT:ICF /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X64 /DLL *.obj
