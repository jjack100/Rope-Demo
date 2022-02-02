#include <stdlib.h>
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
