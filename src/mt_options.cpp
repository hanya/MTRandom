
//#include <stdio.h>

#include "mt_options.hpp"

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/awt/XRadioButton.hpp>
#include <com/sun/star/awt/XButton.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XNumericField.hpp>
#include <com/sun/star/awt/XListBox.hpp>

using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::XInterface;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::beans::XPropertySet;

using ::rtl::OUString;

using namespace ::com::sun::star::awt;

#define AUTOMATIC_OPTION ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AutomaticallyOption"))
#define SIMPLE_OPTION ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SimpleOption"))
#define ARRAY_OPTION ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ArrayOption"))

#define SIMPLE_NUMERIC ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SimpleNumeric"))
#define SIMPLE_SEED_LABEL ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SimpleSeedLabel"))
#define ARRAY_NUMERIC ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ArrayNumeric"))
#define ARRAY_LIST ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ArraySeedList"))

#define ADD_BUTTON ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AddButton"))
#define UP_BUTTON ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("UpButton"))
#define DOWN_BUTTON ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DownButton"))
#define REMOVE_BUTTON ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("RemoveButton"))


namespace mytools { namespace sheet {
/*
enum SeedMode;
class ConfigReader;
class ConfigWriter;
*/

MTRandomOptions::MTRandomOptions( Reference< ::com::sun::star::uno::XComponentContext > const &xComponentContext )
	: m_xComponentContext(xComponentContext)
{
	
}

MTRandomOptions::~MTRandomOptions()
{
}

Reference< ::com::sun::star::uno::XInterface > MTRandomOptions::create( const Reference< ::com::sun::star::uno::XComponentContext > &xComponentContext )
{
	return static_cast< XContainerWindowEventHandler* >(new MTRandomOptions( xComponentContext ));
}

// XContainerWindowEventHandler
::sal_Bool SAL_CALL MTRandomOptions::callHandlerMethod( const Reference< ::com::sun::star::awt::XWindow >& xWindow, const Any& EventObject, const OUString& MethodName ) throw (::com::sun::star::lang::WrappedTargetException, RuntimeException)
{
	if (MethodName.compareToAscii("external_event") == 0)
	{
		OUString evt;
		if (EventObject >>= evt)
		{
			if (evt.compareToAscii("ok") == 0)
			{
				return saveData(xWindow, evt);
			}
			else if (evt.compareToAscii("back") == 0 || 
						evt.compareToAscii("initialize") == 0)
			{
				return loadData(xWindow, evt);
			}
		}
	}
	return sal_False;
}

Sequence< OUString > SAL_CALL MTRandomOptions::getSupportedMethodNames(  ) throw (RuntimeException)
{
	Sequence< OUString > aRet(1);
	aRet[0] = OUString::createFromAscii("external_event");
	return aRet;
}


::sal_Bool MTRandomOptions::loadData( const Reference< XWindow >& xWindow, const OUString eventName )
{
	Reference< XControlContainer > xContainer(xWindow, UNO_QUERY);
	if (xContainer.is())
	{
		// load data from configuration
		ConfigReader* reader = NULL;
		try
		{
			reader = new ConfigReader(m_xComponentContext);
		}
		catch (::com::sun::star::uno::Exception &e)
		{
			return sal_False;
		}
		try
		{
			enum SeedMode mode = reader->GetMode();
			sal_Int64 simple = reader->GetSimple();
			Sequence< sal_Int64 > data = reader->GetArray();
			
			SetMode(xContainer, mode);
			SetModeState(xContainer, mode);
			
			Reference< XControl > xControl;
			
			xControl = xContainer->getControl(SIMPLE_NUMERIC);
			if (xControl.is())
			{
				Reference< XNumericField > xSimpleNumeric(xControl, UNO_QUERY);
				xSimpleNumeric->setValue((double) simple);
			}
			
			xControl = xContainer->getControl(ARRAY_NUMERIC);
			if (xControl.is())
			{
				Reference< XNumericField > xArrayNumeric(xControl, UNO_QUERY);
				xArrayNumeric->setValue(0.0);
			}
			
			xControl = xContainer->getControl(ARRAY_LIST);
			if (xControl.is())
			{
				Sequence< OUString > strData(data.getLength());
				sal_Int64* pData = (sal_Int64*)data.getConstArray();
				for (int i = 0; i < data.getLength(); i++)
				{
					strData[i] = OUString::valueOf(pData[i]);
				}
				Reference< XListBox > xListBox(xControl, UNO_QUERY);
				xListBox->removeItems(0, xListBox->getItemCount());
				xListBox->addItems(strData, (sal_Int16) 0);
			}
			
			// option buttons
			xControl = xContainer->getControl(AUTOMATIC_OPTION);
			if (xControl.is())
			{
				Reference< XRadioButton > xAutomaticallyRadio(xControl, UNO_QUERY);
				if (xAutomaticallyRadio.is())
					xAutomaticallyRadio->addItemListener(this);
			}
			xControl = xContainer->getControl(SIMPLE_OPTION);
			if (xControl.is())
			{
				Reference< XRadioButton > xSimpleRadio(xControl, UNO_QUERY);
				if (xSimpleRadio.is())
					xSimpleRadio->addItemListener(this);
			}
			xControl = xContainer->getControl(ARRAY_OPTION);
			if (xControl.is())
			{
				Reference< XRadioButton > xArrayRadio(xControl, UNO_QUERY);
				if (xArrayRadio.is())
					xArrayRadio->addItemListener(this);
			}
			
			// buttons
			xControl = xContainer->getControl(ADD_BUTTON);
			if (xControl.is())
			{
				Reference< XButton > xAddButton(xControl, UNO_QUERY);
				if (xAddButton.is())
				{
					xAddButton->setActionCommand(ADD_BUTTON);
					xAddButton->addActionListener(this);
				}
			}
			xControl = xContainer->getControl(UP_BUTTON);
			if (xControl.is())
			{
				Reference< XButton > xUpButton(xControl, UNO_QUERY);
				if (xUpButton.is())
				{
					xUpButton->setActionCommand(UP_BUTTON);
					xUpButton->addActionListener(this);
				}
			}
			xControl = xContainer->getControl(DOWN_BUTTON);
			if (xControl.is())
			{
				Reference< XButton > xDownButton(xControl, UNO_QUERY);
				if (xDownButton.is())
				{
					xDownButton->setActionCommand(DOWN_BUTTON);
					xDownButton->addActionListener(this);
				}
			}
			xControl = xContainer->getControl(REMOVE_BUTTON);
			if (xControl.is())
			{
				Reference< XButton > xRemoveButton(xControl, UNO_QUERY);
				if (xRemoveButton.is())
				{
					xRemoveButton->setActionCommand(REMOVE_BUTTON);
					xRemoveButton->addActionListener(this);
				}
			}
		}
		catch (::com::sun::star::uno::Exception &e)
		{
		}
		delete reader;
		
		return sal_True;
	}
	return sal_False;
}


enum SeedMode MTRandomOptions::GetMode( const Reference< XControlContainer >& xContainer )
{
	Reference< XControl > xAutomaticallyControl = xContainer->getControl(AUTOMATIC_OPTION);
	Reference< XControl > xSimpleControl = xContainer->getControl(SIMPLE_OPTION);
	Reference< XControl > xArrayControl = xContainer->getControl(ARRAY_OPTION);

	sal_Int16 nState = 0;
	if (xAutomaticallyControl.is())
	{
		Reference< XControlModel > xAutomaticallModel = xAutomaticallyControl->getModel();
		Reference< XPropertySet > xAutomaticallProps(xAutomaticallModel, UNO_QUERY);
		
		xAutomaticallProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("State"))) >>= nState;
		if (nState == 1)
			return AUTOMATIC;
	}
	if (xSimpleControl.is())
	{
		Reference< XControlModel > xSimpleModel = xSimpleControl->getModel();
		Reference< XPropertySet > xSimpleProps(xSimpleModel, UNO_QUERY);
		
		xSimpleProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("State"))) >>= nState;
		if (nState == 1)
			return SIMPLE;
	}

	if (xSimpleControl.is())
	{
		Reference< XControlModel > xArrayModel = xArrayControl->getModel();
		Reference< XPropertySet > xArrayProps(xArrayModel, UNO_QUERY);
		
		xArrayProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("State"))) >>= nState;
		if (nState == 1)
			return ARRAY;
	}
	return AUTOMATIC;
}


// set option state without event broadcasting
void MTRandomOptions::SetMode( const Reference< XControlContainer >& xContainer, const enum SeedMode mode )
{
	Reference< XControl > xControl;
	switch (mode)
	{
		case AUTOMATIC:
			xControl = xContainer->getControl(AUTOMATIC_OPTION);
			break;
		case SIMPLE:
			xControl = xContainer->getControl(SIMPLE_OPTION);
			break;
		case ARRAY:
			xControl = xContainer->getControl(ARRAY_OPTION);
			break;
	}
	
	if (xControl.is())
	{
		Reference< XControlModel > xControlModel = xControl->getModel();
		Reference< XPropertySet > xPropSet(xControlModel, UNO_QUERY);
		xPropSet->setPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("State")), ::com::sun::star::uno::makeAny((sal_Int16) 1));
	}
}


// set mode state and update enabled state of controls
void MTRandomOptions::SetModeState( const Reference< XControlContainer >& xContainer, const enum SeedMode mode )
{
	Reference< XWindow > xSimpleNumeric(xContainer->getControl(SIMPLE_NUMERIC), UNO_QUERY);
	Reference< XWindow > xSimpleSeedLabel(xContainer->getControl(SIMPLE_SEED_LABEL), UNO_QUERY);
	
	Reference< XWindow > xArrayNumeric(xContainer->getControl(ARRAY_NUMERIC), UNO_QUERY);
	Reference< XWindow > xAddButton(xContainer->getControl(ADD_BUTTON), UNO_QUERY);
	Reference< XWindow > xArraySeedList(xContainer->getControl(ARRAY_LIST), UNO_QUERY);
	Reference< XWindow > xUpButton(xContainer->getControl(UP_BUTTON), UNO_QUERY);
	Reference< XWindow > xDownButton(xContainer->getControl(DOWN_BUTTON), UNO_QUERY);
	Reference< XWindow > xRemoveButton(xContainer->getControl(REMOVE_BUTTON), UNO_QUERY);
	
	sal_Bool bSimpleEnabled = sal_False;
	sal_Bool bArrayEnabled = sal_False;
	switch (mode)
	{
		//case AUTOMATIC:
		//  break;
		case SIMPLE:
			bSimpleEnabled = sal_True;
			break;
		case ARRAY:
			bArrayEnabled = sal_True;
			break;
	}
	
	xSimpleNumeric->setEnable(bSimpleEnabled);
	xSimpleSeedLabel->setEnable(bSimpleEnabled);
	
	xArrayNumeric->setEnable(bArrayEnabled);
	xArraySeedList->setEnable(bArrayEnabled);
	xAddButton->setEnable(bArrayEnabled);
	xUpButton->setEnable(bArrayEnabled);
	xDownButton->setEnable(bArrayEnabled);
	xRemoveButton->setEnable(bArrayEnabled);
}


::sal_Bool MTRandomOptions::saveData( const Reference< XWindow >& xWindow, const OUString eventName )
{
	Reference< XControlContainer > xContainer(xWindow, UNO_QUERY);
	if (xContainer.is())
	{
		// save data to configuration
		ConfigWriter* writer;
		try
		{
			writer = new ConfigWriter(m_xComponentContext);
		}
		catch (::com::sun::star::uno::Exception &e)
		{
			return sal_False;
		}
		try
		{
			enum SeedMode mode = GetMode(xContainer);
			writer->SetMode(mode);
			
			Reference< XControl > xControl;
			xControl = xContainer->getControl(SIMPLE_NUMERIC);
			if (xControl.is())
			{
				Reference< XNumericField > xSimpleNumeric(xControl, UNO_QUERY);
				double v = xSimpleNumeric->getValue();
				if (v >= 0 && v <= SAL_MAX_UINT32)
				{
					sal_Int64 d = (sal_Int64) v;
					writer->SetSimple(d);
				}
			}
			
			xControl = xContainer->getControl(ARRAY_LIST);
			if (xControl.is())
			{
				Reference< XListBox > xArrayList(xControl, UNO_QUERY);
				Sequence< OUString > strData = xArrayList->getItems();
				try
				{
					writer->SetArrayString(strData);
				} catch (RuntimeException &)
				{
				}
			}
			writer->Commit();
		}
		catch (::com::sun::star::uno::Exception &e)
		{
		}
		delete writer;
	}
	return sal_False;
}


// XEventListener
void SAL_CALL MTRandomOptions::disposing( const ::com::sun::star::lang::EventObject& Source ) throw (RuntimeException)
{
}

// XItemLister
void SAL_CALL MTRandomOptions::itemStateChanged( const ::com::sun::star::awt::ItemEvent& rEvent ) throw (RuntimeException)
{
	Reference< XControl > xControl(rEvent.Source, UNO_QUERY);
	if (xControl.is())
	{
		Reference< XInterface > window = xControl->getContext();
		if (window.is())
		{
			Reference< XControlContainer > xContainer(window, UNO_QUERY);
			if (xContainer.is())
			{
				enum SeedMode mode = GetMode(xContainer);
				SetModeState(xContainer, mode);
			}
		}
	}
}

// XActionListener
void SAL_CALL MTRandomOptions::actionPerformed( const ::com::sun::star::awt::ActionEvent& rEvent ) throw (RuntimeException)
{
	Reference< XControl > xControl(rEvent.Source, UNO_QUERY);
	
	Reference< XControlContainer > xContainer(xControl->getContext(), UNO_QUERY);
	if (! xContainer.is())
		return;
	
	OUString sCmd = rEvent.ActionCommand;
	Reference< XControl > xListControl = xContainer->getControl(ARRAY_LIST);
	Reference< XListBox > xListBox(xListControl, UNO_QUERY);
	
	if (sCmd.compareTo(ADD_BUTTON) == 0)
	{
		// add value from the numeric field to the listbox
		Reference< XControl > xControl = xContainer->getControl(ARRAY_NUMERIC);
		Reference< XNumericField > xArrayNumeric(xControl, UNO_QUERY);
		double v = xArrayNumeric->getValue();
		if (v >= 0 && 0 <= SAL_MAX_UINT32)
		{
			OUString strValue = OUString::valueOf((sal_Int64) v);
			sal_Int16 pos = xListBox->getSelectedItemPos();
			if (pos >= 0)
			{
				xListBox->addItem(strValue, pos + 1);
			} else {
				xListBox->addItem(strValue, (sal_Int16)xListBox->getItemCount());
			}
		} else {
			//
		}
	}
	else if (sCmd.compareTo(UP_BUTTON) == 0)
	{
		// move up the selected value
		sal_Int16 pos = xListBox->getSelectedItemPos();
		if (pos > 0)
		{
			OUString item = xListBox->getSelectedItem();
			xListBox->removeItems(pos, (sal_Int16) 1);
			xListBox->addItem(item, pos - 1);
			xListBox->selectItemPos(pos - 1, sal_True);
		}
	}
	else if (sCmd.compareTo(DOWN_BUTTON) == 0)
	{
		// move down the selected value
		sal_Int16 pos = xListBox->getSelectedItemPos();
		if (pos < (xListBox->getItemCount() - 1))
		{
			OUString item = xListBox->getSelectedItem();
			xListBox->removeItems(pos, (sal_Int16) 1);
			xListBox->addItem(item, pos + 1);
			xListBox->selectItemPos(pos + 1, sal_True);
		}
	}
	else if (sCmd.compareTo(REMOVE_BUTTON) == 0)
	{
		// remove a selected value from the listbox
		sal_Int16 pos = xListBox->getSelectedItemPos();
		if (pos >= 0)
		{
			xListBox->removeItems(pos, (sal_Int16) 1);
		}
	}
}


// XServiceInfo

OUString MTRandomOptions::getImplementationName_Static() 
{
	return OUString::createFromAscii(IMPLEMENTATION_NAME);
}

Sequence< OUString > MTRandomOptions::getSupportedServiceNames_Static() 
{
	Sequence< OUString > aRet(1);
	aRet[0] = OUString::createFromAscii(SERVICE_NAME);
	return aRet;
}


OUString SAL_CALL MTRandomOptions::getImplementationName() 
				throw (RuntimeException)
{
	return MTRandomOptions::getImplementationName_Static();
}

::sal_Bool SAL_CALL MTRandomOptions::supportsService( const OUString& ServiceName ) 
				throw (RuntimeException)
{
	return ServiceName.equalsAscii(SERVICE_NAME);
}

Sequence< OUString > SAL_CALL MTRandomOptions::getSupportedServiceNames() 
				throw (RuntimeException)
{
	return MTRandomOptions::getSupportedServiceNames_Static();
}


} } // namespace
