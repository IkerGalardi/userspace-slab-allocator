project "bench"
    kind "ConsoleApp"
    language "C"

    targetdir "."
    objdir "bin/"

    files { "src/**.c", "src/**.h" }
    includedirs { "src/", "../slab_allocator/src"}

    libdirs {"../slab_allocator/"}

    links {"slaballocator"}

    filter "configurations:Debug"
        symbols "on"
        optimize "off"
    filter "configurations:Release"
        optimize "on"
        symbols "off"
        runtime "Release"
