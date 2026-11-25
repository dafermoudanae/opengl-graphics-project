//********************************
// Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1C της OpenGL
//
// ΑΜ:        5206                 Όνομα: Δανάη Δαφέρμου
// ΑΜ:        5319                 Όνομα: Θεόφιλος Πανάγου
//*********************************

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ΦΤΙΑΧΝΟΥΜΕ ΕΝΑ STRUCT ΓΙΑ ΝΑ ΚΡΑΤΑΕΙ ΚΑΠΟΙΕΣ ΠΛΗΡΟΦΟΡΙΕΣ ΓΙΑ ΤΑ ΒΛΗΜΑΤΑ
struct Bullet {
    glm::vec3 startPoint;
    double    fireTime;
    bool      active;     
};

// ΓΙΑ ΤΗΝ ΥΦΗ ΜΑΣ ΔΙΝΕΤΑΙ ΣΑΝ ΟΔΗΓΙΑ ΑΠΟ ΤΟ stb_image.h ΝΑ ΠΡΟΣΤΕΘΟΥΝ ΑΥΤΑ
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;
using namespace std;

/// <summary>
/// //////////////////////////////////////////////
/// </summary
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::mat4 getViewMatrix() { return ViewMatrix; }
glm::mat4 getProjectionMatrix() { return ProjectionMatrix; }

////////////////////////////////////////////// <summary>
/// Add camera function here
/// </summary>
void camera_function()
{
    static const float fixedFOV = 60.0f;
    static const float aspect = 1.0f;
    static const float nearestSight = 0.1f;
    static const float furthestSight = 100.0f;

    static glm::vec3 center(0.0f, 0.0f, 0.0f);
    static glm::vec3 eye(0.0f, -5.0f, 20.0f);
    static glm::vec3 up(0.0f, 1.0f, 0.0f);

    // ΟΡΙΑ ΤΟΥ ΖΟΥΜ
    static const float minZoom = 5.0f;
    static const float maxZoom = 70.0f;

    // ΠΟΣΟ ΘΑ ΠΕΡΙΣΤΡΕΦΕΤΑΙ/ΖΟΥΜΑΡΕΙ ΜΕ ΚΑΘΕ ΠΑΤΗΜΑ ΤΩΝ ΠΛΗΚΤΡΩΝ
    static const float rotationStep = 10.0f;
    static const float zoomStep = 2.5f;

    // ΠΛΗΚΤΡΑ Q,Z,W,X,+,-
    static int prevW = GLFW_RELEASE, prevX = GLFW_RELEASE;
    static int prevQ = GLFW_RELEASE, prevZ = GLFW_RELEASE;
    static int prevPlus = GLFW_RELEASE, prevMinus = GLFW_RELEASE;

    int curW = glfwGetKey(window, GLFW_KEY_W);
    int curX = glfwGetKey(window, GLFW_KEY_X);
    int curQ = glfwGetKey(window, GLFW_KEY_Q);
    int curZ = glfwGetKey(window, GLFW_KEY_Z);
    int curPlus = glfwGetKey(window, GLFW_KEY_KP_ADD);
    int curMinus = glfwGetKey(window, GLFW_KEY_KP_SUBTRACT);

    auto orbitCamera = [&](const glm::vec3& axis, float degrees)
        {
            float rad = glm::radians(degrees);
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), rad, axis);

            glm::vec4 e4 = glm::vec4(eye - center, 1.0f);
            glm::vec4 u4 = glm::vec4(up, 0.0f);

            e4 = R * e4;
            u4 = R * u4;

            eye = center + glm::vec3(e4);
            up = glm::normalize(glm::vec3(u4));
        };

    // ΟΡΙΖΟΥΜΕ ΤΙΣ ΚΙΝΗΣΕΙΣ ΤΩΝ ΠΛΗΚΤΡΩΝ
    if (curW == GLFW_PRESS && prevW == GLFW_RELEASE)
        orbitCamera(glm::vec3(1, 0, 0), +rotationStep);
    if (curX == GLFW_PRESS && prevX == GLFW_RELEASE)
        orbitCamera(glm::vec3(1, 0, 0), -rotationStep);

    if (curQ == GLFW_PRESS && prevQ == GLFW_RELEASE)
        orbitCamera(glm::vec3(0, 1, 0), +rotationStep);
    if (curZ == GLFW_PRESS && prevZ == GLFW_RELEASE)
        orbitCamera(glm::vec3(0, 1, 0), -rotationStep);

    auto getZoom = [&]() { return glm::length(eye - center); };


    if (curPlus == GLFW_PRESS && prevPlus == GLFW_RELEASE)
    {
        float z = getZoom();
        if (z > minZoom)
        {
            glm::vec3 dir = glm::normalize(center - eye);
            eye += dir * zoomStep;
        }
    }
    if (curMinus == GLFW_PRESS && prevMinus == GLFW_RELEASE)
    {
        float z = getZoom();
        if (z < maxZoom)
        {
            glm::vec3 dir = glm::normalize(center - eye);
            eye -= dir * zoomStep;
        }
    }

    prevW = curW;    prevX = curX;
    prevQ = curQ;    prevZ = curZ;
    prevPlus = curPlus; prevMinus = curMinus;

    // ΕΝΗΜΕΡΩΣΗ ΤΩΝ PROJECTION ΚΑΙ VIEW
    ProjectionMatrix = glm::perspective(glm::radians(fixedFOV),
        aspect,
        nearestSight,
        furthestSight);
    ViewMatrix = glm::lookAt(eye, center, up);
}

/////////////////////////////////////////////////

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path){
 
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Impossible to open %s\n", vertex_file_path);
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }
    else {
        printf("Impossible to open %s\n", fragment_file_path);
        return 0;
    }

    GLint Result = GL_FALSE;
    int   InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}
///////////////////////////////////////////////////



int main(void)
{
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow( 850, 850, reinterpret_cast<const char*>(u8"ΕΡΓΑΣΙΑ 1Γ-2025-Καταστροφέας"), NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.18f, 0.18f, 0.18f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders(
        "P1BVertexShader.vertexshader",
        "P1BFragmentShader.fragmentshader"
    );
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Uniforms για texture
    GLint useTextureLoc = glGetUniformLocation(programID, "useTexture");
    GLint textureALoc = glGetUniformLocation(programID, "textureA");

    // ΠΑΡΑΜΕΤΡΟΙ ΒΑΣΗΣ ΚΑΙ ΒΑΣΗΣ/ΥΨΟΥΣ ΠΥΡΑΜΙΔΑΣ
    const float a = 3.0f;
    const float b = 1.5f;
    const float c = 2.0f;
    const float h = 0.75f;

    // ΣΥΝΤΕΤΑΓΜΕΝΕΣ (ΠΡΟΣΤΕΘΗΚΑΝ ΚΑΙ ΤΩΝ ΣΦΑΙΡΩΝ) 
    const float bulletX = 0.25f;
    const float bulletY = 0.5f;
    const float bulletZ = 0.25f;

    const float leftSideX = -a / 2.0f, rightSideX = +a / 2.0f;
    const float yBase = -10.0f;
    const float yTop = yBase + b;
    const float zFront = 0.0f;
    const float zBack = -c;
    const float yTopPoint = -7.75f;
    const float yCenter = 0.5f * (yBase + yTopPoint);

    // ΜΕΤΑΚΙΝΗΣΗ ΜΕ J/L
    float currentPos = 0.0f;
    const float step = a / 2.0f;
    const float LeftLimit = -8.50f + a / 2.0f;
    const float RightLimit = 8.50f - a / 2.0f;

    int prevJ = GLFW_RELEASE, prevL = GLFW_RELEASE;
    int prevSpace = GLFW_RELEASE;

    // ΠΙΝΑΚΑΣ ΓΙΑ ΤΗΝ ΚΑΤΑΣΤΑΣΗ ΤΩΝ ΚΥΒΩΝ/ΕΧΘΡΩΝ ΑΝ ΕΧΟΥΝ ΕΞΟΛΟΘΡΕΥΘΕΙ Ή ΟΧΙ
    bool cubesShowing[5] = { true, true, true, true, true };

    // ΛΙΣΤΑ ΠΟΥ ΚΡΑΤΑΕΙ ΟΛΑ ΤΑ ΒΛΗΜΑΤΑ ΠΟΥ ΕΧΟΥΝ ΕΚΤΟΞΕΥΘΕΙ
    Bullet bulletsFired[1000];
    int bulFirIndex = 0;
    for (int i = 0; i < 1000; ++i) bulletsFired[i].active = false;
    

    // ΤΑΧΥΤΗΤΑ ΒΛΗΜΑΤΩΝ
    const float bulletSpeed = 4.0f;


    // ΚΟΡΥΦΕΣ ΒΑΣΗΣ
    const glm::vec3 v1(leftSideX, yBase, zFront), v2(rightSideX, yBase, zFront), v3(rightSideX, yTop, zFront), v4(leftSideX, yTop, zFront);
    const glm::vec3 v5(leftSideX, yBase, zBack), v6(rightSideX, yBase, zBack), v7(rightSideX, yTop, zBack), v8(leftSideX, yTop, zBack);

    // 5 ΠΛΕΥΡΕΣ ΓΙΑ ΤΟΝ 'ΚΥΒΟ' Η ΚΑΘΕΜΙΑ ΣΧΕΔΙΑΖΕΤΑΙ ΑΠΟ ΔΥΟ ΤΡΙΓΩΝΑ ΚΑΙ ΔΕΝ ΒΑΖΟΥΜΕ ΤΗΝ (6Η) ΑΠΟ ΠΑΝΩ ΠΛΕΥΡΑ (ΒΑΣΗ ΤΟΥ Α)
    static const GLfloat charAbase[] = {
       
        v1.x,v1.y,v1.z,  v2.x,v2.y,v2.z,  v3.x,v3.y,v3.z,
        v1.x,v1.y,v1.z,  v3.x,v3.y,v3.z,  v4.x,v4.y,v4.z,
        
        v2.x,v2.y,v2.z,  v6.x,v6.y,v6.z,  v7.x,v7.y,v7.z,
        v2.x,v2.y,v2.z,  v7.x,v7.y,v7.z,  v3.x,v3.y,v3.z,
        
        v6.x,v6.y,v6.z,  v5.x,v5.y,v5.z,  v8.x,v8.y,v8.z,
        v6.x,v6.y,v6.z,  v8.x,v8.y,v8.z,  v7.x,v7.y,v7.z,
       
        v5.x,v5.y,v5.z,  v1.x,v1.y,v1.z,  v4.x,v4.y,v4.z,
        v5.x,v5.y,v5.z,  v4.x,v4.y,v4.z,  v8.x,v8.y,v8.z,
      
        v1.x,v1.y,v1.z,  v6.x,v6.y,v6.z,  v2.x,v2.y,v2.z,
        v1.x,v1.y,v1.z,  v5.x,v5.y,v5.z,  v6.x,v6.y,v6.z
    };

    // UV ΣΥΝΤΕΤΑΓΜΕΝΕΣ ΓΙΑ ΒΑΣΗ Α
    static const GLfloat charAbaseUV[] = {
        
        0.0f,0.0f, 1.0f,0.0f, 1.0f,1.0f,
        0.0f,0.0f, 1.0f,1.0f, 0.0f,1.0f,
        
        0.0f,0.0f, 1.0f,0.0f, 1.0f,1.0f,
        0.0f,0.0f, 1.0f,1.0f, 0.0f,1.0f,
        
        0.0f,0.0f, 1.0f,0.0f, 1.0f,1.0f,
        0.0f,0.0f, 1.0f,1.0f, 0.0f,1.0f,
      
        0.0f,0.0f, 1.0f,0.0f, 1.0f,1.0f,
        0.0f,0.0f, 1.0f,1.0f, 0.0f,1.0f,
        
        0.0f,0.0f, 1.0f,0.0f, 1.0f,1.0f,
        0.0f,0.0f, 1.0f,1.0f, 0.0f,1.0f
    };

    // ΧΡΩΜΑΤΑ ΚΥΒΟΥ (ΒΑΣΗ ΤΟΥ Α)
    static const GLfloat charAbaseCol[] = {

        1,0,0,1,  1,0,0,1,  1,0,0,1,  1,0,0,1,  1,0,0,1,  1,0,0,1,

        0,1,0,1,  0,1,0,1,  0,1,0,1,  0,1,0,1,  0,1,0,1,  0,1,0,1,

        0,0,1,1,  0,0,1,1,  0,0,1,1,  0,0,1,1,  0,0,1,1,  0,0,1,1,

        1,1,0,1,  1,1,0,1,  1,1,0,1,  1,1,0,1,  1,1,0,1,  1,1,0,1,

        1,0,1,1,  1,0,1,1,  1,0,1,1,  1,0,1,1,  1,0,1,1,  1,0,1,1
    };

    // ΚΟΡΥΦΗ ΠΥΡΑΜΙΔΑΣ ΑΠΟ ΕΚΦΩΝΗΣΗ
    const glm::vec3 v9(0.0f, -7.75f, -1.0f);

    // 4 ΤΡΙΓΩΝΑ ΓΙΑ ΤΗΝ ΠΥΡΑΜΙΔΑΣ (ΣΤΕΓΗ ΤΟΥ Α)
    static const GLfloat charAroof[] = {

        v3.x,v3.y,v3.z,    v4.x,v4.y,v4.z,  v9.x,v9.y,v9.z,

        v7.x,v7.y,v7.z, v3.x,v3.y,v3.z,  v9.x,v9.y,v9.z,

        v8.x,v8.y,v8.z,  v7.x,v7.y,v7.z,  v9.x,v9.y,v9.z,

        v4.x,v4.y,v4.z,  v8.x,v8.y,v8.z,  v9.x,v9.y,v9.z
    };

    //UV ΣΥΝΤΕΤΑΓΜΕΝΕΣ ΓΙΑ ΤΗΝ ΠΥΡΑΜΙΔΑ
    static const GLfloat charAroofUV[] = {


        0.0f,0.0f, 1.0f,0.0f, 0.5f,1.0f,

        0.0f,0.0f, 1.0f,0.0f, 0.5f,1.0f,
        
        0.0f,0.0f, 1.0f,0.0f, 0.5f,1.0f,
        
        0.0f,0.0f, 1.0f,0.0f, 0.5f,1.0f

    };

    //ΧΡΩΜΑΤΑ ΠΥΡΑΜΙΔΑΣ ΧΑΡΑΚΤΗΡΑ Α
    static const GLfloat charAroofCol[] = {


       0.6f,0.0f,0.1f,1.0f,   0.6f,0.0f,0.1f,1.0f,   0.6f,0.0f,0.1f,1.0f,

        0.0f,0.0f,0.4f,1.0f,  0.0f,0.0f,0.4f,1.0f,   0.0f,0.0f,0.4f,1.0f,

        0.2f,0.9f,0.2f,1.0f,   0.2f,0.9f,0.2f,1.0f,   0.2f,0.9f,0.2f,1.0f,

        0.7f,0.3f,0.9f,1.0f,   0.7f,0.3f,0.9f,1.0f,    0.7f,0.3f,0.9f,1.0f

    };

    
        const float bulletXpoint = bulletX * 0.5f;
        const float bulletYpoint = bulletY;
        const float bulletZpoint = bulletZ * 0.5f;

        static const GLfloat bulletPoints[] = {
        
        -bulletXpoint, 0.0f,  bulletZpoint,
         bulletXpoint, 0.0f,  bulletZpoint,
         bulletXpoint,  bulletYpoint,   bulletZpoint,
        -bulletXpoint, 0.0f,  bulletZpoint,
         bulletXpoint,  bulletYpoint,   bulletZpoint,
        -bulletXpoint,  bulletYpoint,   bulletZpoint,

        
         bulletXpoint, 0.0f,  bulletZpoint,
         bulletXpoint, 0.0f, -bulletZpoint,
         bulletXpoint,  bulletYpoint,  -bulletZpoint,
         bulletXpoint,  0.0f, bulletZpoint,
         bulletXpoint,  bulletYpoint,  -bulletZpoint,
         bulletXpoint,  bulletYpoint,   bulletZpoint,

         
          bulletXpoint, 0.0f, -bulletZpoint,
         -bulletXpoint, 0.0f, -bulletZpoint,
         -bulletXpoint,  bulletYpoint,  -bulletZpoint,
          bulletXpoint, 0.0f, -bulletZpoint,
         -bulletXpoint,  bulletYpoint,  -bulletZpoint,
          bulletXpoint,  bulletYpoint,  -bulletZpoint,

          
          -bulletXpoint, 0.0f, -bulletZpoint,
          -bulletXpoint, 0.0f,  bulletZpoint,
          -bulletXpoint,  bulletYpoint,   bulletZpoint,
          -bulletXpoint, 0.0f, -bulletZpoint,
          -bulletXpoint,  bulletYpoint,   bulletZpoint,
          -bulletXpoint,  bulletYpoint,  -bulletZpoint,

         
          -bulletXpoint, bulletYpoint,  bulletZpoint,
           bulletXpoint, bulletYpoint,  bulletZpoint,
           bulletXpoint, bulletYpoint, -bulletZpoint,
          -bulletXpoint, bulletYpoint,  bulletZpoint,
           bulletXpoint, bulletYpoint, -bulletZpoint,
          -bulletXpoint, bulletYpoint, -bulletZpoint,

         
          -bulletXpoint, 0.0f,  bulletZpoint,
           bulletXpoint, 0.0f,  bulletZpoint,
           bulletXpoint, 0.0f, -bulletZpoint,
          -bulletXpoint, 0.0f,  bulletZpoint,
           bulletXpoint, 0.0f, -bulletZpoint,
          -bulletXpoint, 0.0f, -bulletZpoint
        };

        //ΧΡΩΜΑ ΒΛΗΜΑΤΩΝ
        static const GLfloat bulletColors[36 * 4] = {
            
            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,
            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,

            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,
            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,

            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,
            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,

            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,
            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,

            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,
            1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f,  1.0f, 0.84f, 0.0f, 1.0f
        };



    // ΔΗΜΙΟΥΡΓΙΑ BUFFERS
    GLuint baseBuffer, roofBuffer, baseColBuffer, roofColBuffer, 
           baseUVBuffer, roofUVBuffer, bulletBuffer,bulletColBuffer;

    glGenBuffers(1, &baseBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, baseBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charAbase), charAbase, GL_STATIC_DRAW);
    glGenBuffers(1, &roofBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, roofBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charAroof), charAroof, GL_STATIC_DRAW);
    glGenBuffers(1, &baseColBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, baseColBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charAbaseCol), charAbaseCol, GL_STATIC_DRAW);
    glGenBuffers(1, &roofColBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, roofColBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charAroofCol), charAroofCol, GL_STATIC_DRAW);
    glGenBuffers(1, &baseUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, baseUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charAbaseUV), charAbaseUV, GL_STATIC_DRAW);
    glGenBuffers(1, &roofUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, roofUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charAroofUV), charAroofUV, GL_STATIC_DRAW);
    glGenBuffers(1, &bulletBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bulletBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bulletPoints), bulletPoints, GL_STATIC_DRAW);

    glGenBuffers(1, &bulletColBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bulletColBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bulletColors), bulletColors, GL_STATIC_DRAW);

    // ΓΟΡΤΩΣΗ ΥΦΗΣ ΑΠΟ TEXTUREA
    GLuint idTextureA;
    glGenTextures(1, &idTextureA);
    glBindTexture(GL_TEXTURE_2D, idTextureA);

    //ΜΕ ΒΑΣΗ ΑΥΤΕΣ ΤΙΣ ΕΝΤΟΛΕΣ ΞΕΡΕΙ Η OPENGL ΠΩΣ ΝΑ ΔΙΑΧΕΙΡΙΣΤΕΙ ΤΗΝ ΥΦΗ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int textureWidth = 0;
        int textureHeight = 0;
    int textureChannels = 0;

    // ΤΟ STB ΘΑ ΕΠΙΣΤΡΕΦΕΙ ΠΑΝΤΑ RGBA 
    // ΤΟ ΚΑΝΑΜΕ ΝΑ ΕΙΝΑΙ ΣΤΑΘΕΡΟ ΓΙΑΤΙ ΚΡΑΣΑΡΕ ΟΤΑΝ ΕΙΧΕ ΚΑΙ RGB ΛΟΓΩ ΕΝΟΣ DRIVER ΤΗΣ INTEL ΚΑΙ ΗΤΑΝ Ο ΜΟΝΑΔΙΚΟΣ ΤΡΟΠΟΣ ΕΠΙΛΥΣΗΣ
    stbi_set_flip_vertically_on_load(true);
    unsigned char* textureData = stbi_load("textureA.jpg", &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);

    if (!textureData) {
        std::cout << "Failed to load textureA\n";
    }
    else {
        std::cout << "Loading textureA.jpg Successful: "
            << textureWidth << "x" << textureHeight
            << " channels=" << textureChannels << std::endl;

        // Για ασφάλεια, ρύθμιση alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Εσωτερικό format = GL_RGBA, input format = GL_RGBA (σταθερά)
        glTexImage2D( GL_TEXTURE_2D,0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

        glGenerateMipmap(GL_TEXTURE_2D);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cout << "glTexImage2D error: 0x" << std::hex << err << std::dec << std::endl;
        }
    }

    stbi_image_free(textureData);

    // ΚΥΒΟΙ
    const float m = 2.0f;
    const float n = 2.0f;

    // ΣΥΝΤΕΤΑΓΜΕΝΕΣ ΚΟΡΥΦΗΣ Κ (ΤΟΥ ΠΡΩΤΟΥ ΕΚ ΤΩΝ 5 ΚΥΒΩΝ)
    const glm::vec3 pointK(-9.0f, 1.125f - yCenter, 0.0f);

    // ΟΛΟΚΛΗΡΩΜΕΝΟΣ ΚΥΒΟΣ ΑΥΤΗ ΤΗΝ ΦΟΡΑ (6 ΠΛΕΥΡΕΣ) ΜΕ ΔΙΑΔΙΚΑΣΙΑ LOOP ΘΑ ΔΗΜΙΟΘΡΓΗΘΟΥΝ ΚΑΙ ΟΙ ΑΛΛΟΙ 4
    static const GLfloat cubes[] = {
        0,0,0,   m,0,0,   m,-m,0,   0,0,0,   m,-m,0,   0,-m,0,
        m,0,0,   m,0,-m,  m,-m,-m,  m,0,0,   m,-m,-m,  m,-m,0,
        m,0,-m,  0,0,-m,  0,-m,-m,  m,0,-m, 0,-m,-m,  m,-m,-m,
        0,0,-m,  0,0,0,   0,-m,0,   0,0,-m, 0,-m,0,   0,-m,-m,
        0,0,0,   m,0,0,   m,0,-m,   0,0,0,  m,0,-m,   0,0,-m,
        0,-m,0,  m,-m,0,  m,-m,-m,  0,-m,0, m,-m,-m,  0,-m,-m
    };

    // ΧΡΩΜΑ ΤΩΝ ΚΥΒΩΝ (ΟΛΟΙ ΤΟ ΙΔΙΟ)
    static const GLfloat cubesCol[36 * 4] = {
        0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,
    0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,

    0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,
    0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,

     0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,
    0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,

    0.36f,0.22f,0.11f,1.0f, 0.36f,0.22f,0.11f,1.0f,   0.36f,0.22f,0.11f,1.0f,
    0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,     0.36f,  0.22f,0.11f,1.0f,

      0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,   0.36f,0.22f,0.11f,1.0f,
    0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,

    0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f,
       0.36f,0.22f,0.11f,1.0f,     0.36f,0.22f,0.11f,1.0f,  0.36f,0.22f,0.11f,1.0f
    };

    // ΔΗΜΙΟΥΡΓΙΑ BUFFERS ΤΩΝ ΚΥΒΩΝ
    GLuint cubeBuffer, cubeColBuffer;
    glGenBuffers(1, &cubeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubes), cubes, GL_STATIC_DRAW);

    glGenBuffers(1, &cubeColBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeColBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubesCol), cubesCol, GL_STATIC_DRAW);


    
    glfwSetTime(0.0);
    const float cubesStep = m / 2.0f;   // ΒΗΜΑ ΚΥΒΩΝ ΟΣΟ ΠΛΗΣΙΑΖΟΥΝ ΤΟΝ Α
    const float cubeStepTime = 5.0f;  //ΑΝΑ 5 ΔΕΥΤΕΡΟΛΕΠΤΑ ΜΕΤΑΚΙΝΟΥΝΤΑΙ ΚΑΤΑ ΕΝΑ ΒΗΜΑ ΠΡΟΣ ΤΑ ΚΑΤΩ ΟΙ ΚΥΒΟΙ
    bool gameOver = false;                    // θα γίνει true όταν φτάσουν τον Α


    do {
        // ΔΙΑΔΙΚΑΣΙΑ ΓΙΑ ΜΕΤΑΚΙΝΗΣΗ ΜΕ J / L
        int curL = glfwGetKey(window, GLFW_KEY_L);
        int curJ = glfwGetKey(window, GLFW_KEY_J);
        int curSpace = glfwGetKey(window, GLFW_KEY_SPACE);
        double t = glfwGetTime();

        if (curL == GLFW_PRESS && prevL == GLFW_RELEASE) {
            currentPos += step;
            if (currentPos > RightLimit) currentPos = RightLimit;
        }
        if (curJ == GLFW_PRESS && prevJ == GLFW_RELEASE) {
            currentPos -= step;
            if (currentPos < LeftLimit) currentPos = LeftLimit;
        }
        prevL = curL;
        prevJ = curJ;

        //ΔΙΑΔΙΚΑΣΙΑ ΕΚΤΟΞΕΥΣΗΣ ΒΛΗΜΑΤΩΝ ΜΕ ΤΟ SPACE
        if (curSpace == GLFW_PRESS && prevSpace == GLFW_RELEASE && !gameOver) {
            
            glm::vec4 v9_pointK = glm::vec4(v9, 1.0f);
            v9_pointK = glm::translate(glm::mat4(1.0f), glm::vec3(currentPos, -yCenter, 0.0f)) * v9_pointK;

            Bullet bullet;
            bullet.startPoint = glm::vec3(v9_pointK.x, v9_pointK.y, v9_pointK.z);
            bullet.fireTime = t;
            bullet.active = true;

            bulletsFired[bulFirIndex] = bullet;
            bulFirIndex = bulFirIndex + 1;
            if (bulFirIndex >= 1000) bulFirIndex = 0;

        }
         prevSpace = curSpace;

        // ΒΡΙΣΚΟΥΜΕ ΤΟ ΠΟΣΟ ΕΧΟΥΝ ΜΕΤΑΚΙΝΗΘΕΙ ΣΥΝΟΛΙΚΑ ΟΙ ΚΥΒΟΙ
        float cubesFall = cubesStep * -(t / cubeStepTime);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use our shader
        glUseProgram(programID);

        camera_function();

        // Μοντέλο για τον χαρακτήρα Α
        glm::mat4 Model = glm::translate(glm::mat4(1.0f),
            glm::vec3(currentPos, -yCenter, 0.0f));
        glm::mat4 MVP = getProjectionMatrix() * getViewMatrix() * Model;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // ΣΧΕΔΙΑΖΟΥΜΕ ΤΟΝ Α ΜΕ ΤΗΝ ΥΦΗ ΑΠΟ ΤΟ TEXTUREA
        if (useTextureLoc != -1) glUniform1i(useTextureLoc, 1);
        if (textureALoc != -1) glUniform1i(textureALoc, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, idTextureA);

        //ΒΑΣΗ ΧΑΡΑΚΤΗΡΑ Α
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, baseBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, baseColBuffer);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, baseUVBuffer);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 30);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // ΠΥΡΑΜΙΔΑ - ΣΤΕΓΗ ΧΑΡΑΚΤΗΡΑ Α
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, roofBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, roofColBuffer);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, roofUVBuffer);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 12);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);


        for (int i = 0; i < 1000; i++) {
            if (!bulletsFired[i].active) continue;

            //ΒΡΙΣΚΟΥΜΕ ΤΗΝ ΤΡΕΧΟΥΣΑ ΘΕΣΗ ΤΟΥ ΒΛΗΜΑΤΟΣ
            float travelTime = static_cast<float>(t - bulletsFired[i].fireTime);
            glm::vec3 bulletCurPos = bulletsFired[i].startPoint + glm::vec3(0.0f, bulletSpeed * travelTime, 0.0f);

            // ΚΑΝΟΥΜΕ ΤΟ ACTIVE ΝΑ ΕΙΝΑΙ FALSE ΩΣΤΕ ΝΑ ΣΒΗΣΤΕΙ ΤΟ ΒΛΗΜΑ ΑΝ ΞΕΠΕΡΑΣΕΙ ΤΟ ΟΡΙΟ ΥΨΟΥΣ
            if (bulletCurPos.y > 20.0f) {
                bulletsFired[i].active = false;
                continue;
            }

            glm::mat4 bulletModel = glm::translate(glm::mat4(1.0f), bulletCurPos);
            glm::mat4 bulletMVP = getProjectionMatrix() * getViewMatrix() * bulletModel;
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &bulletMVP[0][0]);
            glUniform1i(useTextureLoc, 0);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, bulletBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, bulletColBuffer);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }

        // ΟΡΙΖΟΥΜΕ useTexture = 0 ΩΣΤΕ ΝΑ ΜΗΝ ΧΡΗΣΙΜΟΙΗΘΕΙ Η ΥΦΗ ΓΙΑ ΤΟΥΣ ΚΥΒΟΥΣ ΚΑΙ ΝΑ ΖΩΓΡΑΦΙΣΤΟΥΝ ΚΑΝΟΝΙΚΑ ΜΕ ΤΑ ΧΡΩΜΑΤΑ
        if (useTextureLoc != -1) glUniform1i(useTextureLoc, 0);

        for (int i = 0; i < 5; ++i) {

            //ΕΛΕΓΧΟΥΜΕ ΠΟΙΟΙ ΚΥΒΟΙ/ΕΧΘΡΟΙ ΔΕΝ ΕΧΟΥΝ ΕΞΟΛΟΘΡΕΥΘΕΙ
            if (!cubesShowing[i]) {
                continue; 
            }

            float distBtCubes = i * (m + n);
            glm::vec3 cubeBase = glm::vec3(
                pointK.x + distBtCubes,
                pointK.y + cubesFall,
                pointK.z
            );

            glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), cubeBase);
            glm::mat4 cubeMvp = getProjectionMatrix() * getViewMatrix() * cubeModel;
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &cubeMvp[0][0]);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, cubeColBuffer);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);

            // ΕΛΕΓΧΟΥΜΕ ΑΝ ΕΦΤΑΣΑΝ ΟΙ ΚΥΒΟΙ ΤΟΝ ΧΑΡΑΚΑΤΗΡΑ Α ΓΙΑ ΝΑ ΤΕΡΜΑΤΙΣΤΕΙ ΤΟ ΠΑΙΧΝΙΔΙ
            float cubeBottomY = cubeBase.y - m; 
            if (cubeBottomY <= 0.0f) {
                gameOver = true;
            }

            // ΣΥΓΚΡΟΥΣΗ ΒΛΗΜΑΤΟΣ ΜΕ ΚΥΒΟΥΣ
            float cubeMinX = cubeBase.x;
            float cubeMaxX = cubeBase.x + m;
            float cubeMinY = cubeBase.y - m;
            float cubeMaxY = cubeBase.y;
            float cubeMinZ = cubeBase.z - m;
            float cubeMaxZ = cubeBase.z;


            for (int y = 0; y < 1000; y++) {
                if (!bulletsFired[y].active) continue;

                float travelTime = static_cast<float>(t - bulletsFired[y].fireTime);
                glm::vec3 bulletCurPos = bulletsFired[y].startPoint + glm::vec3(0.0f, bulletSpeed * travelTime, 0.0f);

                float bulletMinX = bulletCurPos.x - bulletX * 0.5f;
                float bulletMaxX = bulletCurPos.x + bulletX * 0.5f;
                float bulletMinY = bulletCurPos.y;
                float bulletMaxY = bulletCurPos.y + bulletY;
                float bulletMinZ = bulletCurPos.z - bulletZ * 0.5f;
                float bulletMaxZ = bulletCurPos.z + bulletZ * 0.5f;

                bool touchX = (bulletMinX <= cubeMaxX) && (bulletMaxX >= cubeMinX);
                bool touchY = (bulletMinY <= cubeMaxY) && (bulletMaxY >= cubeMinY);
                bool touchZ = (bulletMinZ <= cubeMaxZ) && (bulletMaxZ >= cubeMinZ);

                if (touchX && touchY && touchZ) {
                    cubesShowing[i] = false; // ΕΞΟΛΟΘΡΕΥΣΗ ΕΧΘΡΟΥ/ΣΒΗΣΙΜΟ ΚΥΒΟΥ
                    bulletsFired[y].active = false; // ΣΒΗΝΟΥΜΕ ΤΟ ΒΛΗΜΑ ΑΠΟ ΤΗΝ ΣΤΙΓΜΗ ΠΟΥ ΗΡΘΕ ΣΕ ΕΠΑΦΗ ΜΕ ΚΥΒΟ
                    break; 
                }
            }
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_1) != GLFW_PRESS && !gameOver && glfwWindowShouldClose(window) == 0);


    // Cleanup VBO
    glDeleteBuffers(1, &baseBuffer);
    glDeleteBuffers(1, &roofBuffer);
    glDeleteBuffers(1, &baseColBuffer);
    glDeleteBuffers(1, &roofColBuffer);
    glDeleteBuffers(1, &baseUVBuffer);
    glDeleteBuffers(1, &roofUVBuffer);
    glDeleteBuffers(1, &cubeBuffer);
    glDeleteBuffers(1, &cubeColBuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();


    return 0;
}
