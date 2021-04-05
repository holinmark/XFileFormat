CURR_DIR = c:\\users\\ho-lin\\source\\repos\\XFileParser\\
OBJECTS_DIR = .\\objects
PARSER_DIR = .\\Parser
OBJS = $(OBJECTS_DIR)\\cXFile.obj $(OBJECTS_DIR)\\sFileIO.obj $(OBJECTS_DIR)\\SkinInfo.obj $(OBJECTS_DIR)\\sErrorMessageException.obj $(OBJECTS_DIR)\\sMeshList.obj $(OBJECTS_DIR)\\sMaterialList.obj $(OBJECTS_DIR)\\sAnimationSet.obj $(OBJECTS_DIR)\\sFrameList.obj $(OBJECTS_DIR)\\sFunctionCallHistory.obj $(OBJECTS_DIR)\\cTextXFileParser.obj $(OBJECTS_DIR)\\cBinaryXFileParser.obj $(OBJECTS_DIR)\\stdafx.obj $(OBJECTS_DIR)\\XFileParser.obj

CC = cl.exe /c /MP4 /JMC /permissive- /GS /W3 /Zc:wchar_t /ZI /Gm- /Od /sdl /Fd"x64\Debug\vc141.pdb" /Zc:inline /fp:precise /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /RTC1 /Gd /MDd /std:c++latest /FC /EHsc /nologo /Fp"x64\Debug\XFileParser.pch" /diagnostics:classic /I$(CURR_DIR)XFileParser\\Parser\\ /I$(CURR_DIR)XFileParser\\
LINK = link.exe /MANIFEST /NXCOMPAT /PDB:"C:\Users\Ho-Lin\source\repos\XFileParser\x64\Debug\XFileParser.pdb" /DYNAMICBASE "kernel32.lib" "D3D11.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /DEBUG /MACHINE:X64 /INCREMENTAL /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"x64\Debug\XFileParser.exe.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO /TLBID:1

{$(PARSER_DIR)}.cpp{}.obj::
	@echo Compiling Parser directory ...
	$(CC) $<
	move *.obj $(OBJECTS_DIR)

{}.cpp{}.obj::
	@echo Compiling ...
	$(CC) $<
	move *.obj $(OBJECTS_DIR)

app.exe : $(OBJS)
   @echo Linking ...
   $(LINK) /OUT:app.exe $(OBJS)

clean:
	@echo cleaning ...
	del $(OBJS) app.exe
#	del $(OBJECTS_DIR)\\cBinaryXFileParser.obj $(OBJECTS_DIR)\\cXFile.obj $(OBJECTS_DIR)\\SkinInfo.obj $(OBJECTS_DIR)\\sFileIO.obj $(OBJECTS_DIR)\\stdafx.obj $(OBJECTS_DIR)\\sErrorMessageException.obj $(OBJECTS_DIR)\\sMeshList.obj $(OBJECTS_DIR)\\sMaterialList.obj $(OBJECTS_DIR)\\sAnimationSet.obj $(OBJECTS_DIR)\\sFrameList.obj $(OBJECTS_DIR)\\sFunctionCallHistory.obj $(OBJECTS_DIR)\\cTextXFileParser.obj $(OBJECTS_DIR)\\XFileParser.obj app.exe
