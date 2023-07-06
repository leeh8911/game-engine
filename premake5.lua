include "premake/solution_items.lua"
include "Dependencies.lua"

workspace "game_engine"
    architecture "x86_64"
    startproject "gauri"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

	solution_items
	{
		".editorconfig",
        ".clang-format",
        ".clang-tidy",
        "CPPLINT.cfg",
        "cspell.json",
	}
    
    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "gauri/3rdparty/GLFW"
    include "gauri/3rdparty/imgui"
	include "gauri/3rdparty/glad"
	include "gauri/3rdparty/yaml-cpp"
    include "gauri/3rdparty/gtest"

group ""

group "Core"
    include "gauri"

group "Misc"
    include "gauri_editor"
    include "sandbox"
