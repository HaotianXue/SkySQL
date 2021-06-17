cmake_minimum_required(VERSION 3.17)
project(database_management_system)

set(CMAKE_CXX_STANDARD 17)

#add_executable(database_management_system
#        main.cpp
#        InterpreterManager/Parser.h InterpreterManager/Parser.cpp
#        InterpreterManager/InterpreterManager.h InterpreterManager/InterpreterManager.cpp
#        stdafx.h
#        Record.h Record.cpp
#        Query.h Query.cpp
#        Utility/utils.h
#        RecordManager/RecordManager.h RecordManager/RecordManager.cpp
#        IndexManager/IndexManager.h IndexManager/IndexManager.cpp IndexManager/BPlusTree.h
#        CatalogManager/CatalogManager.h CatalogManager/CatalogManager.cpp
#        BufferManager/BufferManager.h BufferManager/BufferManager.cpp
#        API/API.h API/API.cpp)

add_executable(database_management_system main.cpp
        InterpreterManager/InterpreterManager.cpp
        InterpreterManager/Parser.cpp
        API/API.cpp
        BufferManager/BufferManager.cpp
        CatalogManager/CatalogManager.cpp
        RecordManager/RecordManager.cpp
        Query.cpp
        Record.cpp
        IndexManager/IndexManager.cpp
        base_element.cpp Utility/utils.cpp)