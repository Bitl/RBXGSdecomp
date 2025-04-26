#pragma once
#include <set>

namespace RBX
{
	class IMovingManager;
	class IMoving
	{
		friend class IMovingManager;

	private:
		IMovingManager* iMovingManager;
		int stepsToSleep;
  
	private:
		void makeMoving();
	protected:
		virtual void onCanAggregateChanged(bool canAggregate);
		void setMovingManager(IMovingManager* _iMovingManager);
		bool checkSleep();
	public:
		//IMoving(const IMoving&);
		IMoving();
		~IMoving();
	public:
		void notifyMoved();
		virtual bool reportTouches() const;
		bool getCanAggregate() const;
		//IMoving& operator=(const IMoving&);
	};

	class IMovingManager
	{
		friend class IMoving;

	private:
		std::set<IMoving*> moving;
		std::set<IMoving*>::iterator current;
	protected:
		void remove(IMoving* iMoving);
		void moved(IMoving* iMoving);
	public:
		//IMovingManager(const IMovingManager&);
		IMovingManager();
		virtual ~IMovingManager();
	public:
		void onMovingHeartbeat();
		//IMovingManager& operator=(const IMovingManager&);
	};
}
