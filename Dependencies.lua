-- gauri dependencies


IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/gauri/3rdparty/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/gauri/3rdparty/yaml-cpp/include"
IncludeDir["GLFW"] = "%{wks.location}/gauri/3rdparty/glfw/include"
IncludeDir["Glad"] = "%{wks.location}/gauri/3rdparty/Glad/include"
IncludeDir["imgui"] = "%{wks.location}/gauri/3rdparty/imgui"
IncludeDir["glm"] = "%{wks.location}/gauri/3rdparty/glm"
IncludeDir["stb_image"] = "%{wks.location}/gauri/3rdparty/stb_image"
IncludeDir["gtest"] = "%{wks.location}/gauri/3rdparty/gtest/googletest/include"
IncludeDir["gmock"] = "%{wks.location}/gauri/3rdparty/gtest/googlemock/include"


LibraryDir = {}


Library = {}

Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"
