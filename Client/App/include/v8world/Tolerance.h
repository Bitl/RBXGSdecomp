#pragma once

namespace RBX
{
	class Tolerance
	{
	public:
		static float mainGrid();
		static float jointMaxUnaligned();
		static float jointOverlapMin();
		static bool pointsUnaligned(const G3D::Vector3&, const G3D::Vector3&);
		static float jointAngleMax();
		static float jointPlanarMax();
		static float rotateAngleMax();
		static float rotatePlanarMax();
		static float glueAngleMax();
		static float gluePlanarMax();
		static float maxOverlapOrGap();
	};
}