cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(rastin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base adv)


set(SOURCES 
    src/Rastin.cpp
    src/ctwinding.cpp
    src/MainWindow.cpp
    src/GridTableModel.cpp
    src/CTWindow.cpp
    src/GraphPanel.cpp
    src/MyGridCellFloatEditor.cpp
)

set(HEADERS
    include/rastin/ArchiveSplines.h
    include/rastin/Spline.h
    include/rastin/wxstring.h
    include/rastin/ctwinding.h
    include/rastin/CTWindow.h
    include/rastin/GraphPanel.h
    include/rastin/GridTableModel.h
    include/rastin/MainWindow.h
    include/rastin/MyGridCellFloatEditor.h
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${wxWidgets_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${wxWidgets_CXX_FLAGS}
)

find_package(cereal QUIET)

if(cereal_FOUND)
    message(STATUS "Cereal found in the system")
    if(TARGET cereal::cereal)
        set(CEREAL_TARGET cereal::cereal)
    elseif(TARGET cereal)
        set(CEREAL_TARGET cereal)
    else()
        target_include_directories(${PROJECT_NAME} PRIVATE ${cereal_INCLUDE_DIRS})
        set(CEREAL_TARGET "")
    endif()
else()
    message(STATUS "Cereal not found in the system. Download...")

    include(FetchContent)

    FetchContent_Declare(
        cereal
        GIT_REPOSITORY https://github.com/USCiLab/cereal.git
        GIT_TAG v1.3.2
        GIT_SHALLOW TRUE
    )

    set(SKIP_PERFORMANCE_COMPARISON ON)
    set(SKIP_BOOST_PERFORMANCE_COMPARISON ON)
    set(SKIP_EXAMPLES ON)
    set(SKIP_DOCS ON)
    set(SKIP_SERIALIZATION_IN_BOOST_TEST ON)
    set(SKIP_SERIALIZATION_IN_BOOST_CHECK ON)
    set(SKIP_PORTABILITY_TEST ON)
    set(SKIP_BOOST_SERIALIZATION_TEST ON)

    FetchContent_MakeAvailable(cereal)

    set(CEREAL_TARGET cereal)
endif()

if(CEREAL_TARGET)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CEREAL_TARGET})
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ${wxWidgets_DEFINITIONS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${wxWidgets_LIBRARIES}
)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/curves.bin
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/curves.bin
)
