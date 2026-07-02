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
        buildcommands   { 'cmd /C "call ' .. path.join(ce_dev_root, "cedev.bat") .. ' && make debug"' }
        rebuildcommands { 'cmd /C "call ' .. path.join(ce_dev_root, "cedev.bat") .. ' && make clean"', 'cmd /C "call ' .. path.join(ce_dev_root, "cedev.bat") .. ' && make debug"' }
        cleancommands   { 'cmd /C "call ' .. path.join(ce_dev_root, "cedev.bat") .. ' && make clean"' }

    filter "configurations:Release"
        defines { "NDEBUG" }
        buildcommands   { 'cmd /C "call ' .. path.join(ce_dev_root, "cedev.bat") .. ' && make"' }
        rebuildcommands { 'cmd /C "call ' .. path.join(ce_dev_root, "cedev.bat") .. ' && make clean"', 'cmd /C "call ' .. path.join(ce_dev_root, "cedev.bat") .. ' && make"' }
        cleancommands   { 'cmd /C "call ' .. path.join(ce_dev_root, "cedev.bat") .. ' && make clean"' }

    filter {}