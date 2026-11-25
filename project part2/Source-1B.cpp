//********************************
//Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1B της OpenGL
//
//ΑΜ:        5206                 Όνομα: Δανάη Δαφέρμου
//ΑΜ:        5319                 Όνομα: Θεόφιλος Πανάγου

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
using namespace glm;
using namespace std;

/// <summary>
/// //////////////////////////////////////////////
/// </summary>
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

////////////////////////////////////////////// <summary>
/// Add camera function here
/// </summary>
void camera_function()
{
	static const float fixedFOV = 60.0f;              
	static const float aspect = 1.0f;              
	static const float nearestSight = 0.1f;
	static const float furthestSight = 100.0f;
	static bool initialized = false;

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
			const float rad = glm::radians(degrees);
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
		if (z > minZoom) {
			glm::vec3 dir = glm::normalize(center - eye);
			eye += dir * zoomStep;   // πλησιάζουμε
		}
	}
	if (curMinus == GLFW_PRESS && prevMinus == GLFW_RELEASE)
	{
		float z = getZoom();
		if (z < maxZoom) {
			glm::vec3 dir = glm::normalize(center - eye);
			eye -= dir * zoomStep;   // απομακρυνόμαστε
		}
	}

	prevW = curW; prevX = curX;
	prevQ = curQ; prevZ = curZ;
	prevPlus = curPlus; prevMinus = curMinus;

	// ΕΝΗΜΕΡΩΣΗ ΤΩΝ PROJECTION ΚΑΙ VIEW
	ProjectionMatrix = glm::perspective(glm::radians(fixedFOV), aspect, nearestSight, furthestSight);
	ViewMatrix = glm::lookAt(eye, center, up);
}


/////////////////////////////////////////////////

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

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
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
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

	GLint Result = GL_FALSE;
	int InfoLogLength;


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
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(850, 850, reinterpret_cast<const char*>(u8"ΕΡΓΑΣΙΑ 1Β-2025-Σκηνικό Παιχνίδι"), NULL, NULL);


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; 
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// background color
	glClearColor(0.18f, 0.18f, 0.18f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders

	GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -8.875, 0.0f));
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0.0f, -5.0f, 20.0f), // Camera in World Space
		glm::vec3(0.0f, 0.0f, 0.0f), // and looks at the origin
		glm::vec3(0.0f, 1.0f, 0.0f));  // Head is up 
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; 

	// ΠΑΡΑΜΕΤΡΟΙ ΒΑΣΗΣ ΚΑΙ ΒΑΣΗΣ/ΥΨΟΥΣ ΠΥΡΑΜΙΔΑΣ
	const float a = 3.0f;   
	const float b = 1.5f;   
	const float c = 2.0f;   
	const float h = 0.75f;  

	// ΣΥΝΤΕΤΑΓΜΕΝΕΣ
	const float leftSideX = -a / 2.0f, rightSideX = +a / 2.0f;
	const float yBase = -10.0f;          
	const float yTop = yBase + b;            
	const float zFront = 0.0f, zBack = -c;     
	const float yTopPoint = -7.75f;
	const float yCenter = 0.5f * (yBase + yTopPoint);  
	// ΜΕΤΑΚΙΝΗΣΗ ΜΕ J/L
	float currentPos = 0.0f;                 // τρέχουσα θέση
	const float step = a / 2.0f;        // μετακίνηση ανά πάτημα
	const float LeftLimit = -8.50f + a / 2.0f;
	const float RightLimit = 8.50f - a / 2.0f;
	int prevJ = GLFW_RELEASE, prevL = GLFW_RELEASE;

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

		v3.x,v3.y,v3.z,  v4.x,v4.y,v4.z,  v9.x,v9.y,v9.z,

		v7.x,v7.y,v7.z,  v3.x,v3.y,v3.z,  v9.x,v9.y,v9.z,

		v8.x,v8.y,v8.z,  v7.x,v7.y,v7.z,  v9.x,v9.y,v9.z,

		v4.x,v4.y,v4.z,  v8.x,v8.y,v8.z,  v9.x,v9.y,v9.z
	};

		//ΧΡΩΜΑΤΑ ΠΥΡΑΜΙΔΑΣ ΧΑΡΑΚΤΗΡΑ Α
	static const GLfloat charAroofCol[] = {
		0.6f,0.0f,0.1f,1.0f,   0.6f,0.0f,0.1f,1.0f,   0.6f,0.0f,0.1f,1.0f,
	
		0.0f,0.0f,0.4f,1.0f,   0.0f,0.0f,0.4f,1.0f,   0.0f,0.0f,0.4f,1.0f,
	
		0.2f,0.9f,0.2f,1.0f,   0.2f,0.9f,0.2f,1.0f,   0.2f,0.9f,0.2f,1.0f,
	
		0.7f,0.3f,0.9f,1.0f,   0.7f,0.3f,0.9f,1.0f,   0.7f,0.3f,0.9f,1.0f
	};

	// ΔΗΜΙΟΥΡΓΙΑ BUFFERS
	GLuint baseBuffer, roofBuffer, baseColBuffer, roofColBuffer;
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

	// ΚΥΒΟΙ 
	const float m = 2.0f;  // πλευρά κύβου
	const float n = 2.0f;  // απόσταση μεταξύ κύβων

	// ΣΥΝΤΕΤΑΓΜΕΝΕΣ ΚΟΡΥΦΗΣ Κ (ΤΟΥ ΠΡΩΤΟΥ ΕΚ ΤΩΝ 5 ΚΥΒΩΝ)
	const glm::vec3 pointK(-9.0f, 1.125f - yCenter, 0.0f); // 10.0f - 8.875f

	// ΟΛΟΚΛΗΡΩΜΕΝΟΣ ΚΥΒΟΣ ΑΥΤΗ ΤΗΝ ΦΟΡΑ (6 ΠΛΕΥΡΕΣ) ΜΕ ΔΙΑΔΙΚΑΣΙΑ LOOP ΘΑ ΔΗΜΙΟΘΡΓΗΘΟΥΝ ΚΑΙ ΟΙ ΑΛΛΟΙ 4
	static const GLfloat cubes[] = {
		
		 0.0f, 0.0f, 0.0f,
		 m,    0.0f, 0.0f,
		 m,   -m,    0.0f,
		 0.0f, 0.0f, 0.0f,
		 m,   -m,    0.0f,
		 0.0f,-m,    0.0f,

		  m,    0.0f,  0.0f,
		  m,    0.0f, -m,
		  m,   -m,    -m,
		  m,    0.0f,  0.0f,
		  m,   -m,    -m,
		  m,   -m,     0.0f,

		   m,    0.0f, -m,
		   0.0f, 0.0f, -m,
		   0.0f,-m,    -m,
		   m,    0.0f, -m,
		   0.0f,-m,    -m,
		   m,   -m,    -m,

			0.0f, 0.0f, -m,
			0.0f, 0.0f,  0.0f,
			0.0f,-m,     0.0f,
			0.0f, 0.0f, -m,
			0.0f,-m,     0.0f,
			0.0f,-m,    -m,

			 0.0f, 0.0f,  0.0f,
			 m,    0.0f,  0.0f,
			 m,    0.0f, -m,
			 0.0f, 0.0f,  0.0f,
			 m,    0.0f, -m,
			 0.0f, 0.0f, -m,


			  0.0f, -m,  0.0f,
			  m,   -m,  0.0f,
			  m,   -m, -m,
			  0.0f, -m,  0.0f,
			  m,   -m, -m,
			  0.0f, -m, -m
	};

	// ΧΡΩΜΑ ΤΩΝ ΚΥΒΩΝ (ΟΛΟΙ ΤΟ ΙΔΙΟ)
	static const GLfloat cubesCol[36 * 4] = {
		
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,
		0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f,  0.36f, 0.22f, 0.11f, 1.0f
	};

	// ΔΗΜΙΟΥΡΓΙΑ BUFFERS ΤΩΝ ΚΥΒΩΝ
	GLuint cubeBuffer;
	glGenBuffers(1, &cubeBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubes), cubes, GL_STATIC_DRAW);

	GLuint cubeColBuffer;
	glGenBuffers(1, &cubeColBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeColBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubesCol), cubesCol, GL_STATIC_DRAW);

	do {
		// ΔΙΑΔΙΚΑΣΙΑ ΓΙΑ ΜΕΤΑΚΙΝΗΣΗ ΜΕ J/L
		int curL = glfwGetKey(window, GLFW_KEY_L);
		int curJ = glfwGetKey(window, GLFW_KEY_J);

		
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


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// Use our shader
		glUseProgram(programID);

		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(0.0f, -5.0f, 20.0f), //ΕΔΩ ΚΑΝΑΜΕ ΤΗΝ ΑΛΛΑΓΗ ΠΟΥ ΖΗΤΗΘΗΚΕ ΣΤΗΝ ΕΚΦΩΝΗΣΗ
			glm::vec3(0.0f, 0.0f, 0.0f), 
			glm::vec3(0.0f, 1.0f, 0.0f)    // ΚΑΙ ΕΔΩ
		);
		
		glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(currentPos, -yCenter, 0.0f));
		camera_function(); // ΚΑΛΕΙ ΤΗΝ ΜΕΘΟΔΟ ΓΙΑ ΤΗΝ ΚΑΜΕΡΑ 

		glm::mat4 MVP = getProjectionMatrix() * getViewMatrix() * Model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, baseBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, baseColBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 30);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
 
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, roofBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1); 
		glBindBuffer(GL_ARRAY_BUFFER, roofColBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 12); 
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// TO LOOP ΓΙΑ ΝΑ ΦΤΙΑΞΟΥΜΕ 5 ΚΥΒΟΥΣ
		for (int i = 0; i < 5; ++i) {
			
			//ΘΕΣΕΙΣ ΚΥΒΩΝ
			float distBtCubes = i * (m + n);  
			glm::vec3 cubeBase = glm::vec3(pointK.x + distBtCubes, pointK.y, pointK.z);

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
		}


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the 1 key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_1) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &baseBuffer);
	glDeleteBuffers(1, &roofBuffer);
	glDeleteBuffers(1, &baseColBuffer);
	glDeleteBuffers(1, &roofColBuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

