// simple texture viewing code
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <custom/camera.hpp>
#include <custom/shader.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <custom/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

fs::path current_dir = fs::current_path();
fs::path shaderDirPath = current_dir / "media" / "shaders";
fs::path textureDirPath = current_dir / "media" / "textures";

// initialization code

const unsigned int WINWIDTH = 800;
const unsigned int WINHEIGHT = 600;

// camera related

float lastX = WINWIDTH / 2.0f;
float lastY = WINHEIGHT / 2.0f;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 0.0f, 0.3f));

// time related
float deltaTime = 0.0f;
float lastTime = 0.0f;

// function declarations

static void glfwErrorCallBack(int id, const char *desc);
void initializeGLFWMajorMinor(unsigned int maj, unsigned int min);
void framebuffer_size_callback(GLFWwindow *window, int newWidth, int newHeight);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow *window, double xpos, double ypos);
GLuint loadTexture2d_proc(const char *texturePath);
void processInput_proc(GLFWwindow *window);

int main() {
  initializeGLFWMajorMinor(3, 3);
  GLFWwindow *window =
      glfwCreateWindow(WINWIDTH, WINHEIGHT, "Basic Texture", NULL, NULL);

  if (window == NULL) {
    std::cout << "Loading GLFW window had failed" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  //
  // dealing with mouse actions
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);

  // deal with input method
  // glfw should capture cursor movement as well
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // load opengl function
  if (gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)) == 0) {
    std::cout << "Failed to start glad" << std::endl;
    glfwTerminate();
    return -1;
  }

  // set default view port
  glViewport(0, 0, WINWIDTH, WINHEIGHT);

  // deal with global opengl state

  // load shader
  std::string vertFileName = "texture.vert";
  std::string fragFileName = "texture.frag";
  fs::path vertPath = shaderDirPath / vertFileName;
  fs::path fragPath = shaderDirPath / fragFileName;
  Shader textureShader(vertPath.c_str(), fragPath.c_str());

  float vertices[] = {
      // positions            // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  unsigned int vbo, vao, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0); // aPos
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1); // aTexCoord

  // deal with textures
  // rustediron2_basecolor.png
  // rustediron2_metallic.png
  // rustediron2_normal.png
  // rustediron2_roughness.png

  fs::path diffmapPath = textureDirPath / "rustediron2_basecolor.png";
  // fs::path diffmapPath = textureDirPath / "rustediron2_basecolor.png";

  GLuint diffuseMap = loadTexture2d_proc(diffmapPath.c_str());

  textureShader.useProgram();
  textureShader.setIntUni("texture1", 0);

  // render loop
  while (glfwWindowShouldClose(window) == 0) {

    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    textureShader.useProgram();
    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    // glDrawArrays(GL_TRIANGLES, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glfwTerminate();
  return 0;
}
void framebuffer_size_callback(GLFWwindow *window, int newWidth,
                               int newHeight) {
  glViewport(0, 0, newWidth, newHeight);
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xoffset;
  lastY = yoffset;

  camera.processMouseMovement(xoffset, yoffset);
}
void mouse_scroll_callback(GLFWwindow *window, double xpos, double ypos) {
  camera.processMouseScroll(ypos);
}
void processInput_proc(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.processKeyboard(FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.processKeyboard(LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.processKeyboard(BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.processKeyboard(RIGHT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
    camera.processKeyBoardRotate(LEFT, 0.7f);
  }
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
    camera.processKeyBoardRotate(RIGHT, 0.7f);
  }
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
    camera.processKeyBoardRotate(FORWARD, 0.7f);
  }
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    camera.processKeyBoardRotate(BACKWARD, 0.7f);
  }
}
static void glfwErrorCallBack(int id, const char *desc) {
  std::cout << desc << std::endl;
}
void initializeGLFWMajorMinor(unsigned int maj, unsigned int min) {
  // initialize glfw version with correct profiling etc
  glfwSetErrorCallback(glfwErrorCallBack);
  if (glfwInit() == 0) {
    std::cout << "glfw not initialized correctly" << std::endl;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, maj);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, min);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
GLuint loadTexture2d_proc(const char *texturePath) {
  // create and load, bind texture to gl
  unsigned int texture;
  glGenTextures(1, &texture);

  int width, height, nbChannels;
  unsigned char *data = stbi_load(texturePath, &width, &height, &nbChannels, 0);
  if (data) {
    GLenum format;
    if (nbChannels == 1) {
      format = GL_RED;
    } else if (nbChannels == 3) {
      format = GL_RGB;
    } else if (nbChannels == 4) {
      format = GL_RGBA;
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return texture;
}
