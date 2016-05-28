#if !defined(__SINGLETON_H__)
#define __SINGLETON_H__

template <typename T>
//class __declspec(noinline) SisaSingleton
class SisaSingleton
{
protected:
	SisaSingleton() {}
	virtual ~SisaSingleton() {}

public:
	static T* instance()
	{
		static T s_instance;
		return &s_instance;
	}

private:
	SisaSingleton(const SisaSingleton&) {}
	const SisaSingleton& operator = (const SisaSingleton&) { return *this; }
};

#endif
