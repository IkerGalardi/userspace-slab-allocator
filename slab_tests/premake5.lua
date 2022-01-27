project "slabtests"
    kind "ConsoleApp"
    language "C"

    targetdir "."
    objdir "bin/"

    files { "src/**.c", "src/**.h" }
    includedirs { "src/" }

    filter "configurations:Debug"
        symbols "on"
        optimize "off"
    filter "configurations:Release"
        optimize "on"
        symbols "off"
        runtime "Release"
