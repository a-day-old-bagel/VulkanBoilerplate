:: call the batch file that sets up the VS developer console environment
@call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

:: call the compiler
@cl /c /I"C:\libs\VulkanSDK\1.0.3.1\Include" /IC:\Users\Galen\Code\VulkanBoilerplate /nologo /W1 /WX- /O2 /Ob2 /Oy- /D WIN32 /D _WINDOWS /D _REENTRANT /D NDEBUG /D _MBCS /D VK_USE_PLATFORM_WIN32_KHR /Zi /Gm /EHsc /MD /GS /Zc:wchar_t /Zc:forScope /Zc:inline /Gd /TP /analyze- C:\Users\Galen\Code\VulkanBoilerplate\vkbpDefines.cpp C:\Users\Galen\Code\VulkanBoilerplate\vulkanHelpers.cpp C:\Users\Galen\Code\VulkanBoilerplate\VulkanContext.cpp C:\Users\Galen\Code\VulkanBoilerplate\vulkanMain.cpp

:: call the linker
@link /OUT:vkbp.exe /INCREMENTAL:NO /NOLOGO kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib "C:\libs\VulkanSDK\1.0.3.1\Bin32\vulkan-1.lib" /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /SUBSYSTEM:CONSOLE /TLBID:1 /DYNAMICBASE /NXCOMPAT /IMPLIB:vkbp.lib /MACHINE:X86 /SAFESEH  /machine:X86 vkbpDefines.obj vulkanHelpers.obj VulkanContext.obj vulkanMain.obj

:: re-copy the asset tree into the working directory to reflect any changes made there
::@rm -rf C:\Users\Galen\Code\DualityEngine\buildManual\Assets
::@cp -r C:\Users\Galen\Code\DualityEngine\Assets C:\Users\Galen\Code\DualityEngine\buildManual\

:: "press any key to continue"
@pause