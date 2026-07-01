local ce_dev_root = os.getenv("CEDEV_ROOT")

workspace "SNAKEPP"
    configurations { "Debug", "Release" }
    platforms { "ez80" }
    location "."
    targetdir "bin"

project "SNAKEPP"
    kind "Makefile"
    language "C"

    targetname "SNAKEPP"
    files {
        "src/**",
        "makefile"
    }

    includedirs { "src" }
    externalincludedirs { path.join(ce_dev_root, "include") }

    filter "configurations:Debug"
        defines { "_DEBUG" }
        buildcommands { "make debug" }
        rebuildcommands { "make clean", "make debug" }
        cleancommands { "make clean" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        buildcommands { "make" }
        rebuildcommands { "make clean", "make" }
        cleancommands { "make clean" }

    filter {}
