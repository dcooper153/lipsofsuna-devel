/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup limat Math
 * @{
 * \addtogroup limatMatrixMN MatrixMN
 * @{
 */

#ifndef __MATH_MATRIXMN_H__
#define __MATH_MATRIXMN_H__

#include <math.h>
#include <system/lips-system.h>

typedef struct _limatMatrixMN limatMatrixMN;
struct _limatMatrixMN
{
	int rows;
	int cols;
	float* data;
};

static inline limatMatrixMN*
limat_matrixmn_new (int rows,
                    int cols)
{
	limatMatrixMN* self;

	self = (limatMatrixMN*) lisys_calloc (1, sizeof (limatMatrixMN));
	if (self == NULL)
		return NULL;
	self->data = (float*) lisys_calloc (rows * cols, sizeof (float));
	if (self->data == NULL)
	{
		free (self);
		return NULL;
	}
	self->rows = rows;
	self->cols = cols;

	return self;
}

static inline void
limat_matrixmn_free (limatMatrixMN* self)
{
	free (self->data);
	free (self);
}

static inline float
limat_matrixmn_get (limatMatrixMN* self,
                    int            row,
                    int            col)
{
	return self->data[col + row * self->cols];
}

static inline void
limat_matrixmn_set (limatMatrixMN* self,
                    int            row,
                    int            col,
                    float          value)
{
	self->data[col + row * self->cols] = value;
}

static inline limatMatrixMN*
limat_matrixmn_get_col (limatMatrixMN* self,
                        int            col)
{
	int i;
	limatMatrixMN* result;

	result = limat_matrixmn_new (self->rows, 1);
	if (result == NULL)
		return NULL;

	for (i = 0 ; i < self->rows ; i++)
		limat_matrixmn_set (result, i, 0, limat_matrixmn_get (self, i, col));

	return result;
}

static inline void
limat_matrixmn_set_col (limatMatrixMN* self,
                        int            col,
                        float*         data)
{
	int i;

	for (i = 0 ; i < self->rows ; i++)
		self->data[col + i * self->cols] = data[i];
}

static inline limatMatrixMN*
limat_matrixmn_get_col_range (limatMatrixMN* self,
                              int            col,
                              int            row_min,
                              int            row_max)
{
	int i;
	limatMatrixMN* result;

	result = limat_matrixmn_new (row_max - row_min, 1);
	if (result == NULL)
		return NULL;

	for (i = 0 ; i < row_max - row_min ; i++)
		limat_matrixmn_set (result, i, 0, limat_matrixmn_get (self, i + row_min, col));

	return result;
}

static inline void
limat_matrixmn_set_col_range (limatMatrixMN* self,
                              int            col,
                              int            row_min,
                              int            row_max,
                              float*         data)
{
	int i;

	for (i = row_min ; i < row_max ; i++)
		self->data[col + i * self->cols] = data[i - row_min];
}

static inline void
limat_matrixmn_set_col_zero (limatMatrixMN* self,
                             int            col)
{
	int i;

	for (i = 0 ; i < self->rows ; i++)
		self->data[col + i * self->cols] = 0.0f;
}

static inline limatMatrixMN*
limat_matrixmn_get_row (limatMatrixMN* self,
                        int            row)
{
	int i;
	limatMatrixMN* result;

	result = limat_matrixmn_new (1, self->cols);
	if (result == NULL)
		return NULL;

	for (i = 0 ; i < self->cols ; i++)
		limat_matrixmn_set (result, 0, i, limat_matrixmn_get (self, row, i));

	return result;
}

static inline void
limat_matrixmn_set_row (limatMatrixMN* self,
                        int            row,
                        float*         data)
{
	int i;

	for (i = 0 ; i < self->rows ; i++)
		self->data[i + row * self->cols] = data[i];
}

static inline void
limat_matrixmn_set_row_range (limatMatrixMN* self,
                              int            row,
                              int            col_min,
                              int            col_max,
                              float*         data)
{
	int i;

	for (i = col_min ; i < col_max ; i++)
		self->data[i + row * self->cols] = data[i - col_min];
}

static inline void
limat_matrixmn_set_row_zero (limatMatrixMN* self,
                             int            row)
{
	int i;

	for (i = 0 ; i < self->rows ; i++)
		self->data[i + row * self->cols] = 0.0f;
}

static inline limatMatrixMN*
limat_matrixmn_multiply (limatMatrixMN* self,
                         limatMatrixMN* matrix)
{
	int i;
	int j;
	int k;
	float v;
	limatMatrixMN* result;

	assert (self->cols == matrix->rows);
	result = limat_matrixmn_new (self->rows, matrix->cols);
	if (result == NULL)
		return NULL;

	for (i = 0 ; i < self->rows ; i++)
	for (j = 0 ; j < matrix->cols ; j++)
	{
		v = 0.0f;
		for (k = 0 ; k < self->cols ; k++)
		{
			v += limat_matrixmn_get (self, i, k) *
			     limat_matrixmn_get (matrix, k, j);
		}
		limat_matrixmn_set (result, i, j, v);
	}

	return result;
}

static inline void
limat_matrixmn_multiply_by_scalar (limatMatrixMN* self,
                                 float          scalar)
{
	int i;
	int j;

	for (i = 0 ; i < self->rows ; i++)
	for (j = 0 ; j < self->cols ; j++)
	{
		limat_matrixmn_set (self, i, j,
			scalar * limat_matrixmn_get (self, i, j));
	}
}

static inline limatMatrixMN*
limat_matrixmn_transpose (limatMatrixMN* self)
{
	int i;
	int j;
	limatMatrixMN* result;

	result = limat_matrixmn_new (self->cols, self->rows);
	if (result == NULL)
		return NULL;

	for (i = 0 ; i < self->rows ; i++)
	for (j = 0 ; j < self->cols ; j++)
	{
		limat_matrixmn_set (result, j, i,
			limat_matrixmn_get (self, i, j));
	}

	return result;
}

#if 0
static inline int
limat_matrixmn_qr_decomposition (limatMatrixMN*  self,
                                 limatMatrixMN** q,
                                 limatMatrixMN** r)
{
	int i;
	int j;
	int k;
	int row;
	int col;
	float e;
	float* a;

	qn = limat_matrixmn_new (self->cols, self->cols);

	/* Allocate memory. */
	a = calloc (self->rows, sizeof (float));
	if (a == NULL)
		return 0;
	*q = limat_matrixmn_new (self->rows, self->cols);
	if (*q == NULL)
	{
		free (a);
		return 0;
	}
	if (r != NULL)
	{
		*r = limat_matrixmn_new (self->rows, self->cols);
		if (*r == NULL)
		{
			limat_matrixmn_free (*q);
			free (a);
			return 0;
		}
	}
	memcpy ((*q)->data, self->data, self->rows * self->cols * sizeof (float));

	/* Initialize Q as identity matrix. */
	for (rol = 0 ; rol < self->rols ; row++)
	for (col = 0 ; col < self->cols ; col++)
	{
		limat_matrixmn_set (*q, row, col, row == col);
	}

	/* Calculate Q with Householder transformations. */
	for (row = 0 ; row < self->rows ; row++)
	{
		/* Calculate Q_n * A */

		/* Calculate u_n and e_n. */
		for (e = 0.0f, col = row ; col < self->cols ; col++)
		{
			a[col] = limat_matrixmn_get (self, row, col);
			e += a[col] * a[col];
		}
		e = sqrt (e);
		a[row] -= e;

		/* Calculate ||u_n|| and v_n. */
		for (n = 0.0f, col = row ; col < self->cols ; col++)
			n += a[col] * a[col];
		if (n >= 0.0f)
		{
			n = 1.0f / n;
			for (col = row ; col < self->cols ; col++)
				a[col] *= n;
		}

		/* Calculate Q_n = I - 2 * v_n * v_n^T */
		for (j = 0 ; j < row ; j++)
		for (i = 0 ; i < row ; i++)
		{
			limat_matrixmn_set (qn, i, j, (i == j));
		}
		for (j = row ; j < self->cols ; j++)
		for (i = row ; i < self->cols ; i++)
		{
			limat_matrixmn_set (qn, i, j, (i == j) - 2.0f * a[i] * a[j]);
		}

		/* Calculate Q = Q_1^T * Q_2^T * ... */
		memcpy (qt->data, (*q)->data, qt->rows * qt->cols * sizeof (float));
		for (i = 0 ; i < self->cols ; i++)
		for (j = 0 ; j < self->cols ; j++)
		for (k = 0 ; k < self->cols ; k++)
		{
			limat_matrixmn_set (qt, i, j,
				limat_matrixmn_get (qt, i, j) +
				limat_matrixmn_get (qn, i, k) *
				limat_matrixmn_get (*q, k, j));
		}
		memcpy ((*q)->data, qt->data, qt->rows * qt->cols * sizeof (float));
	}

	/* Calculate R = Q^T * A */
	if (r != NULL)
	{
		for (i = 0 ; i < self->rows ; i++)
		for (j = 0 ; j < matrix->cols ; j++)
		for (k = 0 ; k < self->cols ; k++)
		{
			limat_matrixmn_set (*r, i, j,
				limat_matrixmn_get (*r, i, j) +
				limat_matrixmn_get (*q, k, i) *
				limat_matrixmn_get (self, k, j));
		}
	}

	return 1;
}
#endif

static inline void
limat_matrixmn_swap_cols (limatMatrixMN* self,
                          int            col0,
                          int            col1)
{
	int i;
	float tmp;

	for (i = 0 ; i < self->rows ; i++)
	{
		tmp = self->data[col0 + i * self->cols];
		self->data[col0 + i * self->cols] = self->data[col1 + i * self->cols];
		self->data[col1 + i * self->cols] = tmp;
	}
}

static inline void
limat_matrixmn_swap_rows (limatMatrixMN* self,
                          int            row0,
                          int            row1)
{
	int i;
	float tmp;

	for (i = 0 ; i < self->rows ; i++)
	{
		tmp = self->data[i + row0 * self->cols];
		self->data[i + row0 * self->cols] = self->data[i + row1 * self->cols];
		self->data[i + row1 * self->cols] = tmp;
	}
}

#endif

/** @} */
/** @} */
