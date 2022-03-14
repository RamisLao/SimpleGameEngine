// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
// MinDistSq between two line segments:
// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used, distributed and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
// ----------------------------------------------------------------

#include "Collision.h"
#include <algorithm>
#include <array>

LineSegment::LineSegment(const Vector3& start, const Vector3& end)
	:m_Start(start)
	,m_End(end)
{
}

Vector3 LineSegment::PointOnSegment(float t) const
{
	return m_Start + (m_End - m_Start) * t;
}

float LineSegment::MinDistSq(const Vector3& point) const
{
	// Construct vectors
	Vector3 ab = m_End - m_Start;
	Vector3 ba = -1.0f * ab;
	Vector3 ac = point - m_Start;
	Vector3 bc = point - m_End;

	// Case 1: C projects prior to A
	if (Vector3::Dot(ab, ac) < 0.0f)
	{
		return ac.LengthSq();
	}
	// Case 2: C projects after B
	else if (Vector3::Dot(ba, bc) < 0.0f)
	{
		return bc.LengthSq();
	}
	// Case 3: C projects onto line
	else
	{
		// Compute p
		float scalar = Vector3::Dot(ac, ab)
			/ Vector3::Dot(ab, ab);
		Vector3 p = scalar * ab;
		// Compute length squared of ac - p
		return (ac - p).LengthSq();
	}
}

float LineSegment::MinDistSq(const LineSegment & s1, const LineSegment & s2)
{
	Vector3   u = s1.m_End - s1.m_Start;
	Vector3   v = s2.m_End - s2.m_Start;
	Vector3   w = s1.m_Start - s2.m_Start;
	float    a = Vector3::Dot(u, u);         // always >= 0
	float    b = Vector3::Dot(u, v);
	float    c = Vector3::Dot(v, v);         // always >= 0
	float    d = Vector3::Dot(u, w);
	float    e = Vector3::Dot(v, w);
	float    D = a*c - b*b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

								   // compute the line parameters of the two closest points
	if (CustomMath::NearZero(D)) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (CustomMath::NearZero(sN) ? 0.0f : sN / sD);
	tc = (CustomMath::NearZero(tN) ? 0.0f : tN / tD);

	// get the difference of the two closest points
	Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	return dP.LengthSq();   // return the closest distance squared
}

Plane::Plane(const Vector3& normal, float d)
	:m_Normal(normal)
	,m_D(d)
{
}

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
{
	// Compute vectors from a to b and a to c
	Vector3 ab = b - a;
	Vector3 ac = c - a;
	// Cross product and normalize to get normal
	m_Normal = Vector3::Cross(ab, ac);
	m_Normal.Normalize();
	// d = -P dot n
	m_D = -Vector3::Dot(a, m_Normal);
}

float Plane::SignedDist(const Vector3& point) const
{
	return Vector3::Dot(point, m_Normal) - m_D;
}

Sphere::Sphere(const Vector3& center, float radius)
	:m_Center(center)
	, m_Radius(radius)
{
}

bool Sphere::Contains(const Vector3& point) const
{
	// Get distance squared between center and point
	float distSq = (m_Center - point).LengthSq();
	return distSq <= (m_Radius * m_Radius);
}

AABB::AABB(const Vector3& min, const Vector3& max)
	: m_Min(min)
	, m_Max(max)
{
}

void AABB::UpdateMinMax(const Vector3& point)
{
	// Update each component separately
	m_Min.x = CustomMath::Min(m_Min.x, point.x);
	m_Min.y = CustomMath::Min(m_Min.y, point.y);
	m_Min.z = CustomMath::Min(m_Min.z, point.z);

	m_Max.x = CustomMath::Max(m_Max.x, point.x);
	m_Max.y = CustomMath::Max(m_Max.y, point.y);
	m_Max.z = CustomMath::Max(m_Max.z, point.z);
}

void AABB::Rotate(const Quaternion& q)
{
	// Construct the 8 points for the corners of the box
	std::array<Vector3, 8> points;
	// Min point is always a corner
	points[0] = m_Min;
	// Permutations with 2 min and 1 max
	points[1] = Vector3(m_Max.x, m_Min.y, m_Min.z);
	points[2] = Vector3(m_Min.x, m_Max.y, m_Min.z);
	points[3] = Vector3(m_Min.x, m_Min.y, m_Max.z);
	// Permutations with 2 max and 1 min
	points[4] = Vector3(m_Min.x, m_Max.y, m_Max.z);
	points[5] = Vector3(m_Max.x, m_Min.y, m_Max.z);
	points[6] = Vector3(m_Max.x, m_Max.y, m_Min.z);
	// Max point corner
	points[7] = Vector3(m_Max);

	// Rotate first point
	Vector3 p = Vector3::Transform(points[0], q);
	// Reset min/max to first point rotated
	m_Min = p;
	m_Max = p;
	// Update min/max based on remaining points, rotated
	for (size_t i = 1; i < points.size(); i++)
	{
		p = Vector3::Transform(points[i], q);
		UpdateMinMax(p);
	}
}

bool AABB::Contains(const Vector3& point) const
{
	bool outside = point.x < m_Min.x ||
		point.y < m_Min.y ||
		point.z < m_Min.z ||
		point.x > m_Max.x ||
		point.y > m_Max.y ||
		point.z > m_Max.z;
	// If none of these are true, the point is inside the box
	return !outside;
}

float AABB::MinDistSq(const Vector3& point) const
{
	// Compute differences for each axis
	float dx = CustomMath::Max(m_Min.x - point.x, 0.0f);
	dx = CustomMath::Max(dx, point.x - m_Max.x);
	float dy = CustomMath::Max(m_Min.y - point.y, 0.0f);
	dy = CustomMath::Max(dy, point.y - m_Max.y);
	float dz = CustomMath::Max(m_Min.z - point.z, 0.0f);
	dz = CustomMath::Max(dy, point.z - m_Max.z);
	// Distance squared formula
	return dx * dx + dy * dy + dz * dz;
}

Capsule::Capsule(const Vector3& start, const Vector3& end, float radius)
	:m_Segment(start, end)
	, m_Radius(radius)
{
}

Vector3 Capsule::PointOnSegment(float t) const
{
	return m_Segment.PointOnSegment(t);
}

bool Capsule::Contains(const Vector3& point) const
{
	// Get minimal dist. sq. between point and line segment
	float distSq = m_Segment.MinDistSq(point);
	return distSq <= (m_Radius * m_Radius);
}

bool ConvexPolygon::Contains(const Vector2& point) const
{
	float sum = 0.0f;
	Vector2 a, b;
	for (size_t i = 0; i < m_Vertices.size() - 1; i++)
	{
		// From point to first vertex
		a = m_Vertices[i] - point;
		a.Normalize();
		// From point to second vertex
		b = m_Vertices[i + 1] - point;
		b.Normalize();
		// Add angle to sum
		sum += CustomMath::Acos(Vector2::Dot(a, b));
	}
	// Have to add angle for last vertex and first vertex
	a = m_Vertices.back() - point;
	a.Normalize();
	b = m_Vertices.front() - point;
	b.Normalize();
	sum += CustomMath::Acos(Vector2::Dot(a, b));
	// Return true if approximately 2pi
	return CustomMath::NearZero(sum - CustomMath::TwoPi);
}

bool Intersect(const Sphere& a, const Sphere& b)
{
	float distSq = (a.m_Center - b.m_Center).LengthSq();
	float sumRadii = a.m_Radius + b.m_Radius;
	return distSq <= (sumRadii * sumRadii);
}

bool Intersect(const AABB& a, const AABB& b)
{
	bool no = a.m_Max.x < b.m_Min.x ||
		a.m_Max.y < b.m_Min.y ||
		a.m_Max.z < b.m_Min.z ||
		b.m_Max.x < a.m_Min.x ||
		b.m_Max.y < a.m_Min.y ||
		b.m_Max.z < a.m_Min.z;
	// If none of these are true, they must intersect
	return !no;
}

bool Intersect(const Capsule& a, const Capsule& b)
{
	float distSq = LineSegment::MinDistSq(a.m_Segment, 
		b.m_Segment);
	float sumRadii = a.m_Radius + b.m_Radius;
	return distSq <= (sumRadii * sumRadii);
}

bool Intersect(const Sphere& s, const AABB& box)
{
	float distSq = box.MinDistSq(s.m_Center);
	return distSq <= (s.m_Radius * s.m_Radius);
}

bool Intersect(const LineSegment& l, const Sphere& s, float& outT)
{
	// Compute X, Y, a, b, c as per equations
	Vector3 X = l.m_Start - s.m_Center;
	Vector3 Y = l.m_End - l.m_Start;
	float a = Vector3::Dot(Y, Y);
	float b = 2.0f * Vector3::Dot(X, Y);
	float c = Vector3::Dot(X, X) - s.m_Radius * s.m_Radius;
	// Compute discriminant
	float disc = b * b - 4.0f * a * c;
	if (disc < 0.0f)
	{
		return false;
	}
	else
	{
		disc = CustomMath::Sqrt(disc);
		// Compute min and max solutions of t
		float tMin = (-b - disc) / (2.0f * a);
		float tMax = (-b + disc) / (2.0f * a);
		// Check whether either t is within bounds of segment
		if (tMin >= 0.0f && tMin <= 1.0f)
		{
			outT = tMin;
			return true;
		}
		else if (tMax >= 0.0f && tMax <= 1.0f)
		{
			outT = tMax;
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool Intersect(const LineSegment& l, const Plane& p, float& outT)
{
	// First test if there's a solution for t
	float denom = Vector3::Dot(l.m_End - l.m_Start,
		p.m_Normal);
	if (CustomMath::NearZero(denom))
	{
		// The only way they intersect is if start
		// is a point on the plane (P dot N) == d
		if (CustomMath::NearZero(Vector3::Dot(l.m_Start, p.m_Normal)
			- p.m_D))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		float numer = -Vector3::Dot(l.m_Start, p.m_Normal) -
			p.m_D;
		outT = numer / denom;
		// Validate t is within bounds of the line segment
		if (outT >= 0.0f && outT <= 1.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool TestSidePlane(float start, float end, float negd, const Vector3& norm,
	std::vector<std::pair<float, Vector3>>& out)
{
	float denom = end - start;
	if (CustomMath::NearZero(denom))
	{
		return false;
	}
	else
	{
		float numer = -start + negd;
		float t = numer / denom;
		// Test that t is within bounds
		if (t >= 0.0f && t <= 1.0f)
		{
			out.emplace_back(t, norm);
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector3& outNorm)
{
	// Vector to save all possible t values, and normals for those sides
	std::vector<std::pair<float, Vector3>> tValues;
	// Test the x planes
	TestSidePlane(l.m_Start.x, l.m_End.x, b.m_Min.x, Vector3::NegUnitX,
		tValues);
	TestSidePlane(l.m_Start.x, l.m_End.x, b.m_Max.x, Vector3::UnitX,
		tValues);
	// Test the y planes
	TestSidePlane(l.m_Start.y, l.m_End.y, b.m_Min.y, Vector3::NegUnitY,
		tValues);
	TestSidePlane(l.m_Start.y, l.m_End.y, b.m_Max.y, Vector3::UnitY,
		tValues);
	// Test the z planes
	TestSidePlane(l.m_Start.z, l.m_End.z, b.m_Min.z, Vector3::NegUnitZ,
		tValues);
	TestSidePlane(l.m_Start.z, l.m_End.z, b.m_Max.z, Vector3::UnitZ,
		tValues);
	
	// Sort the t values in ascending order
	std::sort(tValues.begin(), tValues.end(), [](
		const std::pair<float, Vector3>& a,
		const std::pair<float, Vector3>& b) {
		return a.first < b.first;
	});
	// Test if the box contains any of these points of intersection
	Vector3 point;
	for (auto& t : tValues)
	{
		point = l.PointOnSegment(t.first);
		if (b.Contains(point))
		{
			outT = t.first;
			outNorm = t.second;
			return true;
		}
	}

	//None of the intersections are within bounds of box
	return false;
}

bool SweptSphere(const Sphere& P0, const Sphere& P1,
	const Sphere& Q0, const Sphere& Q1, float& outT)
{
	// Compute X, Y, a, b, and c
	Vector3 X = P0.m_Center - Q0.m_Center;
	Vector3 Y = P1.m_Center - P0.m_Center -
		(Q1.m_Center - Q0.m_Center);
	float a = Vector3::Dot(Y, Y);
	float b = 2.0f * Vector3::Dot(X, Y);
	float sumRadii = P0.m_Radius + Q0.m_Radius;
	float c = Vector3::Dot(X, X) - sumRadii * sumRadii;
	// Solve discriminant
	float disc = b * b - 4.0f * a * c;
	if (disc < 0.0f)
	{
		return false;
	}
	else
	{
		disc = CustomMath::Sqrt(disc);
		// We only care about the smaller solution
		outT = (-b - disc) / (2.0f * a);
		if (outT >= 0.0f && outT <= 0.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
