#ifndef GL_HPP
#define GL_HPP

#if defined(__GNUC__) || defined(__clang__)
#    define ALWAYS_INLINE __attribute__((always_inline)) inline
#    define PACKED __attribute__((packed))
#elif defined(_MSC_VER)
#    define ALWAYS_INLINE __forceinline
#    define PACKED
#else
#    define ALWAYS_INLINE inline
#    define PACKED
#endif

template <typename T>
struct Maybe
{
    bool has_value;
    T unwrap;
};

#ifndef ASSERT_GL_ERROR
#    ifdef GL_HPP_ASSERT_GL_ERRORS
#        define ASSERT_GL_ERROR assert(glGetError() == GL_NO_ERROR)
#    else
#        define ASSERT_GL_ERROR do {} while(0)
#    endif
#endif

// TODO: gl.hpp supports only OpenGL 3.0 for now

namespace gl
{
    struct PACKED Color4
    {
        GLclampf r, g, b, a;
    };

    template <typename That>
    struct PACKED Bit_Field
    {
        GLbitfield unwrap;

        ALWAYS_INLINE That operator| (That that) const { return {unwrap |  that.unwrap}; }
        ALWAYS_INLINE That operator& (That that) const { return {unwrap &  that.unwrap}; }
        ALWAYS_INLINE That operator^ (That that) const { return {unwrap ^  that.unwrap}; }
        ALWAYS_INLINE That operator>>(That that) const { return {unwrap >> that.unwrap}; }
        ALWAYS_INLINE That operator<<(That that) const { return {unwrap << that.unwrap}; }
        ALWAYS_INLINE That operator~ ()          const { return {~unwrap}; }
    };

    struct PACKED Buffer_Bit: public Bit_Field<Buffer_Bit> {
        static const Buffer_Bit COLOR;
        static const Buffer_Bit DEPTH;
        static const Buffer_Bit ACCUM;
        static const Buffer_Bit STENCIL;
    };

    const Buffer_Bit Buffer_Bit::COLOR   = {GL_COLOR_BUFFER_BIT};
    const Buffer_Bit Buffer_Bit::DEPTH   = {GL_DEPTH_BUFFER_BIT};
    const Buffer_Bit Buffer_Bit::ACCUM   = {GL_ACCUM_BUFFER_BIT};
    const Buffer_Bit Buffer_Bit::STENCIL = {GL_STENCIL_BUFFER_BIT};

    ALWAYS_INLINE void clear(Buffer_Bit buffer)
    {
        glClear(buffer.unwrap);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE void clearColor(Color4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        ASSERT_GL_ERROR;
    }

    enum class Shader_Type
    {
        Vertex   = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER
    };

    struct PACKED Shader
    {
        GLuint unwrap;
    };

    ALWAYS_INLINE Shader createShader(Shader_Type type)
    {
        auto shader = glCreateShader(static_cast<GLenum>(type));
        ASSERT_GL_ERROR;
        return Shader { shader };
    }

    ALWAYS_INLINE void shaderSource(Shader shader,
                                    GLsizei count,
                                    const GLchar ** string,
                                    const GLint * length)
    {
        glShaderSource(shader.unwrap, count, string, length);
        ASSERT_GL_ERROR;
    }

    template <size_t Max_Length>
    struct Info_Log
    {
        GLchar value[Max_Length];
        GLsizei length;
    };

    ALWAYS_INLINE void compileShader(Shader shader)
    {
        glCompileShader(shader.unwrap);
        ASSERT_GL_ERROR;
    }

    template <GLsizei Max_Length>
    ALWAYS_INLINE void getShaderInfoLog(Shader shader, Info_Log<Max_Length> *infoLog)
    {
        glGetShaderInfoLog(shader.unwrap, Max_Length, &infoLog->length, infoLog->value);
        ASSERT_GL_ERROR;
    }

    template <GLsizei Max_Length>
    ALWAYS_INLINE Info_Log<Max_Length> getShaderInfoLog(Shader shader)
    {
        Info_Log<Max_Length> infoLog = {};
        glGetShaderInfoLog(shader.unwrap, Max_Length, &infoLog.length, infoLog.value);
        ASSERT_GL_ERROR;
        return infoLog;
    }

    ALWAYS_INLINE bool compileStatus(Shader shader)
    {
        GLint param = 0;
        glGetShaderiv(shader.unwrap, GL_COMPILE_STATUS, &param);
        ASSERT_GL_ERROR;
        return static_cast<bool>(param);
    }

    struct PACKED Buffer
    {
        GLuint unwrap;
    };

    ALWAYS_INLINE void deleteObject(Shader shader)
    {
        glDeleteShader(shader.unwrap);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE void deleteObject(Buffer buffer)
    {
        GLuint id = buffer.unwrap;
        glDeleteBuffers(1, &id);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE void deleteObjects(GLsizei n, Buffer *buffers)
    {
        static_assert(
                sizeof(Buffer) == sizeof(GLuint),
                "Cannot use gl::deleteObjects(GLsizei n, Buffer *buffers), properly because it makes an assumption "
                "that sizeof(Buffer) is equal to sizeof(GLuint). But this is not true "
                "on this machine. Probably due to the compiler padding the Buffer structure. "
                "This is definitely a bug of gl.hpp and result of the laziness of its developers. "
                "Welcome to Open Source. :) "
                "Please submit an Issue or a Pull Request to https://github.com/tsoding/gl.hpp");
        glDeleteBuffers(n, reinterpret_cast<GLuint*>(buffers));
        ASSERT_GL_ERROR;
    }

    struct PACKED Program
    {
        GLuint unwrap;
    };

    ALWAYS_INLINE Program createProgram(void)
    {
        auto program = glCreateProgram();
        ASSERT_GL_ERROR;
        return Program { program };
    }

    ALWAYS_INLINE void deleteObject(Program program)
    {
        glDeleteProgram(program.unwrap);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE void attachShader(Program program, Shader shader)
    {
        glAttachShader(program.unwrap, shader.unwrap);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE void linkProgram(Program program)
    {
        glLinkProgram(program.unwrap);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE bool linkStatus(Program program)
    {
        GLint linked = 0;
        glGetProgramiv(program.unwrap, GL_LINK_STATUS, &linked);
        ASSERT_GL_ERROR;
        return static_cast<bool>(linked);
    }

    template <GLsizei Max_Length>
    ALWAYS_INLINE void getProgramInfoLog(Program program, Info_Log<Max_Length> *infoLog)
    {
        glGetProgramInfoLog(program.unwrap, Max_Length, &infoLog->length, infoLog->value);
        ASSERT_GL_ERROR;
    }

    template <GLsizei Max_Length>
    ALWAYS_INLINE Info_Log<Max_Length> getProgramInfoLog(Program program)
    {
        Info_Log<Max_Length> infoLog = {};
        glGetProgramInfoLog(program.unwrap, Max_Length, &infoLog.length, infoLog.value);
        ASSERT_GL_ERROR;
        return infoLog;
    }

    void useProgram(Program program)
    {
        glUseProgram(program.unwrap);
        ASSERT_GL_ERROR;
    }

    enum class Draw_Mode {
        POINTS                   = GL_POINTS,
        LINE_STRIP               = GL_LINE_STRIP,
        LINE_LOOP                = GL_LINE_LOOP,
        LINES                    = GL_LINES,
        LINE_STRIP_ADJACENCY     = GL_LINE_STRIP_ADJACENCY,
        LINES_ADJACENCY          = GL_LINES_ADJACENCY,
        TRIANGLE_STRIP           = GL_TRIANGLE_STRIP,
        TRIANGLE_FAN             = GL_TRIANGLE_FAN,
        TRIANGLES                = GL_TRIANGLES,
        TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
        TRIANGLES_ADJACENCY      = GL_TRIANGLES_ADJACENCY
    };

    ALWAYS_INLINE void drawArrays(Draw_Mode mode, GLint first, GLsizei count)
    {
        glDrawArrays(static_cast<GLenum>(mode), first, count);
        ASSERT_GL_ERROR;
    }

    struct PACKED Uniform
    {
        GLint unwrap;
    };

    template <typename T>
    struct PACKED Vec2
    {
        T x, y;
    };

    using Vec2f = Vec2<GLfloat>;

    ALWAYS_INLINE Maybe<Uniform> getUniformLocation(Program program, const GLchar *name)
    {
        auto location = glGetUniformLocation(program.unwrap, name);
        ASSERT_GL_ERROR;
        return {location >= 0, {location}};
    }

    ALWAYS_INLINE void uniform(Uniform uniform, Vec2<GLfloat> vec)
    {
        glUniform2f(uniform.unwrap, vec.x, vec.y);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE void uniform(Uniform uniform, GLfloat x)
    {
        glUniform1f(uniform.unwrap, x);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE void uniform(Uniform uniform, GLsizei count, GLint *xs)
    {
        glUniform1iv(uniform.unwrap, count, xs);
        ASSERT_GL_ERROR;
    }

    struct PACKED Vertex_Array
    {
        GLuint unwrap;
    };

    ALWAYS_INLINE void genVertexArrays(GLsizei n, Vertex_Array *arrays)
    {
        static_assert(
                sizeof(Vertex_Array) == sizeof(GLuint),
                "Cannot use gl::genVertexArrays properly because it makes an assumption "
                "that sizeof(Vertex_Array) is equal to sizeof(GLuint). But this is not true "
                "on this machine. Probably due to the compiler padding the Vertex_Array structure. "
                "This is definitely a bug of gl.hpp and result of the laziness of its developers. "
                "Welcome to Open Source. :) "
                "Please submit an Issue or a Pull Request to https://github.com/tsoding/gl.hpp");
        glGenVertexArrays(n, reinterpret_cast<GLuint*>(arrays));
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE Vertex_Array genVertexArray()
    {
        GLuint id = {};
        glGenVertexArrays(1, &id);
        ASSERT_GL_ERROR;
        return {id};
    }

    ALWAYS_INLINE void bindVertexArray(Vertex_Array array)
    {
        glBindVertexArray(array.unwrap);
        ASSERT_GL_ERROR;
    }

    ALWAYS_INLINE void genBuffers(GLsizei n, Buffer *buffers)
    {
        static_assert(
                sizeof(Buffer) == sizeof(GLuint),
                "Cannot use gl::genBuffers properly because it makes an assumption "
                "that sizeof(Buffer) is equal to sizeof(GLuint). But this is not true "
                "on this machine. Probably due to the compiler padding the Buffer structure. "
                "This is definitely a bug of gl.hpp and result of the laziness of its developers. "
                "Welcome to Open Source. :) "
                "Please submit an Issue or a Pull Request to https://github.com/tsoding/gl.hpp");
        glGenBuffers(n, reinterpret_cast<GLuint*>(buffers));
    }

    ALWAYS_INLINE Buffer genBuffer()
    {
        GLuint id = {};
        glGenBuffers(1, &id);
        ASSERT_GL_ERROR;
        return {id};
    }

    enum class Buffer_Target
    {
        ARRAY         = GL_ARRAY_BUFFER,
        ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER,
        PIXEL_PACK    = GL_PIXEL_PACK_BUFFER,
        PIXEL_UNPACK  = GL_PIXEL_UNPACK_BUFFER
    };

    enum class Buffer_Usage
    {
        STREAM_DRAW   = GL_STREAM_DRAW,
        STREAM_READ   = GL_STREAM_READ,
        STREAM_COPY   = GL_STREAM_COPY,
        STATIC_DRAW   = GL_STATIC_DRAW,
        STATIC_READ   = GL_STATIC_READ,
        STATIC_COPY   = GL_STATIC_COPY,
        DYNAMIC_DRAW  = GL_DYNAMIC_DRAW,
        DYNAMIC_READ  = GL_DYNAMIC_READ,
        DYNAMIC_COPY  = GL_DYNAMIC_COPY
    };

    void bindBuffer(Buffer_Target target, Buffer buffer)
    {
        glBindBuffer(static_cast<GLenum>(target), buffer.unwrap);
        ASSERT_GL_ERROR;
    }

    void bufferData(Buffer_Target  target,
                    GLsizeiptr  size,
                    const GLvoid *data,
                    Buffer_Usage  usage)
    {
        glBufferData(static_cast<GLenum>(target), size, data, static_cast<GLenum>(usage));
        ASSERT_GL_ERROR;
    }

    struct PACKED Attribute_Location
    {
        GLuint unwrap;
    };

    ALWAYS_INLINE void enableVertexAttribArray(Attribute_Location index)
    {
        glEnableVertexAttribArray(index.unwrap);
        ASSERT_GL_ERROR;
    }

    enum class Attribute_Size
    {
        ONE   = 1,
        TWO   = 2,
        THREE = 3,
        FOUR  = 4,
        BGRA  = GL_BGRA
    };

    ALWAYS_INLINE Maybe<Attribute_Location> getAttribLocation(Program program,
                                                              const GLchar * name)
    {
        GLint id = glGetAttribLocation(program.unwrap, name);
        ASSERT_GL_ERROR;
        return {id >= 0, {static_cast<GLuint>(id)}};
    }

    enum class Attribute_Type
    {
        HALF_FLOAT                  = GL_HALF_FLOAT,
        FLOAT                       = GL_FLOAT,
        DOUBLE                      = GL_DOUBLE,
        INT_2_10_10_10_REV          = GL_INT_2_10_10_10_REV,
        UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV
    };

    ALWAYS_INLINE void vertexAttribPointer(Attribute_Location index,
                                           Attribute_Size size,
                                           Attribute_Type type,
                                           GLboolean normalized,
                                           GLsizei  stride,
                                           const GLvoid *pointer)
    {
        glVertexAttribPointer(
                index.unwrap,
                static_cast<GLint>(size),
                static_cast<GLenum>(type),
                normalized,
                stride,
                pointer);
        ASSERT_GL_ERROR;
    }

    enum class Attribute_IType
    {
        BYTE            = GL_BYTE,
        UNSIGNED_BYTE   = GL_UNSIGNED_BYTE,
        SHORT           = GL_SHORT,
        UNSIGNED_SHORT  = GL_UNSIGNED_SHORT,
        INT             = GL_INT,
        UNSIGNED_INT    = GL_UNSIGNED_INT
    };

    ALWAYS_INLINE
    void vertexAttribIPointer(Attribute_Location index,
                              Attribute_Size size,
                              Attribute_IType type,
                              GLsizei stride,
                              const GLvoid *pointer)
    {
        glVertexAttribIPointer(index.unwrap,
                               static_cast<GLint>(size),
                               static_cast<GLenum>(type),
                               stride,
                               pointer);
        ASSERT_GL_ERROR;
    }

    enum class String_Name
    {
        VENDOR                    = GL_VENDOR,
        RENDERER                  = GL_RENDERER,
        VERSION                   = GL_VERSION,
        SHADING_LANGUAGE_VERSION  = GL_SHADING_LANGUAGE_VERSION,
        EXTENSIONS                = GL_EXTENSIONS
    };

    ALWAYS_INLINE
    const GLubyte *getString(String_Name name)
    {
        return glGetString(static_cast<GLenum>(name));
    }

    enum class Element_Index_Type
    {
        UNSIGNED_BYTE  = GL_UNSIGNED_BYTE,
        UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
        UNSIGNED_INT   = GL_UNSIGNED_INT,
    };

    void drawElements(Draw_Mode mode,
                      GLsizei count,
                      Element_Index_Type type,
                      const GLvoid *indices)
    {
        glDrawElements(
                static_cast<GLenum>(mode),
                count,
                static_cast<GLenum>(type),
                indices);
        ASSERT_GL_ERROR;
    }

    void bindAttribLocation(Program program,
                            Attribute_Location index,
                            const GLchar *name)
    {
        glBindAttribLocation(program.unwrap,
                             index.unwrap,
                             name);
        ASSERT_GL_ERROR;
    }
}

#endif  // GL_HPP
