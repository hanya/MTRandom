
#ifndef _MT_OPTIONS_HPP_
#define _MT_OPTIONS_HPP_ 1

#include "mt_config.hpp"

#include <cppuhelper/implbase4.hxx>

#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/awt/XActionListener.hpp>
#include <com/sun/star/awt/XItemListener.hpp>
#include <com/sun/star/awt/XContainerWindowEventHandler.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>


#define IMPLEMENTATION_NAME "mytools.sheet.MTRandomOptions"
#define SERVICE_NAME "mytools.sheet.MTRandomOptions"


namespace mytools { namespace sheet {


class MTRandomOptions : 
	public ::cppu::WeakImplHelper4
		<
			::com::sun::star::awt::XContainerWindowEventHandler, 
			::com::sun::star::awt::XActionListener, 
			::com::sun::star::awt::XItemListener, 
			::com::sun::star::lang::XServiceInfo
		>
{
private :
	
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > m_xComponentContext;
	
	::sal_Bool loadData( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& xWindow, const ::rtl::OUString eventName );
	::sal_Bool saveData( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& xWindow, const ::rtl::OUString eventName );
	
	enum SeedMode GetMode( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >& xContainer );
	void SetMode( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >& xContainer, const enum SeedMode mode );
	void SetModeState( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >& xContainer, const enum SeedMode mode );
public : 
	
	MTRandomOptions( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >&xComponentContext );
	~MTRandomOptions();
	
	static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > create( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > &xComponentContext);
	
	// XContainerWindowEventHandler
	virtual ::sal_Bool SAL_CALL callHandlerMethod( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& xWindow, const ::com::sun::star::uno::Any& EventObject, const ::rtl::OUString& MethodName ) throw (::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedMethodNames(  ) throw (::com::sun::star::uno::RuntimeException);
	
	// XEventListener
	virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);
	
	// XActionListener
	virtual void SAL_CALL actionPerformed( const ::com::sun::star::awt::ActionEvent& rEvent ) throw (::com::sun::star::uno::RuntimeException);
	
	// XItemLister
	virtual void SAL_CALL itemStateChanged( const ::com::sun::star::awt::ItemEvent& rEvent ) throw (::com::sun::star::uno::RuntimeException);
	
	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);
	
	static ::rtl::OUString SAL_CALL getImplementationName_Static();
	static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_Static();
};


} } // namespace

#endif
