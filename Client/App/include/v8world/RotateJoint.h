#pragma once
#include "v8world/MultiJoint.h"
#include "util/SurfaceType.h"

namespace RBX
{
	class RotateJoint : public MultiJoint
	{
	protected:
		RotateConnector* rotateConnector;
	  
	private:
		virtual void putInKernel(Kernel* _kernel);
		virtual void removeFromKernel();
		virtual Joint::JointType getJointType() const
		{
			return ROTATE_JOINT;
		}
	protected:
		float getTorqueArmLength();
		float getChannelValue(int uiStepId);
	public:
		//void RotateJoint(const RotateJoint&);
		RotateJoint(Primitive* axlePrim, Primitive* holePrim, const G3D::CoordinateFrame& c0, const G3D::CoordinateFrame& c1);
		RotateJoint();
		virtual ~RotateJoint();
	public:
		Primitive* getAxlePrim()
		{
			return getPrimitive(0);
		}
		Primitive* getHolePrim()
		{
			return getPrimitive(1);
		}
		NormalId getAxleId()
		{
			return Matrix3ToNormalId(getJointCoord(0).rotation);
		}
		NormalId getHoleId()
		{
			return Matrix3ToNormalId(getJointCoord(1).rotation);
		}
		//RotateJoint& operator=(const RBX::RotateJoint&);
	  
	private:
		static RotateJoint* surfaceTypeToJoint(SurfaceType surfaceType, Primitive* axlePrim, Primitive* holePrim, const G3D::CoordinateFrame& c0, const G3D::CoordinateFrame& c1);
	public:
		static RotateJoint* canBuildJoint(Primitive* p0, Primitive* p1, NormalId nId0, NormalId nId1);
	};

	class RotateVJoint : public RotateJoint
	{
	private:
		virtual Joint::JointType getJointType() const;
		virtual bool canStepUi() const;
		virtual void stepUi(int);
	public:
		//RotateVJoint(const RotateVJoint&);
		RotateVJoint(Primitive*, Primitive*, const G3D::CoordinateFrame&, const G3D::CoordinateFrame&);
		RotateVJoint();
		virtual ~RotateVJoint();
	public:
		//RotateVJoint& operator=(const RotateVJoint&);
	};

	class RotatePJoint : public RotateJoint
	{
	private:
		virtual Joint::JointType getJointType() const;
		virtual bool canStepUi() const;
		virtual void stepUi(int);
	public:
		//RotatePJoint(const RotatePJoint&);
		RotatePJoint(Primitive*, Primitive*, const G3D::CoordinateFrame&, const G3D::CoordinateFrame&);
		RotatePJoint();
		virtual ~RotatePJoint();
	public:
		//RotatePJoint& operator=(const RotatePJoint&);
	};
}