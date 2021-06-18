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

function(target_add_parser TARGET PARSER)
    # TODO Add build-level integration for kaitai-struct-compile.bat
endfunction(target_add_parser)
