
#ifndef _MT_CONFIG_HPP_
#define _MT_CONFIG_HPP_ 1

#include <com/sun/star/uno/XComponentContext.hpp>


#define SEED_NODE_NAME ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/mytools.sheet.MTRandom/Seed"))

#define MODE_NAME ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Mode"))
#define SIMPLE_NAME ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Simple"))
#define ARRAY_NAME ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Array"))

#define NAME_NODE ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("nodepath"))

#define CONFIG_PROVIDER_NAME ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationProvider"))
#define CONFIG_ACCESS_NAME ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationAccess"))
#define CONFIG_UPDATE_ACCESS_NAME ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.configuration.ConfigurationUpdateAccess"))


namespace mytools { namespace sheet {

enum SeedMode { AUTOMATIC, SIMPLE, ARRAY };

class ConfigReader
{
private : 
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > m_rAccess;
public : 
	ConfigReader( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >&xComponentContext );
	~ConfigReader();
	
	enum SeedMode GetMode();
	::sal_Int64 GetSimple();
	::com::sun::star::uno::Sequence< sal_Int64 > GetArray();
};

class ConfigWriter
{
private : 
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > m_xComponentContext;
		
	enum SeedMode m_Mode;
	::sal_Int64 m_Seed;
	::com::sun::star::uno::Sequence< ::sal_Int64 > m_Data;
	
public : 
	ConfigWriter( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >&xComponentContext );
	~ConfigWriter();
	
	void SetMode( const enum SeedMode mode );
	void SetSimple( const ::sal_Int64 seed );
	void SetArray( const ::com::sun::star::uno::Sequence< ::sal_Int64 >& data );
	void SetArrayString( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& strData ) throw (::com::sun::star::uno::RuntimeException);
	
	void Commit();
};

} } // namespace

#endif
