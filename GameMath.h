#pragma once

namespace game
{
#pragma region Vector2 
	template <typename T>
	class Vector2
	{
	public:
		union 
		{
			T x;
			T width;
		};
		union
		{
			T y;
			T height;
		};
		Vector2() : 
			x((T)0.0), y((T)0.0)
		{
			//x = (T)0.0;
			//y = (T)0.0;
		};
		Vector2(const T& xIn, const T& yIn) : 
			x(xIn) , y(yIn)
		{
			//this->x = x;
			//this->y = y;
		}
		Vector2 operator - (const Vector2& rhs)
		{
			Vector2<T> ret;
			ret.x = x - rhs.x;
			ret.y = y - rhs.y;
			return ret;
		}
		Vector2& operator -= (const Vector2& rhs)
		{
			x = x - rhs.x;
			y = y - rhs.y;
			return *this;
		}
		Vector2 operator + (const Vector2& rhs)
		{
			Vector2<T> ret;
			ret.x = x + rhs.x;
			ret.y = y + rhs.y;
			return ret;
		}
		Vector2& operator += (const Vector2& rhs)
		{
			x = x + rhs.x;
			y = y + rhs.y;
			return *this;
		}

	private:
	};
	typedef Vector2<int> Vector2i, Pointi;	
	typedef Vector2<float> Vector2f, Pointf;
	typedef Vector2<double> Vector2d, Pointd;
#pragma endregion

#pragma region Vector3
	template <typename T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;
		T w;
		Vector3() :
			x((T)0), y((T)0), z((T)0), w((T)1)
		{
		}
		Vector3(const T& xIn, const T& yIn, const T& zIn) :
			x(xIn), y(yIn), z(zIn), w((T)1)
		{
			//this->x = x;
			//this->y = y;
			//this->z = z;
			//this->w = (T)1;
		}
		Vector3 operator + (const Vector3& rhs)
		{
			Vector3<T> c;
			c.x = x + rhs.x;
			c.y = y + rhs.y;
			c.z = z + rhs.z;
			return c;
		}
		Vector3& operator += (const Vector3& rhs)
		{
			x = x + rhs.x;
			y = y + rhs.y;
			z = z + rhs.z;
			return *this;
		}
		Vector3 operator - (const Vector3& rhs)
		{
			Vector3<T> c;
			c.x = x - rhs.x;
			c.y = y - rhs.y;
			c.z = z - rhs.z;
			return c;
		}
		Vector3& operator -= (const Vector3& rhs)
		{
			x = x - rhs.x;
			y = y - rhs.y;
			z = z - rhs.z;
			return *this;
		}
		Vector3 operator * (const T& scalar)
		{
			Vector3<T> c;
			c.x = x * scalar;
			c.y = y * scalar;
			c.z = z * scalar;
			return c;
		}
		Vector3& operator *= (const T& scalar)
		{
			x = x * scalar;
			y = y * scalar;
			z = z * scalar;
			return *this;
		}
		Vector3 operator / (const T& scalar)
		{
			Vector3<T> c;
			c.x = x / scalar;
			c.y = y / scalar;
			c.z = z / scalar;
			return c;
		}
		Vector3& operator /= (const T& scalar)
		{
			x = x / scalar;
			y = y / scalar;
			z = z / scalar;
			return *this;
		}
		//Vector3 operator* (const Matrix4x4<T>& mat)
		//{
		//	Vector3<T> ret;
		//	ret.x = (x * mat.m[0] + y * mat.m[4] + z * mat.m[8] + w * mat.m[12]);
		//	ret.y = (x * mat.m[1] + y * mat.m[5] + z * mat.m[9] + w * mat.m[13]);
		//	ret.z = (x * mat.m[2] + y * mat.m[6] + z * mat.m[10] + w * mat.m[14]);
		//	ret.w = (x * mat.m[3] + y * mat.m[7] + z * mat.m[11] + w * mat.m[15]);
		//	//if (w != 0)
		//	//{
		//	//	ret.x /= w;
		//	//	ret.y /= w;
		//	//	ret.z /= w;
		//	//}
		//	return ret;
		//}
		//Vector3& operator*= (const Matrix4x4<T>& mat)
		//{
		//	Vector3<T> ret;
		//	ret.x = (x * mat.m[0] + y * mat.m[4] + z * mat.m[8] + w * mat.m[12]);
		//	ret.y = (x * mat.m[1] + y * mat.m[5] + z * mat.m[9] + w * mat.m[13]);
		//	ret.z = (x * mat.m[2] + y * mat.m[6] + z * mat.m[10] + w * mat.m[14]);
		//	ret.w = (x * mat.m[3] + y * mat.m[7] + z * mat.m[11] + w * mat.m[15]);
		//	*this = ret;
		//	return *this;
		//}
		float Mag2() { return ((x * x) + (y * y) + (z * z)); }
		float Mag() { return sqrt((x * x) + (y * y) + (z * z)); }
		float Dot(const Vector3<T>& rhs) {
			return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
		}
		void Normalize()
		{
			float l = Mag();
			if (l == 0) return;
			x /= l;
			y /= l;
			z /= l;
		}
		Vector3 Cross(const Vector3& rhs)
		{
			Vector3<T> t;
			t.x = (y * rhs.z) - (z * rhs.y);
			t.y = (z * rhs.x) - (x * rhs.z);
			t.z = (x * rhs.y) - (y * rhs.x);
			return t;
		}
		Vector3 Lerp(const Vector3<T>& second, const T by)
		{
			return *this * (1 - by) + (Vector3<T>)second * by;
		}
		Vector3 Lerp(const Vector3<T>& first, const Vector3<T>& second, const T by)
		{
			Vector3<T> ret;
			ret.x = first.x * (1.0 - by) + second.x * by;
			ret.y = first.y * (1.0 - by) + second.y * by;
			ret.z = first.z * (1.0 - by) + second.z * by;
			return ret;
		}
		Vector3 NLerp(const Vector3<T>& second, const T& by)
		{
			return Lerp(second, by).Normalize();
		}
		Vector3 SLerp(const Vector3<T>& second, const T& by)
		{
			double dot = Dot(second);
			if (dot < -1.0) dot = -1.0;
			else if (dot > 1.0) dot = 1.0;
			double theta = std::acos(dot) * by;
			Vector3 relative = (Vector3<T>)second - (*this * by);
			relative = relative.Normalize();
			return ((*this * std::cos(theta)) + (relative * std::sin(theta)));
		}
	private:
	};
	typedef Vector3<float> Vector3f;
	typedef Vector3<double> Vector3d;
	typedef Vector3<int> Vector3i;
#pragma endregion

#pragma region Rect
	template <typename T>
	class Rect
	{
	public:
		Rect(const T left, const T top, const T right, const T bottom)
		{
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}
		union
		{
			T top;
			T y;
		};
		union
		{
			T left;
			T x;
		};
		T bottom;
		T right;
		Rect()
		{
			top = 0;
			left = 0;
			bottom = 0;
			right = 0;
		}
		void Set(const T left, const T top, const T right, const T bottom)
		{
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}
	private:
	};
	typedef Rect<int> Recti;
	typedef Rect<float> Rectf;
	typedef Rect<double> Rectd;
#pragma endregion

}
