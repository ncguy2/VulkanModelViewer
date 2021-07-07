function(target_add_shader TARGET SHADER)

    # Find glslc shader compiler.
    find_program(GLSLC glslc)

    # All shaders for a sample are found here.
    set(current-shader-path ${CMAKE_SOURCE_DIR}/assets/shaders/${SHADER})

    set(current-output-path ${CMAKE_BINARY_DIR}/assets/shaders/${SHADER}.spv)

    # Add a custom command to compile GLSL to SPIR-V.
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})
    add_custom_command(
            OUTPUT ${current-output-path}
            COMMAND ${GLSLC} -o ${current-output-path} ${current-shader-path}
            DEPENDS ${current-shader-path}
            IMPLICIT_DEPENDS CXX ${current-shader-path}
            VERBATIM)

    # Make sure our native build depends on this output.
    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
endfunction(target_add_shader)

function(add_plugin TARGET PLUGIN_ROOT PLUGIN_NAME)
    add_subdirectory(${PLUGIN_ROOT})
    add_dependencies(${TARGET} ${PLUGIN_NAME})
    target_add_kaitai(${PLUGIN_NAME})
endfunction(add_plugin)

function(prep_parser PARSER)
    set(GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gen)

    get_filename_component(FILE_NAME ${PARSER} NAME_WE)
    file(MAKE_DIRECTORY ${GEN_DIR})

    set(FILE_NAME_CPP ${FILE_NAME}.cpp)
    set(FILE_NAME_H ${FILE_NAME}.h)
    file(TOUCH ${GEN_DIR}/${FILE_NAME_CPP})
    file(TOUCH ${GEN_DIR}/${FILE_NAME_H})
endfunction(prep_parser)

macro(target_add_parser TARGET PARSER)
    message("Adding parser ${PARSER} to target ${TARGET}")

    set(KSC ${CMAKE_MODULE_PATH}/ksc.ps1)
    set(KSC_HOME $ENV{KAITAI_STRUCT_COMPILER_HOME})

    get_target_property(TARGET_SOURCE_DIR ${TARGET} SOURCE_DIR)
    set(GEN_DIR ${TARGET_SOURCE_DIR}/gen)

    get_filename_component(FILE_NAME ${PARSER} NAME_WE)
    file(MAKE_DIRECTORY ${GEN_DIR})

    set(FILE_NAME_CPP ${FILE_NAME}.cpp)
    set(FILE_NAME_H ${FILE_NAME}.h)
    set(CMD_OUTPUT ${GEN_DIR}/${FILE_NAME_CPP})
    message("  - CMD_OUTPUT: ${CMD_OUTPUT}")
    message("  - GEN_DIR: ${GEN_DIR}")
    message("  - KSC: ${KSC}")
    message("  - KSC_HOME: ${KSC_HOME}")
    message("  - COMMAND: pwsh -File ${KSC} -KscHome ${KSC_HOME} --target cpp_stl --outdir ${GEN_DIR} ${PARSER}")
    message("  - DEPENDS: ${TARGET_SOURCE_DIR}/${PARSER}")

    add_custom_command(
            OUTPUT ${CMD_OUTPUT}
            COMMAND pwsh -File ${KSC} -KscHome ${KSC_HOME} --target cpp_stl --outdir ${GEN_DIR} ${PARSER}
            DEPENDS "${TARGET_SOURCE_DIR}/${PARSER}"
            WORKING_DIRECTORY ${TARGET_SOURCE_DIR}
            VERBATIM
    )
    execute_process(
            COMMAND pwsh -File ${KSC} -KscHome ${KSC_HOME} --target cpp_stl --outdir ${GEN_DIR} ${PARSER}
            WORKING_DIRECTORY ${TARGET_SOURCE_DIR}
    )

    set_source_files_properties(${GEN_DIR} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PUBLIC ${GEN_DIR})
    target_include_directories(${TARGET} PRIVATE ${GEN_DIR})
endmacro(target_add_parser)

macro(add_parser TARGET PARSER)
    message("Adding parser ${PARSER} to target ${TARGET}")

    # TODO Add build-level integration for kaitai-struct-compile.bat
#    set(KSC "kaitai-struct-compile.bat")
    set(KSC ${CMAKE_MODULE_PATH}/ksc.ps1)
    set(KSC_HOME $ENV{KAITAI_STRUCT_COMPILER_HOME})

    set(GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gen)

    get_filename_component(FILE_NAME ${PARSER} NAME_WE)
    file(MAKE_DIRECTORY ${GEN_DIR})

    set(FILE_NAME_CPP ${FILE_NAME}.cpp)
    set(FILE_NAME_H ${FILE_NAME}.h)
    set(CMD_OUTPUT ${GEN_DIR}/${FILE_NAME_CPP})
    message("  - CMD_OUTPUT: ${CMD_OUTPUT}")
    message("  - GEN_DIR: ${GEN_DIR}")
    message("  - KSC: ${KSC}")
    message("  - KSC_HOME: ${KSC_HOME}")
    message("  - COMMAND: pwsh -File ${KSC} -KscHome ${KSC_HOME} --target cpp_stl --outdir ${GEN_DIR} ${PARSER}")
    message("  - DEPENDS: ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER}")

    add_custom_command(
            OUTPUT ${CMD_OUTPUT}
            COMMAND pwsh -File ${KSC} -KscHome ${KSC_HOME} --target cpp_stl --outdir ${GEN_DIR} ${PARSER}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${PARSER}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
    )

    set_source_files_properties(${GEN_DIR} PROPERTIES GENERATED TRUE)
endmacro(add_parser)

function(target_add_kaitai TARGET)
    target_link_directories(${TARGET} PUBLIC ${CMAKE_BINARY_DIR}/external/kaitai_struct_cpp_stl_runtime)
    target_link_libraries(${TARGET} kaitai_struct_cpp_stl_runtime)
    add_dependencies(${TARGET} kaitai_struct_cpp_stl_runtime)
endfunction()