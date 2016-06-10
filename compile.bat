:: This batch file compiles Vulkan Boilerplate using the MSVC++ compiler
:: Use it if you're in Windows and don't want to play whack-a-mole with visual studio.
:: the environment variables VS140COMNTOOLS, VULKAN_SDK and GLFW_PATH must be defined
:: VS140COMNTOOLS should be defined if you have VS 2015 installed.
:: VULKAN_SDK should be defined if you installed the LunarG SDK via their installer.
:: You'll have to point GLFW_PATH to the directory in which reside both the glfw 'include'
:: directory and the 'lib-vc2015' directory. These can be downloaded from
:: http://www.glfw.org/download.html (click the 32 or 64 bit windows binaries button)

:: TO USE:
:: do not move this file from the VulkanBoilerplate directory.
:: rather, call it from the location where you want the executable to appear.
:: For example, make a folder called 'build', then go into command prompt and
:: navigate to 'build'.  Now run the batch file. So if your build directory were in
:: the VulkanBoilerplate directory, you'd enter "call ../compile.bat"
:: You can re-run it in the same directory again to only compile what's changed
:: since the last time you compiled.

@echo off

setlocal

set "batdir=%~dp0"

:: calls the batch file that sets up the VS developer console environment
call "%VS140COMNTOOLS%VsDevCmd.bat"

cl /c /I"%GLFW_PATH%\include" /I"%VULKAN_SDK%\Include" /I"%batdir%vkbp" /I"%batdir%vkbp\source" /nologo /W1 /WX- /O2 /Ob2 /Oy- /D WIN32 /D _WINDOWS /D _REENTRANT /D NDEBUG /D _MBCS /Zi /Gm /EHsc /MD /GS /Zc:wchar_t /Zc:forScope /Zc:inline /Gd /TP /analyze- "%batdir%vkbp\source\vkbpGlobal.cpp" "%batdir%vkbp\source\vkbpHelpers.cpp"  "%batdir%vkbp\source\vkbpValidator.cpp"  "%batdir%vkbp\source\Vkbp.cpp"  "%batdir%vkbpTest.cpp"

link /OUT:vkbp.exe /INCREMENTAL:NO /NOLOGO kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib "%GLFW_PATH%\lib-vc2015\glfw3.lib" "%VULKAN_SDK%\Bin32\vulkan-1.lib" /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /SUBSYSTEM:CONSOLE /TLBID:1 /DYNAMICBASE /NXCOMPAT /IMPLIB:vkbp.lib /MACHINE:X86 /SAFESEH  /machine:X86 vkbpGlobal.obj vkbpHelpers.obj vkbpValidator.obj Vkbp.obj vkbpTest.obj

endlocal

pause