/*
 * Represents a mathematical expression as a tree structure to be used for symbolic
 * computation. Expressions should be dynamically allocated so that nested expressions
 * can be recursively freed. Variables are represented by double pointers and should
 * be dynamically allocated or set to NULL before freeing the parent expression.
 */
typedef struct expression {
	enum type{ 
		CONST, VAR, SUM, PRODUCT, POWER, SIN, COS
	}type;
	union value {
		double constant;
		double *variable;
		struct expression *operands[2];
	}value;
}expression;

double evaluate(expression *input);

expression * differentiate(expression *input, double *respectTo);

void gaussElim(int equations, double augMat[][equations]);
