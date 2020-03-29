// declare the include guard
#ifndef MODEL_HPP
#define MODEL_HPP

// declare libs
// opengl functions
#include <glad/glad.h>

// glm vectors, matrices etc
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// load image
#define STB_IMAGE_IMPLEMENTATION
#include <custom/stb_image.h>

// mesh shader
#include <custom/mesh.hpp>
#include <custom/shader.hpp>

// assimp model loading library
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

//
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// end declare libs

// function declarations
unsigned int loadTextureFromFile(const char *path, const std::string &directory,
                                 bool gamma = false);

// class declarations

class Model {
public:

  bool gammaCorrection;
  std::vector<Mesh> meshes;
  std::vector<Texture> loadedTextures;
  std::string directory;
  // constructor
  Model(const char* path, bool gamma = false) : gammaCorrection(gamma)
  { loadModel(path); }
  // functions
  void draw(Shader shader);

private:
  // model data
  // functions
  void loadModel(std::string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName);
};

// defining methods
void Model::draw(Shader shader) {
  for (unsigned int i = 0; i < this->meshes.size(); i++) {
    this->meshes[i].draw(shader);
  }
}

void Model::loadModel(std::string path) {
  // read the file with assimp
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                  aiProcess_CalcTangentSpace);
  /*
We first declare an actual Importer object from Assimp's namespace and then
call its ReadFile function. The function expects a file path and as its
second argument several post-processing options. Aside from simply loading
the file, Assimp allows us to specify several options that forces Assimp to
do some extra calculations/operations on the imported data. By setting
aiProcess_Triangulate we tell Assimp that if the model does not (entirely)
consist of triangles it should transform all the model's primitive shapes to
triangles. The aiProcess_FlipUVs flips the texture coordinates on the y-axis
where necessary during processing (you might remember from the Textures
tutorial that most images in OpenGL were reversed around the y-axis so this
little postprocessing option fixes that for us). A few other useful options
are:

  aiProcess_GenNormals : actually creates normals for each vertex if the
  model didn't contain normal vectors.
  aiProcess_SplitLargeMeshes : splits large meshes into smaller sub-meshes
  which is useful if your rendering has a maximum number of vertices allowed
  and can only process smaller meshes.
  aiProcess_OptimizeMeshes : actually does the reverse by trying to join
  several meshes into one larger mesh, reducing drawing calls for
  optimization.

Assimp provides a great set of postprocessing instructions and you can find
all of them here. Actually loading a model via Assimp is (as you can see)
surprisingly easy. The hard work lies in using the returned scene object to
translate the loaded data to an array of Mesh objects.
   */
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  }
  directory = path.substr(0, path.find_last_of('/'));

  // start processing from root node recursively
  this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // process the meshes of the given node on scene
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    this->meshes.push_back(this->processMesh(mesh, scene));
  }
  // now all meshes of this node has been processed
  // we should continue to meshes of child nodes
  for (unsigned int k = 0; k < node->mNumChildren; k++) {
    this->processNode(node->mChildren[k], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  // process meshes
  /*
Processing a mesh basically consists of 3 sections: retrieving all the vertex
data, retrieving the mesh's indices and finally retrieving the relevant
material data. The processed data is stored in one of the 3 vectors and from
those a Mesh is created and returned to the function's caller.
   */

  // data
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  // iteration on vertices of the mesh
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertice;
    // place holder
    glm::vec3 vec;
    vec.x = mesh->mVertices[i].x;
    vec.y = mesh->mVertices[i].y;
    vec.z = mesh->mVertices[i].z;
    // add to vertice
    vertice.position = vec;
    // normals
    vec.x = mesh->mNormals[i].x;
    vec.y = mesh->mNormals[i].y;
    vec.z = mesh->mNormals[i].z;
    vertice.normal = vec;

    // texture coordinates
    if (mesh->mTextureCoords[0]) // if it contains texture coordinates
    {
      glm::vec2 vect;
      vect.x = mesh->mTextureCoords[i]->x;
      vect.y = mesh->mTextureCoords[i]->y;
      vertice.TexCoords = vect;
    } else {
      vertice.TexCoords = glm::vec2(0.0f, 0.0f);
    }

    // now onto tangent
    vec.x = mesh->mTangents[i].x;
    vec.y = mesh->mTangents[i].y;
    vec.z = mesh->mTangents[i].z;
    vertice.Tangent = vec;

    // and finally bitangent
    vec.x = mesh->mBitangents[i].x;
    vec.y = mesh->mBitangents[i].y;
    vec.z = mesh->mBitangents[i].z;
    vertice.BiTangent = vec;
    vertices.push_back(vertice);
  }
  // vertice iteration done now we should deal with indices
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    /*
Assimp's interface defined each mesh having an array of faces where each face
represents a single primitive, which in our case (due to the
aiProcess_Triangulate option) are always triangles. A face contains the
indices that define which vertices we need to draw in what order for each
primitive so if we iterate over all the faces and store all the face's indices
in the indices vector we're all set
     */
    aiFace face = mesh->mFaces[i];
    for (unsigned int k = 0; k < face.mNumIndices; k++) {
      indices.push_back(face.mIndices[k]);
    }
    // now deal with materials
    if (mesh->mMaterialIndex >= 0) {
      aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
      /*
Just like with nodes, a mesh only contains an index to a material object and
to retrieve the actual material of a mesh we need to index the scene's
mMaterials array. The mesh's material index is set in its mMaterialIndex
property which we can also query to check if the mesh actually contains a
material or not
       */
      // we retrieve textures
      // 1. diffuse maps
      std::vector<Texture> diffuseMaps = this->loadMaterialTextures(
          material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
      // 2. specular maps
      std::vector<Texture> specularMaps = this->loadMaterialTextures(
          material, aiTextureType_SPECULAR, "texture_specular");
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
      // 3. normal maps
      std::vector<Texture> normalMaps = this->loadMaterialTextures(
          material, aiTextureType_HEIGHT, "texture_normal");
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

      // 4. height maps
      std::vector<Texture> heightMaps = this->loadMaterialTextures(
          material, aiTextureType_AMBIENT, "texture_height");
      textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
  }
  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 std::string typeName) {
  std::vector<Texture> texvec;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    const char *newTexPath = str.C_Str();
    // check if the texture is loaded before
    bool skipCheck = false;
    for (unsigned int j = 0; j < this->loadedTextures.size(); j++) {
      const char *pdata = this->loadedTextures[j].path.data();
      if (std::strcmp(pdata, newTexPath) == 0) {
        // texture with same path has already been loaded
        texvec.push_back(this->loadedTextures[j]);
        skipCheck = true;
        break;
      }
    }
    if (!skipCheck) { // texture is not loaded so let's load it
      Texture tex;
      tex.id = loadTextureFromFile(newTexPath, this->directory);
      tex.type = typeName;
      tex.path = newTexPath;
      texvec.push_back(tex);
      this->loadedTextures.push_back(tex);
    }
  }
  return texvec;
}

unsigned int loadTextureFromFile(const char *path, const std::string &directory,
                                 bool gamma) {
  std::string fname = std::string(path);
  fname = fname + '/' + directory;

  // generate texture
  unsigned int texId;
  glGenTextures(1, &texId);

  // load the image with stb image
  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(fname.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    switch (nrComponents) {
    case 1:
      format = GL_RED;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
    }
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }
  return texId;
}

#endif
