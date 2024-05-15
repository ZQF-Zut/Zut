-- project
set_project("zut")
set_xmakever("2.8.9")

-- language
set_warnings("all")
set_languages("c++23")
set_encodings("utf-8")

-- allows
set_allowedplats("windows")
set_allowedarchs("x86", "x64")
set_allowedmodes("debug", "release", "releasedbg")

-- rules
add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release", "mode.releasedbg")

-- defines
if is_plat("windows")then
    add_defines("ZUT_WIN32")
elseif is_plat("linux") then 
    add_defines("ZUT_LINUX")
end 

-- lto
if is_mode("releasedbg") or is_mode("release") then
    if is_plat("windows") then
        set_policy("build.optimization.lto", true)
    end
end

-- pkg
includes("xmake/pkg.lua")

-- targets
target("zut")
    set_kind("$(kind)")
    if is_plat("windows") then
        if is_kind("shared") then
            add_rules("utils.symbols.export_all", {export_classes = true})
        end
    end
    add_files("src/**.cpp")
    add_includedirs("include", {public = true})
    add_headerfiles("include/(**.h)")
    add_packages("wx32", {public = true})

target("zut-test")
    set_default(false)
    set_kind("binary")
    add_files("test/main.cpp")
    add_deps("zut")
