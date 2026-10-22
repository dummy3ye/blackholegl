#include "bitmap_font.h"
#include "shader.hpp"
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// global states
int windowWidth = 1280;
int windowHeight = 720;
int renderWidth = 1280;
int renderHeight = 720;

// Quality
// 0 - Very Low
// 1 - Low
// 2 - Medium
// 3 - High
int graphicsQuality = 2;

float camDistance = 16.0f;
float camYaw = 0.0f;
float camPitch = 0.16f;
double lastMouseX = 0.0;
double lastMouseY = 0.0;
bool isDragging = false;
bool showFPS = false;

const char *getQualityName(int q) {
  switch (q) {
  case 0:
    return "Very Low";
  case 1:
    return "Low";
  case 2:
    return "Medium";
  case 3:
    return "High";
  default:
    return "Custom";
  }
}

// vec3 helpers
struct Vec3 {
  float x, y, z;
};

static Vec3 vnorm(const Vec3 &v) {
  float l = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (l < 1e-6f)
    return {0, 0, 0};
  return {v.x / l, v.y / l, v.z / l};
}
static Vec3 vcross(const Vec3 &a, const Vec3 &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

// glfw callbacks
void mouseButtonCB(GLFWwindow *, int button, int action, int) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      isDragging = true;
      glfwGetCursorPos(glfwGetCurrentContext(), &lastMouseX, &lastMouseY);
    }
    if (action == GLFW_RELEASE) {
      isDragging = false;
    }
  }
}
void cursorPosCB(GLFWwindow *, double xpos, double ypos) {
  if (!isDragging)
    return;
  camYaw += float(xpos - lastMouseX) * 0.005f;
  camPitch += float(ypos - lastMouseY) * 0.005f;
  camPitch = std::clamp(camPitch, -1.52f, 1.52f);
  lastMouseX = xpos;
  lastMouseY = ypos;
}
void scrollCB(GLFWwindow *, double, double yoff) {
  camDistance = std::clamp(camDistance - float(yoff), 3.5f, 60.0f);
}
void fbSizeCB(GLFWwindow *, int w, int h) {
  if (w > 0 && h > 0) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
  }
}

// cli
static void printHelp() {
  std::cout << "Usage: blackholegl [OPTIONS]\n"
               "\n"
               "a blackhole simulation written in cpp and OpenGL.\n"
               "\n"
               "Options:\n"
               "  --res=<int>, -r <int>     Render height (default 720, range "
               "120-4320)\n"
               "  --quality=<name|int>      very-low (0), low (1), medium (2), "
               "high (3)\n"
               "  --show-fps                 Show real-time FPS / GPU overlay\n"
               "  --very-low, -vl            Preset: quality=0, res=360p\n"
               "  --help, -h                 Show this message and exit\n"
               "  --version                  Show version and exit\n"
               "\n"
               "controls:\n"
               "  mouse drag      camera movement\n"
               "  scroll          zoom | capped at 3.5M - 60.0M\n"
               "  0-5             cycle resolution (240p-1440p)\n"
               "  q               cycle quality\n"
               "  f               toggle overlay\n"
               "  esc             exit\n";
}

static void printVersion() { std::cout << "blackholegl 1.0.0\n"; }

static int parseArgs(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      printHelp();
      return 2; // exit 0 handled by caller
    } else if (arg == "--version" || arg == "-v") {
      printVersion();
      return 2;
    } else if (arg == "--show-fps") {
      showFPS = true;
    } else if (arg.rfind("--res=", 0) == 0) {
      try {
        int r = std::stoi(arg.substr(6));
        if (r < 120 || r > 4320)
          throw std::out_of_range("out of range");
        renderHeight = r;
        renderWidth = int(std::round(r * 16.0 / 9.0));
      } catch (...) {
        std::cerr
            << "blackholegl: error: --res requires integer in [120, 4320]\n";
        return 1;
      }
    } else if (arg == "-r" || arg == "--res") {
      if (i + 1 >= argc) {
        std::cerr << "blackholegl: error: " << arg << " requires a value\n";
        return 1;
      }
      try {
        int r = std::stoi(argv[++i]);
        if (r < 120 || r > 4320)
          throw std::out_of_range("out of range");
        renderHeight = r;
        renderWidth = int(std::round(r * 16.0 / 9.0));
      } catch (...) {
        std::cerr
            << "blackholegl: error: --res requires integer in [120, 4320]\n";
        return 1;
      }
    } else if (arg.rfind("--quality=", 0) == 0) {
      std::string qStr = arg.substr(10);
      if (qStr == "very-low" || qStr == "0")
        graphicsQuality = 0;
      else if (qStr == "low" || qStr == "1")
        graphicsQuality = 1;
      else if (qStr == "medium" || qStr == "2")
        graphicsQuality = 2;
      else if (qStr == "high" || qStr == "3")
        graphicsQuality = 3;
      else {
        std::cerr << "blackholegl: error: unknown quality '" << qStr
                  << "'. Expected: very-low/low/medium/high\n";
        return 1;
      }
    } else if (arg == "--very-low" || arg == "-vl") {
      graphicsQuality = 0;
      renderHeight = 360;
      renderWidth = 640;
    } else {
      std::cerr << "blackholegl: error: unknown argument '" << arg << "'\n";
      std::cerr << "Run 'blackholegl --help' for usage.\n";
      return 1;
    }
  }
  return 0;
}

// main
static std::string shaderPath(const std::string &file) {
  std::ifstream f("shaders/" + file);
  if (f.good())
    return "shaders/" + file;
  f.open("/usr/local/share/blackholegl/shaders/" + file);
  if (f.good())
    return "/usr/local/share/blackholegl/shaders/" + file;
  return "shaders/" + file;
}

int main(int argc, char **argv) {
  int parseCode = parseArgs(argc, argv);
  if (parseCode == 2)
    return 0;
  if (parseCode == 1)
    return 1;

  // GLFW + GLAD init
  if (!glfwInit()) {
    std::cerr << "GLFW init failed\n";
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  std::string title = "blackholegl (" + std::to_string(renderWidth) + "x" +
                      std::to_string(renderHeight) + ") [" +
                      getQualityName(graphicsQuality) + "]";
  GLFWwindow *win = glfwCreateWindow(windowWidth, windowHeight, title.c_str(),
                                     nullptr, nullptr);
  if (!win) {
    std::cerr << "Window creation failed\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);

  int glv = gladLoadGL(glfwGetProcAddress);
  if (!glv) {
    std::cerr << "GLAD init failed\n";
    return -1;
  }

  const char *renderer = (const char *)glGetString(GL_RENDERER);
  const char *glVerStr = (const char *)glGetString(GL_VERSION);
  std::cout << "GPU: " << renderer << "\n"
            << "OpenGL: " << glVerStr << "\n"
            << "Render: " << renderWidth << "x" << renderHeight << "\n"
            << "Quality: " << getQualityName(graphicsQuality) << "\n"
            << "Controls: Drag=Orbit  Scroll=Zoom  [0-5]=Res  [Q]=Toggle "
               "Quality  [F]=FPS  ESC=Quit\n";
  if (showFPS)
    std::cout << "FPS overlay enabled (--show-fps)\n";
  std::cout << std::endl;

  glfwSetMouseButtonCallback(win, mouseButtonCB);
  glfwSetCursorPosCallback(win, cursorPosCB);
  glfwSetScrollCallback(win, scrollCB);
  glfwSetFramebufferSizeCallback(win, fbSizeCB);

  // shaders
  Shader computeShader(shaderPath("raymarch.comp"));
  Shader screenShader(shaderPath("quad.vert"), shaderPath("quad.frag"));
  // overlay shader
  Shader overlayShader(shaderPath("overlay.vert"), shaderPath("overlay.frag"));
  GLuint overlayVAO, overlayVBO;
  glGenVertexArrays(1, &overlayVAO);
  glGenBuffers(1, &overlayVBO);
  glBindVertexArray(overlayVAO);
  glBindBuffer(GL_ARRAY_BUFFER, overlayVBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
  glBindVertexArray(0);

  // screen quad
  float qv[] = {-1, 1, 0, 1, -1, -1, 0, 0, 1, -1, 1, 0,
                -1, 1, 0, 1, 1,  -1, 1, 0, 1, 1,  1, 1};
  GLuint qVAO, qVBO;
  glGenVertexArrays(1, &qVAO);
  glGenBuffers(1, &qVBO);
  glBindVertexArray(qVAO);
  glBindBuffer(GL_ARRAY_BUFFER, qVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(qv), qv, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // textures
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, renderWidth, renderHeight, 0,
               GL_RGBA, GL_FLOAT, nullptr);

  int prevRW = renderWidth, prevRH = renderHeight;
  double fpsTimer = glfwGetTime();
  int frameCnt = 0;
  float fps = 0.0f;
  std::vector<float> textVerts;

  // Render
  while (!glfwWindowShouldClose(win)) {
    glfwPollEvents();
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(win, true);

    // --show-fps toggle
    static bool fWasPressed = false;
    bool fNow = glfwGetKey(win, GLFW_KEY_F) == GLFW_PRESS;
    if (fNow && !fWasPressed)
      showFPS = !showFPS;
    fWasPressed = fNow;

    // Toggle graphics quality ==> Q
    // Very Low
    // Low
    // Medium
    // High
    // Very Low

    static bool qWasPressed = false;
    bool qNow = glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS;
    if (qNow && !qWasPressed) {
      graphicsQuality = (graphicsQuality + 1) % 4;
      std::cout << "Switched Quality: " << getQualityName(graphicsQuality)
                << std::endl;
    }
    qWasPressed = qNow;

    // Resolution hot-keys
    // 0=240p Ultra Low
    // 1=360p,
    // 2=480p,
    // 3=720p,
    // 4=1080p,
    // 5=1440p

    int nr = 0;
    if (glfwGetKey(win, GLFW_KEY_0) == GLFW_PRESS)
      nr = 240;
    else if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS)
      nr = 360;
    else if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS)
      nr = 480;
    else if (glfwGetKey(win, GLFW_KEY_3) == GLFW_PRESS)
      nr = 720;
    else if (glfwGetKey(win, GLFW_KEY_4) == GLFW_PRESS)
      nr = 1080;
    else if (glfwGetKey(win, GLFW_KEY_5) == GLFW_PRESS)
      nr = 1440;
    if (nr && nr != renderHeight) {
      renderHeight = nr;
      renderWidth = int(std::round(nr * 16.0 / 9.0));
    }

    // resize compute texture when needed
    if (renderWidth != prevRW || renderHeight != prevRH) {
      glBindTexture(GL_TEXTURE_2D, tex);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, renderWidth, renderHeight, 0,
                   GL_RGBA, GL_FLOAT, nullptr);
      prevRW = renderWidth;
      prevRH = renderHeight;
      title = "blackholegl (" + std::to_string(renderWidth) + "x" +
              std::to_string(renderHeight) + ") [" +
              getQualityName(graphicsQuality) + "]";
      glfwSetWindowTitle(win, title.c_str());
    }

    // Camera
    float t = float(glfwGetTime());
    Vec3 cp = {camDistance * std::cos(camPitch) * std::sin(camYaw),
               camDistance * std::sin(camPitch),
               camDistance * std::cos(camPitch) * std::cos(camYaw)};
    Vec3 fwd = vnorm({-cp.x, -cp.y, -cp.z});
    Vec3 rt = vnorm(vcross(fwd, {0, 1, 0}));
    Vec3 up = vcross(rt, fwd);

    // Dispatch compute
    computeShader.use();
    computeShader.setVec2("resolution", float(renderWidth),
                          float(renderHeight));
    computeShader.setVec3("camPos", cp.x, cp.y, cp.z);
    computeShader.setVec3("camForward", fwd.x, fwd.y, fwd.z);
    computeShader.setVec3("camRight", rt.x, rt.y, rt.z);
    computeShader.setVec3("camUp", up.x, up.y, up.z);
    computeShader.setFloat("fov", 0.95f);
    computeShader.setFloat("time", t);
    computeShader.setInt("quality", graphicsQuality);

    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute((renderWidth + 15) / 16, (renderHeight + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // Draw fullscreen quad
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    screenShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    screenShader.setInt("screenTexture", 0);
    glBindVertexArray(qVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // FPS measurement
    ++frameCnt;
    double now = glfwGetTime();
    if (now - fpsTimer >= 0.5) {
      fps = float(frameCnt / (now - fpsTimer));
      frameCnt = 0;
      fpsTimer = now;
    }

    // fps and gpu info overlay
    if (showFPS) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      overlayShader.use();

      char line[256];
      std::snprintf(line, sizeof(line),
                    "FPS: %d\n"
                    "GPU: %s\n"
                    "GL:  %s\n"
                    "Res: %dx%d\n"
                    "Quality: %s",
                    int(std::round(fps)), renderer ? renderer : "?",
                    glVerStr ? glVerStr : "?", renderWidth, renderHeight,
                    getQualityName(graphicsQuality));

      float scale = 2.5f;
      float padX = 10.0f, padY = 10.0f;
      int numVerts = bitmapfont::generateTextVertices(
          padX, padY, line, scale, windowWidth, windowHeight, textVerts);

      // Draw translucent dark background behind text
      {
        int maxCols = 0, cols = 0;
        for (const char *c = line; *c; ++c) {
          if (*c == '\n') {
            if (cols > maxCols)
              maxCols = cols;
            cols = 0;
          } else
            ++cols;
        }
        if (cols > maxCols)
          maxCols = cols;
        int lineCount = 1;
        for (const char *c = line; *c; ++c)
          if (*c == '\n')
            ++lineCount;

        float bgW = padX + maxCols * 6.0f * scale + padX;
        float bgH = padY + lineCount * 9.0f * scale + padY * 0.5f;

        auto px2ndcX = [&](float px) {
          return (px / windowWidth) * 2.0f - 1.0f;
        };
        auto px2ndcY = [&](float py) {
          return (1.0f - py / windowHeight) * 2.0f - 1.0f;
        };

        float bx0 = px2ndcX(0), by0 = px2ndcY(0);
        float bx1 = px2ndcX(bgW), by1 = px2ndcY(bgH);
        float bgQuad[] = {bx0, by0, bx1, by0, bx0, by1,
                          bx1, by0, bx1, by1, bx0, by1};
        overlayShader.setInt("mode", 0); // dark bg mode
        glBindVertexArray(overlayVAO);
        glBindBuffer(GL_ARRAY_BUFFER, overlayVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bgQuad), bgQuad, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
      }

      // Draw text glyphs
      if (numVerts > 0) {
        overlayShader.setInt("mode", 1); // white text mode
        glBindVertexArray(overlayVAO);
        glBindBuffer(GL_ARRAY_BUFFER, overlayVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     GLsizeiptr(textVerts.size() * sizeof(float)),
                     textVerts.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
      }

      glDisable(GL_BLEND);
    }

    glfwSwapBuffers(win);
  }

  glDeleteVertexArrays(1, &overlayVAO);
  glDeleteBuffers(1, &overlayVBO);
  glDeleteVertexArrays(1, &qVAO);
  glDeleteBuffers(1, &qVBO);
  glDeleteTextures(1, &tex);
  glfwTerminate();
  return 0;
}
