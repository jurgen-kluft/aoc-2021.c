-- add target
target("day05")

    -- set kind
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"aoc\"")

    -- add packages
    add_packages("tbox")

    -- add files
    add_files("*.cpp")

