#pragma once

#include <cmath>

class Collision
{
public:
	struct Vec2
	{
		Vec2()
		{
			this->x = 0.0f;
			this->y = 0.0f;
		}

		Vec2(float x, float y)
		{
			this->x = x;
			this->y = y;
		}

		Vec2 operator-(const Vec2& v) const
		{

			Vec2 result(*this);
			result.subtract(v);
			return result;
		}

		void subtract(const Vec2& v)
		{
			x -= v.x;
			y -= v.y;
		}

		float distanceSquared(const Vec2& v) const
		{

			float dx = v.x - x;
			float dy = v.y - y;
			return (dx * dx + dy * dy);
		}

		float dot(const Vec2& v) const
		{
			return (x * v.x + y * v.y);
		}

		void normalize()
		{
			float n = x * x + y * y;

			if (n == 1.0f)
				return;

			n = std::sqrt(n);

			if (n <= 0)
				return;

			n = 1.0f / n;
			x *= n;
			y *= n;
		}

		float x;
		float y;
	};

	struct Size
	{
		float width;
		float height;

		Size operator*(float f) const
		{
			Size result(*this);
			result.height *= f;
			result.width *= f;
			return result;
		};
	};

	struct AABB
	{
		Vec2 lower;
		Vec2 upper;
	};

	struct OBB
	{
		Vec2 pivot;
		Size size;
		float rotation; // in radian
	};

	struct Circle
	{
		Vec2 pivot;
		float radius;
	};

	struct Convex
	{
		Vec2* vertices;
		size_t numVerts;
	};

	static bool isAABBOverlap(const AABB& a1, const AABB& a2)
	{
		Vec2 v1 = a1.lower - a2.upper;
		if (v1.x > 0 || v1.y > 0)
		{
			return false;
		}

		Vec2 v2 = a2.lower - a1.upper;
		if (v2.x > 0 || v2.y > 0)
		{
			return false;
		}

		return true;
	}

	// use separating axis theorem
	static bool isOBBOverlap(const OBB& o1, const OBB& o2)
	{
		// axes vector
		Vec2 a1(cos(o1.rotation), sin(o1.rotation));
		Vec2 a2(-sin(o1.rotation), cos(o1.rotation));
		Vec2 a3(cos(o2.rotation), sin(o2.rotation));
		Vec2 a4(-sin(o2.rotation), cos(o2.rotation));

		// edge length
		Size l1 = o1.size * 0.5f;
		Size l2 = o2.size * 0.5f;

		// vector between pivots
		Vec2 l = o1.pivot - o2.pivot;

		float r1, r2, r3, r4;

		// project to a1
		r1 = l1.width  * fabs(a1.dot(a1));
		r2 = l1.height * fabs(a2.dot(a1));
		r3 = l2.width  * fabs(a3.dot(a1));
		r4 = l2.height * fabs(a4.dot(a1));
		if (r1 + r2 + r3 + r4 <= fabs(l.dot(a1)))
		{
			return false;
		}

		// project to a2
		r1 = l1.width  * fabs(a1.dot(a2));
		r2 = l1.height * fabs(a2.dot(a2));
		r3 = l2.width  * fabs(a3.dot(a2));
		r4 = l2.height * fabs(a4.dot(a2));
		if (r1 + r2 + r3 + r4 <= fabs(l.dot(a2)))
		{
			return false;
		}

		// project to a3
		r1 = l1.width  * fabs(a1.dot(a3));
		r2 = l1.height * fabs(a2.dot(a3));
		r3 = l2.width  * fabs(a3.dot(a3));
		r4 = l2.height * fabs(a4.dot(a3));
		if (r1 + r2 + r3 + r4 <= fabs(l.dot(a3)))
		{
			return false;
		}

		// project to a4
		r1 = l1.width  * fabs(a1.dot(a4));
		r2 = l1.height * fabs(a2.dot(a4));
		r3 = l2.width  * fabs(a3.dot(a4));
		r4 = l2.height * fabs(a4.dot(a4));
		if (r1 + r2 + r3 + r4 <= fabs(l.dot(a4)))
		{
			return false;
		}

		return true;
	}

	static bool isCircleOverlap(const Circle& c1, const Circle& c2)
	{
		return c1.pivot.distanceSquared(c2.pivot) <= (c1.radius - c2.radius) * (c1.radius - c2.radius);
	}

	static bool isConvexOverlap(const Convex& c1, const Convex& c2)
	{
		// axes
		size_t numAxes = c1.numVerts + c2.numVerts;
		Vec2* axes = new Vec2[numAxes];
		size_t i = 0u, j = 0u;
		for (i = 0u; i < c1.numVerts; ++i, ++j)
		{
			Vec2 v = c1.vertices[i] - c1.vertices[(i + 1) % c1.numVerts];
			v.normalize();
			axes[j] = Vec2(-v.y, v.x);
		}

		for (i = 0u; i < c2.numVerts; ++i, ++j)
		{
			Vec2 v = c2.vertices[i] - c2.vertices[(i + 1) % c2.numVerts];
			v.normalize();
			axes[j] = Vec2(-v.y, v.x);
		}

		for (auto i = 0u; i < numAxes; ++i)
		{
			// project c1 vertices to axis
			float min1 = _FMAX, max1 = -_FMAX, ret1;
			for (auto j = 0u; j < c1.numVerts; ++j)
			{
				ret1 = c1.vertices[j].dot(axes[i]);
				min1 = min1 > ret1 ? ret1 : min1;
				max1 = max1 < ret1 ? ret1 : max1;
			}

			// project c2 vertices to axis
			float min2 = _FMAX, max2 = -_FMAX, ret2;
			for (auto j = 0u; j < c2.numVerts; ++j)
			{
				ret2 = c2.vertices[j].dot(axes[i]);
				min2 = min2 > ret2 ? ret2 : min2;
				max2 = max2 < ret2 ? ret2 : max2;
			}

			// overlap check
			float r1 = max1 - min1;
			float r2 = max2 - min2;
			float r = (max1 > max2 ? max1 : max2) - (min1 < min2 ? min1 : min2);
			if (r1 + r2 <= r)
			{
				delete[] axes;
				return false;
			}
		}

		delete[] axes;
		return true;
	}
};