cmake_minimum_required(VERSION 3.5)

find_package(Qt5 COMPONENTS Core
                            Gui
                            Widgets
                            Network
                            Sql
                            Svg
                            Xml
                            MultimediaWidgets
                            Test
                            PrintSupport
                            Qml
                            Quick
                            QuickWidgets
                            QuickControls2 REQUIRED)

set(QT_LIBRARIES ${Qt5Core_LIBRARIES}
                 ${Qt5Gui_LIBRARIES}
                 ${Qt5Widgets_LIBRARIES}
                 ${Qt5Network_LIBRARIES}
                 ${Qt5Sql_LIBRARIES}
                 ${Qt5Svg_LIBRARIES}
                 ${Qt5Xml_LIBRARIES}
                 ${Qt5MultimediaWidgets_LIBRARIES}
                 ${Qt5Test_LIBRARIES}
                 ${Qt5PrintSupport_LIBRARIES}
                 ${Qt5Qml_LIBRARIES}
                 ${Qt5Quick_LIBRARIES}
                 ${Qt5QuickWidgets_LIBRARIES}
                 ${Qt5QuickControls2_LIBRARIES})

set(QT_INCLUDES ${Qt5Core_INCLUDE_DIRS}
                ${Qt5Gui_INCLUDE_DIRS}
                ${Qt5Widgets_INCLUDE_DIRS}
                ${Qt5Network_INCLUDE_DIRS}
                ${Qt5Sql_INCLUDE_DIRS}
                ${Qt5Svg_INCLUDE_DIRS}
                ${Qt5Xml_INCLUDE_DIRS}
                ${Qt5MultimediaWidgets_INCLUDE_DIRS}
                ${Qt5Test_INCLUDE_DIRS}
                ${Qt5PrintSupport_INCLUDE_DIRS}
                ${Qt5Qml_INCLUDE_DIRS}
                ${Qt5Quick_INCLUDE_DIRS}
                ${Qt5QuickWidgets_INCLUDE_DIRS}
                ${Qt5QuickControls2_INCLUDE_DIRS})

function(qt_find_moc_headers moc_headers_var)

    foreach(FILE ${ARGN})
        file(READ ${FILE} CONTENT)
        string(REGEX MATCH "Q_OBJECT" RES "${CONTENT}")
        if(NOT ${RES} EQUAL -1)
            set(${moc_headers_var} ${${moc_headers_var}} ${FILE})
        endif()
    endforeach()
    set(${moc_headers_var} ${${moc_headers_var}} ${FILE} PARENT_SCOPE)

endfunction()

function(qt_add_executable target)

    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES LIBRARIES)

    cmake_parse_arguments(_ADD_EXE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    list( APPEND  libraries  ${_ADD_EXE_UNPARSED_ARGUMENTS})
    list( APPEND  libraries  ${_ADD_EXE_LIBRARIES})
    list( APPEND  sources    ${_ADD_EXE_SOURCES})


    file( GLOB_RECURSE  found_headers  *.h *.hpp)
    file( GLOB_RECURSE  found_sources  *.cpp *.c)

    add_executable( ${target}
        ${found_headers}
        ${found_sources}
        ${sources}
        )
    target_link_libraries( ${target}
        ${QT_LIBRARIES}
        ${libraries}
        )
    target_include_directories( ${target}  PRIVATE
        ${QT_INCLUDES}
        ${MODULE_PATH}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )
    target_compile_features( ${target} PRIVATE ${CXX_COMPILE_FEATURES})
    target_compile_definitions( ${target}  PRIVATE
        -DQT_MESSAGELOGCONTEXT
        -DQT_FORCE_ASSERTS
        )


    qt_find_moc_headers( found_moc_headers  ${found_headers})
    qt5_wrap_cpp( ${target}_moc_rcs
        ${found_moc_headers}
        TARGET  ${target}
        )

    file( GLOB_RECURSE  uis  *.ui)
    qt5_wrap_ui( ${target}_ui_rcs
        ${uis}
        )

    file( GLOB_RECURSE  res  *.rcs *.qrc)
    qt5_add_resources( ${target}_res_rcs
        ${res}
        )

    target_sources( ${target}  PRIVATE
        ${${target}_moc_rcs}
        ${${target}_ui_rcs}
        ${${target}_res_rcs}
        )

endfunction()

add_subdirectory(src)
