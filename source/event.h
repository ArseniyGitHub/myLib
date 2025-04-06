#pragma once
#include <thread>
#include "types.h"

namespace LIB {
	class Event {
	public:

		enum BodyCallTypes {
			CallOnce,
			CallWhileActive
		};
	private:
		bool (*_event_flag)();
		void (*_event_body)();

		ui32 _delay = 0;
		bool _active = false;  BodyCallTypes _rs = CallWhileActive;
		std::thread _event_thread;
		
	public:

		void EventBody() {
			bool boof = false;  ui32 _del;
			while (_active) {
				_del = 0;
				if (_event_flag()) {
					_event_body();
					boof = true;
					while (_event_flag() && _rs == BodyCallTypes::CallOnce);
				}
				else boof = false;
				while (_active && _del < _delay) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					_del++;
				}
			}
		}

		void deleteEvent() {
			_active = false;
			_event_thread.detach();
		}

		void set(bool (*activateFlag)(), void (*eventBody)(), ui32 delay = 0, BodyCallTypes rs = BodyCallTypes::CallWhileActive) {
			if(_active) deleteEvent();
			_delay = delay;    _rs = rs;
			_event_flag = activateFlag;
			_event_body = eventBody;
		}

		void start() {
			_active = true;
			_event_thread = std::thread([this]() { EventBody(); });
		}

		Event (bool (*activateFlag)(), void(*eventBody)(), ui32 delay = 0, BodyCallTypes rs = BodyCallTypes::CallWhileActive) {
			set(activateFlag, eventBody, delay, rs);
			start();
		}

		Event(const Event& from) {
			deleteEvent();
			_event_flag = from._event_flag;
			_event_body = from._event_body;
			_delay = from._delay;
			_rs = from._rs;
		}
		Event() {
			;
		}
		~Event() {
			deleteEvent();
		}
	};
}