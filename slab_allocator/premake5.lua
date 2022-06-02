workspace "Userspace Slab"
    configurations { "Testing", "Debug", "Release", "Integrated" }

    project "slaballocator"
        kind "StaticLib"
        language "C"
    
        targetdir "."
        objdir "bin/"
    
        files { "src/**.c", "src/**.h" }
        includedirs { "src/" }
    
        filter "configurations:Testing"
            symbols "on"
            optimize "off"
            defines { 
                "SLAB_DEBUG", 
                "SLAB_CONFIG_PARANOID_ASSERTS", 
                "POOL_CONFIG_PARANOID_ASSERTS", 
                "SMALLOC_CONFIG_PARANOID_ASSERTS" 
            }
        filter "configurations:Debug"
            symbols "on"
            optimize "off"
            defines { "SLAB_DEBUG" }
        filter "configurations:Release"
            optimize "on"
            symbols "on"
            runtime "Release"
            defines { "SLAB_RELEASE" }
        filter "configurations:integrated"
            optimize "on"
            symbols "on"
            runtime "Release"
            defines {
            	"SLAB_RELEASE",
            	"SMALLOC_CONFIG_CAMOUFLAGE"
            }
