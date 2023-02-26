#include <iostream>         // cout, cerr
#include <cstdlib> // EXIT_FAILURE

#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions


// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <sstream>


using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Pyramid Assignment"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh

        GLuint cylinderVAO;
        GLuint cylinderVBO;
        GLuint cylinderEBO;

        GLuint cubeVAO;
        GLuint cubeVBOs[2];
        GLuint cubeEBO;

        GLuint sphereVAO;
        GLuint sphereVBO;
        GLuint sphereEBO;

        GLuint cupVAO;
        GLuint cupVBO;
        GLuint cupEBO;
        GLuint cupIndices;

        GLuint airpodVAO;
        GLuint airpodVBO;
        GLuint airpodEBO;
        GLuint airpodIndices;


    };

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    
    

    // Texture
    GLuint gTextureId;
    GLuint gTextureId2;
    GLuint gTextureId3;
    GLuint gTextureId4;
    GLuint gTextureId5;
    GLuint gTextureId6;
    GLuint gTextureId7;
    glm::vec2 gUVScale(5.0f, 5.0f);
    glm::vec2 gUVScale2(5.0f, 5.0f);
    glm::vec2 gUVScale3(5.0f, 5.0f);
    GLint gTexWrapMode = GL_REPEAT;


    // Shader program
    GLuint gProgramId;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void createCylinder(int slices, float radius, float height, std::vector<float>& vertices, std::vector<unsigned int>& indices);
void UCreatCylinderMesh(GLMesh& mesh);
void UCreateCube(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void UCreateSphereMesh(GLMesh& mesh);
void UCreateSphere(float radius, int numSlices, int numStacks, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texCoords, std::vector<unsigned int>& indices);
void UCreateCup(std::vector <glm::vec3 >& vertices, std::vector<unsigned int>& indices);
void UCreateCupMesh(GLMesh& mesh);

void UCreateAirpodMesh(GLMesh& mesh);


//float radius = 10.0f;
float camX = 0;
float camY = 0;
float camZ = -15;
const float PI = 3.14159265358979323846;

float yaw = 0.0f;
float pitch = 0.0f;
float sensitivity = 0.05f;
float cameraSpeed = 0.1f;
bool isOrtho = false;
glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

std::vector<float> cylinderVertices;
std::vector<unsigned int> cylinderIndices;

std::vector<glm::vec3> sphereVertices;
std::vector<unsigned int> sphereIndices;
std::vector<glm::vec3> sphereNormals;
std::vector<glm::vec2> sphereTexCoords;

std::vector<Vertex> cupVertices;
std::vector<glm::vec3> cupNormals;
std::vector<glm::vec2> cupTexCoords;
std::vector<unsigned int> cupIndices;


// Define the number of segments used to create the cup shape.
const int numSegments = 20;

// Define the radius of the cup base and height.
const float radius = 0.5f;
const float height = 1.0f;





// camera vecs
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Lighting
glm::vec3 lightPosition = glm::vec3(10.0, 3.0, -5.0);
glm::vec3 diffuseColor = glm::vec3(0.5, 0.5, 0.5);
glm::vec3 ambientColor = glm::vec3(0.2, 0.2, 0.2);


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position;
layout(location = 3) in vec3 normal;
layout(location = 2) in vec2 textureCoordinate;


out vec2 vertexTextureCoordinate;
out vec3 vertexNormal;
out vec3 vertexPosition;

//Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelCylinder;
uniform int shapeType;
uniform mat4 modelCube;
uniform mat4 modelSphere;
uniform mat4 modelCup;
uniform mat4 modelAirpod;


void main()
{

    //conditional on what model matrix to use
    if (shapeType == 0)
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates 
    }

    

    if (shapeType == 1)
    {
        gl_Position = projection * view * modelCylinder * vec4(position, 1.0f); // transforms vertices to clip coordinates 
    }

    if (shapeType == 2)
    {
        gl_Position = projection * view * modelCube * vec4(position, 1.0f);
    }
    if (shapeType == 3)
    {
        vec4 transformedModelMatrix = projection * view * modelSphere * vec4(position, 1.0f);

        gl_Position = transformedModelMatrix;

    }
    if (shapeType == 4)
    {
        gl_Position = projection * view * modelCup * vec4(position, 1.0f);
    }
    if (shapeType == 5)
    {
        gl_Position = projection * view * modelAirpod * vec4(position, 1.0f);
    }


    

    vertexTextureCoordinate = textureCoordinate;

    vertexNormal = normal;

    vertexPosition = position;
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec2 vertexTextureCoordinate;
in vec3 vertexNormal;
in vec3 vertexPosition;


out vec4 fragmentColor;


uniform sampler2D uTexture;
uniform sampler2D uTexture2;
uniform sampler2D uTexture3;
uniform sampler2D uTexture4;
uniform sampler2D uTexture5;
uniform sampler2D uTexture6;
uniform sampler2D uTexture7;


uniform vec2 uvScale;
uniform vec2 uvScale2;
uniform vec2 uvScale3;
uniform int shapeType;
uniform vec3 lightPosition;
uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform float shininess;



void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    if (shapeType == 0)
    {
        color = texture(uTexture4, vertexTextureCoordinate * uvScale * 5);
    }
    else if (shapeType == 1)
    {
        color = texture(uTexture2, vertexTextureCoordinate * uvScale2 * 100.0);
    }
    else if (shapeType == 2)
    {
        vec4 firstColor = texture(uTexture3, vertexTextureCoordinate * uvScale / 5.0);
        vec4 secondColor = texture(uTexture, vertexTextureCoordinate * uvScale / 5.0);
        
        vec4 blended = mix(firstColor, secondColor, 0.5);

        color = blended;

    }
    else if (shapeType == 3)
    {
        color = texture(uTexture5, vertexTextureCoordinate * uvScale3 / 10.0);
    }
    else if (shapeType == 4)
    {
        color = texture(uTexture6, vertexTextureCoordinate * uvScale / 4.0);
    }

    else if (shapeType == 5)
    {
        color = texture(uTexture7, vertexTextureCoordinate * uvScale / 4.0);
    }

   
    vec3 norm = normalize(vertexNormal) * -1.0;
    // Simulate a lamp positioned at (0, 1, 1) in the room
    vec3 lampPosition = vec3(0.0, 2.0, 4.0);
    vec3 light = normalize(lampPosition - vertexPosition);

    float diffuse = max(dot(norm, light), 0.0);
    vec3 diffuseLight = diffuseColor * diffuse;

    vec3 view = normalize(-vertexPosition);
    vec3 reflection = reflect(-light, norm);
    float specular = pow(max(dot(reflection, view), 0.0), shininess);
    vec3 specularLight = specularColor * specular;

    vec3 ambientLight = ambientColor * vec3(2.5, 2.5, 2.5);

    vec3 lighting = diffuseLight + ambientLight + specularLight;

    fragmentColor = color * vec4(lighting, 1.0);

    
    
    
    

    
}
);

    


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        static bool perspective = true;
        perspective = !perspective;
        if (perspective)
        {
            projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        }
        else
        {
            projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh

    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object


    /* UCreateCube(gMesh);*/
    UCreatCylinderMesh(gMesh);
    UCreateCube(gMesh);
    UCreateSphereMesh(gMesh);
    UCreateCupMesh(gMesh);
    UCreateAirpodMesh(gMesh);
    




    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;
    
    
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);
    // set to 1
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture2"), 1);
    // set to 2
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture3"), 2);
    
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture4"), 3);

    glUniform1i(glGetUniformLocation(gProgramId, "uTexture5"), 4);

    glUniform1i(glGetUniformLocation(gProgramId, "uTexture6"), 5);

    glUniform1i(glGetUniformLocation(gProgramId, "uTexture7"), 6);
    
    
    
    
    // Load texture after create shader program
    const char* texFilename = "Chrome.jpg";
    if (!UCreateTexture(texFilename, gTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename2 = "MatteBlack.png";
    if (!UCreateTexture(texFilename2, gTextureId2))
    {
        cout << "Failed to load texture " << texFilename2 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename3 = "ShinyBlack.jpg";
    if (!UCreateTexture(texFilename3, gTextureId3))
    {
        cout << "Failed to load texture " << texFilename3 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename4 = "PlaneTexture.jpg";
    if (!UCreateTexture(texFilename4, gTextureId4))
    {
        cout << "Failed to load texture " << texFilename4 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename5 = "Watermelon2.jpg";
    if (!UCreateTexture(texFilename5, gTextureId5))
    {
        cout << "Failed to load texture " << texFilename5 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename6 = "GlassTexture2.jpg";
    if (!UCreateTexture(texFilename6, gTextureId6))
    {
        cout << "Failed to load texture " << texFilename6 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename7 = "AirpodTexture.jpg";
    if (!UCreateTexture(texFilename7, gTextureId7))
    {
        cout << "Failed to load texture " << texFilename6 << endl;
        return EXIT_FAILURE;
    }


    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // input
        // -----
        UProcessInput(gWindow);
        



        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);
    

    

    // Release shader program
    UDestroyShaderProgram(gProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    

    glfwSetCursorPosCallback(*window, mouseCallback);
    glfwSetScrollCallback(*window, scrollCallback); // set the scrollCallback as the scroll callback function
    glfwSetKeyCallback(*window, key_callback);

    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    


}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    static float lastX = 400, lastY = 300;
    static bool firstMouse = true;

    if (firstMouse == true) // set lastXand lastY to current mouse position
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // update lastX and lastY to the current mouse position
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // sensitivity

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // limit pitch to prevent camera flip
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // calculate x,y,z component
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);





}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) // this function will be called every time the mouse scroll wheel moves
{
    cameraSpeed += yoffset * 0.1; // add yoffset * 0.1 to cameraSpeed to control the movement speed
    if (cameraSpeed < 0.1) // limit cameraSpeed to 0.1, to prevent the movement from becoming too slow
        cameraSpeed = 0.1;
}



// Functioned called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_CULL_FACE);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 1. Scales the object by 10
    glm::mat4 scale = glm::scale(glm::vec3(20.0f, 10.0f, 10.0f));
    // Rotated object to show 3D Aspects of Pyramid
    glm::mat4 rotation = glm::rotate(180.0f, glm::vec3(0.80f, 1.0f, 1.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Transforms the camera: move the camera back (z axis)
    //glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -5.0f));
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

   
    // Creates a perspective projection
    
    
    /*glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);*/
    

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");
    glUniform1i(glGetUniformLocation(gProgramId, "shapeType"), 0);

    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPosition");
    GLint diffuseColorLoc = glGetUniformLocation(gProgramId, "diffuseColor");
    GLint ambientColorLoc = glGetUniformLocation(gProgramId, "ambientColor");
    
    glUniform3f(glGetUniformLocation(gProgramId, "specularColor"), 1.0, 1.0, 1.0);
    glUniform1f(glGetUniformLocation(gProgramId, "shininess"), 2.0);
    glUniform3fv(lightPositionLoc, 1, glm::value_ptr(lightPosition));
    glUniform3fv(diffuseColorLoc, 1, glm::value_ptr(diffuseColor));
    glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColor));
    
    
   
    

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
    


    

    

    

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.vao);

   
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gTextureId4);

    
    

    // Draws the triangles
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
    
    




    // transformations for modelCylinder
    glm::mat4 scaleCylinder = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f)); // 1. Scale the cylinder
    glm::mat4 rotationCylinder = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f)); // 2. Rotate the cylinder
    glm::mat4 translationCylinder = glm::translate(glm::vec3(-0.50f, 2.50f, -1.10));
    glm::mat4 modelCylinder = translationCylinder * rotationCylinder * scaleCylinder; // Apply the transformations
    glUniform1i(glGetUniformLocation(gProgramId, "shapeType"), 1);
    // Pass the modelCylinder matrix to the Shader program
    GLint modelCylinderLoc = glGetUniformLocation(gProgramId, "modelCylinder");




    /*glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));*/
    glUniformMatrix4fv(modelCylinderLoc, 1, GL_FALSE, glm::value_ptr(modelCylinder));

    

    // Bind the cylinder's VAO and draw the cylinder
    glBindVertexArray(gMesh.cylinderVAO);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureId2);

    

   

    
    
    glDrawElements(GL_TRIANGLES, cylinderIndices.size(), GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    

    glm::mat4 scaleCube = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
    glm::mat4 rotationCube = glm::rotate(240.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translationCube = glm::translate(glm::vec3(0.0f, 1.01f, -0.90));
    glm::mat4 modelCube = translationCube * rotationCube * scaleCube;
    glUniform1i(glGetUniformLocation(gProgramId, "shapeType"), 2);
    // Pass the modelCylinder matrix to the Shader program
    GLint modelCubeLoc = glGetUniformLocation(gProgramId, "modelCube");
    GLint UVScaleLoc2 = glGetUniformLocation(gProgramId, "uvScale2");
    glUniform2fv(UVScaleLoc2, 1, glm::value_ptr(gUVScale2));
    

    

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(modelCubeLoc, 1, GL_FALSE, glm::value_ptr(modelCube));

    glBindVertexArray(gMesh.cubeVAO);

    // bind textures after binding of vertexArray
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gTextureId3);


    

    glDrawElements(GL_TRIANGLES, gMesh.cubeEBO, GL_UNSIGNED_SHORT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    

    glm::mat4 scaleSphere = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f)); // 1. Scale the sphere
    glm::mat4 rotationSphere = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f)); // 2. Rotate the cylinder
    glm::mat4 translationSphere = glm::translate(glm::vec3(1.0f, 1.45f, -5.0f));
    glm::mat4 modelSphere = translationSphere * rotationSphere * scaleSphere; // Apply the transformations
    glUniform1i(glGetUniformLocation(gProgramId, "shapeType"), 3);

    /*std::stringstream ss;
    ss << "Model Sphere transformation:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ss << modelSphere[i][j] << " ";
        }
        ss << "\n";
    }
    std::cout << ss.str() << std::endl;*/

    
    

    GLint modelSphereLoc = glGetUniformLocation(gProgramId, "modelSphere");
    GLint UVScaleLoc3 = glGetUniformLocation(gProgramId, "uvScale3");
    glUniform2fv(UVScaleLoc3, 1, glm::value_ptr(gUVScale3));

    
    

    
    glUniformMatrix4fv(modelSphereLoc, 1, GL_FALSE, glm::value_ptr(modelSphere));

    
    
    glBindVertexArray(gMesh.sphereVAO);

    

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, gTextureId5);

    


    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    

    glBindVertexArray(0);

    glm::mat4 scaleCup = glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));
    glm::mat4 rotationCup = glm::rotate(110.0f, glm::vec3(1.0f, 0.0f, 1.0f));
    glm::mat4 translationCup = glm::translate(glm::vec3(1.0f, -0.50f, -1.0f));
    glm::mat4 modelCup = scaleCup * rotationCup * translationCup;

    glUniform1i(glGetUniformLocation(gProgramId, "shapeType"), 4);
    GLint modelCupLoc = glGetUniformLocation(gProgramId, "modelCup");
    glUniformMatrix4fv(modelCupLoc, 1, GL_FALSE, glm::value_ptr(modelCup));

    glBindVertexArray(gMesh.cupVAO);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, gTextureId6);

    glDrawElements(GL_TRIANGLES, cupIndices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glm::mat4 scaleAirpod = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 rotationAirpod = glm::rotate(80.0f, glm::vec3(3.0f, 1.0f, 1.0f));
    glm::mat4 translationAirpod = glm::translate(glm::vec3(-3.0f, 0.0f, 0.10f));
    glm::mat4 modelAirpod = scaleAirpod * rotationAirpod * translationAirpod;

    glUniform1i(glGetUniformLocation(gProgramId, "shapeType"), 5);
    GLint modelAirpodLoc = glGetUniformLocation(gProgramId, "modelAirpod");
    glUniformMatrix4fv(modelAirpodLoc, 1, GL_FALSE, glm::value_ptr(modelAirpod));

    glBindVertexArray(gMesh.airpodVAO);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, gTextureId7);

    glDrawElements(GL_TRIANGLES, gMesh.airpodIndices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);






    


    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

void UCreateCube(GLMesh& mesh)
{
    // Position, Tex coords, and Normals
    GLfloat cubeVerts[] = {
        // Vertex Positions    // Tex coords    //Normals    
        -0.5f, -0.5f, 0.0f,    0.0f, 1.0f,      0.0f, 1.0f, -0.10f,        
         0.5f, -0.5f, 0.0f,    1.0f, 1.0f,      0.0f, 1.0f, -0.10f,
         0.5f,  0.5f, 0.0f,    1.0f, 0.0f,      0.0f, 1.0f, -0.10f,
        -0.5f,  0.5f, 0.0f,    0.0f, 0.0f,      0.0f, 1.0f, -0.10f,

        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f,      0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f,      0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f,      0.0f, 0.0f, -1.0f,

    };

    // Index data to share position data
    GLushort cubeIndices[] = {
        0, 1, 2,  // Triangle 1 (front face)
        2, 3, 0,   // Triangle 2 (front face)
        4, 5, 6,  // Triangle 3 (back face)
        6, 7, 4,  // Triangle 4 (back face)
        1, 5, 6,  // Triangle 5 (right face)
        6, 2, 1,  // Triangle 6 (right face)
        0, 4, 7,  // Triangle 7 (left face)
        7, 3, 0,  // Triangle 8 (left face)
        3, 2, 6,  // Triangle 9 (top face)
        6, 7, 3,  // Triangle 10 (top face)
        0, 1, 5,  // Triangle 11 (bottom face)
        5, 4, 0   // Triangle 12 (bottom face)
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerUV = 2;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.cubeVAO); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.cubeVAO);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.cubeVBOs);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.cubeVBOs[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.cubeEBO = sizeof(cubeIndices) / sizeof(cubeIndices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.cubeVBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint cubeStride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, cubeStride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, cubeStride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, cubeStride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerUV)));
    glEnableVertexAttribArray(3);

    
}

// logic for creating texture
bool UCreateTexture(const char* filename, GLuint & textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        else if (channels == 1)
        {
            // Create an RGB image from the grayscale image
            int newSize = width * height * 3;
            unsigned char* rgbImage = new unsigned char[newSize];
            for (int i = 0; i < width * height; ++i)
            {
                rgbImage[i * 3] = image[i];
                rgbImage[i * 3 + 1] = image[i];
                rgbImage[i * 3 + 2] = image[i];
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbImage);
            delete[] rgbImage;
        }

        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

// Destroy texture logic
void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
    
}



// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions    // Texture       // Normals
        -0.5f, -0.5f, 0.0f,    0.0f, 1.0f,      0.0f, -1.0f, -1.0f,
         0.5f, -0.5f, 0.0f,    1.0f, 1.0f,      0.0f, -1.0f, -1.0f,  
         0.5f,  0.5f, 0.0f,    0.0f, 1.0f,      0.0f, -1.0f, -1.0f,
        -0.5f,  0.5f, 0.0f,    1.0f, 1.0f,      0.0f, -1.0f, -1.0f




    };

    // Index data to share position data
    GLushort indices[] = {
          0, 1, 2,
          2, 3, 0




    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerUV = 2;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerUV)));
    glEnableVertexAttribArray(3);
}

void UCreateSphere(float radius, int numSlices, int numStacks, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texCoords, std::vector<unsigned int>& indices) {
    float sliceAngle = glm::two_pi<float>() / numSlices;
    float stackAngle = glm::pi<float>() / numStacks;

    // Generate the vertices, normals, and texture coordinates for the sphere
    for (int i = 0; i <= numStacks; i++) {
        float phi = i * stackAngle;
        float cosPhi = cos(phi);
        float sinPhi = sin(phi);

        for (int j = 0; j <= numSlices; j++) {
            float theta = j * sliceAngle;
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            // Calculate the vertex positions
            glm::vec3 vertex(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            vertex *= radius;
            vertices.push_back(vertex);

            // Calculate the normal vector
            glm::vec3 normal = vertex / radius;
            normals.push_back(normal);

            // Calculate the texture coordinates
            float u = 1.5f; // Set u to a constant value
            float v = static_cast<float>(i) / numStacks;
            texCoords.push_back(glm::vec2(u, v));
        }
    }

    // Generate the indices for the sphere
    for (int i = 0; i < numStacks; i++) {
        for (int j = 0; j < numSlices; j++) {
            indices.push_back(i * (numSlices + 1) + j);
            indices.push_back((i + 1) * (numSlices + 1) + j);
            indices.push_back((i + 1) * (numSlices + 1) + j + 1);

            indices.push_back((i + 1) * (numSlices + 1) + j + 1);
            indices.push_back(i * (numSlices + 1) + j + 1);
            indices.push_back(i * (numSlices + 1) + j);
        }
    }
}






void UCreateSphereMesh(GLMesh& mesh)
{
    
    
    UCreateSphere(0.75f, 100, 100, sphereVertices, sphereNormals, sphereTexCoords, sphereIndices);


    // Create and bind the VAO
    glGenVertexArrays(1, &mesh.sphereVAO);
    glBindVertexArray(mesh.sphereVAO);

    // Create the VBO and EBO
    glGenBuffers(1, &mesh.sphereVBO);
    glGenBuffers(1, &mesh.sphereEBO);

    // Bind the VBO and copy the vertex data to it
    glBindBuffer(GL_ARRAY_BUFFER, mesh.sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3),
        sphereVertices.data(), GL_STATIC_DRAW);

    // Bind the EBO and copy the index data to it
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int),
        sphereIndices.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec2), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
    glEnableVertexAttribArray(3);

    



    

    
}

void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
    glDeleteVertexArrays(1, &mesh.cubeVAO);
    glDeleteBuffers(2, mesh.cubeVBOs);
    glDeleteVertexArrays(1, &mesh.cylinderVAO);
    glDeleteVertexArrays(2, &mesh.cylinderVBO);
    glDeleteVertexArrays(1, &mesh.sphereVAO);
    glDeleteBuffers(2, &mesh.sphereVBO);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}

void UCreatCylinderMesh(GLMesh& mesh)
{
    createCylinder(30, 0.25f, 0.5f, cylinderVertices, cylinderIndices);

    // Generate and bind the VAO, VBO and EBO for the cylinder
    glGenVertexArrays(1, &mesh.cylinderVAO);
    glGenBuffers(1, &mesh.cylinderVBO);
    glGenBuffers(1, &mesh.cylinderEBO);
    glBindVertexArray(mesh.cylinderVAO);

    // Upload the cylinderVertices and cylinderIndices data to the VBO and EBO
    mesh.cylinderEBO = sizeof(cylinderIndices) / sizeof(cylinderIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(float), cylinderVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.cylinderEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinderIndices.size() * sizeof(unsigned int), cylinderIndices.data(), GL_STATIC_DRAW);

    // Enable the vertex attribute arrays for position and color in the cylinderVAO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);


}

// Create Cylinder method
void createCylinder(int slices, float radius, float height, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    // Number of vertices and indices in the cylinder
    int vertexCount = slices * 4;
    int indexCount = slices * 6;

    // Generate vertex data
    float angle = 0.0f;
    float angleIncrement = 2.0f * glm::pi<float>() / slices;
    for (int i = 0; i < slices; i++) {
        // Top vertex
        vertices.push_back(radius * cos(angle));
        vertices.push_back(height / 2);
        vertices.push_back(radius * sin(angle));

        // Bottom vertex
        vertices.push_back(radius * cos(angle));
        vertices.push_back(-height / 2);
        vertices.push_back(radius * sin(angle));



        angle += angleIncrement;
    }

    // Generate index data
    for (unsigned int i = 0; i < slices * 2; i += 2) {
        indices.push_back(i);
        indices.push_back((i + 2) % (slices * 2));
        indices.push_back(i + 1);

        indices.push_back((i + 2) % (slices * 2));
        indices.push_back((i + 3) % (slices * 2));
        indices.push_back(i + 1);
    }

    // Generate indices for top and bottom
    for (unsigned int i = 0; i < slices; i++) {
        indices.push_back(i * 2);
        indices.push_back((i * 2 + 2) % (slices * 2));
        indices.push_back(slices * 2);

        indices.push_back(i * 2 + 1);
        indices.push_back((i * 2 + 3) % (slices * 2));
        indices.push_back(slices * 2 + 1);
    }
}

void UCreateCup(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    const int numSegments = 30;
    const int numDiscs = 30;
    const float height = 0.5f;
    const float radius = 0.2f;

    // Generate vertices
    vertices.clear();
    for (int j = 0; j < numDiscs; j++) {
        float y = height * (float)j / (float)(numDiscs - 1);
        float r = radius * sqrt(1.0f - pow(y / height, 2));
        for (int i = 0; i < numSegments; i++) {
            float angle = i * (2.0f * glm::pi<float>()) / numSegments;
            float x = r * cos(angle);
            float z = r * sin(angle);
            float u = (float)i / (float)(numSegments - 1);
            float v = 1.0f - (float)j / (float)(numDiscs - 1);
            glm::vec3 position = glm::vec3(x, y, z);
            glm::vec3 normal = glm::normalize(glm::vec3(x, radius * cos(glm::asin(y / height)), z));
            glm::vec2 texCoords = glm::vec2(u, v);
            vertices.push_back({ position, normal, texCoords });
        }
    }

    // Generate indices
    
    indices.clear();
    for (int j = 0; j < numDiscs - 1; j++) {
        for (int i = 0; i < numSegments; i++) {
            int index1 = j * numSegments + i;
            int index2 = j * numSegments + i + 1;
            int index3 = (j + 1) * numSegments + i;
            int index4 = (j + 1) * numSegments + i + 1;
            indices.push_back(index1);
            indices.push_back(index3);
            indices.push_back(index2);
            indices.push_back(index2);
            indices.push_back(index3);
            indices.push_back(index4);
        }
        // Add triangle to connect last vertex of current disc to first vertex of next disc
        int lastVertexIndex = (j + 1) * numSegments - 1;
        int firstVertexIndex = (j + 1) * numSegments;
        indices.push_back(lastVertexIndex);
        indices.push_back(firstVertexIndex);
        indices.push_back(lastVertexIndex + 1);

        // Add triangle to connect first vertex of next disc to second vertex of current disc
        indices.push_back(lastVertexIndex + 1);
        indices.push_back(firstVertexIndex);
        indices.push_back(firstVertexIndex + 1);
    }

    


}









void UCreateCupMesh(GLMesh& mesh) 
{
    UCreateCup(cupVertices, cupIndices);

    // Create and bind vertex array object
    
    glGenVertexArrays(1, &gMesh.cupVAO);
    glBindVertexArray(gMesh.cupVAO);

    // Create vertex buffer and copy data to it
    
    glGenBuffers(1, &gMesh.cupVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.cupVBO);
    glBufferData(GL_ARRAY_BUFFER, cupVertices.size() * sizeof(Vertex), cupVertices.data(), GL_STATIC_DRAW);

    // Specify vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // Create index buffer and copy data to it
    
    glGenBuffers(1, &gMesh.cupEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gMesh.cupEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cupIndices.size() * sizeof(unsigned int), cupIndices.data(), GL_STATIC_DRAW);


   

}

void UCreateAirpodMesh(GLMesh& mesh)
{

    
    // Compute the corner vertices of the cube
    GLfloat vertices[] = {
        
            // Front face
            -0.5,  0.5,  0.1,  0.0,  1.0,  0.0,  0.0,  1.0,
            -0.5, -0.5,  0.1,  0.0,  0.0,  0.0,  0.0,  1.0,
             0.5, -0.5,  0.1,  1.0,  0.0,  0.0,  0.0,  1.0,
             0.5,  0.5,  0.1,  1.0,  1.0,  0.0,  0.0,  1.0,

             // Back face
             -0.5,  0.5, -0.1,  0.0,  1.0,  0.0,  0.0, -1.0,
             -0.5, -0.5, -0.1,  0.0,  0.0,  0.0,  0.0, -1.0,
              0.5, -0.5, -0.1,  1.0,  0.0,  0.0,  0.0, -1.0,
              0.5,  0.5, -0.1,  1.0,  1.0,  0.0,  0.0, -1.0,

              // Top face
              -0.5,  0.5, -0.1,  0.0,  1.0,  0.0,  1.0,  0.0,
              -0.5,  0.5,  0.1,  0.0,  0.0,  0.0,  1.0,  0.0,
               0.5,  0.5,  0.1,  1.0,  0.0,  0.0,  1.0,  0.0,
               0.5,  0.5, -0.1,  1.0,  1.0,  0.0,  1.0,  0.0,

               // Bottom face
               -0.5, -0.5, -0.1,  0.0,  1.0,  0.0, -1.0,  0.0,
               -0.5, -0.5,  0.1,  0.0,  0.0,  0.0, -1.0,  0.0,
                0.5, -0.5,  0.1,  1.0,  0.0,  0.0, -1.0,  0.0,
                0.5, -0.5, -0.1, 1.0, 1.0, 0.0, -1.0, 0.0,

        };

   


    // Compute the vertex indices for the cube faces
    GLuint indices[] = {
    0, 1, 2,    // front face
    2, 3, 0,
    4, 5, 6,    // back face
    6, 7, 4,
    3, 2, 6,    // top face
    6, 7, 3,
    0, 1, 5,    // bottom face
    5, 4, 0,
    2, 1, 5,    // right face
    5, 6, 2,
    0, 3, 7,    // left face
    7, 4, 0
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerUV = 2;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.airpodVAO); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.airpodVAO);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.airpodVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.airpodVBO); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.airpodIndices = sizeof(indices) / sizeof(indices[0]);
    glGenBuffers(1, &mesh.airpodEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.airpodEBO); // bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerUV)));
    glEnableVertexAttribArray(3);

}



    







void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

