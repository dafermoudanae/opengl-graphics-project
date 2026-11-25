//********************************
//Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1Α της OpenGL
//
//ΑΜ: 5206                        Όνομα: ΔΑΦΕΡΜΟΥ ΧΡΙΣΤΙΝΑ ΔΑΝΑΗ
//ΑΜ: 5319						  Όνομα:ΠΑΝΑΓΟΥ ΘΕΟΦΙΛΟΣ

//*********************************

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <windows.h>

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

//******************
// Η LoadShaders είναι black box για σας - αγνοήστε την συνάρτηση Loadshaders

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

// Παράμετροι αστεριού
static void Star(GLuint vbo, float centerX, float centerY, float m, float n) {
	const float half = m * 0.5f;

	// τετράγωνο (2 τρίγωνα)
	const float squareLeftSide = centerX - half, squareRightSide = centerX + half;
	const float squareBase = centerY - half, squareTopSide = centerY + half;

	const GLfloat points[] = {
		// τετράγωνο
		squareLeftSide, squareBase, 0.0f, squareRightSide, squareBase, 0.0f, squareRightSide, squareTopSide, 0.0f,
		squareLeftSide, squareBase, 0.0f, squareRightSide, squareTopSide, 0.0f, squareLeftSide, squareTopSide, 0.0f,
		// επάνω «ακτίνα»
		centerX - half, squareTopSide, 0.0f, centerX + half, squareTopSide, 0.0f, centerX, squareTopSide + n, 0.0f,
		// κάτω «ακτίνα»
		centerX - half, squareBase, 0.0f, centerX + half, squareBase, 0.0f, centerX, squareBase - n, 0.0f,
		// αριστερή «ακτίνα»
		squareLeftSide, centerY - half, 0.0f, squareLeftSide, centerY + half, 0.0f, squareLeftSide - n, centerY, 0.0f,
		// δεξιά «ακτίνα»
		squareRightSide, centerY - half, 0.0f, squareRightSide, centerY + half, 0.0f, squareRightSide + n, centerY, 0.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}
// Παράμετροι χαρακτήρα “Α”
static void characterA(GLuint vbo, float centerX, float a, float b, float c, float yBase) {
	const float xLeft = centerX - a / 2.0f;
	const float xRight = centerX + a / 2.0f;
	const float yTop = yBase + b;
	const float xTopPoint = centerX;
	const float yTopPoint = yTop + c;

	const GLfloat points[] = {
	xLeft, yBase, 0.0f, xRight, yBase, 0.0f, xRight, yTop, 0.0f,
	xLeft, yBase, 0.0f, xRight, yTop, 0.0f, xLeft, yTop, 0.0f,
	xLeft, yTop, 0.0f, xRight, yTop, 0.0f, xTopPoint, yTopPoint, 0.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}

/*******************************************************************/
int main(void)
{
	// Initialise GLFW
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

	

	
	window = glfwCreateWindow(900, 900, reinterpret_cast<const char*>(u8"ΕΡΓΑΣΙΑ 1Α-5206-5319"),  NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// ΓΚΡΙ background
	glClearColor(0.18f, 0.18f, 0.18f, 1.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//***********************************************
	// Οι shaders σας είναι οι
	// ProjectVertexShader.vertexshader
	// ProjectFragmentShader.fragmentshader

	GLuint programID = LoadShaders("ProjectVertexShader.vertexshader", "ProjectFragmentShader.fragmentshader");

	///////////////////////////////////////////////////////////////////////////////////////
	/**Το παρακάτω το αγνοείτε - είναι τοποθέτηση κάμερας ***/
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 30),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;
	///////////////////////////////////////////////////////////////////////////////////////
	//**************************************************
	/// Για βοήθεια το πρόγραμμα αναπαριστά δυο τυχαία τρίγωνα - εσείς θα πρέπει να βάλετε κορυφές κατάλληλες
	// για τα δικό σας τρίγωνα.
	//Στην άσκηση αυτή δουλεύετε στις 2 διαστάσεις x,y οπότε η z συνιστώσα θα ειναι πάντα 0.0f
	// ή θα την παραλείψετε.
	/////////////////////////////////////////////////////////////////////////////////////////

	// χαρακτήρας "A"
	const float a = 3.0f;
	const float b = a / 2.0f;
	const float c = a / 4.0f;
	const float yBase = -10.0f;
	  
	// μεταβλητές κίνησης
	const float step = a / 2.0f;
	const float LEFT_BOUND = -11.0f + a / 2.0f;
	const float RIGHT_BOUND = 11.0f - a / 2.0f;
	float charX = 0.0f;
	int prevJ = GLFW_RELEASE;
	int prevL = GLFW_RELEASE;

	//αστέρι
	const float m_side = 0.5f; // πλευρά τετραγώνου
	const float n_height = 1.5f; // ύψος “ακτίνας”
	const double star_change = 2.0; // κάθε 2s νέα θέση

	// τρέχουσα θέση αστεριού
	float starX = 0.0f, starY = 0.0f;
	bool star_active = false;
	double last_star_time = glfwGetTime();

	GLuint vbo_star; // VBO για το αστέρι
	glGenBuffers(1, &vbo_star);
	GLuint vbuffer; // VBO για το Α
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	characterA(vbuffer, charX, a, b, c, yBase); // αρχικό ανέβασμα κορυφών
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	srand((unsigned)time(NULL));

	// πρώτη τυχαία θέση
	auto respawn_star = [&]() {
		starX = ((float)rand() / RAND_MAX) * (11.0f - (-11.0f)) + (-11.0f); // [-11, 11]
		starY = ((float)rand() / RAND_MAX) * (11.0f - (-5.0f)) + (-5.0f); // [-5, 11]
		Star(vbo_star, starX, starY, m_side, n_height);
		star_active = true;
		};

	respawn_star();

	// Main loop
	do {
		// διαβάζουμε στιγμιαία κατάσταση πλήκτρων
		int curL = glfwGetKey(window, GLFW_KEY_L);
		int curJ = glfwGetKey(window, GLFW_KEY_J);

		// L: μία θέση δεξιά
		if (curL == GLFW_PRESS && prevL == GLFW_RELEASE) {
			charX += step;
			if (charX > RIGHT_BOUND) charX = RIGHT_BOUND;
			characterA(vbuffer, charX, a, b, c, yBase); // ανεβάζουμε τις νέες κορυφές
		}

		// J: μία θέση αριστερά
		if (curJ == GLFW_PRESS && prevJ == GLFW_RELEASE) {
			charX -= step;
			if (charX < LEFT_BOUND) charX = LEFT_BOUND;
			characterA(vbuffer, charX, a, b, c, yBase); // ανεβάζουμε τις νέες κορυφές
		}

		// ενημέρωση previous states
		prevL = curL;
		prevJ = curJ;

		// --- timing για αστέρι: κάθε 2s respawn ---
		double now = glfwGetTime();
		if (!star_active || (now - last_star_time) >= star_change) {
			starX = ((float)rand() / RAND_MAX) * (11.0f - (-11.0f)) + (-11.0f);
			starY = ((float)rand() / RAND_MAX) * (11.0f - (-5.0f)) + (-5.0f);
			Star(vbo_star, starX, starY, m_side, n_height);
			star_active = true;
			last_star_time = now;
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// σχεδιαση Αστεριού
		glBindBuffer(GL_ARRAY_BUFFER, vbo_star);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // pointer -> vbo_star
		glDrawArrays(GL_TRIANGLES, 0, 6 + 3 * 4); // 18 κορυφές

		// σχεδιαση χαρακτήρα Α
		glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // pointer -> vbuffer
		glDrawArrays(GL_TRIANGLES, 0, 9);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_1) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDisableVertexAttribArray(0);
	// Cleanup VBO
	glDeleteBuffers(1, &vbuffer);
	glDeleteBuffers(1, &vbo_star);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
