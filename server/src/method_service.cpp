#include <method_service.h>

#ifdef USE_QUERY_GENERATOR
#include <decl.h>
#endif

namespace abcserver
{

void MethodService::LoadMethods()
{
#ifdef USE_QUERY_GENERATOR
    RegisterMethods(methods_);
#endif
}

MethodService::MethodType& MethodService::Method(const std::string& method)
{
    auto it = methods_.find(method);

    if(it == methods_.end())
        throw std::invalid_argument{"Method " + method + " not found"};

    return it->second;
}

}
