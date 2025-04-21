#pragma once
#include "v8world/Controller.h"

namespace RBX
{
	class SurfaceData
	{
	public:
		Controller::InputType inputType;
		float paramA;
		float paramB;

	public:
		SurfaceData()
			: paramA(-0.5f),
			  paramB(0.5f),
			  inputType(Controller::NO_INPUT)
		{
		}
		bool operator==(const SurfaceData& other) const
		{
			return
				inputType == other.inputType &&
				paramA == other.paramA &&
				paramB == other.paramB;
		}
		bool isEmpty() const
		{
			return *this == empty();
		}

	public:
		static const SurfaceData& empty()
		{
			static SurfaceData s;
			return s;
		}
	};
}
