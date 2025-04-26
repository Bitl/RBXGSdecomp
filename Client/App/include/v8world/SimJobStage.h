#pragma once
#include <list>
#include "v8world/IWorldStage.h"

namespace RBX
{
	class Mechanism;
	class Assembly;
	class World;
	class Edge;

	class SimJobStage : public IWorldStage
	{
	private:
		std::list<Mechanism*> mechanisms;
	  
	private:
		void combineMechanisms(Edge* e);
		void insertMechanism(Mechanism* m);
		void destroyMechanism(Mechanism* m);
		void splitMechanisms(Assembly*, Assembly*);
		bool validateEdge(Edge* e);
	public:
		Mechanism* nextMechanism(std::list<Mechanism*>& list, const Mechanism* current);
	public:
		//SimJobStage(const SimJobStage&);
		SimJobStage(IStage*, World*);
		virtual ~SimJobStage();
	public:
		virtual IStage::StageType getStageType();
		virtual void onEdgeAdded(Edge* e);
		virtual void onEdgeRemoving(Edge* e);
		void onAssemblyAdded(Assembly* a);
		void onAssemblyRemoving(Assembly* a);
		void notifyMovingPrimitives();
		//SimJobStage& operator=(const SimJobStage&);
	};
}
