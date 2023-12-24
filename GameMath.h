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
		Vector2()  
		{
			x = (T)0.0;
			y = (T)0.0;
		};
		Vector2(const T& x, const T& y)
		{
			this->x = x;
			this->y = y;
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
