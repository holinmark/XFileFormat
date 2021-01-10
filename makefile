CURR_DIR = c:\\users\\ho-lin\\source\\repos\\XFileParser\\
OBJECTS_DIR = .\\objects
PARSER_DIR = .\\Parser
OBJS = $(OBJECTS_DIR)\\sFileIO.obj $(OBJECTS_DIR)\\SkinInfo.obj $(OBJECTS_DIR)\\sErrorMessageException.obj $(OBJECTS_DIR)\\sMeshList.obj $(OBJECTS_DIR)\\sMaterialList.obj $(OBJECTS_DIR)\\sAnimationSet.obj $(OBJECTS_DIR)\\sFrameList.obj $(OBJECTS_DIR)\\sFunctionCallHistory.obj $(OBJECTS_DIR)\\cTextXFileParser.obj $(OBJECTS_DIR)\\stdafx.obj $(OBJECTS_DIR)\\XFileParser.obj

CC = cl.exe /MP4 /Gd /JMC /W3 /Zc:wchar_t /ZI /Od /c /EHsc /std:c++latest /favor:INTEL64 /D"_CONSOLE" /D"_UNICODE" /D"UNICODE" /I$(CURR_DIR)XFileParser\\Parser\\ /I$(CURR_DIR)XFileParser\\
LINK = link.exe /NXCOMPAT /DYNAMICBASE /DEBUG:FASTLINK /INCREMENTAL /NOLOGO /TLBID:1 /MACHINE:X64 ole32.lib user32.lib D3D11.lib kernel32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

{}.cpp{}.obj::
	@echo Compiling ...
	$(CC) $<
	move *.obj $(OBJECTS_DIR)

{$(PARSER_DIR)}.cpp{}.obj::
	@echo Compiling Parser directory ...
	$(CC) $<
	move *.obj $(OBJECTS_DIR)

app.exe : $(OBJS)
   @echo Linking ...
   $(LINK) /OUT:app.exe $(OBJS)

clean:
	@echo cleaning ...
	del $(OBJECTS_DIR)\SkinInfo.obj $(OBJECTS_DIR)\sFileIO.obj $(OBJECTS_DIR)\stdafx.obj $(OBJECTS_DIR)\sErrorMessageException.obj $(OBJECTS_DIR)\sMeshList.obj $(OBJECTS_DIR)\sMaterialList.obj $(OBJECTS_DIR)\sAnimationSet.obj $(OBJECTS_DIR)\sFrameList.obj $(OBJECTS_DIR)\sFunctionCallHistory.obj $(OBJECTS_DIR)\cTextXFileParser.obj $(OBJECTS_DIR)\XFileParser.obj app.exe
