#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "physmath.h"
/*
 * Transforms the given augmented matrix into reduced row echelon form
 * using gaussian elimination
 */
void gaussElim(int equations, double augMat[][equations + 1]) {
	/* forward elimination */
	for (int row = 0; row < equations - 1; row++) {
		for (int subrow = row + 1; subrow < equations; subrow++) {
			double multiplier = -augMat[subrow][row] / augMat[row][row];
			augMat[subrow][row] = 0;
			for (int col = row + 1; col < equations + 1; col++)
				augMat[subrow][col] += augMat[row][col] * multiplier;
		}
	}

	/* back substitution */
	for (int row = 0; row < equations; row++) {
		for (int subrow = row + 1; subrow < equations; subrow++) {
			double multiplier = -augMat[row][subrow] / augMat[subrow][subrow];
			for (int col = row; col < equations + 1; col++)
				augMat[row][col] += augMat[subrow][col] * multiplier;
			augMat[row][subrow] = 0;
	}
		double leadingCoeff = augMat[row][row];
		for (int col = row; col < equations + 1; col++)
			augMat[row][col] /= leadingCoeff;	
	}
}


/*
 * Recursively evaluates the expression and returns the result. 
 * The value of a variable is taken to be the value referenced by the pointer which represents it. 
 */
double evaluate(expression *input) {
	switch(input->type) {
		case CONST:
			return input->value.constant;
		case VAR:
			return *(input->value.variable);
		case SUM:
			return evaluate(input->value.operands[0]) + evaluate(input->value.operands[1]);
		case PRODUCT:
			return evaluate(input->value.operands[0]) * evaluate(input->value.operands[1]);
		case POWER:
			return pow(evaluate(input->value.operands[0]), evaluate(input->value.operands[1]));
		case SIN:
			return sin(evaluate(input->value.operands[0]));
		case COS:
			return cos(evaluate(input->value.operands[0]));
	}
	return 0.;
}

/*
 * Finds the derivative of the given expression with respect to the given variable and
 * returns a pointer to it.
 */
expression * differentiate (expression *input, double *respectTo) {
	expression *output = malloc(sizeof(expression));
	switch(input->type) {
		case CONST:
			output->type = CONST;
			output->value.constant = 0.;
			break;
		case VAR:
			output->type = CONST;
			if (input->value.variable == respectTo) 
				output->value.constant = 1.;
			else 
				output->value.constant = 0.;				
			break;
		case SUM:
			output->type = SUM;
			output->value.operands[0] = differentiate(input->value.operands[0], respectTo);
			output->value.operands[1] = differentiate(input->value.operands[1], respectTo);
			break;
		case PRODUCT: 
			/* The product rule: (fg)' = fg' + f'g */
			expression *term1 = malloc(sizeof(expression));
			expression *term2 = malloc(sizeof(expression));
			term1->type = PRODUCT;
			term1->value.operands[0] = input->value.operands[0];
			term1->value.operands[1] = differentiate(input->value.operands[1], respectTo);
			term2->type = PRODUCT;
			term2->value.operands[0] = differentiate(input->value.operands[0], respectTo);
			term2->value.operands[1] = input->value.operands[1];
			output->type = SUM;
			output->value.operands[0] = term1;
			output->value.operands[1] = term2;
			break;
		case POWER:
			assert(input->value.operands[1]->type == CONST); /* Assume exponent is a constant */

			expression *coeff = malloc(sizeof(expression));
			expression *base  = malloc(sizeof(expression));
			expression *exponent  = malloc(sizeof(expression));
			expression *term  = malloc(sizeof(expression)); /* the sub-expression of base^exponent */
			expression *outerTerm; /* Only used if the chain rule is needed */
			output->type = PRODUCT; 

			/* coeff should be the old exponent because of the power rule */
			coeff->type = CONST;
			coeff->value.constant = input->value.operands[1]->value.constant;	
			exponent->type = CONST;
			exponent->value.constant = input->value.operands[1]->value.constant - 1;	

			/* If the base is a variable, only perform power rule */
			if (input->value.operands[0]->type == VAR) {
				base->type = VAR;
				base->value.variable = input->value.operands[0]->value.variable;
				term->type = POWER;
				term->value.operands[0] = base;
				term->value.operands[1] = exponent;
				output->value.operands[0] = coeff;
			}
			else { /* Perform chain rule if nested expression */
				outerTerm = malloc(sizeof(expression));
				outerTerm->type = PRODUCT;
				outerTerm->value.operands[0] = coeff;
				outerTerm->value.operands[1] = differentiate(input->value.operands[0], respectTo);
				output->value.operands[0] = outerTerm;
			}
			output->value.operands[1] = term;
	}
	return output;
	
	/*
	 * Frees the memory of an expression pointer and recursively frees
	 * the memory of all nested expressions
	 */
	void freeExpr(expression *expr) {
		if (expr->type = VAR) {
			free(expr->value.variable);
			expr->value.variable = NULL;
		}
		else if (expr->type == SIN || expr->type == COS)
		/* for unary operators, only free the first arg */
			freeExpr(expr->value.operands[0]);
		else if (expr->type == SUM || expr->type == PRODUCT || expr->type == POWER) {
		/* for binary operators, free both */
			freeExpr(expr->value.operands[0]);
			freeExpr(expr->value.operands[1]);
		}
		
		if (expr->type != CONST) {
			free(expr);
			expr = NULL;
		}
	}
}
