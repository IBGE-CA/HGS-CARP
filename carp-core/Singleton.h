#pragma once

namespace carp_core
{
	template<typename T>
	class Singleton
	{
	public:
		virtual ~Singleton() = default;

		static T& getInstance()
		{
			if (singleton == nullptr)
				singleton = new T();
			return *singleton;
		}

		static T& getBuilt()
		{
			return *singleton;
		}

		void destroy()
		{
			delete singleton;
		}

	protected:
		Singleton() = default;

		static T* singleton;
	};

	template<typename T>
	T* Singleton<T>::singleton = nullptr;
}
