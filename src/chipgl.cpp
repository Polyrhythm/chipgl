
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "eglo.h"
#include "filewatcher.h"

#define FAIL(...) do { fprintf(stderr, __VA_ARGS__); exit(1); } while (0)

namespace {

GLuint loadShader(const char* shaderSource, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0)
    {
        std::vector<char> buffer(length);
        glGetShaderInfoLog(shader, length, nullptr, buffer.data());
        printf("shader info: %s\n", buffer.data());
    }

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        FAIL("Could not compile shader");
    }

    return shader;
}

GLint positionAttribute, timeUniform;
GLuint vao, vbo[1], shaderProgram;
GLuint fShader, vShader;
GLsizei stride = 0;
void* offset = 0;
int screenWidth, screenHeight;

typedef std::chrono::high_resolution_clock Clock;
auto startTime = Clock::now();
float currTime = 0.0f;

Filewatcher* vWatcher;
Filewatcher* fWatcher;

void compileShaders()
{
    const char* vertexShader = vWatcher->readFile();
    const char* fragShader = fWatcher->readFile();

    vShader = loadShader(vertexShader, GL_VERTEX_SHADER);
    fShader = loadShader(fragShader, GL_FRAGMENT_SHADER);

    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    glLinkProgram(shaderProgram);
    glDetachShader(shaderProgram, vShader);
    glDetachShader(shaderProgram, fShader);
}

void setAttrUnf()
{
    glUseProgram(shaderProgram);
    positionAttribute = glGetAttribLocation(shaderProgram, "position");

    if (positionAttribute < 0)
    {
        FAIL("Unable to get attribute location");
    }

    GLint resolutionUniform = glGetUniformLocation(shaderProgram, "resolution");
    if (resolutionUniform != -1)
    {
        glUniform2f(resolutionUniform, screenWidth, screenHeight);
    }

    timeUniform = glGetUniformLocation(shaderProgram, "time");
    if (timeUniform != -1)
    {
        glUniform1f(timeUniform, currTime);
    }

    glUseProgram(0);
}

void init(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    const char* eglExtensions = eglQueryString(eglGetDisplay(EGL_DEFAULT_DISPLAY),
        EGL_EXTENSIONS);

    const GLubyte* glExtensions = glGetString(GL_EXTENSIONS);

    printf("= EGL extensions==\n%s\n\n= GL extensions =\n%s\n\n",
        eglExtensions, glExtensions);

    shaderProgram = glCreateProgram();

    vWatcher = new Filewatcher("../src/vert.glsl");
    fWatcher = new Filewatcher("../src/frag.glsl");
    compileShaders();
    setAttrUnf();

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void initBuffers()
{
    static const GLfloat quad[6][3] = {
        { -1.0f, -1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        { -1.0f,  1.0f, 0.0f },
        { -1.0f,  1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        {  1.0f,  1.0f, 0.0f },
    };

    glUseProgram(shaderProgram);

    glGenVertexArraysOES(1, &vao);
    glBindVertexArrayOES(vao);
    glGenBuffers(1, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), quad,
        GL_STATIC_DRAW);
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, stride,
        offset);
    glEnableVertexAttribArray(0);

    glBindVertexArrayOES(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}


void render()
{
    setAttrUnf();

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glBindVertexArrayOES(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArrayOES(0);

    glUseProgram(0);
}

void update()
{
    auto now = Clock::now();
    currTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime)
        .count()
        * 0.001;


    if (vWatcher->fileChanged() || fWatcher->fileChanged())
    {
        compileShaders();
    }

    if (timeUniform != -1)
    {
        glUniform1f(timeUniform, currTime);
    }
}

void cleanup()
{
    glDisableVertexAttribArray(0);
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArraysOES(1, &vao);

    delete vWatcher;
    vWatcher = nullptr;
    delete fWatcher;
    fWatcher = nullptr;
}

}; // end namespace

int main(int argc, char* argv[])
{
    int width = 0, height = 0;

    if (eglo_init(&width, &height, 2) != 0)
    {
        FAIL("Could not create window");
    }

    init(width, height);
    initBuffers();

    bool quit = false;
    while (!quit) {
        EgloEvent e;
        while (eglo_next_event(&e)) {
            switch(e.type) {
                case EGLO_QUIT:
                    quit = true;
                    break;

                case EGLO_KEY_DOWN:
                    quit = true;
                    break;

                case EGLO_KEY_UP:
                    break;

                case EGLO_MOUSE_DOWN:
                    break;

                case EGLO_MOUSE_MOTION:
                    break;

                case EGLO_MOUSE_UP:
                    break;

                default:
                    break;
            }
        }

        render();
        update();

        eglo_swap_buffers();
    }

    cleanup();
    eglo_quit();

    return 0;
}
