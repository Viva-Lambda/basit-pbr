// author: Kaan Eraslan
// license: see, LICENSE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

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
GLuint loadTexture2d_proc(const char *texturePath, const char *textType);
void processInput_proc(GLFWwindow *window);
void cubeShaderInit_proc(Shader cubeShader);
void renderSurface(glm::vec3 pos[4], glm::vec2 texpos[4], glm::vec3 snormal);

int main() {
  initializeGLFWMajorMinor(4, 2);
  GLFWwindow *window = glfwCreateWindow(WINWIDTH, WINHEIGHT,
                                        "Basic Color With Cubes", NULL, NULL);

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
  glEnable(GL_DEPTH_TEST);

  // deal with textures
  // rustediron2_basecolor.png
  // rustediron2_metallic.png
  // rustediron2_normal.png
  // rustediron2_roughness.png

  fs::path diffmapPath = textureDirPath / "rustediron2_basecolor.png";
  fs::path specularMapPath = textureDirPath / "rustediron2_metallic.png";
  fs::path normalMapPath = textureDirPath / "rustediron2_normal.png";

  GLuint diffuseMap = loadTexture2d_proc(diffmapPath.c_str(), "PNG");
  GLuint specularMap = loadTexture2d_proc(specularMapPath.c_str(), "PNG");
  GLuint normalMap = loadTexture2d_proc(normalMapPath.c_str(), "PNG");

  // load shaders
  // cube shader
  std::string vertFileName = "phong.vert";
  std::string fragFileName = "phong.frag";
  fs::path vertPath = shaderDirPath / vertFileName;
  fs::path fragPath = shaderDirPath / fragFileName;
  Shader cubeShader(vertPath.c_str(), fragPath.c_str());

  // lamp shader
  fs::path frag2FileName("basic_color_light.frag");
  fs::path frag2Path = shaderDirPath / frag2FileName;
  Shader lampShader(vertPath.c_str(), frag2Path.c_str());

  // let's set up some uniforms

  // cube shader
  glm::vec3 lightPos(0.2f, 1.0f, 0.5f);
  //

  cubeShader.useProgram();
  // init proc for uniforms that don't change over rendering
  cubeShaderInit_proc(cubeShader);

  // set up vertex data
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

  // let's deal with vertex array objects and buffers
  GLuint vao, vbo, lightVao;
  // changes
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  //
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // add data to buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // set attributes
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // deal with lamp vertex array objects
  glGenVertexArrays(1, &lightVao); // separate object to isolate lamp from
  glBindVertexArray(lightVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // render loop
  while (glfwWindowShouldClose(window) == 0) {
    float currentTime = (float)glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    processInput_proc(window);
    glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setting model, view, projection

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.zoom),
                         (float)WINWIDTH / (float)WINHEIGHT, 0.1f, 100.0f);
    glm::mat4 viewMat = camera.getViewMatrix();

    // render cube object
    glm::mat4 cubeModel(1.0f);
    // float angle = 20.0f;
    cubeShader.useProgram();
    cubeShader.setMat4Uni("view", viewMat);
    cubeShader.setMat4Uni("model", cubeModel);
    cubeShader.setMat4Uni("projection", projection);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // unbind the light vertex array object
    glBindVertexArray(0);
    //
    glm::mat4 lampModel(1.0f);
    lampModel = glm::translate(lampModel, lightPos);
    lampModel = glm::scale(lampModel, glm::vec3(0.2f));
    lampShader.useProgram();
    lampShader.setMat4Uni("model", lampModel);
    lampShader.setMat4Uni("projection", projection);
    lampShader.setMat4Uni("view", viewMat);
    // render lamp
    glBindVertexArray(lightVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // unbind the light vertex array object
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteVertexArrays(1, &lightVao);
  glDeleteBuffers(1, &vbo);
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

GLuint loadTexture2d_proc(const char *texturePath, const char *textType) {
  // create and load, bind texture to gl
  std::string ttype(textType);
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // use nearest neighbor interpolation when zooming out
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // use nearest neighbor interpolation when zooming out
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nbChannels;
  unsigned char *data = stbi_load(texturePath, &width, &height, &nbChannels, 0);
  if (data) {
    if (ttype == "JPG") {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else if (ttype == "PNG") {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return texture;
}
void cubeShaderInit_proc(cubeShader) {
  float ambientCoeff = 0.1f;
  float shininess = 32.0f;
  glm::vec3 attc(1.0f, 0.0f, 0.0f);
  cubeShader.setFloatUni("ambientCoeff", ambientCoeff);
  cubeShader.setFloatUni("shininess", shininess);
  cubeShader.setVec3Uni("attC", attc);
}
unsigned int surfaceVAO = 0;
unsigned int surfaceVBO;
void renderSurface(glm::vec3 pos[4], glm::vec2 texpos[4], glm::vec3 snormal) {
  // render surface for the given context

  if (surfaceVAO == 0) {

    // square surface position
    glm::vec3 p1 = pos[0];
    glm::vec3 p2 = pos[1];
    glm::vec3 p3 = pos[2];
    glm::vec3 p4 = pos[3];

    // square texture position
    glm::vec3 tex1 = texpos[0];
    glm::vec3 tex2 = texpos[1];
    glm::vec3 tex3 = texpos[2];
    glm::vec3 tex4 = texpos[3];
    // there are two triangles in a square

    // first triangle
    glm::vec3 edge1 = p2 - p1;
    glm::vec3 edge2 = p3 - p1;
    glm::vec2 deltaUV1 = tex2 - tex1;
    glm::vec2 deltaUV2 = tex3 - tex1;
    GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    // computing tangent
    glm::vec3 tangent1;
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    // computing bitangent
    glm::vec3 bitangent1;
    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    // second triangle
    edge1 = p3 - p1;
    edge2 = p4 - p1;
    deltaUV1 = tex3 - tex1;
    deltaUV2 = tex4 - tex1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent2;
    glm::vec3 bitangent2;

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = glm::normalize(tangent2);

    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = glm::normalize(bitangent2);

    float vertices[] = {
        // first triangle
        p1.x, p1.y, p1.z, snormal.x, snormal.y, snormal.z, tex1.x, tex1.y,
        tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y,
        bitangent1.z, p1.x, p2.y, p2.z, snormal.x, snormal.y, snormal.z, tex2.x,
        tex2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y,
        bitangent1.z, p3.x, p3.y, p3.z, snormal.x, snormal.y, snormal.z, tex3.x,
        tex3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y,
        bitangent1.z,

        // second triangle
        p1.x, p1.y, p1.z, snormal.x, snormal.y, snormal.z, tex1.x, tex1.y,
        tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y,
        bitangent2.z, p3.x, p3.y, p3.z, snormal.x, snormal.y, snormal.z, tex3.x,
        tex3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y,
        bitangent2.z, p4.x, p4.y, p4.z, snormal.x, snormal.y, snormal.z, tex4.x,
        tex4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y,
        bitangent2.z,
    };
    // vao vbo etc
    glGenVertexArrays(1, &surfaceVAO);
    glGenBuffers(1, &surfaceVBO);
    glBindVertexArray(surfaceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // location
    // specify attributes
    glVertexAttribPointer(0, // location ==  aPos
                          3, // vec3
                          GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1); // location
    glVertexAttribPointer(1,      // location ==  aNormal
                          3,      // vec3
                          GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); // location
    glVertexAttribPointer(2,      // location ==  aTexCoord
                          2,      // vec2
                          GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(3); // location
    glVertexAttribPointer(3,      // location ==  aTan
                          3,      // vec3
                          GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void *)(8 * sizeof(float)));
    glEnableVertexAttribArray(4); // location
    glVertexAttribPointer(4,      // location ==  aTan
                          3,      // vec3
                          GL_FLOAT, GL_FALSE, 14 * sizeof(float),
                          (void *)(11 * sizeof(float)));
  };
  glBindVertexArray(surfaceVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
