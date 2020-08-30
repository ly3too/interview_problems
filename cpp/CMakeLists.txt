cmake_minimum_required(VERSION 3.15)
project(problems)

set(CMAKE_CXX_STANDARD 14)

# auto add subdirs to project
function(auto_add_subdirs)
    file(GLOB DIRS *)
    foreach(DIR ${DIRS})
        if ((IS_DIRECTORY ${DIR}) AND (EXISTS ${DIR}/CMakeLists.txt))
            message("add subdir: ${DIR}")
            add_subdirectory(${DIR})
        endif()
    endforeach()
endfunction()

# add each subdir as an exe project with dependencies
function(auto_add_test_projects)
    cmake_parse_arguments(auto_add_exe_projects "" "" "DEPENDS" ${ARGN})

    file(GLOB DIRS *)
    foreach(DIR ${DIRS})
        if (IS_DIRECTORY ${DIR})
            file(GLOB SRCS ${DIR}/*.c ${DIR}/*.cpp ${DIR}/*.h ${DIR}/*.hpp)

            if (SRCS)
                get_filename_component(EXE_NAME ${DIR} NAME)
                message("add project: ${EXE_NAME}")
                add_executable(test_${EXE_NAME} ${SRCS})
                add_test(test_${EXE_NAME} test_${EXE_NAME})

                if (auto_add_exe_projects_DEPENDS)
                    target_link_libraries(test_${EXE_NAME} PRIVATE ${auto_add_exe_projects_DEPENDS})
                endif()
            endif(SRCS)

        endif()
    endforeach()
endfunction()

if (NOT WIN32)
find_package(GTest REQUIRED)
endif()

enable_testing()
auto_add_subdirs()