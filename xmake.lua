set_policy("package.install_locally", true)
package("wx32")
    set_homepage("https://github.com/Dir-A/Wx32/")
    set_description("a win32 api wrapper using utf-8 and modern c++")
    -- add_urls("https://github.com/Dir-A/Wx32.git")
    set_sourcedir("D:/Space/Code/Git/Wx32")

    -- add_versions("v1.0", "e768c82b22c435ff2140454fcf0b13c01a717515")

    -- link system lib
    if is_plat("windows", "mingw") then
        add_syslinks("user32")
    end

    -- on load, get and set configs
    on_load("macosx", "linux", "windows", "mingw", "android", "iphoneos", function (package)
        --
    end)

    -- on install, check configs and compile
    on_install("windows", "mingw", function (package)
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        end
        import("package.tools.xmake").install(package, configs)
    end)

    -- on test
    on_test(function (package)
        -- 
    end)
package_end()

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
set_allowedmodes("debug", "release")

-- rules
add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release")

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

-- repo
-- add_repositories("Drepo https://github.com/Dir-A/Drepo.git")

-- packages
add_requires("wx32")

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
