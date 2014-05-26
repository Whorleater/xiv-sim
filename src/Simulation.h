#pragma once

#include "Actor.h"

#include <chrono>
#include <queue>
#include <random>
#include <string>
#include <memory>

class Simulation {
	public:
		struct Configuration {
			std::chrono::microseconds length;
			std::random_device* rng = nullptr;
			Actor::Configuration* subjectConfiguration = nullptr;
			Actor::Configuration* targetConfiguration = nullptr;
		};

		Simulation(const Configuration* configuration)
			: _configuration(configuration)
			, _subject(configuration->subjectConfiguration)
			, _target(configuration->targetConfiguration)
		{}

		void run();
		
		const Actor* subject() const { return &_subject; }

	private:
		const Configuration* const _configuration = nullptr;
		Actor _subject;
		Actor _target;

		std::chrono::microseconds _time = 0_us;

		struct ScheduledFunction {
			ScheduledFunction(std::shared_ptr<std::function<void()>> function, std::chrono::microseconds time, uint64_t order)
				: function(function), time(time), order(order) {}
			
			std::shared_ptr<std::function<void()>> function;
			std::chrono::microseconds time;
			uint64_t order;
			
			bool operator<(const ScheduledFunction& other) const {
				if (time > other.time) {
					return true;
				} else if (time == other.time) {
					return order > other.order;
				}
				return false;
			}
		};
	
		std::priority_queue<ScheduledFunction> _scheduledFunctions;	
		uint64_t _nextOrder = 0;
		bool _shouldStop = false;

		void _advanceTime(std::chrono::microseconds time);

		void _schedule(const std::function<void()>& function, std::chrono::microseconds delay = 0_us);

		void _checkActors();
		void _tick();
		void _resolveAction(const Action* action, Actor* subject, Actor* target);
};
