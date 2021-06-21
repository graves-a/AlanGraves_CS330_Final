#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnOpengl/camera.h> // Camera class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Alan Graves Final"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 1000;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint tissueBoxVAO, planeVAO, tissueVAO, wristPadVAO, chargerProngVAO;         // Handle for the vertex array object
        GLuint tissueBoxVBO, planeVBO, tissueVBO, wristPadVBO, chargerProngVBO;         // Handle for the vertex buffer object
        GLuint tissueBoxVertices, planeVertices, tissueVertices, wristPadVertices, chargerProngVertices;    // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    // Texture
    GLuint gTissueBoxTextureId;
    glm::vec2 gUVScale(5.0f, 5.0f);
    GLint gTexWrapMode = GL_REPEAT;
    GLuint gPlaneTextureId;
    GLuint gTissueTextureId;
    GLuint gGlassTextureId;
    GLuint gWristPadTextureId;
    GLuint gChargerBrickTextureId;
    GLuint gChargerProngTextureId;
    GLuint gGlassTopTextureId;

    // Shader programs
    GLuint gCubeProgramId;
    GLuint gLampProgramId;

    // camera
    Camera gCamera(glm::vec3(1.0f, 1.0f, 8.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Subject position and scale
    glm::vec3 gCubePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gCubeScale(2.0f);

    // Cube and light color
    //m::vec3 gObjectColor(0.6f, 0.5f, 0.75f);
    glm::vec3 gObjectColor(1.f, 0.2f, 0.0f);
    glm::vec3 gLightColor(1.0f, 1.0f, 1.0f);

    // Light position and scale
    glm::vec3 gLightPosition(3.5f, 0.0f, 10.0f);
    glm::vec3 gLightScale(0.1f);

    // Plane position
    glm::vec3 gPlanePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gPlaneScale(5.0f);

    // Tissue position
    glm::vec3 gTissuePosition(0.0f, 1.0f, 0.0f);
    glm::vec3 gTissueScale(1.0f);

    // Glass position
    glm::vec3 gGlassPosition(3.0f, -0.6f, 0.0f);
    glm::vec3 gGlassScale(0.8f);

    // Glass top position
    glm::vec3 gGlassTopPosition(3.0f, -0.0f, 0.0f);
    glm::vec3 gGlassTopScale(0.4f);

    // Wrist Pad Location
    glm::vec3 gWristPadPosition(0.5f, -1.0f, 2.5f);
    glm::vec3 gWristPadScale(1.0f);

    // Charger brick position
    glm::vec3 gChargerBrickPosition(0.8f, -0.15f, 2.5f);
    glm::vec3 gChargerBrickScale(0.7);

    // Charger prong 1 position
    glm::vec3 gChargerProng1Position(1.0f, 0.2f, 2.5f);
    glm::vec3 gChargerProng1Scale(0.2f);

    // Charger prong 2 position
    glm::vec3 gChargerProng2Position(0.6f, 0.2f, 2.5f);
    glm::vec3 gChargerProng2Scale(0.2f);

    // Lamp animation
    bool gIsLampOrbiting = true;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Cube Vertex Shader Source Code*/
const GLchar* cubeVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
    layout(location = 1) in vec3 normal; // VAP position 1 for normals
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
    out vec2 vertexTextureCoordinate;

    //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Cube Fragment Shader Source Code*/
const GLchar* cubeFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTissueBoxTexture; // Useful when working with multiple textures
uniform sampler2D uPlaneTexture;
uniform sampler2D uTissueTexture;
uniform sampler2D uGlassTexture;
uniform sampler2D uWristPadTexture;
uniform sampler2D uChargerBrickTexture;
uniform sampler2D uChargerProngTexture;
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.5f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 30.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTissueBoxTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

    // Create the shader programs
    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gCubeProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
        return EXIT_FAILURE;

    // Load textures
    const char* tissueBoxFile = "../../resources/textures/tissue_box.jpg";
    UCreateTexture(tissueBoxFile, gTissueBoxTextureId);    
    const char* planeFile = "../../resources/textures/leather2.jpg";
    UCreateTexture(planeFile, gPlaneTextureId);
    const char* tissueFile = "../../resources/textures/tissue_paper.jpg";
    UCreateTexture(tissueFile, gTissueTextureId);
    const char* glassFile = "../../resources/textures/glass.jpg";
    UCreateTexture(glassFile, gGlassTextureId);
    const char* wristPadFile = "../../resources/textures/leather.jpg";
    UCreateTexture(wristPadFile, gWristPadTextureId);
    const char* chargerBrickFile = "../../resources/textures/charger.jpg";
    UCreateTexture(chargerBrickFile, gChargerBrickTextureId);
    const char* chargerProngFile = "../../resources/textures/brass.jpg";
    UCreateTexture(chargerProngFile, gChargerProngTextureId);
    const char* glassTopFile = "../../resources/textures/leather.jpg";
    UCreateTexture(glassTopFile, gGlassTopTextureId);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gCubeProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uTissueBoxTexture"), 0);
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uPlaneTexture"), 1);
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uTissueTexture"), 2);
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uGlassTexture"), 3);
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uWristPadTexture"), 4);
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uChargerBrickTexture"), 5);
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uChargerProngTexture"), 6);
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uGlassTopTexture"), 7);






    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);

    // Release texture
    UDestroyTexture(gTissueBoxTextureId);

    // Release shader programs
    UDestroyShaderProgram(gCubeProgramId);
    UDestroyShaderProgram(gLampProgramId);

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
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
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
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && gTexWrapMode != GL_REPEAT)
    {
        glBindTexture(GL_TEXTURE_2D, gTissueBoxTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_REPEAT;

        cout << "Current Texture Wrapping Mode: REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && gTexWrapMode != GL_MIRRORED_REPEAT)
    {
        glBindTexture(GL_TEXTURE_2D, gTissueBoxTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_MIRRORED_REPEAT;

        cout << "Current Texture Wrapping Mode: MIRRORED REPEAT" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_EDGE)
    {
        glBindTexture(GL_TEXTURE_2D, gTissueBoxTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_EDGE;

        cout << "Current Texture Wrapping Mode: CLAMP TO EDGE" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && gTexWrapMode != GL_CLAMP_TO_BORDER)
    {
        float color[] = { 1.0f, 0.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        glBindTexture(GL_TEXTURE_2D, gTissueBoxTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glBindTexture(GL_TEXTURE_2D, 0);

        gTexWrapMode = GL_CLAMP_TO_BORDER;

        cout << "Current Texture Wrapping Mode: CLAMP TO BORDER" << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
    {
        gUVScale += 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
    {
        gUVScale -= 0.1f;
        cout << "Current scale (" << gUVScale[0] << ", " << gUVScale[1] << ")" << endl;
    }

    // Pause and resume lamp orbiting
    static bool isLKeyDown = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !gIsLampOrbiting)
        gIsLampOrbiting = true;
    else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && gIsLampOrbiting)
        gIsLampOrbiting = false;

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Functioned called to render a frame
void URender()
{
    // Lamp orbits around the origin
    const float angularVelocity = glm::radians(45.0f);
    if (gIsLampOrbiting)
    {
        glm::vec4 newPosition = glm::rotate(angularVelocity * gDeltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(gLightPosition, 1.0f);
        gLightPosition.x = newPosition.x;
        gLightPosition.y = newPosition.y;
        gLightPosition.z = newPosition.z;
    }

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the cube VAO (used by cube and lamp)
    glBindVertexArray(gMesh.tissueBoxVAO);

    // CUBE: draw cube
    //----------------
    // Set the shader to be used
    glUseProgram(gCubeProgramId);

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = glm::translate(gCubePosition) * glm::scale(gCubeScale) * glm::rotate(15.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gCubeProgramId, "view");
    GLint projLoc = glGetUniformLocation(gCubeProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gCubeProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTissueBoxTextureId);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.tissueBoxVertices);


    // LAMP: draw lamp
    //----------------
    glUseProgram(gLampProgramId);

    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gLightPosition) * glm::scale(gLightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLampProgramId, "model");
    viewLoc = glGetUniformLocation(gLampProgramId, "view");
    projLoc = glGetUniformLocation(gLampProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, gMesh.tissueBoxVertices);

    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);


    /* Draw Plane */
    glBindVertexArray(gMesh.planeVAO);
    glUseProgram(gCubeProgramId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPlaneTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.planeVertices);
    glBindVertexArray(0);
    glUseProgram(0);

    /* Draw Tissue */
    glBindVertexArray(gMesh.tissueVAO);
    glUseProgram(gCubeProgramId);
    model = glm::translate(gTissuePosition) * glm::scale(gTissueScale) * glm::rotate(15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTissueTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.tissueVertices);

    /* Draw Glass */
    glBindVertexArray(gMesh.tissueBoxVAO);
    glUseProgram(gCubeProgramId);
    model = glm::translate(gGlassPosition) * glm::scale(gGlassScale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gGlassTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.tissueBoxVertices);

    /* Draw Glass Top */
    glBindVertexArray(gMesh.tissueBoxVAO);
    glUseProgram(gCubeProgramId);
    model = glm::translate(gGlassTopPosition) * glm::scale(gGlassTopScale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gGlassTopTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.tissueBoxVertices);

    /* Draw Wrist Pad */
    glBindVertexArray(gMesh.wristPadVAO);
    glUseProgram(gCubeProgramId);
    model = glm::translate(gWristPadPosition) * glm::scale(gWristPadScale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gWristPadTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.wristPadVertices);

    /* Draw Charger Brick */
    glBindVertexArray(gMesh.tissueBoxVAO);
    glUseProgram(gCubeProgramId);
    model = glm::translate(gChargerBrickPosition) * glm::scale(gChargerBrickScale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //model = glm::rotate(15.0f, glm::vec3(1.0f, 0.0f, 0.0f));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gChargerBrickTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.tissueBoxVertices);

    /* Draw Charger Prong One */
    glBindVertexArray(gMesh.chargerProngVAO);
    glUseProgram(gCubeProgramId);
    model = glm::translate(gChargerProng1Position) * glm::scale(gChargerProng1Scale);
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gChargerProngTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.chargerProngVertices);

    /* Draw Charger Prong Two */
    glBindVertexArray(gMesh.chargerProngVAO);
    glUseProgram(gCubeProgramId);
    model = glm::translate(gChargerProng2Position) * glm::scale(gChargerProng2Scale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gChargerProngTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMesh.chargerProngVertices);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat tissueBoxV[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

       //Front Face         //Positive Z Normal
      -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      //Left Face          //Negative X Normal
      -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

      //Right Face         //Positive X Normal
       0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Bottom Face        //Negative Y Normal
      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

      //Top Face           //Positive Y Normal
      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    GLfloat planeV[] = {
        -10.0f, -0.5f, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         10.0f, -0.5f, 10.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         10.0f, -0.5f, -10.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
         10.0f, -0.5f, -10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -10.0f, -0.5f, -10.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -10.0f, -0.5f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f

    };

    GLfloat tissueV[] = {
        0.0f,  1.0f,  0.0f,    0.0f, 0.0f, -1.0f,       0.0f, 0.0f,     // Front triangle
       -0.5f,  0.0f,  0.5f,    0.0f, 0.0f, -1.0f,       1.0f, 0.0f,
        0.5f,  0.0f,  0.5f,    0.0f, 0.0f, -1.0f,       1.0f, 1.0f,

        0.0f,  1.0f,  0.0f,    0.0f, 0.0f, -1.0f,       1.0f, 1.0f,     // Left triangle
       -0.5f,  0.0f,  0.5f,    0.0f, 0.0f, -1.0f,       0.0f, 1.0f,
       -0.5f,  0.0f, -0.5f,    0.0f, 0.0f, -1.0f,       0.0f, 0.0f,

        0.0f,  1.0f,  0.0f,    0.0f, 0.0f, -1.0f,       0.0f, 0.0f,     // Back triangle
       -0.5f,  0.0f, -0.5f,    0.0f, 0.0f, -1.0f,       1.0f, 0.0f,
        0.5f,  0.0f, -0.5f,    0.0f, 0.0f, -1.0f,       1.0f, 1.0f,

        0.0f,  1.0f,  0.0f,    0.0f, 0.0f, -1.0f,       0.0f, 1.0f,     // Right triangle
        0.5f,  0.0f, -0.5f,    0.0f, 0.0f, -1.0f,       0.0f, 0.0f,
        0.5f,  0.0f,  0.5f,    0.0f, 0.0f, -1.0f,       0.0f, 0.0f,

       -0.5f,  0.0f,  0.5f,    0.0f, 0.0f, -1.0f,       1.0f, 0.0f,     // Base of pyramid left
       -0.5f,  0.0f, -0.5f,    0.0f, 0.0f, -1.0f,       1.0f, 1.0f,
        0.5f,  0.0f,  0.5f,    0.0f, 0.0f, -1.0f,       0.0f, 1.0f,

        0.5f,  0.0f,  0.5f,    0.0f, 0.0f, -1.0f,       0.0f, 0.0f,     // Base of pyramid right
       -0.5f,  0.0f, -0.5f,    0.0f, 0.0f, -1.0f,       1.0f, 0.0f,
        0.5f,  0.0f, -0.5f,    0.0f, 0.0f, -1.0f,       1.0f, 1.0f

    };

    GLfloat wristPadV[] = {
       // Bottom square
      -2.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // index 0
       4.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // index 1
       4.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 2
       4.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 2
      -2.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // index 3
      -2.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // index 0

       // Top sqaure
      -2.0f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, // index 4
       4.0f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, // index 5
       4.0f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 1.0f, // index 6
       4.0f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 1.0f, // index 6
      -2.0f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f,  0.0f, 1.0f, // index 7
      -2.0f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, // index 4

       // Left square
      -2.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // index 0
      -2.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // index 3
      -2.0f,  0.5f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 4
      -2.0f,  0.5f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 4
      -2.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // index 3
      -2.0f,  0.5f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // index 7

       // Right sqaure
       4.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // index 1
       4.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 2
       4.0f,  0.5f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 5
       4.0f,  0.5f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 5
       4.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // index 2
       4.0f,  0.5f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // index 6

       // Back square
      -2.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // index 3
       4.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // index 2
       4.0f,  0.5f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 6
       4.0f,  0.5f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // index 6
      -2.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // index 3
      -2.0f,  0.5f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // index 7

       // Front sqaure
      -2.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, // index 0
       4.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, // index 1
       4.0f,  0.5f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f, // index 5
       4.0f,  0.5f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f, // index 5
      -2.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  0.0f, 1.0f, // index 0
      -2.0f,  0.5f,  1.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f // index 4
    };

    GLfloat chargerProngV[] = {
      // Front face
     -0.1f, 1.5f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  // index 0
      0.1f, 1.5f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  // index 1
     -0.1f, 0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  // index 2
     -0.1f, 0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  // index 2
      0.1f, 0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  // index 3
      0.1f, 1.5f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  // index 1

      //Back Face         //Positive Z Normal
     -0.1f, 1.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  // index 4
      0.1f, 1.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // index 5
     -0.1f, 0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  // index 6
     -0.1f, 0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  // index 6
      0.1f, 0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,  // index 7
      0.1f, 1.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  // index 5

     //Left Face          //Negative X Normal
     -0.1f, 1.5f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // index 0
     -0.1f, 0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  // index 2
     -0.1f, 0.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // index 6
     -0.1f, 0.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // index 6
     -0.1f, 1.5f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  // index 4
     -0.1f, 1.5f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // index 0

     //Right Face         //Positive X Normal
      0.1f, 1.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // index 1
      0.1f, 0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  // index 3
      0.1f, 0.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // index 7
      0.1f, 0.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // index 7
      0.1f, 1.5f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  // index 5
      0.1f, 1.5f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // index 1

      //Bottom Face        //Negative Y Normal
     -0.1f, 0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  // index 2
      0.1f, 0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  // index 3
      0.1f, 0.0f, -1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  // index 7
      0.1f, 0.0f, -1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  // index 7
     -0.1f, 0.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  // index 6
     -0.1f, 0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  // index 2

       //Top Face           //Positive Y Normal
     -0.1f, 1.5f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  // index 0
      0.1f, 1.5f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,  // index 1
      0.1f, 1.5f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  // index 5
      0.1f, 1.5f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  // index 5
     -0.1f, 1.5f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  // index 4
     -0.1f, 1.5f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f   // index 0
    };



    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    mesh.tissueBoxVertices = sizeof(tissueBoxV) / (sizeof(tissueBoxV[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    mesh.planeVertices = sizeof(planeV) / (sizeof(planeV[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    mesh.tissueVertices = sizeof(tissueV) / (sizeof(tissueV[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    mesh.wristPadVertices = sizeof(wristPadV) / (sizeof(wristPadV[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
    mesh.chargerProngVertices = sizeof(chargerProngV) / (sizeof(chargerProngV[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    /* Plane */
    glGenVertexArrays(1, &mesh.planeVAO);
    glBindVertexArray(mesh.planeVAO);


    glGenBuffers(1, &mesh.planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeV), planeV, GL_STATIC_DRAW);

    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    /* Tissue Box */
    glGenVertexArrays(1, &mesh.tissueBoxVAO); 
    glBindVertexArray(mesh.tissueBoxVAO);

    glGenBuffers(1, &mesh.tissueBoxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.tissueBoxVBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(tissueBoxV), tissueBoxV, GL_STATIC_DRAW);

    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    /* Tissue */
    glGenVertexArrays(1, &mesh.tissueVAO);
    glBindVertexArray(mesh.tissueVAO);

    glGenBuffers(1, &mesh.tissueVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.tissueVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tissueV), tissueV, GL_STATIC_DRAW);

    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    /* Wrist Pad */
    glGenVertexArrays(1, &mesh.wristPadVAO);
    glBindVertexArray(mesh.wristPadVAO);

    glGenBuffers(1, &mesh.wristPadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.wristPadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wristPadV), wristPadV, GL_STATIC_DRAW);

    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    /* Charger Prong */
    glGenVertexArrays(1, &mesh.chargerProngVAO);
    glBindVertexArray(mesh.chargerProngVAO);

    glGenBuffers(1, &mesh.chargerProngVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.chargerProngVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(chargerProngV), chargerProngV, GL_STATIC_DRAW);

    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

   
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.tissueBoxVAO);
    glDeleteBuffers(1, &mesh.tissueBoxVBO);
}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

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


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
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


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
