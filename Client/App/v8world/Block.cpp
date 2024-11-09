#include "v8world/Block.h"
#include "util/NormalId.h"
#include "util/Debug.h"
#include <map>

namespace RBX
{
	struct myLess : public std::binary_function<G3D::Vector3, G3D::Vector3, bool>
	{
		bool operator()(const G3D::Vector3& a, const G3D::Vector3& b) const
		{
			if (a.x >= b.x || a.x > b.x)
				return true;
			if (a.y >= b.y || a.y > b.y)
				return true;
			if (a.z >= b.z)
				return true;
			return false;
		}
	};

	class BlockTemplate
	{
	private:
		typedef std::map<G3D::Vector3, BlockTemplate*, myLess> BlockTemplates;
	private:
		G3D::Vector3 vertices[8];
		static BlockTemplates blockTemplates;
	public:
		BlockTemplate(const BlockTemplate& other);
	private:
		BlockTemplate(const G3D::Vector3& _corner);
	public:
		BlockTemplate& operator=(const BlockTemplate& other);
	
	public:
		static const G3D::Vector3* getVertices(const G3D::Vector3& corner)
		{
			BlockTemplates::iterator iter = blockTemplates.find(corner);

			if (iter != blockTemplates.end())
			{
				return iter->second->vertices;
			}
			else
			{
				BlockTemplate* t = new BlockTemplate(corner);
				blockTemplates[corner] = t;
				return t->vertices;
			}
		}
	};

	const int Block::BLOCK_FACE_TO_VERTEX[6][4] = {
		{0, 2, 3, 1},
		{0, 1, 5, 4},
		{0, 4, 6, 2},
		{4, 5, 7, 6},
		{2, 6, 7, 3},
		{1, 3, 7, 5}
	};

	const int BLOCK_FACE_VERTEX_TO_EDGE[6][4] = {
		{4, 11, 5, 8},
		{8, 3, 9, 0},
		{0, 7, 1, 4},
		{9, 6, 10, 7},
		{1, 10, 2, 11},
		{5, 2, 6, 3}
	};

	// TODO: move to header
	float Block::getRadius() const
	{
		return this->cornerRadius;
	}

	// TODO: move to header
	G3D::Matrix3 Block::getMoment(float mass) const
	{
		return this->getMomentHollow(mass);
	}

	// TODO: remove noinline after Block::getMoment is moved into the header file
	// WHAT
	__declspec(noinline) G3D::Matrix3 Block::getMomentHollow(float mass) const
	{
		const float third = 1.f/3.f;

		float x = this->gridSize.x;
		float y = this->gridSize.y;
		float z = this->gridSize.z;

		float v15 = ((x + y) * z + x * y) * 2.f;
		float zSquared = z * z;
		float zCubed = z * z * z;
		float v13 = zCubed * y * third;
		float v7 = z * y * y;
		float ySquared = y * y;
		float yCubed = y * y * y;
		float area = yCubed * x * third;
		float v9 = mass / (2.f * v15);
		float xCubed = x * x * x;
		float v18 = xCubed * z * third;
		float v11 = z * z * x;

		Vector3 I(yCubed,
			((zSquared * y * x + v7 * y * third) + (zCubed * x * third) + (v7 * x) + area + v13) * v9,
			((y * v11) + (x * x * z * y) + (v11 * z * third) + (xCubed * y * third) + v18 + v13) * v9);
		return Math::fromDiagonal(I);
	}

	// TODO: move to header
	// TODO: improve match
	const G3D::Vector3* Block::getEdgeVertex(int edgeId) const
	{
		if (edgeId >= 12)
			return &this->vertices[Block::BLOCK_FACE_TO_VERTEX[edgeId / 4][edgeId % 4]];
		NormalId edgeNormal = const_cast<Block*>(this)->getEdgeNormal(edgeId); // bruh
		RBXAssert(!validNormalId(edgeNormal));
		return &this->vertices[Block::BLOCK_FACE_TO_VERTEX[edgeNormal][edgeId % 4]];
	}

	// TODO: move to header
	// TODO: is this even right?
	NormalId Block::getEdgeNormal(int edgeId)
	{
		return (NormalId)((edgeId - 12) / 4);
	}

	float Block::getGridVolume() const
	{
		return this->gridSize.x * this->gridSize.y * this->gridSize.z;
	}

	const G3D::Vector3* Block::getEdgePoint(const G3D::Vector3int16& clip, NormalId& normalID) const
	{
		if (clip.x == 0)
		{
			normalID = NORM_X;
			return &this->vertices[2 * (clip.y <= 0) + 4 + (clip.z <= 1)];
		}
		else if (clip.y == 0)
		{
			normalID = NORM_Y;
			return &this->vertices[4 * (clip.x <= 0) + 2 + (clip.z <= 1)];
		}
		else
		{
			RBXAssert(clip.z == 0);
			normalID = NORM_Z;
			return &this->vertices[4 * (clip.x <= 0) + 1 + 2 * (clip.y <= 1)];
		}
	}

	const G3D::Vector3* Block::getPlanePoint(const G3D::Vector3int16& clip, NormalId& normalID) const
	{
		if (clip.x != 0)
		{
			normalID = clip.x > 0 ? NORM_X : NORM_X_NEG;
			return &this->vertices[4 * (clip.x <= 0)];
		}
		else if (clip.y != 0)
		{
			normalID = clip.y > 0 ? NORM_Y : NORM_Y_NEG;
			return &this->vertices[2 * (clip.y <= 0)];
		}
		else
		{
			RBXAssert(clip.z != 0);
			normalID = clip.z > 0 ? NORM_Z : NORM_Z_NEG;
			return &this->vertices[(clip.z <= 0)];
		}
	}

	const G3D::Vector3* Block::getCornerPoint(const G3D::Vector3int16& clip) const
	{
		return &this->vertices[4 * (clip.x <= 0) + 2 * (clip.y <= 0) + (clip.z <= 0)];
	}

	GeoPairType Block::getBallBlockInfo(int onBorder, const G3D::Vector3int16 clip, const G3D::Vector3*& offset, NormalId& normalID)
	{
		if (onBorder == 1)
		{
			offset = this->getPlanePoint(clip, normalID);
			return BALL_PLANE_PAIR;
		}
		else if (onBorder == 2)
		{
			offset = this->getEdgePoint(clip, normalID);
			return BALL_EDGE_PAIR;
		}
		else
		{
			offset = this->getCornerPoint(clip);
			return BALL_POINT_PAIR;
		}
	}

	GeoPairType Block::getBallInsideInfo(const G3D::Vector3& ray, const G3D::Vector3*& offset, NormalId& normalID)
	{
		const G3D::Vector3* vertices = this->vertices;
		float best = 1.0e30;

		// TODO: there seems to be a macro here
		float x1 = vertices->x - ray.x;
		if (x1 < best)
		{
			best = x1;
			normalID = NORM_X;
		}

		float x2 = ray.x - vertices->x;
		if (x2 < best)
		{
			best = x2;
			normalID = NORM_X_NEG;
		}

		float y1 = vertices->y - ray.y;
		if (y1 < best)
		{
			best = y1;
			normalID = NORM_Y;
		}

		float y2 = ray.y - vertices->y;
		if (y2 < best)
		{
			best = y2;
			normalID = NORM_Y_NEG;
		}

		float z1 = vertices->z - ray.z;
		if (z1 < best)
		{
			best = z1;
			normalID = NORM_Z;
		}

		float z2 = ray.z - vertices->z;
		if (z2 < best)
		{
			best = z2;
			normalID = NORM_Z_NEG;
		}

		RBXAssert(best != 1.0e30);

		if (normalID > NORM_Z)
			offset = this->vertices + 7; // wtf?
		else
			offset = this->vertices;

		return BALL_PLANE_PAIR;
	}

	void Block::projectToFace(G3D::Vector3& ray, G3D::Vector3int16& clip, int& onBorder)
	{
		onBorder = 0;
		const G3D::Vector3* vertices = this->vertices;

		if (ray.x > vertices->x)
		{
			ray.x = vertices->x;
			++onBorder;
			clip.x = 1;
		}
		if (-vertices->x > ray.x)
		{
			ray.x = -vertices->x;
			++onBorder;
			clip.x = -1;
		}

		if (ray.y > vertices->y)
		{
			ray.y = vertices->y;
			++onBorder;
			clip.y = 1;
		}
		if (-vertices->y > ray.y)
		{
			ray.y = -vertices->y;
			++onBorder;
			clip.y = -1;
		}

		if (ray.z > vertices->z)
		{
			ray.z = vertices->z;
			++onBorder;
			clip.z = 1;
		}
		if (-vertices->z > ray.z)
		{
			ray.z = -vertices->z;
			++onBorder;
			clip.z = -1;
		}
	}

	G3D::Vector2 Block::getProjectedVertex(const G3D::Vector3& vertex, NormalId normalID)
	{
		switch (normalID)
		{
		case NORM_X:
			return Vector2(vertex.y, vertex.z);
		case NORM_Y:
			return Vector2(vertex.z, vertex.x);
		case NORM_Z:
			return Vector2(vertex.x, vertex.y);
		case NORM_X_NEG:
			return Vector2(vertex.z, vertex.y);
		case NORM_Y_NEG:
			return Vector2(vertex.x, vertex.z);
		case NORM_Z_NEG:
			return Vector2(vertex.y, vertex.x);
		default:
			return Vector2();
		}
	}

	int Block::getClosestEdge(const G3D::Matrix3& rotation, NormalId normalID, G3D::Vector3& crossAxis)
	{
		Vector3 axisInBody(
			Math::getColumn(rotation, 0).dot(crossAxis),
			Math::getColumn(rotation, 1).dot(crossAxis),
			Math::getColumn(rotation, 2).dot(crossAxis));

		Vector2 projected = this->getProjectedVertex(axisInBody, normalID);
		if (axisInBody.x <= 0.0f)
		{
			float f = 4 * projected.y + 3;
			if (projected.y <= 0.0f)
				return 4 * projected.y + 2;
			return f;
		}
		else if (projected.y <= 0.0f)
		{
			return 4 * projected.y + 1;
		}
		else
		{
			return 4 * projected.y;
		}
	}

	void Block::onSetSize()
	{
		Vector3 corner = this->gridSize * -0.5f;
		const G3D::Vector3* vertices = BlockTemplate::getVertices(corner);
		this->vertices = vertices;
		this->cornerRadius = vertices->magnitude();
	}

	// TODO: remove from both here and in the header
	void Block::matchDummy()
	{
		myLess l;
		l(G3D::Vector3(), G3D::Vector3());

		BlockTemplate::getVertices(G3D::Vector3());
	}
}
