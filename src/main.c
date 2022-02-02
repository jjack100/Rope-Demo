#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<assert.h>
#include<math.h>
#include<stdio.h>

#include "shader.h"
#include "physmath.h"

#define TOTAL_SEGMENTS (3)
#define SEGMENT_LENGTH (0.2f)
#define THICKNESS (0.01f)
#define GRAVITY (-0.5) //In half-window-heights per second^2
typedef struct point {
	double x;
	double y;
} point;

typedef struct segment {
	double angle; /*Angle in radians from the vertical, ranging -pi to pi*/
	double angVelocity; /*Angular velocity*/
} segment;

// Vertex coordinates
GLfloat vertices[TOTAL_SEGMENTS * 12] = {0};


/*
 * Updates polygon vertices for rendering the given chain
 */
void vertsFromChain (point pivot, segment *segments) {
	assert (sizeof(vertices) == sizeof(vertices[0]) * TOTAL_SEGMENTS * 12);
	for (int i = 0; i < TOTAL_SEGMENTS; i++) {
		double ang = segments[i].angle;

		vertices[(i * 12) + 0]  = (float) (pivot.x + cos(ang) * THICKNESS);
		vertices[(i * 12) + 1]  = (float) (pivot.y + sin(ang) * THICKNESS);
		
		vertices[(i * 12) + 3]  = (float) (pivot.x - cos(ang) * THICKNESS);
		vertices[(i * 12) + 4]  = (float) (pivot.y - sin(ang) * THICKNESS);

		pivot.x +=  sin(ang) * SEGMENT_LENGTH;
		pivot.y += -cos(ang) * SEGMENT_LENGTH;

		vertices[(i * 12) + 6]  = (float) (pivot.x + cos(ang) * THICKNESS);
		vertices[(i * 12) + 7]  = (float) (pivot.y + sin(ang) * THICKNESS);
		
		vertices[(i * 12) + 9]  = (float) (pivot.x - cos(ang) * THICKNESS);
		vertices[(i * 12) + 10] = (float) (pivot.y - sin(ang) * THICKNESS);
	}	
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(800, 800, "test", NULL, NULL);

	// Error check for failure to create window
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, 800, 800);
	
	GLuint shaderProgram = 0;
	buildShaderProgram( &shaderProgram );

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	point pivot = {0}; /* starting point of the chain */
	segment segments[TOTAL_SEGMENTS] = {0};
	segments[0].angle = M_PI * (3. / 4.);
	vertsFromChain(pivot, segments);

	// Main loop
	double oldTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		double timeDiff = glfwGetTime() - oldTime; //time in seconds since last iteration
		oldTime = glfwGetTime();

		//set bg color
		glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		
		segments[0].angVelocity += (GRAVITY / SEGMENT_LENGTH) *sin(segments[0].angle) * 0.2;
		for (int i = 0; i < TOTAL_SEGMENTS; i++) {
			segments[i].angle += segments[i].angVelocity * timeDiff;
		}
		vertsFromChain(pivot, segments);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, TOTAL_SEGMENTS * 4);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
