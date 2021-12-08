-- add target
target("day05")

    -- set kind
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"aoc\"")
    
    if is_plat("windows") then
        add_defines("PLATFORM_WINDOWS")
    elseif is_plat("macosx") then
        add_defines("PLATFORM_MAC")
    end    

    -- add packages
    add_packages("tbox")

    -- add files
    add_files("main.cpp")
    add_files("parser.cpp")

    
