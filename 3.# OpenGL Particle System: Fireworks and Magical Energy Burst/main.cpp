#include <SDL2/SDL.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

#define NUM_PARTICLES 1024

struct Particle {
    float pos[3];
    float vel[3];
    float color[4];
    float lifetime;
};

std::vector<Particle> particles;
GLuint vbo, vao, shaderProgram;
glm::mat4 projection, view;

const char* vertexShaderSource = R"(
#version 300 es
uniform mat4 u_mvp;
uniform float u_time;
uniform vec3 u_gravity;
in vec3 a_position;
in vec3 a_velocity;
in vec4 a_color;
in float a_lifetime;
out vec4 v_color;
void main() {
    float t = u_time;
    if (t > a_lifetime) t = a_lifetime;
    vec3 pos = a_position + a_velocity * t + 0.5 * u_gravity * t * t;
    gl_Position = u_mvp * vec4(pos, 1.0);
    v_color = a_color;
    v_color.a *= max(0.0, 1.0 - (t / a_lifetime));
    gl_PointSize = 10.0 * v_color.a;
}
)";

const char* fragmentShaderSource = R"(
#version 300 es
precision mediump float;
in vec4 v_color;
out vec4 fragColor;
void main() {
    fragColor = v_color;
}
)";

GLuint createShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Shader compile error: %s\n", infoLog);
        exit(1);
    }
    return shader;
}

GLuint createProgram(GLuint vs, GLuint fs) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Program link error: %s\n", infoLog);
        exit(1);
    }
    return program;
}

void initParticles(float x, float y, float z) {
    particles.clear();
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        Particle p;
        p.pos[0] = x; p.pos[1] = y; p.pos[2] = z;
        float angle = (rand() % 360) * 3.14159265f / 180.0f;
        float pitch = (rand() % 360) * 3.14159265f / 180.0f;
        float speed = 0.2f + (rand() % 100) * 0.01f;
        p.vel[0] = cos(angle) * sin(pitch) * speed;
        p.vel[1] = sin(angle) * sin(pitch) * speed;
        p.vel[2] = cos(pitch) * speed;
        p.color[0] = (rand() % 100) / 100.0f;
        p.color[1] = (rand() % 100) / 100.0f;
        p.color[2] = (rand() % 100) / 100.0f;
        p.color[3] = 1.0f;
        p.lifetime = 1.0f + (rand() % 100) * 0.02f;
        particles.push_back(p);
    }
}

void initGL() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * NUM_PARTICLES, NULL, GL_DYNAMIC_DRAW);

    GLuint vs = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    shaderProgram = createProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint posLoc = glGetAttribLocation(shaderProgram, "a_position");
    GLint velLoc = glGetAttribLocation(shaderProgram, "a_velocity");
    GLint colLoc = glGetAttribLocation(shaderProgram, "a_color");
    GLint lifeLoc = glGetAttribLocation(shaderProgram, "a_lifetime");

    glEnableVertexAttribArray(posLoc);
    glEnableVertexAttribArray(velLoc);
    glEnableVertexAttribArray(colLoc);
    glEnableVertexAttribArray(lifeLoc);

    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, pos));
    glVertexAttribPointer(velLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, vel));
    glVertexAttribPointer(colLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glVertexAttribPointer(lifeLoc, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void render(float time) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * particles.size(), particles.data());

    glUseProgram(shaderProgram);

    glm::mat4 mvp = projection * view * glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_mvp"), 1, GL_FALSE, &mvp[0][0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "u_time"), time);
    glUniform3f(glGetUniformLocation(shaderProgram, "u_gravity"), 0.0f, -0.5f, 0.0f);

    glDrawArrays(GL_POINTS, 0, particles.size());
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Particle Fireworks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    initGL();
    initParticles(0.0f, 0.0f, 0.0f);

    bool quit = false;
    float burstTime = 0.0f;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                float wx = (x / 400.0f - 1.0f) * 2.0f;
                float wy = (1.0f - y / 300.0f) * 2.0f;
                initParticles(wx, wy, 0.0f);
                burstTime = 0.0f;
            }
        }
        burstTime += 0.016f;
        render(burstTime);
        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

