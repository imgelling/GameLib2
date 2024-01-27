#pragma once

namespace game
{
	template <typename T>
	class Matrix4x4;
#pragma region Vector2 
	template <typename T>
	class Vector2
	{
	public:
		union 
		{
			T x;
			T width;
			T u;
		};
		union
		{
			T y;
			T height;
			T v;
		};
		Vector2() : 
			x((T)0.0), y((T)0.0)
		{
		};
		Vector2(const T& xIn, const T& yIn) : 
			x(xIn) , y(yIn)
		{
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
		Vector3 operator* (const Matrix4x4<T>& mat)
		{
			Vector3<T> ret;
			ret.x = (x * mat.m[0] + y * mat.m[4] + z * mat.m[8] + w * mat.m[12]);
			ret.y = (x * mat.m[1] + y * mat.m[5] + z * mat.m[9] + w * mat.m[13]);
			ret.z = (x * mat.m[2] + y * mat.m[6] + z * mat.m[10] + w * mat.m[14]);
			ret.w = (x * mat.m[3] + y * mat.m[7] + z * mat.m[11] + w * mat.m[15]);
			//if (w != 0)
			//{
			//	ret.x /= w;
			//	ret.y /= w;
			//	ret.z /= w;
			//}
			return ret;
		}
		Vector3& operator*= (const Matrix4x4<T>& mat)
		{
			Vector3<T> ret;
			ret.x = (x * mat.m[0] + y * mat.m[4] + z * mat.m[8] + w * mat.m[12]);
			ret.y = (x * mat.m[1] + y * mat.m[5] + z * mat.m[9] + w * mat.m[13]);
			ret.z = (x * mat.m[2] + y * mat.m[6] + z * mat.m[10] + w * mat.m[14]);
			ret.w = (x * mat.m[3] + y * mat.m[7] + z * mat.m[11] + w * mat.m[15]);
			*this = ret;
			return *this;
		}
		float_t Mag2() { return ((x * x) + (y * y) + (z * z)); }
		float_t Mag() { return sqrt((x * x) + (y * y) + (z * z)); }
		float_t Dot(const Vector3<T>& rhs) {
			return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
		}
		void Normalize()
		{
			float_t l = Mag();
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

#pragma region Matrix4x4
	template <typename T>
	class Matrix4x4
	{
	public:
		T m[16];// = { 0.0 };
		Matrix4x4() { SetIdentity(); }
		Matrix4x4(const Matrix4x4& in)
		{
			for (int i = 0; i < 16; i++)
			{
				m[i] = in.m[i];
			}
		}
		Matrix4x4(const T(&in)[16])
		{
			memcpy(m, in, sizeof(in));
		}
		Matrix4x4 operator+ (const Matrix4x4& rhs)
		{
			Matrix4x4<T> ret;
			for (int i = 0; i < 16; i++)
			{
				ret.m[i] = m[i] + rhs.m[i];
			}
			return ret;
		}
		Matrix4x4& operator+= (const Matrix4x4& rhs)
		{
			for (int i = 0; i < 16; i++)
			{
				m[i] = m[i] + rhs.m[i];
			}
			return *this;
		}
		Matrix4x4 operator- (const Matrix4x4& rhs)
		{
			Matrix4x4<T> ret;
			for (int i = 0; i < 16; i++)
			{
				ret.m[i] = m[i] - rhs.m[i];
			}
			return ret;
		}
		Matrix4x4& operator-= (const Matrix4x4& rhs)
		{
			for (int i = 0; i < 16; i++)
			{
				m[i] = m[i] - rhs.m[i];
			}
			return *this;
		}
		Matrix4x4 operator* (const T& scalar)
		{
			Matrix4x4<T> ret;
			for (int i = 0; i < 16; i++)
			{
				ret.m[i] = m[i] * scalar;
			}
			return ret;
		}
		Matrix4x4& operator*= (const T& scalar)
		{
			//Matrix4x4<T> ret;
			for (int i = 0; i < 16; i++)
			{
				m[i] = m[i] * scalar;
			}
			return *this;
		}
		Matrix4x4 operator* (const Matrix4x4& rhs)
		{
			Matrix4x4<T> ret;
			for (int i = 0; i < 4; i++) // row
			{
				for (int j = 0; j < 4; j++) // col
				{
					ret.m[j * 4 + i] = 0;  // zero out location in return
					for (int k = 0; k < 4; k++)
					{
						ret.m[j * 4 + i] += m[k * 4 + i] * rhs.m[j * 4 + k];
					}
				}
			}


			return ret;
		}
		void SetIdentity()
		{
			//for (int i = 0; i < 16; i++)
			//{
			//	//if (i % 5 == 0) m[i] = 1.0;
			//	//else m[i] = 0.0;
			//	m[i] = (T)0.0;
			//}
			ZeroMemory(m, 16*4);
			m[0] = m[5] = m[10] = m[15] = (T)1.0;
		}
		void SetRotationX(const T& rot)
		{
			m[5] = cos(rot);
			m[6] = sin(rot);
			m[9] = -sin(rot);
			m[10] = cos(rot);
		}
		void SetRotationY(const T& rot)
		{
			m[0] = cos(rot);
			m[2] = -sin(rot);
			m[8] = sin(rot);
			m[10] = cos(rot);
		}
		void SetRotationZ(const T& rot)
		{
			m[0] = cos(rot);
			m[1] = sin(rot);
			m[4] = -sin(rot);
			m[5] = cos(rot);
		}
		void SetTranslation(const T& x, const T& y, const T& z)
		{
			//m[12] = x;
			//m[13] = y;
			//m[14] = z;
			m[3] = x;
			m[7] = y;
			m[11] = z;
		}
		void SetScale(const T& x, const T& y, const T& z)
		{
			m[0] = x;
			m[5] = y;
			m[10] = z;
		}

		double Determinant() {
			return
				m[12] * m[9] * m[6] * m[3] - m[8] * m[13] * m[6] * m[3] -
				m[12] * m[5] * m[10] * m[3] + m[4] * m[13] * m[10] * m[3] +
				m[8] * m[5] * m[14] * m[3] - m[4] * m[9] * m[14] * m[3] -
				m[12] * m[9] * m[2] * m[7] + m[8] * m[13] * m[2] * m[7] +
				m[12] * m[1] * m[10] * m[7] - m[0] * m[13] * m[10] * m[7] -
				m[8] * m[1] * m[14] * m[7] + m[0] * m[9] * m[14] * m[7] +
				m[12] * m[5] * m[2] * m[11] - m[4] * m[13] * m[2] * m[11] -
				m[12] * m[1] * m[6] * m[11] + m[0] * m[13] * m[6] * m[11] +
				m[4] * m[1] * m[14] * m[11] - m[0] * m[5] * m[14] * m[11] -
				m[8] * m[5] * m[2] * m[15] + m[4] * m[9] * m[2] * m[15] +
				m[8] * m[1] * m[6] * m[15] - m[0] * m[9] * m[6] * m[15] -
				m[4] * m[1] * m[10] * m[15] + m[0] * m[5] * m[10] * m[15];
		}
	};
	typedef Matrix4x4<double> Matrix4x4d;
	typedef Matrix4x4<float> Matrix4x4f;
	typedef Matrix4x4<int> Matrix4x4i;
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
		T top;
		T left;
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

#pragma region Quaternion

#pragma endregion

}
