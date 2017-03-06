#pragma once

/**
* Structure for capsules.
*
* A capsule consists of two sphere connected by a cylinder.
*/
struct YCapsuleShape
{
	/** The capsule's center point. */
	YVector Center;

	/** The capsule's radius. */
	float Radius;

	/** The capsule's orientation in space. */
	YVector Orientation;

	/** The capsule's length. */
	float Length;

public:

	/** Default constructor. */
	YCapsuleShape() { }

	/**
	* Create and inintialize a new instance.
	*
	* @param InCenter The capsule's center point.
	* @param InRadius The capsule's radius.
	* @param InOrientation The capsule's orientation in space.
	* @param InLength The capsule's length.
	*/
	YCapsuleShape(YVector InCenter, float InRadius, YVector InOrientation, float InLength)
		: Center(InCenter)
		, Radius(InRadius)
		, Orientation(InOrientation)
		, Length(InLength)
	{ }
};
