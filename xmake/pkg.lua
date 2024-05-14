--[[
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
--]]

-- repo
add_repositories("Drepo https://github.com/Dir-A/Drepo.git")

-- packages
add_requires("wx32")