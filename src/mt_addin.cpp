
#include "mt_addin.hpp"

#include <rtl/math.hxx>

using ::rtl::OUString;

using ::com::sun::star::uno::XComponentContext;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::uno::Exception;


namespace mytools { namespace sheet {


void checkArraySeed( const Sequence< sal_Int64 >& aSeed, Sequence< sal_uInt32 >& aChecked ) throw (RuntimeException)
{
	sal_uInt64* pArray = (sal_uInt64*) aSeed.getConstArray();
	aChecked.realloc(aSeed.getLength());
	
	for (int i = 0; i < aSeed.getLength(); i++)
	{
		if (pArray[i] < 0 || pArray[i] > SAL_MAX_UINT32)
			throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("Illegal argument.")), Reference< ::com::sun::star::uno::XInterface >());
		aChecked[i] = (sal_uInt32) pArray[i];
	}
}


MTRandomAddIn::MTRandomAddIn( Reference< ::com::sun::star::uno::XComponentContext > const &xComponentContext )
{
    ::sal_Bool bInitialized = sal_False;
	ConfigReader* reader;
	try
	{
		reader = new ConfigReader(xComponentContext);
		
		enum SeedMode mode = reader->GetMode();
		switch (mode)
		{
			case AUTOMATIC:
            {
				mMTRand = MTRand();
				bInitialized = sal_True;
				break;
            }
			case SIMPLE:
            {
				sal_Int64 seed = 0;
                seed = reader->GetSimple();
				if (seed >= 0 && seed <= SAL_MAX_UINT32)
				{
					mMTRand = MTRand((sal_uInt32) seed);
					bInitialized = sal_True;
				}
				break;
            }
			case ARRAY:
            {
				Sequence< sal_Int64 > bigSeed = reader->GetArray();
				try
				{
					Sequence< sal_uInt32 > pSeed;
					checkArraySeed(bigSeed, pSeed);
					mMTRand.seed((MTRand::uint32*) pSeed.getConstArray(), pSeed.getLength());
					bInitialized = sal_True;
				}
				catch (RuntimeException &)
				{
				}
				break;
            }
		}
		delete reader;
	} catch (Exception &) {
		// failed to initialize with configuration value
	}
	
	if (! bInitialized)
		mMTRand = MTRand();
}

MTRandomAddIn::~MTRandomAddIn()
{
}

Reference< ::com::sun::star::uno::XInterface > MTRandomAddIn::create( const Reference< XComponentContext > &xComponentContext )
{
	return static_cast< ::cppu::OWeakObject* >(new MTRandomAddIn( xComponentContext ));
}


// XMTRandomAddIn
double SAL_CALL MTRandomAddIn::getMTRand(  ) throw (RuntimeException)
{
    osl::MutexGuard g(lock);
	return mMTRand.randExc();
}

double SAL_CALL MTRandomAddIn::getMTRandBetween( double min, double max ) throw (::com::sun::star::lang::IllegalArgumentException, RuntimeException)
{
    osl::MutexGuard g(lock);
	min = ::rtl::math::round(min, 0, rtl_math_RoundingMode_Up);
	max = ::rtl::math::round(max, 0, rtl_math_RoundingMode_Up);
	if (min > max)
		throw ::com::sun::star::lang::IllegalArgumentException();
	
	//return floor((max - min + 1.0) * mMTRandBetween.rand() + min);
	return floor((max - min + 1.0) * mMTRand.rand() + min);
}

// not implemented yet.
// XLocalizable
void SAL_CALL MTRandomAddIn::setLocale( const ::com::sun::star::lang::Locale& eLocale ) throw (::com::sun::star::uno::RuntimeException)
{
	mLocale = eLocale;
}

::com::sun::star::lang::Locale SAL_CALL MTRandomAddIn::getLocale(  ) throw (::com::sun::star::uno::RuntimeException)
{
	return mLocale;
}


// XAddIn
OUString SAL_CALL MTRandomAddIn::getProgrammaticFuntionName( const OUString& aDisplayName ) throw (RuntimeException)
{
	if (aDisplayName.compareToAscii(MTRAND_DISPLAY_NAME) == 0)
		return OUString::createFromAscii(MTRAND_NAME);
	else if (aDisplayName.compareToAscii(MTRANDBETWEEN_DISPLAY_NAME) == 0)
		return OUString::createFromAscii(MTRANDBETWEEN_NAME);
	else
		throw RuntimeException();
}

OUString SAL_CALL MTRandomAddIn::getDisplayFunctionName( const OUString& aProgrammaticName ) throw (RuntimeException)
{
	if (aProgrammaticName.compareToAscii(MTRAND_NAME) == 0)
		return OUString::createFromAscii(MTRAND_DISPLAY_NAME);
	else if (aProgrammaticName.compareToAscii(MTRANDBETWEEN_NAME) == 0)
		return OUString::createFromAscii(MTRANDBETWEEN_DISPLAY_NAME);
	else
		throw RuntimeException();
}

OUString SAL_CALL MTRandomAddIn::getFunctionDescription( const OUString& aProgrammaticName ) throw (RuntimeException)
{
	if (aProgrammaticName.compareToAscii(MTRAND_NAME) == 0)
		return OUString::createFromAscii(MTRAND_DESCRIPTION);
	else if (aProgrammaticName.compareToAscii(MTRANDBETWEEN_NAME) == 0)
		return OUString::createFromAscii(MTRANDBETWEEN_DESCRIPTION);
	else
		throw RuntimeException();
}

OUString SAL_CALL MTRandomAddIn::getDisplayArgumentName( const OUString& aProgrammaticFunctionName, ::sal_Int32 nArgument ) throw (RuntimeException)
{
	if (aProgrammaticFunctionName.compareToAscii(MTRANDBETWEEN_NAME) == 0)
	{
		if (nArgument == 0)
			return OUString::createFromAscii(MTRANDBETWEEN_ARG_NAME_1);
		else if (nArgument == 1)
			return OUString::createFromAscii(MTRANDBETWEEN_ARG_NAME_2);
	}
	throw RuntimeException();
}

OUString SAL_CALL MTRandomAddIn::getArgumentDescription( const OUString& aProgrammaticFunctionName, ::sal_Int32 nArgument ) throw (RuntimeException)
{
	if (aProgrammaticFunctionName.compareToAscii(MTRANDBETWEEN_NAME) == 0)
	{
		if (nArgument == 0)
			return OUString::createFromAscii(MTRANDBETWEEN_ARG_DESC_1);
		else if (nArgument == 1)
			return OUString::createFromAscii(MTRANDBETWEEN_ARG_DESC_2);
	}
	throw RuntimeException();
}

OUString SAL_CALL MTRandomAddIn::getProgrammaticCategoryName( const OUString& aProgrammaticFunctionName ) throw (RuntimeException)
{
	return OUString::createFromAscii(CATEGORY_NAME);
}

OUString SAL_CALL MTRandomAddIn::getDisplayCategoryName( const OUString& aProgrammaticFunctionName ) throw (RuntimeException)
{
	return OUString::createFromAscii(CATEGORY_NAME);
}

// XServiceName
OUString SAL_CALL MTRandomAddIn::getServiceName(  ) throw (::com::sun::star::uno::RuntimeException)
{
	return OUString::createFromAscii(SERVICE_NAME);
}

// XCompatibilityNames
/*
Sequence< ::com::sun::star::sheet::LocalizedName > SAL_CALL MTRandomAddIn::getCompatibilityNames( const OUString& aProgrammaticName ) throw (RuntimeException);
*/

// XServiceInfo

OUString MTRandomAddIn::getImplementationName_Static() 
{
	return OUString::createFromAscii(IMPLEMENTATION_NAME);
}

Sequence< OUString > MTRandomAddIn::getSupportedServiceNames_Static() 
{
	Sequence< OUString > aRet(2);
	aRet[0] = OUString::createFromAscii(SERVICE_NAME);
	aRet[1] = OUString::createFromAscii(ADDIN_SERVICE_NAME);
	return aRet;
}


::rtl::OUString SAL_CALL MTRandomAddIn::getImplementationName() 
				throw (RuntimeException)
{
	return MTRandomAddIn::getImplementationName_Static();
}

::sal_Bool SAL_CALL MTRandomAddIn::supportsService( const ::rtl::OUString& ServiceName ) 
				throw (RuntimeException)
{
	return ServiceName.equalsAscii(SERVICE_NAME) || 
		ServiceName.equalsAscii(ADDIN_SERVICE_NAME);
}

Sequence< ::rtl::OUString > SAL_CALL MTRandomAddIn::getSupportedServiceNames() 
				throw (RuntimeException)
{
	return MTRandomAddIn::getSupportedServiceNames_Static();
}

} } // namespace
