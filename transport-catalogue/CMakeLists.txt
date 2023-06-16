cmake_minimum_required(VERSION 3.0.0)
project(transport_catalogue VERSION 0.1.0 LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.16)
project(transport_catalogue)


set(CMAKE_CXX_STANDARD 17)


include_directories(.)


add_executable(transport_catalogue
        geo.h
        input_reader.cpp
        input_reader.h
        main.cpp
        stat_reader.cpp
        stat_reader.h
        transport_catalogue.cpp
        transport_catalogue.h)