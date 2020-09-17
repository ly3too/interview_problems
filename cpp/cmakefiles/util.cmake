include(GNUInstallDirs)
include(CMakeParseArguments)
include(GenerateExportHeader)

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

# auto add a dir as library. directories should be in following structure：
# + subproject_root
#   - include/
#   - src/
#       - *.cpp/*.c
# usage: auto_add_library(target TYPE static DEPENDS libxxx)
function(auto_add_library TARGET_NAME)
    # parse args
    cmake_parse_arguments(AUTO_ADD_LIBRARY "NO_INSTALL;NO_INSTALL_HEADER;NO_PREFIX;NO_EXPORT;GEN_EXPORT" "TYPE;DESTINATION;GEN_INC_DIR" "DEPENDS" ${ARGN} )
    set(TYPE ${AUTO_ADD_LIBRARY_TYPE})
    set(DEPEND_TARGETS ${AUTO_ADD_LIBRARY_DEPENDS})
    # TYPE required
    if(NOT TYPE)
        message(FATAL_ERROR "library TYPE required")
    endif()

    # add the library
    message("add library ${TARGET_NAME}, type: ${TYPE}, depends: ${DEPEND_TARGETS}")
    set(_HEADER_TYPE PUBLIC)
    if(TYPE STREQUAL "INTERFACE")
        set(_HEADER_TYPE INTERFACE)
        add_library(${TARGET_NAME} ${TYPE}) # interface, only headers allowed
    else()
        # add src files
        file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
        add_library(${TARGET_NAME} ${TYPE} ${SRCS})
    endif()
    # link the target
    if (DEPEND_TARGETS)
        target_link_libraries(${TARGET_NAME} ${_HEADER_TYPE} ${DEPEND_TARGETS})
    endif()
    # add public headers
    target_include_directories(${TARGET_NAME} ${_HEADER_TYPE} $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
    # set prefix empty if specified
    if(AUTO_ADD_LIBRARY_NO_PREFIX)
        set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
    endif()

    # generate dllexport/dllimport macros
    if (AUTO_ADD_LIBRARY_GEN_EXPORT AND NOT TYPE STREQUAL "INTERFACE")
        if (NOT AUTO_ADD_LIBRARY_GEN_INC_DIR)
            set(_EXPORT_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGET_NAME}/${TARGET_NAME}_export.h)
        else()
            set(_EXPORT_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/${AUTO_ADD_LIBRARY_GEN_INC_DIR}/${TARGET_NAME}_export.h)
        endif()
        generate_export_header(${TARGET_NAME} EXPORT_FILE_NAME ${_EXPORT_HEADER_PATH})
    endif()

    if(NOT AUTO_ADD_LIBRARY_NO_INSTALL)
        # install headers
        if(NOT AUTO_ADD_LIBRARY_NO_INSTALL_HEADER)
            file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
            foreach(_HEADER ${HEADERS})
                if( IS_DIRECTORY "${_HEADER}" )
                    list( APPEND _DIRS_TO_DEPLOY "${_HEADER}" )
                else()
                    list( APPEND _FILES_TO_DEPLOY "${_HEADER}" )
                endif()
            endforeach()
            install (DIRECTORY ${_DIRS_TO_DEPLOY} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
            install (FILES ${_FILES_TO_DEPLOY} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
        endif()


        # install binaries
        install(TARGETS ${TARGET_NAME}
                EXPORT ${TARGET_NAME}-export
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${AUTO_ADD_LIBRARY_DESTINATION}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${AUTO_ADD_LIBRARY_DESTINATION}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/${AUTO_ADD_LIBRARY_DESTINATION}
                )


        # export cmake files for find package
        if(ENABLE_EXPORT_INSTALL AND NOT AUTO_ADD_LIBRARY_NO_EXPORT)
            install(EXPORT ${TARGET_NAME}-export
                    FILE ${TARGET_NAME}Config.cmake
                    NAMESPACE ${TARGET_NAME}::
                    DESTINATION ${CMAKE_INSTALL_LIBDIR}/${AUTO_ADD_LIBRARY_DESTINATION}/cmake/${TARGET_NAME}/
                    )
        endif()
    endif()
endfunction()

# automatically add an executable
# default install to bindir
# usage: auto_add_executable(target SRCS main.cpp DEPENDS libxxx)
function(auto_add_executable TARGET_NAME )
    cmake_parse_arguments(AUTO_ADD_EXECUTABLE "NO_INSTALL" "DESTINATION" "SRCS;DEPENDS" ${ARGN} )
    if(NOT AUTO_ADD_EXECUTABLE_SRCS)
        message(FATAL_ERROR "source files (SRCS) required")
    endif()

    message("add executable ${TARGET_NAME}, srcs: ${AUTO_ADD_EXECUTABLE_SRCS}, depends: ${AUTO_ADD_EXECUTABLE_DEPENDS}")
    add_executable(${TARGET_NAME} ${AUTO_ADD_EXECUTABLE_SRCS})
    if (AUTO_ADD_EXECUTABLE_DEPENDS)
        target_link_libraries(${TARGET_NAME} ${AUTO_ADD_EXECUTABLE_DEPENDS})
    endif()

    # install the binaries
    if(NOT AUTO_ADD_EXECUTABLE_NO_INSTALL)
        install(TARGETS ${TARGET_NAME}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${AUTO_ADD_EXECUTABLE_DESTINATION}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${AUTO_ADD_EXECUTABLE_DESTINATION}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/${AUTO_ADD_EXECUTABLE_DESTINATION}
                )
    endif()
endfunction()

# automatically add an test
function(auto_add_test TARGET_NAME)
    cmake_parse_arguments(AUTO_ADD_TEST "" "" "SRCS;DEPENDS" ${ARGN} )
    if(NOT AUTO_ADD_TEST_SRCS)
        message(FATAL_ERROR "source files (SRCS) required")
    endif()

    message("add test ${TARGET_NAME}, srcs: ${AUTO_ADD_TEST_SRCS}, depends: ${AUTO_ADD_TEST_DEPENDS}")
    enable_testing()
    add_executable(${TARGET_NAME} ${AUTO_ADD_TEST_SRCS})
    if (AUTO_ADD_TEST_DEPENDS)
        target_link_libraries(${TARGET_NAME} ${AUTO_ADD_TEST_DEPENDS})
    endif()
    add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
endfunction()