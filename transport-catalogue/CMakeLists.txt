cmake_minimum_required(VERSION 3.0.0)
project(transport_catalogue VERSION 0.1.0 LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.16)
project(transport_catalogue)


set(CMAKE_CXX_STANDARD 17)


include_directories(.)


add_executable(transport_catalogue
        domain.h
        geo.cpp
        geo.h
        input_reader.cpp
        input_reader.h
        json_builder.cpp
        json_builder.h
        json_reader.cpp
        json_reader.h
        json.cpp
        json.h
        main.cpp
        map_renderer.cpp
        map_renderer.h
        request_handler.cpp
        request_handler.h
        stat_reader.cpp
        stat_reader.h
        svg.cpp
        svg.h
        transport_catalogue.cpp
        transport_catalogue.h)