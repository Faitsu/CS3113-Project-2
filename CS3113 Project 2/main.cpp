//Erica Chou
//February 26,2020
//Pong Clone

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"


//globals
GLuint kirbyTextureID;
GLuint kirbyBlockTextureID;
GLuint DDDBlockTextureID;


float LastTicks = 0.0f;
SDL_Window* displayWindow;
bool gameIsRunning = true;
bool start = false;
bool goLeft = false;

float edgeLeft = -5.00f;
float edgeRight = 5.00f;

float edgeTop = 3.65f;
float edgeBottom = -3.65f;

float boxModelx = 0.25f; //for ball
float boxBlockModelx = 0.75f; //for all not edge entities



float boxModely = 1; //for ball
float boxBlockModely = 1;

float bounce = 0;

int score1 = 0;
int score2 = 0;

bool flyUp;


// Start at 0, 0, 0 
//vector<glm::vec3> entites = { glm::vec3(4.0f, 0, 0), glm::vec3(-4.0f,0,0),.... }
glm::vec3 player_position = glm::vec3(4.0f, 0, 0);
glm::vec3 player2_position = glm::vec3(-4.0f, 0, 0);
glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 player_movement = glm::vec3(0, 0, 0);
glm::vec3 player2_movement = glm::vec3(0, 0, 0); 

float player_speed = 1.5f;
float ball_speed = 5.0f;



ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, modelMatrix2, modelMatrix3, projectionMatrix;

void reset() { //reset button to set everything back to original position 
	player_position = glm::vec3(4.0f, 0, 0);
	player2_position = glm::vec3(-4.0f, 0, 0);
	ball_position = glm::vec3(0, 0, 0);
	player_movement = glm::vec3(0, 0, 0);
	player2_movement = glm::vec3(0, 0, 0);

	modelMatrix = glm::mat4(1.0f);
	modelMatrix2 = glm::mat4(1.0f);
	modelMatrix3 = glm::mat4(1.0f);

	bounce = 0;


}

GLuint LoadTexture(const char* filePath) { //loads textures into the game
	//pixel width, height, and depth
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("KIRBY PONG!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	modelMatrix2 = glm::mat4(1.0f);
	modelMatrix3= glm::mat4(1.0f);

	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	//load 3 textures
	kirbyTextureID = LoadTexture("Kirby.png");
	kirbyBlockTextureID = LoadTexture("Kirby Block.png");
	DDDBlockTextureID = LoadTexture("DDD Block.png");

	//we know heights[0] =ball, the rest are the blocks, and the blocks should be the same height

	



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {//tells us either when we end game or if the space key was pressed
	SDL_Event event;
	while (SDL_PollEvent(&event)) {

		player_movement = glm::vec3(0);

		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				start = true;
			}
		}
		
	}

	//reset player movement vector
	player_movement = glm::vec3(0, 0, 0);
	player2_movement = glm::vec3(0, 0, 0);

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	/*if (keys[SDL_SCANCODE_LEFT]) {
		player_movement.x = -1.0f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		player_movement.x = 1.0f;
	}*/
	if (start) {
		//right side player controls
		if (keys[SDL_SCANCODE_UP]) {
			player_movement.y = 1.0f;
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			player_movement.y = -1.0f;
		}
		//left side player controls
		if (keys[SDL_SCANCODE_W]) {
			player2_movement.y = 1.0f;
		}
		else if (keys[SDL_SCANCODE_S]) {
			player2_movement.y = -1.0f;
		}
		//normalize movement vector
		if (glm::length(player_movement) > 1.0f) {
			player_movement = glm::normalize(player_movement);
		}
		if (glm::length(player2_movement) > 1.0f) {
			player2_movement = glm::normalize(player2_movement);
		}
	}
}




void Update() {
		//set up deltaTime
		float tick = (float)SDL_GetTicks() / 1000.f;
		float deltaTime = tick - LastTicks;
		LastTicks = tick;


		//player position adjustments for both left and right
		player_position += player_movement * player_speed * deltaTime;
		player2_position += player2_movement * player_speed * deltaTime;

		//transformations for both players, right and left respectively
		modelMatrix2 = glm::mat4(1.0f);
		modelMatrix2 = glm::translate(modelMatrix2, player_position);
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(1.5f, 1.5f, 0));

		modelMatrix3 = glm::mat4(1.0f);
		modelMatrix3 = glm::translate(modelMatrix3, player2_position);
		modelMatrix3 = glm::scale(modelMatrix3, glm::vec3(1.5f, 1.5f, 0));

		//determining hitboxes of ball and both paddles
		float xdist = fabs(ball_position.x - player_position.x) - ((boxBlockModelx + boxModelx) / 2.0f);
		float x2dist = fabs(ball_position.x - player2_position.x) - ((boxBlockModelx + boxModelx) / 2.0f);

		float ydist = fabs(ball_position.y - player_position.y) - ((boxBlockModely + boxModely) / 2.0f);
		float y2dist = fabs(ball_position.y - player2_position.y) - ((boxBlockModely + boxModely) / 2.0f);

		


		if (xdist < 0 && ydist < 0) {//determining if ball and right paddle have hit
			goLeft = true;
			bounce = abs(ball_position.y - player_position.y);
			if (ball_position.y >= player_position.y) {
				flyUp = true;
			}
			else {
				flyUp = false;
			}
		
		}

		else if (x2dist < 0 && y2dist < 0) {//determining of ball and left paddle have hit
			goLeft = false;
			bounce = abs(ball_position.y - player2_position.y);
			if (ball_position.y >= player2_position.y) {
				flyUp = true;
			}
			else {
				flyUp = false;
			}
		}

		if (start) {//if not started, then the ball won't move, otherwise it will
			if (goLeft) {
				ball_position.x -= ball_speed * deltaTime; //go Left 
				if (flyUp) {
					ball_position.y += bounce * deltaTime;
				}
				else {
					ball_position.y -= abs(bounce)  * deltaTime;
				}
			}
			else {
				ball_position.x += ball_speed * deltaTime; //go Right
				if (flyUp) {
					ball_position.y += bounce * deltaTime;
				}
				else {
					ball_position.y -= abs(bounce)  * deltaTime;
				}

			}

			if (glm::length(ball_position) > 1.0f) {
				player2_movement = glm::normalize(ball_position);
			}
		}

		//ball moving
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, ball_position);

		if (!flyUp && ball_position.y <= edgeBottom) {//if the ball hit the bottom then we make it go hit the top
			flyUp = true;
		}
		else if (flyUp && ball_position.y >= edgeTop ) {//vice versa
			flyUp = false;
		}
		
		if (edgeRight < ball_position.x) {
			score1++;
			reset();
		}
		else if (edgeLeft > ball_position.x) {
			score2++;
			reset();
		}
		if (score1 == 5 || score2 == 5) {//a score of 5 is a win
			start = false;
			score1 = 0;
			score2 = 0;
		}
		


}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	//kirby ball
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0,1.0, 1.0,1.0, 1.0,0.0, 0.0,1.0, 1.0,0.0, 0.0,0.0 };

	float fliptexCoords[] = { 1.0,1.0, 0.0,1.0, 0.0,0.0, 1.0,1.0, 0.0,0.0 , 1.0,0.0 };

	program.SetModelMatrix(modelMatrix);


	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	
	if (goLeft) {
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, fliptexCoords);
	}
	else {
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	}
	glEnableVertexAttribArray(program.texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, kirbyTextureID);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	//kirby block
	float vertices2[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords2[] = { 0.0,1.0, 1.0,1.0, 1.0,0.0, 0.0,1.0, 1.0,0.0, 0.0,0.0 };

	program.SetModelMatrix(modelMatrix2);


	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
	glEnableVertexAttribArray(program.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, kirbyBlockTextureID);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	//DDD block

	float vertices3[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords3[] = { 0.0,1.0, 1.0,1.0, 1.0,0.0, 0.0,1.0, 1.0,0.0, 0.0,0.0 };

	program.SetModelMatrix(modelMatrix3);


	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
	glEnableVertexAttribArray(program.texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, DDDBlockTextureID);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		
		ProcessInput();
		Update();
		Render();
	
	}

	Shutdown();
	return 0;
}