project "sandbox"
    kind "ConsoleApp"
    language "C++"
    staticruntime "off"
    cppdialect "C++20"
    externalwarnings "Off"
    -- warnings "Extra"
    -- buildoptions { "/WX" }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    defines
    {
        "FMT_HEADER_ONLY"
    }

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        "%{wks.location}/gauri/include",
    }
    externalincludedirs 
    {
        "%{wks.location}/gauri/3rdparty/spdlog/include",
        "%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
    }

    links
    {
        "gauri",
        "imgui",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "GR_DEBUG"
        runtime "Debug"
        symbols "on"

	filter "configurations:Release"
		defines "GR_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "GR_DIST"
		runtime "Release"
		optimize "on"

