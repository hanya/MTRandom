

#ifndef _MT_HPP_
#define _MT_HPP_ 1

#include "MersenneTwister.h"

#include <cppuhelper/implbase3.hxx>

#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include "mytools/math/XMTRandom.hpp"
#include "mytools/math/MTRandom.hpp"


#define IMPLEMENTATION_NAME "mytools.math.MTRandom"
#define SERVICE_NAME "mytools.math.MTRandom"


namespace mytools { namespace math {

class MTRandomImpl : 
	public ::cppu::WeakImplHelper3
		<
			::mytools::math::XMTRandom, 
			::com::sun::star::lang::XInitialization, 
			::com::sun::star::lang::XServiceInfo
		>
{
private :
	MTRand mMTRand;
	
public :
	
	MTRandomImpl( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const &xComponentContext );
	MTRandomImpl(MTRand mtrand);
	~MTRandomImpl();
	
	static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > create( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > &xComponentContext);
	
	// XInitialization
	virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
	
	// XMTRandomImpl
	virtual ::com::sun::star::uno::Reference< ::mytools::math::XMTRandom > SAL_CALL copy(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual ::sal_Int64 SAL_CALL randInt(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual ::sal_Int64 SAL_CALL randIntTo( ::sal_Int64 n ) throw (::com::sun::star::uno::RuntimeException);
	virtual double SAL_CALL rand(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual double SAL_CALL randTo( ::sal_Int64 n ) throw (::com::sun::star::uno::RuntimeException);
	virtual double SAL_CALL randExc(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual double SAL_CALL randExcTo( double n ) throw (::com::sun::star::uno::RuntimeException);
	virtual double SAL_CALL randDblExc(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual double SAL_CALL randDblExcTo( double n ) throw (::com::sun::star::uno::RuntimeException);
	virtual double SAL_CALL rand53(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual double SAL_CALL randNorm( double mean, double stddev ) throw (::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL seed(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL seedInt( ::sal_Int64 n ) throw (::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL seedArray( const ::com::sun::star::uno::Sequence< ::sal_Int64 >& bigSeed ) throw (::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::sal_Int64 > SAL_CALL getState(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL setState( const ::com::sun::star::uno::Sequence< ::sal_Int64 >& aState ) throw (::com::sun::star::uno::RuntimeException);
	
	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);
	
	
	static ::rtl::OUString SAL_CALL getImplementationName_Static();
	static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_Static();
};

} } // namesapce

#endif
