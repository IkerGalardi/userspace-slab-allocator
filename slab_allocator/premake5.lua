project "slaballocator"
    kind "StaticLib"
    language "C"

    targetdir "."
    objdir "bin/"

    files { "src/**.c", "src/**.h" }
    includedirs { "src/" }

    filter "configurations:Debug"
        symbols "on"
        optimize "off"
        defines { "SLAB_DEBUG" }
    filter "configurations:Release"
        optimize "on"
        symbols "off"
        runtime "Release"
        defines { "SLAB_RELEASE" }
