
//#include <stdio.h>
#include "mt_config.hpp"

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/XChangesBatch.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

using ::com::sun::star::lang::XMultiComponentFactory;
using ::com::sun::star::lang::XMultiServiceFactory;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::XInterface;

using ::rtl::OUString;

namespace mytools { namespace sheet {


ConfigReader::ConfigReader( Reference< ::com::sun::star::uno::XComponentContext > const &xComponentContext )
{
	Reference< XMultiComponentFactory > xMcf = xComponentContext->getServiceManager();
	Reference< XMultiServiceFactory > xMsf(
		xMcf->createInstanceWithContext(CONFIG_PROVIDER_NAME, xComponentContext), UNO_QUERY);
	
	Sequence< Any > arguments(1);
	arguments[0] = ::com::sun::star::uno::makeAny(
		::com::sun::star::beans::PropertyValue(NAME_NODE, 0, 
			::com::sun::star::uno::makeAny(SEED_NODE_NAME), 
			::com::sun::star::beans::PropertyState_DIRECT_VALUE));
	
	Reference< XInterface > rAccess = xMsf->createInstanceWithArguments(CONFIG_ACCESS_NAME, arguments);
	m_rAccess = rAccess;
}

ConfigReader::~ConfigReader()
{
}

enum SeedMode ConfigReader::GetMode()
{
	enum SeedMode retMode = AUTOMATIC;
	
	sal_Int32 mode = 0;
	Reference< XPropertySet > xPropSet(m_rAccess, UNO_QUERY);
	xPropSet->getPropertyValue(MODE_NAME) >>= mode;
	switch (mode)
	{
		//case 0:
		//	return AUTOMATIC;
		//	break;
		case 1:
			retMode = SIMPLE;
			break;
		case 2:
			retMode = ARRAY;
			break; 
	}
	return retMode;
}

::sal_Int64 ConfigReader::GetSimple()
{
	sal_Int64 retSimple = 0;
	
	Reference< XPropertySet > xPropSet(m_rAccess, UNO_QUERY);
	xPropSet->getPropertyValue(SIMPLE_NAME) >>= retSimple;
	return retSimple;
}

::com::sun::star::uno::Sequence< sal_Int64 > ConfigReader::GetArray()
{
	Sequence< sal_Int64 > retData(0);
	Sequence< sal_Int8 > data;
	Reference< XPropertySet > xPropSet(m_rAccess, UNO_QUERY);
	xPropSet->getPropertyValue(ARRAY_NAME) >>= data;
	if (data.getLength() > 0)
	{
		if ((data.getLength() > 0) && ((data.getLength() % 4) == 0))
		{
			const sal_Int8* pData = data.getConstArray();
			retData.realloc(data.getLength() / 4);
			sal_Int64* pArray = retData.getArray();
			
			for (int i = 0; i < data.getLength();)
			{
				// little endian
				pArray[i / 4] = ((0x80 + pData[i+3]) << 24) ^ ((0x80 + pData[i+2]) << 16) ^ ((0x80 + pData[i+1]) << 8) ^ (0x80 + pData[i]);
				i += 4;
			}
		}
	}
	return retData;
}



ConfigWriter::ConfigWriter( Reference< ::com::sun::star::uno::XComponentContext > const &xComponentContext )
	: m_xComponentContext(xComponentContext)
{
}

ConfigWriter::~ConfigWriter()
{
}

void ConfigWriter::SetMode( const enum SeedMode mode )
{
	m_Mode = mode;
}

void ConfigWriter::SetSimple( const ::sal_Int64 seed )
{
	m_Seed = seed;
}

void ConfigWriter::SetArray( const ::com::sun::star::uno::Sequence< ::sal_Int64 >& data )
{
	m_Data = data;
}

void ConfigWriter::SetArrayString( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& strData ) throw (RuntimeException)
{
	try
	{
		OUString* pStrData = (OUString*) strData.getConstArray();
		
		Sequence< sal_Int64 > data(strData.getLength());
		sal_Int64* pData = (sal_Int64*) data.getArray();
		
		for (int i = 0; i < strData.getLength(); i++)
		{
			pData[i] = pStrData[i].toInt64();
		}
		SetArray(data);
	}
	catch (::com::sun::star::uno::Exception &e)
	{
		throw RuntimeException();
	}
}

void ConfigWriter::Commit()
{
	Reference< XMultiComponentFactory > xMcf = m_xComponentContext->getServiceManager();
	Reference< XMultiServiceFactory > xMsf(
		xMcf->createInstanceWithContext(CONFIG_PROVIDER_NAME, m_xComponentContext), UNO_QUERY);
	
	Sequence< Any > arguments(1);
	arguments[0] = ::com::sun::star::uno::makeAny(
		::com::sun::star::beans::PropertyValue(NAME_NODE, 0, 
			::com::sun::star::uno::makeAny(SEED_NODE_NAME), 
			::com::sun::star::beans::PropertyState_DIRECT_VALUE));
	
	Reference< XInterface > rUpdateAccess = xMsf->createInstanceWithArguments(CONFIG_UPDATE_ACCESS_NAME, arguments);
	
	sal_Int32 mode = 0;
	switch (m_Mode)
	{
		//case AUTOMATIC:
		//	mode = 0;
		//		break;
		case SIMPLE:
			mode = 1;
			break;
		case ARRAY:
			mode = 2;
			break;
	}
	
	Sequence< sal_Int8 > data(m_Data.getLength() * 4);
	sal_Int8* pData = data.getArray();
	sal_Int64* pArray = (sal_Int64*) m_Data.getConstArray();
	
	for (int i = 0; i < m_Data.getLength(); i++)
	{
		sal_Int64 n = pArray[i];
		// little endian
		pData[i * 4] = (n & 0xff) - 0x80;
		pData[i * 4 + 1] = ((n & 0xff00) >> 8) - 0x80;
		pData[i * 4 + 2] = ((n & 0xff0000) >> 16) - 0x80;
		pData[i * 4 + 3] = ((n & 0xff000000) >> 24) - 0x80;
	}
	
	Reference< XPropertySet > xPropSet(rUpdateAccess, UNO_QUERY);
	xPropSet->setPropertyValue(MODE_NAME, ::com::sun::star::uno::makeAny(mode));
	xPropSet->setPropertyValue(SIMPLE_NAME, ::com::sun::star::uno::makeAny(m_Seed));
	xPropSet->setPropertyValue(ARRAY_NAME, ::com::sun::star::uno::makeAny(data));
	
	Reference< ::com::sun::star::util::XChangesBatch > xChangesBatch(rUpdateAccess, UNO_QUERY);
	xChangesBatch->commitChanges();
}

} } // namespace
