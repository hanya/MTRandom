
#include "mt.hpp"

#include <cppuhelper/implementationentry.hxx>


static struct cppu::ImplementationEntry g_entries[] = 
{
	{
		mytools::math::MTRandomImpl::create, 
		mytools::math::MTRandomImpl::getImplementationName_Static, 
		mytools::math::MTRandomImpl::getSupportedServiceNames_Static, 
		cppu::createSingleComponentFactory, 
		0, 
		0
	}, 
	{0, 0, 0, 0, 0, 0}
};


extern "C"
{

SAL_DLLPUBLIC_EXPORT void SAL_CALL
component_getImplementationEnvironment(const sal_Char **ppEnvTypeName, uno_Environment **)
{
	*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

SAL_DLLPUBLIC_EXPORT sal_Bool SAL_CALL
component_writeInfo(void *pServiceManager, void *pRegistryKey)
{
	return cppu::component_writeInfoHelper(pServiceManager, pRegistryKey, g_entries);
}

SAL_DLLPUBLIC_EXPORT void* SAL_CALL
component_getFactory(const sal_Char *pImplName, void *pServiceManager, void *pRegistryKey)
{
	return cppu::component_getFactoryHelper(pImplName, pServiceManager, pRegistryKey, g_entries);
}

}
