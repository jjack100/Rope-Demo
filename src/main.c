#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<assert.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>

#include "shader.h"
#include "physmath.h"

#define TOTAL_SEGMENTS (2)
#define SEGMENT_LENGTH (0.4f)
#define THICKNESS (0.01f)
#define GRAVITY (0.6) //In half-window-heights per second^2
typedef struct point {
	double x;
	double y;
} point;

typedef struct segment {
	double angle; /*Angle in radians from the vertical, ranging -pi to pi*/
	double angVelocity; /*Angular velocity*/
	double mass;
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
	double *a = malloc(sizeof(double));
	double b = 3;
	expression *expr = malloc(sizeof(expression));
	expr->type = POWER;
	expr->value.operands[0] = malloc(sizeof(expression));
	expr->value.operands[1] = malloc(sizeof(expression));
	expr->value.operands[0]->type = VAR;
	expr->value.operands[0]->value.variable = a;
	expr->value.operands[1]->type = CONST;
	expr->value.operands[1]->value.constant = b;

	*a = 4;
	expression *deriv = differentiate(expr, a);
	printf("%.2f\n", evaluate(deriv));


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
	segments[0].angle = M_PI * (3./4.);
	segments[1].angle = M_PI * (3./4.);
	segments[0].mass = 1;
	segments[1].mass = 1;
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
	
		double m1 = segments[0].mass;
		double m2 = segments[1].mass;
		double a1 = segments[0].angle;
		double a2 = segments[1].angle;
		double v1 = segments[0].angVelocity;
		double v2 = segments[1].angVelocity;
		double l = SEGMENT_LENGTH;

		double mat[2][3] = {(m1 + m2) * l, m2 * l * cos(a1 - a2), -m2*l*v2*v2*sin(a1-a2) - (m1+m2)*GRAVITY*sin(a1),
				    m2 * l * cos(a1 - a2), m2 * l, + m2*l*v1*v1*sin(a1-a2) - m2*GRAVITY*sin(a2)};
		gaussElim(2, mat);
		for (int i = 0; i < TOTAL_SEGMENTS; i++) {
			segments[i].angVelocity += mat[i][TOTAL_SEGMENTS] * timeDiff;
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
