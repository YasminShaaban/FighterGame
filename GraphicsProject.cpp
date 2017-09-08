// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <glew.h>

// GLFW
#include <glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include<vector>
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Other Libs
#include <SOIL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
// Library for sound
#include <irrKlang.h>
using namespace irrklang;
// flags for enemy required for incrementing Score at collision
vector <bool> flagsenemy;

// flags for missile enemy required for incrementing Score at collision
vector<bool>flagsmissileenemy;

//Sound Engine for playing music
ISoundEngine *SoundEngine = createIrrKlangDevice();
ISoundEngine *SoundEngine2 = createIrrKlangDevice();

// Properties
GLuint screenWidth = 1400, screenHeight = 700;
// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void sendLightingInformationToShaders(Shader shader);
void Do_Movement();
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint VAO, VBO;

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
// Light attributes
// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
// Positions of the point lights
float factor = 10;
// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GameObject player;
vector<GameObject> Enemy;//vector of Enemy gameobjects
//vector<GameObject>gameObjectsVector;//vector of player ,missile player and missile enemy gameobjects
int Score = 0;// score
string scoretext = " ";//scoretext Score as string

//positions of light points
glm::vec3 pointLightPositions[] = {
	glm::vec3(factor*0.7f, factor*factor*0.2f, factor*2.0f),
	glm::vec3(factor*2.3f, factor*factor*3.3f, factor* -4.0f),
	glm::vec3(factor*-4.0f, factor*factor* 2.0f, factor*-12.0f),
	glm::vec3(factor*0.0f, factor*factor*1.0f, factor* -3.0f)
};
// The MAIN function, from here we start our application and run our Game loop
void player_Do_Movement()
{
	// Player controls
	if (keys[GLFW_KEY_W])
		player.ProcessKeyboard(UPWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		player.ProcessKeyboard(DOWNWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		player.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		player.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_Q])
		player.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_E])
		player.ProcessKeyboard(BACKWARD, deltaTime);


}
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Fighter", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();


	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);
	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Compile and setup the shader of text
	Shader shadertext("shadertext.vs", "shadertext.frag");//Text
	glm::mat4 projectiontext = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));
	//glm::mat4 projectiontext = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	shadertext.Use();
	glUniformMatrix4fv(glGetUniformLocation(shadertext.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projectiontext));



	// Setup and compile our shaders

	Shader  shader3("Shader/shader.vs", "Shader/shader.frag");

	Shader  shader4("Shader/shader.vs", "Shader/shader.frag");

	Shader  shader5("Shader/shader.vs", "Shader/shader.frag");

	Shader  shader6("Shader/shader.vs", "Shader/shader.frag");
	Shader oceanShader("OceanShader/oceanShader.vs", "OceanShader/oceanShader.frag");


	std::ifstream::in;
	std::ifstream::binary;


	//Intializing the flagsenemy vector required for incrementing Score

	for (int i = 0; i < 50; i++) {

		flagsenemy.push_back(true);

	}
	//Intializing the flagsmissileenemy vector required for incrementing Score

	for (int i = 0; i < 1; i++) {

		flagsmissileenemy.push_back(true);

	}
	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "planetbe.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set Text Rendering
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
			);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Load models
	Model ourModelPlayer("ModelsFolder/Player/aircraft.obj");
	Model ourOceanModel("ModelsFolder/Ocean/Ocean_obj/Ocean.obj");
	Model ourShip("ModelsFolder/Enemy/ARC170.obj");
	Model ourBoundary("Earth/earth.obj");
	Model ourSky("Earth/earth.obj");

	//Play music 
	SoundEngine->play2D("aircraft056.mp3", GL_TRUE);
	// Draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	ourModelPlayer.generateBoundingSphere();
	ourShip.generateBoundingSphere();

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Check and call events
		glfwPollEvents();
		///camera with player
		player_Do_Movement();
		// Clear the colorbuffer
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//function for rendering text 
		RenderText(shadertext, "Score:" + scoretext, 25.0f, 660.0f, 0.6f, glm::vec3(0.5, 0.8f, 0.2f));
		// Intializing Enemy gameobjects setting parameters
		for (int i = 0; i < 50; i++) {
			GameObject enemy;
			enemy.setParameters(glm::vec3(0, 0, 1), glm::vec3(50, 0, 1), 10, &ourShip);
			Enemy.push_back(enemy);

		}

		//}
		//Intializing the missile player, and the player setting parameters

		player.setParameters(glm::vec3(0, 0, 1), glm::vec3(50, 0, 1), 10, &ourModelPlayer);



		//------------------- aircraft::Player ----------------------
		shader6.Use();
		// Transformation matrices
		glm::mat4 projection3 = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view3 = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader6.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection3));
		glUniformMatrix4fv(glGetUniformLocation(shader6.Program, "view"), 1, GL_FALSE, glm::value_ptr(view3));
		player.setParameters(glm::vec3(0, 0, 1), glm::vec3(0.0f, 2.0f, 0.0f), 50, &ourModelPlayer);
		player.ModelMatrix = glm::mat4();
		player.mBoundingSphereModelMatrix = glm::mat4();
		player.translateBy(player.currentWorldPosition);
		player.translateBoundingSphereBy(player.currentWorldPosition);
		player.translateBy(glm::vec3(-1.0f, 2.0f, 0.0f));
		player.translateBoundingSphereBy(glm::vec3(-1.0f, 2.0f, 0.0f));
		player.rotateByAround(glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		player.rotateBoundingSphereByAround(glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		player.rotateByAround(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		player.rotateBoundingSphereByAround(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		player.scaleBy(glm::vec3(0.09f, 0.09f, 0.09f));
		player.scaleBoundingSphereBy(glm::vec3(0.09f, 0.09f, 0.09f));
		player.scaleBoundingSphereBy(glm::vec3(ourModelPlayer.boundingSphereRadius / 2.0f, ourModelPlayer.boundingSphereRadius / 2.0f, ourModelPlayer.boundingSphereRadius / 2.0f));
		player.scaleBoundingSphereBy(glm::vec3(1 / 350.0f, 1 / 350.0f, 1 / 350.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader6.Program, "model"), 1, GL_FALSE, glm::value_ptr(player.ModelMatrix));
		// Follow model (CAMERA)
		camera.camerFollowObject(player);
		ourModelPlayer.Draw(shader6);
		sendLightingInformationToShaders(shader6);

		// ------------------------ Enemy another aircraft playing against our aircraft player -------------------------

		shader3.Use();
		glm::mat4 projectionSH = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 viewSH = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader3.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionSH));
		glUniformMatrix4fv(glGetUniformLocation(shader3.Program, "view"), 1, GL_FALSE, glm::value_ptr(viewSH));

		// Draw the loaded model


		for (int i = 0; i < 50; i++) {

			if (i % 2 == 0){
				if (i % 3 == 0 || i % 5 == 0 || i % 7 == 0) {
					Enemy[i].translateBy(glm::vec3(((GLfloat)glfwGetTime()) *6.0f, 0.0f, 0.0f));
					Enemy[i].translateBoundingSphereBy(glm::vec3(((GLfloat)glfwGetTime()) *6.0f, 0.0f, 0.0f));
					Enemy[i].translateBoundingSphereBy(glm::vec3(-17.0f*(i + 0.5f), 2.0f, 2.0f));
					Enemy[i].translateBy(glm::vec3(-17.0f*(i + 0.5f), 2.0f, 2.0f));
				}
				else {
					Enemy[i].translateBy(glm::vec3(((GLfloat)glfwGetTime()) *6.0f, 0.0f, 0.0f));
					Enemy[i].translateBoundingSphereBy(glm::vec3(((GLfloat)glfwGetTime()) *6.0f, 0.0f, 0.0f));
					Enemy[i].translateBoundingSphereBy(glm::vec3(-20.0f*(i + 0.5f), 0.0f, 8.0f));
					Enemy[i].translateBy(glm::vec3(-20.0f*(i + 0.5f), 0.0f, 8.0f));
				}
			}
			if (i % 2 == 1){
				if (i % 3 == 0 || i % 5 == 0 || i % 7 == 0) {
					//continue;
					Enemy[i].translateBy(glm::vec3(((GLfloat)glfwGetTime()) *6.0f, 0.0f, 0.0f));
					Enemy[i].translateBoundingSphereBy(glm::vec3(((GLfloat)glfwGetTime()) *6.0f, 0.0f, 0.0f));
					Enemy[i].translateBoundingSphereBy(glm::vec3(-22.0f*(i + 0.5f), 0.7f, -2.0f));
					Enemy[i].translateBy(glm::vec3(-22.0f*(i + 0.5f), 0.7f, -2.0f));

				}
				else  {
					Enemy[i].translateBy(glm::vec3(((GLfloat)glfwGetTime()) *6.0f, 0.0f, 0.0f));
					Enemy[i].translateBoundingSphereBy(glm::vec3(((GLfloat)glfwGetTime()) *6.0f, 0.0f, 0.0f));
					Enemy[i].translateBoundingSphereBy(glm::vec3(-25.0f*(i + 0.5f), 1.3f, -8.0f));
					Enemy[i].translateBy(glm::vec3(-25.0f*(i + 0.5f), 1.3f, -8.0f));

				}
			}
			Enemy[i].rotateByAround(glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			Enemy[i].scaleBy(glm::vec3(0.003f, 0.003f, 0.003f));
			Enemy[i].rotateBoundingSphereByAround(glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			Enemy[i].scaleBoundingSphereBy(glm::vec3(0.003f, 0.003f, 0.003f));
			Enemy[i].scaleBoundingSphereBy(glm::vec3(ourShip.boundingSphereRadius / 2.0f, ourShip.boundingSphereRadius / 2.0f, ourShip.boundingSphereRadius / 2.0f));
			Enemy[i].scaleBoundingSphereBy(glm::vec3(1 / 350.0f, 1 / 350.0f, 1 / 350.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader3.Program, "model"), 1, GL_FALSE, glm::value_ptr(Enemy[i].ModelMatrix));
			//ourShip.Draw(shader3);
			if (!GameObject::DoObjectsIntersect(Enemy[i], player)) {
				// ourShip.Draw(shader3);
			}
			if (Enemy[i].isVisible) {
				ourShip.Draw(shader3);
				sendLightingInformationToShaders(shader3);

			}
			else if (flagsenemy[i]) {
				flagsenemy[i] = false;
				Score += 10;
				scoretext = to_string(Score);
				SoundEngine2->play2D("explosion.wav", GL_FALSE);
			}
			//bool x= GameObject::DoObjectsIntersect(Enemy[i], player);
		}
		for (int i = 0; i < 50; i++)
		{
			Enemy[i].ModelMatrix = glm::mat4();
			Enemy[i].mBoundingSphereModelMatrix = glm::mat4();
		}


		// ------------Boundary Sphere of the whole game--------------------
		shader4.Use();
		// Transformation matrices for BOUNDARY
		glm::mat4 projectionBB = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 viewBB = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader4.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionBB));
		glUniformMatrix4fv(glGetUniformLocation(shader4.Program, "view"), 1, GL_FALSE, glm::value_ptr(viewBB));
		// Draw the loaded model
		glm::mat4 modelBB;
		modelBB = glm::translate(modelBB, glm::vec3(0.0f, 1.2f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		modelBB = glm::scale(modelBB, glm::vec3(0.01f, 0.01f, 0.01f));	// It's a bit too big for our scene, so scale it down

		modelBB = glm::scale(modelBB, glm::vec3((1.0f / 350.0f), (1.0f / 350.0f), (1.0f / 350.0f)));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader4.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelBB));
		//ourBoundary.Draw(shader4);

		//------------------------- Sky---------------------
		shader5.Use();
		// Transformation matrices for EARTH BOUNDARY
		glm::mat4 projectionS = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 viewS = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader5.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionS));
		glUniformMatrix4fv(glGetUniformLocation(shader5.Program, "view"), 1, GL_FALSE, glm::value_ptr(viewS));
		// Draw the loaded model

		glm::mat4 modelS;
		//modelS = glm::translate(modelS, glm::vec3(0.0f, -1.5f, (GLfloat)glfwGetTime()*0.3f)); // Translate it down a bit so it's at the center of the scene
		//modelS = glm::scale(modelS, glm::vec3(200.0f, 200.0f, 200.0f));	// It's a bit too big for our scene, so scale it down
		//modelS= glm::translate(modelS, glm::vec3(0.0f, -1.80f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		modelS = glm::rotate(modelS, glm::radians((GLfloat)glfwGetTime()*0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelS = glm::scale(modelS, glm::vec3((1.0 / 14.0f), (1.0 / 14.0f), (1.0 / 14.0f)));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader5.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelS));
		ourSky.Draw(shader5);

		sendLightingInformationToShaders(shader5);

		// -----------------------Ocean----------------------------
		oceanShader.Use();
		// Transformation matrices
		glm::mat4 projection2 = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view2 = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(oceanShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection2));
		glUniformMatrix4fv(glGetUniformLocation(oceanShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view2));
		glUniform1f(glGetUniformLocation(oceanShader.Program, "time"), (GLfloat)glfwGetTime());

		glm::mat4 model2;
		model2 = glm::translate(model2, glm::vec3(0.0f, -1.80f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		//model2 = glm::rotate(model2, , glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		model2 = glm::scale(model2, glm::vec3(20.0f, 20.0f, 20.0f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(oceanShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model2));
		//GLfloat timeValue = ;
		ourOceanModel.Draw(oceanShader);
		sendLightingInformationToShaders(oceanShader);


		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

#pragma region "User input"

// Moves/alters the camera positions based on user input

void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

void launchMissile(){
	GameObject gameObject;
	gameObject.translateBy(glm::vec3(((GLfloat)glfwGetTime()) *-15.0f, 0.0f, 0.0f));
	gameObject.translateBoundingSphereBy(glm::vec3(((GLfloat)glfwGetTime()) *15.0f, 0.0f, 0.0f));

}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		keys[key] = true;
	//launchMissile();
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// function for scrolling of mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


// function to send lighting information to shaders

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos, ypos, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },

			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void sendLightingInformationToShaders(Shader shader)
{
	GLint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
	glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
	// Set material properties
	glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 32.0f);
	// == ==========================
	// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
	// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
	// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
	// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
	// == ==========================
	// Directional light
	glUniform3f(glGetUniformLocation(shader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(shader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
	glUniform3f(glGetUniformLocation(shader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
	// Point light 1
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].quadratic"), 0.032);
	// Point light 2
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].quadratic"), 0.032);
	// Point light 3
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].quadratic"), 0.032);
	// Point light 4
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].quadratic"), 0.032);


}
#pragma endregion