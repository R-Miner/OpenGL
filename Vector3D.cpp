//#include "stdafx.h"
#include <math.h>

class Vector3D
{
public:

	double X,Y,Z;
	
	// Default constructor, sets all values to ZERO
	Vector3D()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}  
	
	Vector3D(double XIn, double YIn, double ZIn)
	{
		X = XIn;
		Y = YIn;
		Z = ZIn;
	}

	Vector3D ScaleBy(double Scalar)
	{
		return Vector3D(X * Scalar, Y * Scalar, Z * Scalar);
	}

	
	//This function adds two vectors together
	Vector3D Add(Vector3D& ToAdd)
	{
		return Vector3D(X + ToAdd.X, Y + ToAdd.Y, Z + ToAdd.Z);
	}

	
	//This functions subtracts two vectors
	Vector3D Subtract(Vector3D& ToSub)
	{
		return Vector3D(X - ToSub.X, Y - ToSub.Y, Z - ToSub.Z);
	}

	
	// This function calculates dot product
	double DotProduct(Vector3D& ToDot)
	{
		return (X * ToDot.X) + (Y * ToDot.Y) + (Z * ToDot.Z);
	}



	
	//This function calculates cross product
	Vector3D CrossProduct(Vector3D& ToCross)
	{
		return Vector3D((Y * ToCross.Z - Z * ToCross.Y), (Z * ToCross.X - X * ToCross.Z), (X * ToCross.Y - Y * ToCross.X)); 
	}

	float GetLength()   // Use pythagorean calculations to return the length of the vector
	{
      return (float)sqrt((X * X) + (Y * Y) + (Z * Z));
	}
	
};



