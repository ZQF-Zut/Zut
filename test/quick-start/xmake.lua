add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release")
add_repositories("Drepo https://github.com/Dir-A/Drepo.git")

set_encodings("utf-8")
set_languages("c++23")
add_requires("zqf-zut")

target("quick-start")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("zqf-zut")
