
EXTENSION_VERSION=0.2.5
EXTENSION_STATE=
EXTENSION_ID=mytools.math.MTRandom
EXTENSION_NAME=MTRandom
EXTENSION_DISPLAY_NAME=Mersenne Twister Random Number Generator Add-In

OPTIONS_PAGE_LABEL=MT Random

PRJ=$(OO_SDK_HOME)
SETTINGS=$(PRJ)/settings

include $(SETTINGS)/settings.mk
include $(SETTINGS)/std.mk
include $(SETTINGS)/dk.mk
include $(SETTINGS)/platform.mk

COMP_NAME=mtrandom
COMP_IMPL_NAME=$(COMP_NAME).uno.$(SHAREDLIB_EXT)
RDB_NAME=MTRandom
RDB_FILE_NAME=$(RDB_NAME).rdb

ADDINCOMP_NAME=mtrandomaddin
ADDINCOMP_IMPL_NAME=$(ADDINCOMP_NAME).uno.$(SHAREDLIB_EXT)
ADDINRDB_NAME=MTRandomAddIn
ADDINRDB_FILE_NAME=$(ADDINRDB_NAME).rdb

OPTIONS_COMP_NAME=mtoptions
OPTIONS_IMPL_NAME=$(OPTIONS_COMP_NAME).uno.$(SHAREDLIB_EXT)

ifeq "$(OS)" "WIN"
ORIGIN_NAME=%%origin%%
CC_FLAGS+= /O2 
else
ORIGIN_NAME=%origin%
CC_FLAGS=-c -O -fpic
COMP_LINK_FLAGS=$(LIBRARY_LINK_FLAGS) 
ifeq "$(SDKVERSION)" "3.4"
CC_FLAGS+= -fvisibility=hidden
else
COMP_LINK_FLAGS+= -Wl,--version-script,$(SETTINGS)/component.uno.map 
endif
endif

SRC=./src
OUT=.
IDL_LOC_DIR=./idl
EXT_DIR=./ext
BUILD_DIR=./build

OUT_COMP_INC=$(OUT_INC)/$(COMP_NAME)
OUT_COMP_GEN=$(OUT_MISC)/$(COMP_NAME)
OUT_COMP_SLO=$(OUT_SLO)/$(COMP_NAME)


CXXFILES = mt_service.cpp mt.cpp 
ADDINCXXFILES = mt_addin_service.cpp mt_addin.cpp mt_config.cpp 
OPTIONSCXXFILES = mt_options_service.cpp mt_options.cpp mt_config.cpp 

OBJFILES = $(patsubst %.cpp,$(OUT_SLO)/%.$(OBJ_EXT),$(CXXFILES))
ADDINOBJFILES = $(patsubst %.cpp,$(OUT_SLO)/%.$(OBJ_EXT),$(ADDINCXXFILES))
OPTIONSOBJFILES = $(patsubst %.cpp,$(OUT_SLO)/%.$(OBJ_EXT),$(OPTIONSCXXFILES))

MATH_PATH=mytools/math
IDL_FILES = XMTRandom.idl MTRandom.idl

SHEET_PATH=mytools/sheet
ADDIN_IDL_FILES = XMTRandomAddIn.idl

IDL_FILES2 = $(patsubst %.idl,$(IDL_LOC_DIR)/$(MATH_PATH)/%.idl,$(IDL_FILES))
URD_FILES = $(patsubst %.idl,$(OUT_MISC)/%.urd,$(IDL_FILES))
TYPE_NAMES = $(patsubst %.idl,-Tmytools.math.%,$(IDL_FILES))

ADDIN_IDL_FILES2 = $(patsubst %.idl,$(IDL_LOC_DIR)/$(SHEET_PATH)/%.idl,$(ADDIN_IDL_FILES))
ADDIN_URD_FILES = $(patsubst %.idl,$(OUT_MISC)/%.urd,$(ADDIN_IDL_FILES))
ADDINTYPE_NAMES = $(patsubst %.idl,-Tmytools.sheet.%,$(ADDIN_IDL_FILES))

IDL_INC=../inc
IDL_LOC_INC=./inc
CC_INCLUDES=-I. -I$(IDL_INC) -I$(IDL_LOC_INC) -I$(PRJ)/include
# -I$(PRJ)/include/stl




MANIFEST=$(BUILD_DIR)/META-INF/manifest.xml
DESCRIPTION=$(BUILD_DIR)/description.xml
HELP_CONTENTS=$(BUILD_DIR)/help/en/help.tree
OPTIONPAGE=$(BUILD_DIR)/OptionsDialog.xcu
COMP_DIR=$(BUILD_DIR)/libs
COMP_REGISTRATION=$(COMP_DIR)/registration.components


UNO_PKG_NAME=$(EXTENSION_NAME)-$(EXTENSION_VERSION)$(EXTENSION_STATE)-$(subst _,-,$(EXTENSION_PLATFORM)).$(UNOOXT_EXT)


.PHONY: ALL
ALL : \
	MTRandom MTRandomAddin MTOptions

include $(SETTINGS)/stdtarget.mk

MTRandom : $(UNO_PKG_NAME)
MTRandomAddin : $(UNO_PKG_NAME)
MTOptions : $(UNO_PKG_NAME)


$(UNO_PKG_NAME) : $(COMP_DIR)/$(COMP_IMPL_NAME) mtra mtop $(MANIFEST) $(DESCRIPTION) $(HELP_CONTENTS) $(OPTIONPAGE) $(COMP_REGISTRATION) 
	$(COPY) README $(subst /,$(PS),$(BUILD_DIR)/README)
	$(COPY) LICENSE $(subst /,$(PS),$(BUILD_DIR)/LICENSE)
	$(COPY) -r $(EXT_DIR)/* $(BUILD_DIR)
	cd $(BUILD_DIR) && $(SDK_ZIP) -9 -r -o ../$(UNO_PKG_NAME) META-INF/manifest.xml README LICENSE description.xml libs/*.$(SHAREDLIB_EXT) libs/*.components descriptions/* help/* $(RDB_FILE_NAME) $(ADDINRDB_FILE_NAME) OptionsDialog.xcu MTRandom.xcs MTRandom.xcu dialogs/MTRandomAddIn.xdl


$(OUT_SLO)/%.$(OBJ_EXT) : $(SRC)/%.cpp $(SDKTYPEFLAG)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(CC) $(CC_OUTPUT_SWITCH)$(subst /,$(PS),$@) $(CC_FLAGS) $< $(CC_INCLUDES) $(CC_DEFINES) 

ifeq "$(OS)" "WIN"
$(COMP_DIR)/$(COMP_IMPL_NAME) : mtridl $(OBJFILES)
	-$(MKDIR) $(subst /,$(PS),$(COMP_DIR))
	$(LINK) $(COMP_LINK_FLAGS) /OUT:$(COMP_DIR)/$(COMP_IMPL_NAME) $(OBJFILES) $(LINK_LIBS) -lm $(CPPUHELPERLIB) $(CPPULIB) $(SALLIB) $(STC++LIB) $(CPPUHELPERDYLIB) $(CPPUDYLIB) $(SALDYLIB) $(STLPORTLIB) msvcrt.lib kernel32.lib
# $(LINK_MANIFEST)
else
#mtr : mtridl $(OBJFILES)
$(COMP_DIR)/$(COMP_IMPL_NAME) : mtridl $(OBJFILES)
	-$(MKDIR) $(subst /,$(PS),$(COMP_DIR))
	$(LINK) -o $(COMP_DIR)/$(COMP_IMPL_NAME) $(COMP_LINK_FLAGS) $(OBJFILES) $(LINK_LIBS) -lm $(CPPUHELPERLIB) $(CPPULIB) $(SALLIB) $(STC++LIB) $(CPPUHELPERDYLIB) $(CPPUDYLIB) $(SALDYLIB)
endif

mtridl : 
	$(IDLC) -I"$(OO_SDK_HOME)/idl" -I$(IDL_LOC_DIR) -O$(OUT_MISC) $(IDL_FILES2)
	-$(MKDIR) $(subst /,$(PS),$(BUILD_DIR))
	$(REGMERGE) $(BUILD_DIR)/$(RDB_FILE_NAME) /UCR $(URD_FILES)
	$(CPPUMAKER) -BUCR -O$(IDL_LOC_INC) $(TYPE_NAMES) $(OFFICE_TYPES) $(URE_TYPES) $(BUILD_DIR)/$(RDB_FILE_NAME)

ifeq "$(OS)" "WIN"
mtra : mtraidl $(ADDINOBJFILES)
	-$(MKDIR) $(subst /,$(PS),$(COMP_DIR))
	$(LINK) $(COMP_LINK_FLAGS) /OUT:$(COMP_DIR)/$(ADDINCOMP_IMPL_NAME) $(ADDINOBJFILES) $(LINK_LIBS) -lm $(CPPUHELPERLIB) $(CPPULIB) $(SALLIB) $(STC++LIB) $(CPPUHELPERDYLIB) $(CPPUDYLIB) $(SALDYLIB) $(STLPORTLIB) msvcrt.lib kernel32.lib
else
mtra : mtraidl $(ADDINOBJFILES)
	-$(MKDIR) $(subst /,$(PS),$(COMP_DIR))
	$(LINK) -o $(COMP_DIR)/$(ADDINCOMP_IMPL_NAME) $(COMP_LINK_FLAGS) $(ADDINOBJFILES) $(LINK_LIBS) -lm $(CPPUHELPERLIB) $(CPPULIB) $(SALLIB) $(STC++LIB) $(CPPUHELPERDYLIB) $(CPPUDYLIB) $(SALDYLIB)
endif

mtraidl : 
	$(IDLC) -I"$(OO_SDK_HOME)/idl" -I$(IDL_LOC_DIR) -O$(OUT_MISC) $(ADDIN_IDL_FILES2)
	$(REGMERGE) $(BUILD_DIR)/$(ADDINRDB_FILE_NAME) /UCR $(ADDIN_URD_FILES)
	$(CPPUMAKER) -BUCR -O$(IDL_LOC_INC) $(ADDINTYPE_NAMES) $(OFFICE_TYPES) $(URE_TYPES) $(BUILD_DIR)/$(ADDINRDB_FILE_NAME)


ifeq "$(OS)" "WIN"
mtop : $(OPTIONSOBJFILES)
	-$(MKDIR) $(subst /,$(PS),$(COMP_DIR))
	$(LINK) $(COMP_LINK_FLAGS) /OUT:$(COMP_DIR)/$(OPTIONS_IMPL_NAME) $(OPTIONSOBJFILES) $(LINK_LIBS) -lm $(CPPUHELPERLIB) $(CPPULIB) $(SALLIB) $(STC++LIB) $(CPPUHELPERDYLIB) $(CPPUDYLIB) $(SALDYLIB) $(STLPORTLIB) msvcrt.lib kernel32.lib
else
mtop : $(OPTIONSOBJFILES)
	-$(MKDIR) $(subst /,$(PS),$(COMP_DIR))
	$(LINK) -o $(COMP_DIR)/$(OPTIONS_IMPL_NAME) $(COMP_LINK_FLAGS) $(OPTIONSOBJFILES) $(LINK_LIBS) -lm $(CPPUHELPERLIB) $(CPPULIB) $(SALLIB) $(STC++LIB) $(CPPUHELPERDYLIB) $(CPPUDYLIB) $(SALDYLIB)
endif


$(MANIFEST) : 
	@-$(MKDIR) $(subst /,$(PS),$(@D))
	@echo $(OSEP)?xml version="$(QM)1.0$(QM)" encoding="$(QM)UTF-8$(QM)"?$(CSEP) > $@
	@echo $(OSEP)manifest:manifest$(CSEP) >> $@
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)$(RDB_FILE_NAME)$(QM)" >> $@
	@echo manifest:media-type="$(QM)application/vnd.sun.star.uno-typelibrary;type=RDB$(QM)"/$(CSEP) >> $@
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)$(ADDINRDB_FILE_NAME)$(QM)" >> $@
	@echo manifest:media-type="$(QM)application/vnd.sun.star.uno-typelibrary;type=RDB$(QM)"/$(CSEP) >> $@
ifeq "$(SDKVERSION)" "3.3"
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)libs/$(COMP_IMPL_NAME)$(QM)"  >> $@
	@echo manifest:media-type="$(QM)application/vnd.sun.star.uno-component;type=native;platform=$(UNOPKG_PLATFORM)$(QM)"/$(CSEP)  >> $@
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)libs/$(ADDINCOMP_IMPL_NAME)$(QM)"  >> $@
	@echo manifest:media-type="$(QM)application/vnd.sun.star.uno-component;type=native;platform=$(UNOPKG_PLATFORM)$(QM)"/$(CSEP)  >> $@
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)libs/$(OPTIONS_IMPL_NAME)$(QM)"  >> $@
	@echo manifest:media-type="$(QM)application/vnd.sun.star.uno-component;type=native;platform=$(UNOPKG_PLATFORM)$(QM)"/$(CSEP)  >> $@
else
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)libs/registration.components$(QM)"  >> $@
	@echo manifest:media-type="$(QM)application/vnd.sun.star.uno-components;platform=$(UNOPKG_PLATFORM)$(QM)"/$(CSEP)  >> $@
endif
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)OptionsDialog.xcu$(QM)" manifest:media-type="$(QM)application/vnd.sun.star.configuration-data$(QM)"/$(CSEP)  >> $@
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)MTRandom.xcs$(QM)" manifest:media-type="$(QM)application/vnd.sun.star.configuration-schema$(QM)"/$(CSEP) >> $@
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)MTRandom.xcu$(QM)" manifest:media-type="$(QM)application/vnd.sun.star.configuration-data$(QM)"/$(CSEP) >> $@
	#@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)CalcAddIns.xcu$(QM)" manifest:media-type="$(QM)application/vnd.sun.star.configuration-data$(QM)"/$(CSEP)  >> $@
	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)help$(QM)"   >> $@
	@echo manifest:media-type="$(QM)application/vnd.sun.star.help$(QM)"/$(CSEP)  >> $@
	@echo $(OSEP)/manifest:manifest$(CSEP) >> $@

#	@echo $(OSEP)manifest:file-entry manifest:full-path="$(QM)help$(QM)"   >> $@
#	@echo manifest:media-type="$(QM)application/vnd.sun.star.help$(QM)"/$(CSEP)  >> $@

$(DESCRIPTION) : 
	@echo $(OSEP)?xml version="$(QM)1.0$(QM)" encoding="$(QM)UTF-8$(QM)"?$(CSEP) > $@
	@echo $(OSEP)description xmlns="$(QM)http://openoffice.org/extensions/description/2006$(QM)" >> $@
	@echo xmlns:xlink="$(QM)http://www.w3.org/1999/xlink$(QM)" >> $@
	@echo xmlns:d="$(QM)http://openoffice.org/extensions/description/2006$(QM)"$(CSEP) >> $@
	@echo $(OSEP)identifier value="$(QM)$(EXTENSION_ID)_$(UNOPKG_PLATFORM)$(QM)" /$(CSEP) >> $@
ifeq "$(SDKVERSION)" "3.3"
else
	@echo $(OSEP)dependencies$(CSEP) >> $@
	@echo $(OSEP)OpenOffice.org-minimal-version value="$(QM)4.0$(QM)" d:name="$(QM)OpenOffice.org 4.0$(QM)" /$(CSEP) >> $@
	@echo $(OSEP)/dependencies$(CSEP) >> $@
endif
	@echo $(OSEP)version value="$(QM)$(EXTENSION_VERSION)$(QM)" /$(CSEP) >> $@
	@echo $(OSEP)registration$(CSEP) >> $@
	@echo $(OSEP)simple-license accept-by="$(QM)admin$(QM)" default-license-id="$(QM)this$(QM)" suppress-on-update="$(QM)true$(QM)"$(CSEP) >> $@
	@echo $(OSEP)license-text xlink:href="$(QM)LICENSE$(QM)" license-id="$(QM)this$(QM)" /$(CSEP) >> $@
	@echo $(OSEP)/simple-license$(CSEP) >> $@
	@echo $(OSEP)/registration$(CSEP) >> $@
	@echo $(OSEP)platform value="$(QM)$(EXTENSION_PLATFORM)$(QM)" /$(CSEP) >> $@
	@echo $(OSEP)display-name$(CSEP) >> $@
	@echo $(OSEP)name lang="$(QM)en$(QM)"$(CSEP)$(EXTENSION_DISPLAY_NAME)$(OSEP)/name$(CSEP) >> $@
	@echo $(OSEP)/display-name$(CSEP) >> $@
	@echo $(OSEP)extension-description$(CSEP) >> $@
	@echo $(OSEP)src lang="$(QM)en$(QM)" xlink:href="$(QM)descriptions/desc.en$(QM)"/$(CSEP) >> $@
	@echo $(OSEP)/extension-description$(CSEP) >> $@
	@echo $(OSEP)/description$(CSEP) >> $@

$(HELP_CONTENTS) : 
	@-$(MKDIR) $(subst /,$(PS),$(@D))
	@-$(MKDIR) $(subst /,$(PS),$(BUILD_DIR)/help/en/$(EXTENSION_ID)_$(UNOPKG_PLATFORM))
#	$(COPY) $(subst /,$(PS),help/en/$(EXTENSION_ID)) $(subst /,$(PS),$(BUILD_DIR)/help/en/$(EXTENSION_ID)_$(UNOPKG_PLATFORM))
	$(COPY) $(subst /,$(PS),help/en/$(EXTENSION_ID)/index.xhp) $(subst /,$(PS),$(BUILD_DIR)/help/en/$(EXTENSION_ID)_$(UNOPKG_PLATFORM)/index.xhp)
	@echo $(OSEP)?xml version="$(QM)1.0$(QM)" encoding="$(QM)UTF-8$(QM)"?$(CSEP) >> $@
	@echo $(OSEP)tree_view version="$(QM)17-11-2010$(QM)"$(CSEP) >> $@
	@echo $(OSEP)help_section application="$(QM)MTRNGAddIn$(QM)" id="$(QM)88596961$(QM)" title="$(QM)Mersenne Twister Random Number Generator Add-In$(QM)"$(CSEP) >> $@
	@echo $(OSEP)topic id="$(QM)MTRNGAddIn/$(EXTENSION_ID)_$(UNOPKG_PLATFORM)/index.xhp$(QM)"$(CSEP)index$(OSEP)/topic$(CSEP) >> $@
	@echo $(OSEP)/help_section$(CSEP) >> $@
	@echo $(OSEP)/tree_view$(CSEP) >> $@
	

$(OPTIONPAGE) : 
	@echo $(OSEP)?xml version="$(QM)1.0$(QM)" encoding="$(QM)UTF-8$(QM)"?$(CSEP) >> $@
	@echo $(OSEP)oor:component-data  >> $@
	@echo oor:name="$(QM)OptionsDialog$(QM)"  >> $@
	@echo oor:package="$(QM)org.openoffice.Office$(QM)" >> $@
	@echo xmlns:oor="$(QM)http://openoffice.org/2001/registry$(QM)"  >> $@
	@echo xmlns:xs="$(QM)http://www.w3.org/2001/XMLSchema$(QM)"  >> $@
	@echo xmlns:xsi="$(QM)http://www.w3.org/2001/XMLSchema-instance$(QM)"$(CSEP) >> $@
	@echo $(OSEP)node oor:name="$(QM)Nodes$(QM)"$(CSEP) >> $@
	@echo $(OSEP)node oor:name="$(QM)Calc$(QM)" oor:op="$(QM)fuse$(QM)"$(CSEP) >> $@
	@echo $(OSEP)node oor:name="$(QM)Leaves$(QM)"$(CSEP) >> $@
	@echo $(OSEP)node oor:name="$(QM)mytools.sheet.MTRandomOptions_01$(QM)" oor:op="$(QM)fuse$(QM)"$(CSEP) >> $@
	@echo $(OSEP)prop oor:name="$(QM)Id$(QM)"$(CSEP) >> $@
	@echo $(OSEP)value$(CSEP)$(EXTENSION_ID)_$(UNOPKG_PLATFORM)$(OSEP)/value$(CSEP) >> $@
	@echo $(OSEP)/prop$(CSEP) >> $@
	@echo $(OSEP)prop oor:name="$(QM)Label$(QM)"$(CSEP) >> $@
	@echo $(OSEP)value xml:lang="$(QM)en-US$(QM)"$(CSEP)$(OPTIONS_PAGE_LABEL)$(OSEP)/value$(CSEP) >> $@
	@echo $(OSEP)/prop$(CSEP) >> $@
	@echo $(OSEP)prop oor:name="$(QM)OptionsPage$(QM)"$(CSEP) >> $@
	@echo $(OSEP)value$(CSEP)$(ORIGIN_NAME)/dialogs/MTRandomAddIn.xdl$(OSEP)/value$(CSEP) >> $@
	@echo $(OSEP)/prop$(CSEP) >> $@
	@echo $(OSEP)prop oor:name="$(QM)EventHandlerService$(QM)"$(CSEP) >> $@
	@echo $(OSEP)value$(CSEP)mytools.sheet.MTRandomOptions$(OSEP)/value$(CSEP) >> $@
	@echo $(OSEP)/prop$(CSEP) >> $@
	@echo $(OSEP)/node$(CSEP) >> $@
	@echo $(OSEP)/node$(CSEP) >> $@
	@echo $(OSEP)/node$(CSEP) >> $@
	@echo $(OSEP)/node$(CSEP) >> $@
	@echo $(OSEP)/oor:component-data$(CSEP) >> $@

$(COMP_REGISTRATION) : 
	@echo $(OSEP)?xml version="$(QM)1.0$(QM)" encoding="$(QM)UTF-8$(QM)"?$(CSEP) >> $@
	@echo $(OSEP)components xmlns="$(QM)http://openoffice.org/2010/uno-components$(QM)"$(CSEP) >> $@
	@echo $(OSEP)component loader="$(QM)com.sun.star.loader.SharedLibrary$(QM)" uri="$(QM)mtrandom.uno.so$(QM)"$(CSEP) >> $@
	@echo $(OSEP)implementation name="$(QM)mytools.math.MTRandom$(QM)"$(CSEP) >> $@
	@echo $(OSEP)service name="$(QM)mytools.math.MTRandom$(QM)"/$(CSEP) >> $@
	@echo $(OSEP)/implementation$(CSEP) >> $@
	@echo $(OSEP)/component$(CSEP) >> $@
	@echo $(OSEP)component loader="$(QM)com.sun.star.loader.SharedLibrary$(QM)" uri="$(QM)mtrandomaddin.uno.so$(QM)"$(CSEP) >> $@
	@echo $(OSEP)implementation name="$(QM)mytools.sheet.MTRandomAddIn$(QM)"$(CSEP) >> $@
	@echo $(OSEP)service name="$(QM)mytools.sheet.MTRandomAddIn$(QM)"/$(CSEP) >> $@
	@echo $(OSEP)service name="$(QM)com.sun.star.sheet.AddIn$(QM)"/$(CSEP) >> $@
	@echo $(OSEP)/implementation$(CSEP) >> $@
	@echo $(OSEP)/component$(CSEP) >> $@
	@echo $(OSEP)component loader="$(QM)com.sun.star.loader.SharedLibrary$(QM)" uri="$(QM)mtoptions.uno.so$(QM)"$(CSEP) >> $@
	@echo $(OSEP)implementation name="$(QM)mytools.sheet.MTRandomOptions$(QM)"$(CSEP) >> $@
	@echo $(OSEP)service name="$(QM)mytools.sheet.MTRandomOptions$(QM)"/$(CSEP) >> $@
	@echo $(OSEP)/implementation$(CSEP) >> $@
	@echo $(OSEP)/component$(CSEP) >> $@
	@echo $(OSEP)/components$(CSEP) >> $@


clean : 
	@- $(RM)  $(subst \\,\,$(subst /,$(PS),$(MANIFEST)))
	@- $(RM)  $(subst \\,\,$(subst /,$(PS),$(DESCRIPTION)))
	@- $(DELRECURSIVE) $(subst \,$(PS),$(OUT_SLO))
	@- $(RM)  $(subst \\,\,$(subst /,$(PS),$(COMP_DIR)/$(COMP_IMPL_NAME)))
	@- $(RM) $(subst \\,\,$(subst /,$(PS),$(BUILD_DIR)/$(RDB_FILE_NAME)))
	@- $(RM) $(subst \\,\,$(subst /,$(PS),$(BUILD_DIR)/$(ADDINRDB_FILE_NAME)))
	@- $(RM) $(subst \\,\,$(subst /,$(PS),$(COMP_DIR)/$(ADDINCOMP_IMPL_NAME)))
	@- $(RM) $(subst \\,\,$(subst /,$(PS),$(COMP_DIR)/$(OPTIONS_IMPL_NAME)))
	@- $(RM) $(subst \\,\,$(subst /,$(PS),$(BUILD_DIR)/OptionsDialog.xcu))
	@- $(RM) $(subst /,$(PS),$(BUILD_DIR)/LICENSE)
	@- $(RM) $(subst /,$(PS),$(BUILD_DIR)/README)
	@- $(RM) $(UNO_PKG_NAME)
	@- $(DELRECURSIVE) $(BUILD_DIR)

