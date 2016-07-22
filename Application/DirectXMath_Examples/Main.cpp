#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>

/* XMVerifyCPUSupport*/
#include <Windows.h>

/* This is DirectX main math library, which uses SIMD 128-bit instructions, for 4D vectors. */
#include <DirectXMath.h>

/* Additional data types */
#include <DirectXPackedVector.h>

/* 
	
	Notes - Read before moving forward
	-----

	* LENGTH(v1)*LENGTH(v2)*cos(t) = DOTP(v1,v2) hence t = arccos(dotp(v1,v2)/LENGTH(v1)*LENGTH(v2))
	  trig is expensive <-- this observation is for you to keep in mind.

	* Always include Windows.h to verify XM support with XMVerifyCPUSupport before using any DirectXMath functions.
	
	* Always enable Enhanced Instruction Set and Fast Floating Point arithmetic in every C++ project which uses DirectXMath

	* Main type: XMVector - 128-bit struct defined in DirectXMath
	
	* To take advantage of SIMD (using SSE/SSE2 registers), every vector must be a 16 bytes (128-bit) aligned XMVector/XMMatrix 
	(relly important optimization):
		url: https://msdn.microsoft.com/en-us/library/windows/desktop/ee418728(v=vs.85).aspx
		General: for every function you implement with XM<something> parameters, always include the XM_CALLCONV 
		XMVector:
			A.
				When using as a local, global variable: XMVector is ok
				When using as a class member: XMFLOAT2, XMFLOAT3 or XMFLOAT4 as needed.
					i. These are to be converted into XMVector type
					This is achieve by usign loading/storage functions XMFLOAT(k) <> XMVECTOR
					e.g.
						XMVECTOR v = XMLoadVector2(&xfloat2Vector)
			B.	Do all computations using the loaded XMVECTOR instance.
	
	* Parameters - When using XM_CALLCONV and passing XMVECTOR or XMMATRIX as parameters to functions (each 16-bytes aligned)
			A. 
				use FXMVECTOR for the first 3
				use GXMVECTOR for the 4th one
				use HXMVECTOR for the 5-6th
				use CXMVECTOR for the rest
			B.
				use FXMMATRIX for the first matrix - it counts as 4 XMVECTORS
				use CXMMATRIX for the second and on - AND FOR CONSTRUCTORS
*/

using namespace std;

/* Invoke main namespaces */
using namespace DirectX;
using namespace DirectX::PackedVector;

/* Forward references */
void PrintVectors(std::vector<XMVECTOR>);
bool IsParallel(XMVECTOR, XMVECTOR);
float GetVectorLength(XMVECTOR);
float GetDotProd(XMVECTOR, XMVECTOR);
XMVECTOR GetNormalizedVector(XMVECTOR);
XMVECTOR GetOrthogonal2DVector(XMVECTOR);
XMVECTOR GetProjectedVector(XMVECTOR, XMVECTOR);
std::vector<XMVECTOR> GramSchmidt(vector<XMVECTOR>);
XMMATRIX XM_CALLCONV GetMatrixTranspose(const FXMMATRIX*);
XMMATRIX XM_CALLCONV GetMatrixMinor(const XMMATRIX*, int, int);
float XM_CALLCONV GetMatrixDeterminant(const XMMATRIX*, int);
XMMATRIX XM_CALLCONV GetMatrixAdjoint(const XMMATRIX*,int);
XMMATRIX XM_CALLCONV GetMatrixInverse(const XMMATRIX*,int);

/* Util */
float getFloatAt2(float f) {
	return round(f * 100) / 100;
}
/* -- */

// overloading cout for printing vectors
ostream& XM_CALLCONV operator<< (ostream& os, FXMVECTOR v) {
	XMFLOAT4 v3;
	XMStoreFloat4(&v3, v);
	os << "(" << getFloatAt2(v3.x) << "," << getFloatAt2(v3.y) << "," << getFloatAt2(v3.z) << "," << v3.w << ")";
	return os;
}

ostream& XM_CALLCONV operator<< (ostream& os, FXMMATRIX m) {
	int rows = sizeof(m.r) / sizeof(XMVECTOR);
	os << endl;
	for (int i = 0; i < rows; i++) {
		os << "|\t" << getFloatAt2(XMVectorGetX(m.r[i]))
			<< "\t" << getFloatAt2(XMVectorGetY(m.r[i]))
			<< "\t" << getFloatAt2(XMVectorGetZ(m.r[i]))
			<< "\t" << getFloatAt2(XMVectorGetW(m.r[i])) << "\t|" << endl;
	}
	return os;
}

	/* 0. Defyning Constant Vectors */
		// always use the 16-bytes aligned XMVECTOR32
static const XMVECTORF32 zeroVector = { 0.0f,0.0f,0.0f,0.0f };

void main() {

	cout << "Welcome, we will verify system support for SIMD instructions ... " << endl;
	char c = 'c';

	/* Examples */

	// check the current CPU supports SIMD 
	if (XMVerifyCPUSupport()) {

		cout << "\nYour system does support the DirectX's SIMD set :) ...\n";

#pragma region Vectors
		/* 1. XMVECTOR, FXMVECTOR and XMFLOAT<k> */
		//	use these XMFLOAT<k> for data members
		XMFLOAT3 v3d = XMFLOAT3(0, 0, 0);
		// always Load/Store XMFLOAT<k> into XMVECTOR to use SIMD sets
		// the same applies to other types
		// XMFLOAT<k>	--> XMVECTOR
		XMVECTOR vector = XMLoadFloat3(&v3d);
		// FXMVECTOR should be used for 32/64-bit systems compatibility.
		FXMVECTOR vectorF = XMLoadFloat3(&v3d);
		// XMVECTOR		--> XMFLOAT<k>
		XMFLOAT3 v3d2;
		XMStoreFloat3(&v3d2, vectorF);

		// single components manipulation
		float vectorX = XMVectorGetX(vectorF);
		XMVectorSetX(vector, vectorX);

		/* 2. Parameters rules - IMPORTANT especially for XMMATRIX functions */
		// use FXMVECTOR for the first 3
		// use GXMVECTOR for the 4th one
		// use HXMVECTOR for the 5-6th
		// use CXMVECTOR for the rest

		/* 3. Available Operators */
		// Add, subtraction and scalar multiplication/division
		XMFLOAT3 vf1 = { 1.0f,1.0f,1.0f };
		XMFLOAT3 vf2 = { 2.0f,2.0f,2.0f };
		XMVECTOR v1 = XMLoadFloat3(&vf1);
		FXMVECTOR v2 = XMLoadFloat3(&vf2);
		float scalar = 1.0f;
		v1 = v1 + v2;
		v1 = v1 - v2;
		v1 += v1;
		v1 -= v1;
		v1 = scalar * v1;
		v1 = v1 / scalar;
		cout << "\nv1: " << v1 << endl;
		cout << "v2: " << v2 << "\n\n";
		
		/* 4. Given constants and degree conversions */
		XM_PI;	XM_2PI;
		XM_1DIVPI; XM_1DIV2PI;
		XM_PIDIV2; XM_PIDIV4;
		XMConvertToRadians(360);
		XMConvertToDegrees(XM_PI);

		/* 5. Some utility setter functions */

			// { 0,0,0,0 }
		cout << "Zero vector: " << XMVectorZero() << endl;
			// { 1,1,1,1 }
		cout << "Splay 1 vector: " << XMVectorSplatOne() << endl;						
			// initializes and returns a new vector with x,y,z,w values
		cout << "New vector: " << XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f) << endl;	
			// { 1.0f ... all the same }
		XMVectorReplicate(1.0f);
			// all values of the returned vector will have 1.0f
			// same for XMVectorSplat[x,y,z](XMVECTOR);
		XMVectorSplatX(XMVectorSplatOne());		

		/* 6. Some examples:		        */
			// Normalization
		XMVECTOR norm = GetNormalizedVector(v2);
		cout << "Length of v2: \t" << GetVectorLength(v2) << endl;
		cout << "Normalized v2: \t" << norm << endl;
			// 2D Orthogonal
		cout << "2D Orthogonal to: (1,0) is : " << GetOrthogonal2DVector(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)) << endl;
			// Projection
		XMFLOAT2 f2 = { 2.0f,-5.0f };
		XMFLOAT2 f22 = { 5.0f,1.0f };
		XMVECTOR u = XMLoadFloat2(&f2);
		XMVECTOR v = XMLoadFloat2(&f22);
		cout << "v2: " << u
			 << " projected on v1: " << v
			 << " is: " << GetProjectedVector(v, u) << endl;
			// Gram-Schmidt Orthogonalization
		XMVECTOR gv1 = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
		XMVECTOR gv2 = XMVectorSet(1.0f, 2.0f, 0.0f, 0.0f);
		XMVECTOR gv3 = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		std::vector<XMVECTOR> vecs;
		vecs.push_back(gv1); vecs.push_back(gv2); vecs.push_back(gv3);
		cout << "Gram-Schmidt " << vecs.size() << " vectos: ";
		PrintVectors(vecs);
		vecs = GramSchmidt(vecs);
		cout << "Orthagonalized vectors: " << endl;
		PrintVectors(vecs);
#pragma endregion

#pragma region Matrices

		/* We will mostly use 4x4 matrices, you will see why ... */
		cout << endl << "Matrices: \n\n";
		/* 0. NON SIMD matrix, just as XMFLOATx struct */

		XMFLOAT4X4 m1 = {	1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f };

		/* 1. Load and Store */

		XMMATRIX m = XMLoadFloat4x4(&m1);
		XMStoreFloat4x4(&m1, m);

		/* 3. XMMatrix uses SIMD with 4 XMVECTORS in it - Some examples */

		XMMATRIX matrix = XMMatrixSet(	1.0f,  2.0f,  3.0f,  4.0f,
										5.0f,  6.0f,  7.0f,  8.0f,
										9.0f,  10.0f, 11.0f, 12.0f,
										13.0f, 14.0f, 15.0f, 16.0f);
		XMMATRIX matrix2 = XMMatrixSet(	2.0f, -5.0f, 3.0f, 0.0f,
										1.0f, 3.0f, 4.0f, 0.0f,
										-2.0f, 3.0f, 7.0f, 0.0f,
										0.0f, 0.0f, 0.0f, 0.0f);
		/* 4. Examples */
		cout << "\nCreated matrix m: " << matrix;
		cout << "\nm + m: " << (matrix + matrix);
		cout << "\nm * m: " << (matrix * matrix);
		cout << "\nm / 2: " << (matrix / 2);
		int cols = (sizeof(matrix) / sizeof(XMVECTOR));
		int rows = (sizeof(matrix.r) / sizeof(XMVECTOR));
		cout << "Size - rows/cols: " << rows << "x" << cols << endl;
		XMMATRIX t = GetMatrixTranspose(&matrix);
		cout << "Original: " << matrix;
		cout << "Transposed: " << t;
		cout << "\nm: " << matrix;
		cout << "\nDeterminant of m: \t" << XMVectorGetX(XMMatrixDeterminant(matrix)) << endl;
		cout << "\nm2: " << matrix2;
		cout << "\nDeterminant of m2 (our implementation): \t" << GetMatrixDeterminant(&matrix2, 3);
		XMMATRIX adjExample = XMMatrixSet(	1.0f, 2.0f, 3.0f, 0.0f,
											0.0f, 4.0f, 5.0f, 0.0f,
											1.0f, 0.0f, 6.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 0.0f);
		cout << "\nAdjoint of m: " << GetMatrixAdjoint(&adjExample,3);
		XMMATRIX invExample = XMMatrixSet(	4.0f, 3.0f, 0.0f, 0.0f,
											3.0f, 2.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 0.0f);
		cout << "Inverse of " << invExample << " is: " << GetMatrixInverse(&invExample, 2);
		
#pragma endregion

#pragma region Transformations

		// simple triangle
		XMVECTOR pa = XMVectorSet(1.0, 0.0, 0.0, 1.0);
		XMVECTOR pb = XMVectorSet(-1.0, 0.0, 0.0, 1.0);
		XMVECTOR pc = XMVectorSet(0.0, 1.0, 0.0, 1.0);
		cout << "Point a is: " << pa << endl;
		cout << "Point b is: " << pb << endl;
		cout << "Point c is: " << pc << "\n\n";
		
		// scale them
		XMMATRIX scaleMatrix = XMMatrixScaling(2.0, 2.0, 2.0);
		XMMATRIX rotationMatrix = XMMatrixRotationZ(XM_PI/2);
		cout << "Scaling matrix: " << scaleMatrix << "\n\n";
		pa = XMVector4Transform(pa, scaleMatrix);
		pb = XMVector4Transform(pb, scaleMatrix);
		pc = XMVector4Transform(pc, scaleMatrix);
		cout << "Point a is: " << pa << endl;
		cout << "Point b is: " << pb << endl;
		cout << "Point c is: " << pc << "\n\n";
		cout << "Rotation matrix (Z,90) (counter clockwise): " << rotationMatrix << "\n\n";
		pa = XMVector4Transform(pa, rotationMatrix);
		pb = XMVector4Transform(pb, rotationMatrix);
		pc = XMVector4Transform(pc, rotationMatrix);
		cout << "Point a is: " << pa << endl;
		cout << "Point b is: " << pb << endl;
		cout << "Point c is: " << pc << endl;

#pragma endregion

	}
	else {
		cout << "\nYour system does not support the DirectX's SIMD set :( ...";
	}

	cout << "\n\nGood bye!\n" << endl;
	system("PAUSE");

}

/* For any vector v, any vector obtained from kv is parallel to v */
bool IsParallel(XMVECTOR v1, XMVECTOR v2) {
	float k = XMVectorGetX(v1) / XMVectorGetX(v2);
	return (XMVectorGetY(v1) / XMVectorGetY(v2) == k)
		&& (XMVectorGetZ(v1) / XMVectorGetZ(v2) == k)
		&& (XMVectorGetW(v1) / XMVectorGetW(v2) == k);
}

/* SQRT(v1^2 + .. + vi^2) */
float GetVectorLength(XMVECTOR v) {
	return XMVectorGetX(XMVector3Length(v));
}

float GetDotProd(XMVECTOR v1, XMVECTOR v2) {
	return XMVectorGetX(v1) * XMVectorGetX(v2)
		+ XMVectorGetY(v1) * XMVectorGetY(v2)
		+ XMVectorGetZ(v1) * XMVectorGetZ(v2)
		+ XMVectorGetW(v1) * XMVectorGetW(v2);
}

/* Unit vector (of length 1) obtained from v / LENGTH(v) */
XMVECTOR GetNormalizedVector(FXMVECTOR v) {
		// question for you: do you need to normalize a normalized vector?
	XMVECTOR l = XMVector4Length(v);
	return		XMVectorSet(	XMVectorGetX(v) / XMVectorGetX(l), 
								XMVectorGetY(v) / XMVectorGetY(l),
								XMVectorGetZ(v) / XMVectorGetZ(l),
								XMVectorGetW(v) / XMVectorGetW(l));
}

/* Given a 2D vector v, v' = (-b,a) is always orthogonal to v */
XMVECTOR GetOrthogonal2DVector(XMVECTOR v) {
	return XMVectorSet(		XMVectorGetY(v)  != 0.0 ? -1*XMVectorGetY(v) : 0.0,
							XMVectorGetX(v),
							0.0f,
							0.0f
	);
}

/* Obtain the projection of vector v2 on v1 --> DOT(v1,v2)*v2 where DOT(v1,v2) == k of p = kn with n being unit vector */
XMVECTOR GetProjectedVector(XMVECTOR v1, XMVECTOR v2) {
	XMVECTOR v1Norm = GetNormalizedVector(v1);
	float dotP = GetDotProd(v1Norm, v2);
	return dotP*v1Norm;
}

std::vector<XMVECTOR> GramSchmidt(vector<XMVECTOR> vectors) {
	std::vector<XMVECTOR> orth;
	// let w0 <= (unit vector == v1 / ||v1||)
	orth.push_back(GetNormalizedVector(vectors[0]));
	// for each vector wi , 1 <= i < n
	for (int i = 1; i < vectors.size(); ++i) {
		// wi <- vi - proj(wi-1) - ... - proj(w0)
		XMVECTOR w = vectors[i];
		for (int j = 0; j < i; ++j) {
			w -= GetProjectedVector(orth[j], w);
		}
		orth.push_back(w);
	}
	orth[0] = vectors[0];
	return orth;
}

void PrintVectors(std::vector<XMVECTOR> vs) {
	for (int i = 0; i < vs.size(); ++i) {
		cout << (i != 0 ? ", " : "") << vs[i];
	}
	cout << endl;
}

/* Ony for 4x4 XMMatrix */
XMMATRIX XM_CALLCONV GetMatrixTranspose(const FXMMATRIX* m) {
	XMMATRIX t = XMMatrixSet(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);
	int rows = (sizeof(m->r) / sizeof(XMVECTOR));
	for (int i = 0; i < rows; i++) {
		switch (i) {
		case 0:
			t.r[0] = XMVectorSetX(t.r[0], XMVectorGetX(m->r[i]));
			t.r[1] = XMVectorSetX(t.r[1], XMVectorGetY(m->r[i]));
			t.r[2] = XMVectorSetX(t.r[2], XMVectorGetZ(m->r[i]));
			t.r[3] = XMVectorSetX(t.r[3], XMVectorGetW(m->r[i]));
			break;
		case 1:
			t.r[0] = XMVectorSetY(t.r[0], XMVectorGetX(m->r[i]));
			t.r[1] = XMVectorSetY(t.r[1], XMVectorGetY(m->r[i]));
			t.r[2] = XMVectorSetY(t.r[2], XMVectorGetZ(m->r[i]));
			t.r[3] = XMVectorSetY(t.r[3], XMVectorGetW(m->r[i]));
			break;
		case 2:
			t.r[0] = XMVectorSetZ(t.r[0], XMVectorGetX(m->r[i]));
			t.r[1] = XMVectorSetZ(t.r[1], XMVectorGetY(m->r[i]));
			t.r[2] = XMVectorSetZ(t.r[2], XMVectorGetZ(m->r[i]));
			t.r[3] = XMVectorSetZ(t.r[3], XMVectorGetW(m->r[i]));
			break;
		case 3:
			t.r[0] = XMVectorSetW(t.r[0], XMVectorGetX(m->r[i]));
			t.r[1] = XMVectorSetW(t.r[1], XMVectorGetY(m->r[i]));
			t.r[2] = XMVectorSetW(t.r[2], XMVectorGetZ(m->r[i]));
			t.r[3] = XMVectorSetW(t.r[3], XMVectorGetW(m->r[i]));
			break;
		}
	}
	return t;
}

/* Given a matrix and an i value, the new minor will be m[0][i] - we use 0 indexed ij s */
XMMATRIX XM_CALLCONV GetMatrixMinor(const XMMATRIX* m, int skipRow, int i) {
	int skipCol = i, intcurRow = 0, curCol = 0, curRow = 0;
	XMMATRIX tmp = XMMatrixSet(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);
	// for each row, skipping always 0
	for (int j = 0; j < 4; j++) {
		if (j == skipRow) continue;
		curCol = 0;
		for (int u = 0; u < 4; u++) {
			if (u == i) {
				continue;
			} else {
				// get the propert current float from m
				float val;
				switch(u) {
				case 0:
					val = XMVectorGetX(m->r[j]);
					break;
				case 1:
					val = XMVectorGetY(m->r[j]);
					break;
				case 2:
					val = XMVectorGetZ(m->r[j]);
					break;
				case 3:
					val = XMVectorGetW(m->r[j]);

				}
				// set the proper value in the new minor
				switch (curCol) {
				case 0:
					tmp.r[curRow] = XMVectorSetX(tmp.r[curRow], val);
					break;
				case 1:
					tmp.r[curRow] = XMVectorSetY(tmp.r[curRow], val);
					break;
				case 2 :
					tmp.r[curRow] = XMVectorSetZ(tmp.r[curRow], val);
					break;
				case 3:
					tmp.r[curRow] = XMVectorSetW(tmp.r[curRow], val);
				}
				curCol++;
			}
		}
		curRow++;
	}
	return tmp;
}

/* This has a complexity T(n) = 3T(n/3) + 3n, hence n^log3(b) = n with n being at most 4. */
float XM_CALLCONV GetMatrixDeterminant(const XMMATRIX* m, int n) {
	if (n == 1) return XMVectorGetX(m->r[0]);
	else {
		float d = 0;
		/* det(M) = SUM (-1)^1+i * A[1][i] * det(min(A[1][i])) */
		for (int i = 0; i < n; i++) {
			float c = 0;
			switch (i) {
			case 0:
				c = XMVectorGetX(m->r[0]);
				break;
			case 1:
				c = XMVectorGetY(m->r[0]);
				break;
			case 2:
				c = XMVectorGetZ(m->r[0]);
				break;
			default:
				c = XMVectorGetW(m->r[0]);
			}
			d += ((i % 2) == 1 ? (-1) : 1) * c * GetMatrixDeterminant(&GetMatrixMinor(m, 0, i), n - 1);
		}
		return d;
	}
}

XMMATRIX XM_CALLCONV GetMatrixAdjoint(const XMMATRIX* m,int size) {
	// compute the cofactor matrix
	XMMATRIX tmp = XMMatrixSet(	0.0f, 0.0f, 0.0f, 0.0f, 
								0.0f, 0.0f, 0.0f, 0.0f, 
								0.0f, 0.0f, 0.0f, 0.0f, 
								0.0f, 0.0f, 0.0f, 0.0f);
	int rows = size == -1 ? (sizeof(m->r) / sizeof(XMVECTOR)) : size;
	for (int i = 0; i < rows; i++) {
		float f = GetMatrixDeterminant(&GetMatrixMinor(m, i, 0), rows - 1);
		tmp.r[i] = XMVectorSetX(tmp.r[i], ((i+0) % 2 == 0 ? 1 : -1) * f);
		f = GetMatrixDeterminant(&GetMatrixMinor(m, i, 1), rows - 1);
		tmp.r[i] = XMVectorSetY(tmp.r[i], ((i+1) % 2 == 0 ? 1 : -1) * f);
		if (rows > 2) {
			f = GetMatrixDeterminant(&GetMatrixMinor(m, i, 2), rows - 1);
			tmp.r[i] = XMVectorSetZ(tmp.r[i], ((i+2) % 2 == 0 ? 1 : -1) * f);
		}
		if(rows > 3) {
			f = GetMatrixDeterminant(&GetMatrixMinor(m, i, 3), rows - 1);
			tmp.r[i] = XMVectorSetW(tmp.r[i], ((i+3) % 2 == 0 ? 1 : -1) * f);
		}
	}
	// transpose the cofactor to obtain the adjoint
	tmp = GetMatrixTranspose(&tmp);
	return tmp;
}

/* We apply the formula for computing the inverse for small 4x4 matrices  INV(A) = Adj(A) / det(A) */
XMMATRIX XM_CALLCONV GetMatrixInverse(const XMMATRIX* m, int size) {
	XMMATRIX t = GetMatrixAdjoint(m, size);
	return t / GetMatrixDeterminant(&t,size);
}