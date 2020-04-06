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

namespace filesystem = std::filesystem;

filesystem::path current_dir(filesystem::current_path());
filesystem::path mediaDir("media");
filesystem::path textureDir("textures");
filesystem::path shaderDir("shaders");
filesystem::path shaderDirPath = current_dir / mediaDir / shaderDir;
filesystem::path textureDirPath = current_dir / mediaDir / textureDir;

// initialization code

const unsigned int WINWIDTH = 800;
const unsigned int WINHEIGHT = 600;

void initializeGLFWMajorMinor(unsigned int maj, unsigned int min) {
  // initialize glfw version with correct profiling etc
  if (glfwInit() == 0) {
    std::cout << "glfw not initialized correctly" << std::endl;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, maj);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, min);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void framebuffer_size_callback(GLFWwindow *window, int newWidth,
                               int newHeight) {
  glViewport(0, 0, newWidth, newHeight);
}

void processInput_proc(GLFWwindow *window) {
  // process input to window
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    // user presses escape key and window closes
    glfwSetWindowShouldClose(window, true);
  } else if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) {
    // user presses semi colon polygon mode changes to foo
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void loadTexture2d_proc(const char *texturePath) {

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // use nearest neighbor interpolation when zooming out
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // use nearest neighbor interpolation when zooming in
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nbChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(texturePath, &width, &height, &nbChannels, 0);
  if (data) {
    if (nbChannels == 1) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else if (nbChannels == 3) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else if (nbChannels == 4) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
}

int main() {
  // let's initialize
  initializeGLFWMajorMinor(4, 2);

  // let's get that window going
  GLFWwindow *window;
  window = glfwCreateWindow(WINWIDTH, WINHEIGHT, "Texture Window", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to create a glfw window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // window resize
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // load glad
  if (gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)) == 0) {
    std::cout << "Failed to start glad" << std::endl;
    glfwTerminate();
    return -1;
  }

  // set default viewport
  glViewport(0, 0, WINWIDTH, WINHEIGHT); // viewport equal to window

  /*
  float textureCoords[] = {
      // texture coords are between 0 - 1
      0.0f, 0.0f, // bottom left
      1.0f, 0.0f, // bottom right
      0.5f, 1.0f // top middle
  };
  */

  // Texture coords
  float vertices[] = {
      // viewport position || colors           ||   texture coords
      1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };

  // let us load shaders
  filesystem::path fragFileName("texture.frag");
  filesystem::path fragPath = shaderDirPath / fragFileName;
  filesystem::path vertFileName("texture.vert");
  filesystem::path vertPath = shaderDirPath / vertFileName;
  Shader myShader(vertPath.c_str(), fragPath.c_str());

  // indices
  GLuint indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  // VAO, VBO, EBO related
  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao); // bind the array

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // let's add data to buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // let's add element array buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // let's specify the attributes

  // position attribute
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE,
      8 * sizeof(float), // stride value =: when does the next value comes
      // relating to this attribute
      (void *)0);
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
      (void *)(3 * sizeof(float)) // where does the first value related
      // to this attribute in the vertices array start
      );
  glEnableVertexAttribArray(1);

  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // ------ Load and Generate Texture ----------
  filesystem::path imname("Stone_001_Diffuse.png");
  filesystem::path impath = textureDirPath / imname;

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  loadTexture2d_proc(impath.c_str());

  // -------- main loop -----------
  while (glfwWindowShouldClose(window) == 0) {
    // process inputs
    processInput_proc(window);

    // render stuff
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // use shader program
    myShader.useProgram();

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // unbind array
    glBindVertexArray(0);

    // swap buffers etc
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);

  glfwTerminate();
  return 0;
}
