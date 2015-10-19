// "TypeName type" will be lazily created on the first time it's accessed

#ifndef BASE_MEMORY_SINGLETON_H_
#define BASE_MEMORY_SINGLETON_H_


namespace nbase
{

#define SINGLETON_DEFINE(TypeName)				\
static TypeName* GetInstance()					\
{												\
	static TypeName type_instance;				\
	return &type_instance;						\
}												\
												\
TypeName(const TypeName&) = delete;				\
TypeName& operator=(const TypeName&) = delete

}



#endif // BASE_MEMORY_SINGLETON_H_
