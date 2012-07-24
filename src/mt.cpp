
#include "mt.hpp"

using ::rtl::OUString;

using ::com::sun::star::uno::XComponentContext;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::uno::Exception;

namespace mytools { namespace math {


MTRandomImpl::MTRandomImpl( Reference< ::com::sun::star::uno::XComponentContext > const &xComponentContext )
{
	mMTRand = MTRand();
}

MTRandomImpl::MTRandomImpl(MTRand mtrand)
	: mMTRand(mtrand)
{
}

MTRandomImpl::~MTRandomImpl()
{
}

Reference< ::com::sun::star::uno::XInterface > MTRandomImpl::create( const Reference< XComponentContext > &xComponentContext )
{
	return static_cast< ::cppu::OWeakObject* >(new MTRandomImpl( xComponentContext ));
}


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


// XInitialization
void SAL_CALL MTRandomImpl::initialize( const Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (Exception, RuntimeException)
{
	if (aArguments.getLength() == 1)
	{
		sal_Int64 v = 0;
		if ((aArguments[0] >>= v) && v >= 0 && v <= SAL_MAX_UINT32)
		{
			mMTRand = MTRand((sal_uInt32) v);
		}  else {
			Sequence< sal_Int64 > a;
			if (aArguments[0] >>= a)
			{
				Sequence< sal_uInt32 > pSeed;
				checkArraySeed(a, pSeed);
				mMTRand = MTRand((sal_uInt32*) pSeed.getConstArray(), (sal_uInt32) pSeed.getLength());
			} else {
				/*
				Sequence< double > d;
				if (aArguments[0] >>= d)
				{
					
				} else
				*/
					throw RuntimeException(OUString(RTL_CONSTASCII_USTRINGPARAM("Illegal argument.")), Reference< XInterface >());
			}
		}
	}
}


// XMTRandomImpl

Reference< ::mytools::math::XMTRandom > SAL_CALL MTRandomImpl::copy(  ) throw (RuntimeException)
{
	return static_cast< ::mytools::math::XMTRandom* >(new MTRandomImpl(mMTRand));
}

::sal_Int64 SAL_CALL MTRandomImpl::randInt(  ) throw (RuntimeException)
{
	return mMTRand.randInt();
}

::sal_Int64 SAL_CALL MTRandomImpl::randIntTo( ::sal_Int64 n ) throw (RuntimeException)
{
	if (n < 0 || n > UINT_MAX)
		throw RuntimeException();
	return mMTRand.randInt(n);
}

double SAL_CALL MTRandomImpl::rand(  ) throw (RuntimeException)
{
	return mMTRand.rand();
}

double SAL_CALL MTRandomImpl::randTo( ::sal_Int64 n ) throw (RuntimeException)
{
	if (n < 0 || n > UINT_MAX)
		throw RuntimeException();
	return mMTRand.rand(n);
}

double SAL_CALL MTRandomImpl::randExc(  ) throw (RuntimeException)
{
	return mMTRand.randExc();
}

double SAL_CALL MTRandomImpl::randExcTo( double n ) throw (RuntimeException)
{
	return mMTRand.randExc(n);
}

double SAL_CALL MTRandomImpl::randDblExc(  ) throw (RuntimeException)
{
	return mMTRand.randDblExc();
}

double SAL_CALL MTRandomImpl::randDblExcTo( double n ) throw (RuntimeException)
{
	return mMTRand.randDblExc(n);
}

double SAL_CALL MTRandomImpl::rand53(  ) throw (RuntimeException)
{
	return mMTRand.rand53();
}

double SAL_CALL MTRandomImpl::randNorm( double mean, double stddev ) throw (::RuntimeException)
{
	return mMTRand.randNorm(mean, stddev);
}

void SAL_CALL MTRandomImpl::seed(  ) throw (::RuntimeException)
{
	mMTRand.seed();
}

void SAL_CALL MTRandomImpl::seedInt( ::sal_Int64 n ) throw (::RuntimeException)
{
	if (n < 0 || n > UINT_MAX)
		throw RuntimeException();
	mMTRand.seed(n);
}

void SAL_CALL MTRandomImpl::seedArray( const ::Sequence< ::sal_Int64 >& bigSeed ) throw (::RuntimeException)
{
	Sequence< sal_uInt32 > pSeed;
	checkArraySeed(bigSeed, pSeed);
	mMTRand.seed((sal_uInt32*) pSeed.getConstArray(), pSeed.getLength());
}


Sequence< ::sal_Int64 > SAL_CALL MTRandomImpl::getState(  ) throw (RuntimeException)
{
	MTRand::uint32 state[MT__SAVE];//[MTRand::SAVE];
	mMTRand.save(state);
	
	Sequence< sal_Int64 > aRet(MT__SAVE);//(MTRand::SAVE);
	sal_Int64* pArray = aRet.getArray();
	for (int i = 0; i < aRet.getLength(); i++)
		pArray[i] = (sal_Int64) state[i];
	
	return aRet;
}


void SAL_CALL MTRandomImpl::setState( const Sequence< ::sal_Int64 >& aState ) throw (RuntimeException)
{
	if (aState.getLength() == MT__SAVE)//MTRand::SAVE)
	{
		MTRand::uint32 state[MT__SAVE];//MTRand::SAVE];
		sal_Int64* pArray = (sal_Int64*) aState.getConstArray();
		
		for (int i = 0; i < MT__SAVE; i++)//MTRand::SAVE; i++)
		{
			if (! (pArray[i] > 0 && pArray[i] <= UINT_MAX))
				throw RuntimeException();
			state[i] = pArray[i];
		}
		mMTRand.load(state);
	} else
		throw RuntimeException();
}


// XServiceInfo

OUString MTRandomImpl::getImplementationName_Static() 
{
	return OUString::createFromAscii(IMPLEMENTATION_NAME);
}

Sequence< OUString > MTRandomImpl::getSupportedServiceNames_Static() 
{
	Sequence< OUString > aRet(1);
	aRet[0] = OUString::createFromAscii(SERVICE_NAME);
	return aRet;
}


::rtl::OUString SAL_CALL MTRandomImpl::getImplementationName() 
				throw (RuntimeException)
{
	return MTRandomImpl::getImplementationName_Static();
}

::sal_Bool SAL_CALL MTRandomImpl::supportsService( const ::rtl::OUString& ServiceName ) 
				throw (RuntimeException)
{
	return ServiceName.equalsAscii(SERVICE_NAME);
}

Sequence< ::rtl::OUString > SAL_CALL MTRandomImpl::getSupportedServiceNames() 
				throw (RuntimeException)
{
	return MTRandomImpl::getSupportedServiceNames_Static();
}


} } // namespace
