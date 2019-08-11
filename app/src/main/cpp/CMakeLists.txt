cmake_minimum_required(VERSION 3.4.1)

aux_source_directory(. src)

add_library(
        native-lib
        SHARED
        ${src})


find_library( # Sets the name of the path variable.
        log-lib

        log)



target_link_libraries( # Specifies the target library.
        native-lib
        jnigraphics
        ${log-lib})