-- premake5.lua
workspace "CameraProject"
    architecture "x86_64"
    startproject "CameraGui"
	
	configurations 
	{ 
		"Debug", 
		"Release" 
	}

OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDirs = {}
IncludeDirs["ImGuiBundle"]    = "CameraGui/vendor/imgui_bundle/include"
IncludeDirs["OpenCV"] 	      = "CameraGui/vendor/opencv-4.10.0-windows/include"

LibDirs = {}
LibDirs["ImGuiBundleDebug"]   = "CameraGui/vendor/imgui_bundle/Debug/lib"
LibDirs["ImGuiBundleRelease"] = "CameraGui/vendor/imgui_bundle/Release/lib"
LibDirs["OpenCV"]             = "CameraGui/vendor/opencv-4.10.0-windows/lib"

DLLDirs = {}
DLLDirs["OpenCV"] 			  = "CameraGui/vendor/opencv-4.10.0-windows/bin"

project "CameraGui"
    language "C++"
    cppdialect "C++17"
	staticruntime "Off"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. OutputDir .. "/%{prj.name}")

    files 
	{ 
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.cpp" 
	}
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"IMGUI_BUNDLE_WITH_IMGUI_NODE_EDITOR"
	}

    includedirs 
	{
		"assets",
		"%{prj.name}/src",
		"%{IncludeDirs.ImGuiBundle}",
		"%{IncludeDirs.OpenCV}"
    }

    libdirs 
	{		
		"%{LibDirs.OpenCV}"
    }
	
	links 
	{
		"glad",
		"glfw3",
		"hello_imgui",
		"imcoolbar",
		"imgui",
		"imguizmo",
		"imgui_color_text_edit",
		"imgui_command_palette",
		"imgui_knobs",
		"imgui_md",
		"imgui_node_editor",
		"imgui_test_engine",
		"imgui_tex_inspect",
		"imgui_toggle",
		"immapp",
		"immvision",
		"immvision_gl_loader",
		"implot",
		"implot3d",
		"imspinner",
		"im_file_dialog",
		"nanovg",
		"nvg_imgui",
		"plutosvg",
		"plutovg",
		"stb_hello_imgui",
	}

--    postbuildcommands {
--            "{COPYFILE} %[" .. libtorch_dlls_and_libs .. "/*.dll] %[%{!cfg.targetdir}]",
--            "{COPYFILE} %[" .. torchvision_dlls .. "/*.dll] %[%{!cfg.targetdir}]"
--    }

    filter "configurations:Debug"
	    kind "ConsoleApp"
        defines { "DEBUG" }
        symbols "On"
		
		libdirs { "%{LibDirs.ImGuiBundleDebug}" }
				
		links 
		{ 
			"freetyped",
			"opencv_world4100d"
		}
		
		postbuildcommands {
			"{COPYFILE} %[%{DLLDirs.OpenCV}/opencv_world4100d.dll] %[%{!cfg.targetdir}]",
			"{COPYFILE} %[cameras.txt] %[%{!cfg.targetdir}]",
			"{COPYFILE} %[Camera_tool.ini] %[%{!cfg.targetdir}]",
			"{COPYDIR} assets/* %[%{!cfg.targetdir}/assets]"
			--"{COPYFILE} %[" .. torchvision_dlls .. "/*.dll] %[%{!cfg.targetdir}]"
		}
				
    filter "configurations:Release"
		kind "WindowedApp"
        defines { "NDEBUG" }
        optimize "On"
		
		linkoptions { "/ENTRY:\"mainCRTStartup\"" }
		libdirs { "%{LibDirs.ImGuiBundleRelease}" }
		
		links 
		{ 
			"freetype",
			"opencv_world4100"
		}
		
		postbuildcommands {
			"{COPYFILE} %[%{DLLDirs.OpenCV}/opencv_world4100.dll] %[%{!cfg.targetdir}]",
			"{COPYFILE} %[cameras.txt] %[%{!cfg.targetdir}]",
			"{COPYFILE} %[Camera_tool.ini] %[%{!cfg.targetdir}]",
			"{COPYDIR} assets/* %[%{!cfg.targetdir}/assets]"
			--"{COPYFILE} %[" .. torchvision_dlls .. "/*.dll] %[%{!cfg.targetdir}]"
		}
