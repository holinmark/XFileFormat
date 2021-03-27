#include "stdafx.h"

namespace ns_HoLin
{
	sXFileData::~sXFileData()
	{
		Cleanup();
	}

	void sXFileData::Cleanup()
	{
		smateriallist.clear();
		smeshlist.Cleanup();
		sframeslist.Cleanup();
		sanimationsetlist.Cleanup();
	}
	
	cTextXFileParser::cTextXFileParser()
	{
		linenumber = 1;
		endoffile = FALSE;
		trackoutput = FALSE;
	}

	cTextXFileParser::~cTextXFileParser()
	{
		linenumber = 1;
		endoffile = FALSE;
		trackoutput = FALSE;
		functioncalls.history.clear();
	}

	BOOL cTextXFileParser::ParseFile(HANDLE hfile, BOOL btrack)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("ParseFile"));
#endif
		const std::size_t blen = 1024;
		char buff[blen];
		
		trackoutput = btrack;
		sfile.hfile = &hfile;
		while (TRUE) {
			if (GetChar()) {
				if (IsWhiteSpace(this, (int)sfile.ch))
					continue;
				else if (sfile.ch == '/') {
					if (GetComment()) {
						continue;
					}
				}
				else if (std::isalpha((int)sfile.ch)) {
					buff[0] = sfile.ch;
					buff[1] = '\0';
					if (GetReservedWord(&buff[1], blen - 1, '{')) {
						if (ExtractTemplates(buff, blen))
							continue;
					}
					return PrintOffendingLine("\n%s \'%s\' %zu %u\n", "Error unknown string", buff, linenumber, __LINE__);
				}
				else {
					return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unknown token", sfile.ch, linenumber, __LINE__);
				}
			}
			if (endoffile) {
				return TRUE;
			}
			else {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error unknown token", sfile.ch, linenumber, __LINE__);
			}
		}
		return FALSE;
	}

	cTextXFileParser::operator bool() const
	{
		return (sfile.hfile != nullptr);
	}

	bool cTextXFileParser::operator !() const
	{
		return !(sfile.hfile != nullptr);
	}

	BOOL cTextXFileParser::GetChar()
	{
		if (sfile.GetNextCharFromBuffer(trackoutput)) {
			if (sfile.ch == '\r') {
				if (sfile.GetNextCharFromBuffer(trackoutput) == FALSE)
					return FALSE;
			}
			if (sfile.ch == '\n') {
				linenumber++;
			}
			if (trackoutput)
				std::clog << sfile.ch;
			return TRUE;
		}
		if (sfile.bytes_read_from_file == 0)
			endoffile = TRUE;
		return FALSE;
	}

	BOOL cTextXFileParser::GetXFileHeader()
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetXFileHeader"));
#endif
		struct sXFileHeader
		{
			char magic_number[4];
			char major[2];
			char minor[2];
			char format_type[4];
			char float_size[4];
		};
		sXFileHeader headerbuffer;
		BOOL textfile = FALSE, mode32bit = FALSE;
		DWORD datasize = sizeof(sXFileHeader);
		DWORD bytesreceived = 0;
		long headerdata = 0;

		if (GetChar() == FALSE) {
			return FALSE;
		}
		std::clog << "\nHeader read.\n";
		memset((void*)&headerbuffer, 0, sizeof(sXFileHeader));
		if (memcpy_s((void*)&headerbuffer, datasize, (const void*)sfile.file_buffer, datasize) != 0) {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Error reading x file header.", linenumber, __LINE__);
		}
		sfile.index_of_next_char_to_read += datasize;
		memcpy_s((void*)&headerdata, sizeof(long), (const void*)&headerbuffer.magic_number, sizeof(long));
		if (headerdata != XOFFILE_FORMAT_MAGIC) {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Not an x file format.", linenumber, __LINE__);
		}
		memcpy_s((void*)&headerdata, sizeof(long), (const void*)headerbuffer.format_type, sizeof(long));
		if (headerdata == XOFFILE_FORMAT_TEXT) {
			textfile = TRUE;
			std::clog << "Text file.\n";
		}
		else if (headerdata == XOFFILE_FORMAT_BINARY) {
			textfile = FALSE;
			return PrintOffendingLine("\n%s\n%zu %u\n", "Binary format unsupported at this time.", linenumber, __LINE__);
		}
		else if (headerdata == XOFFILE_FORMAT_COMPRESSED) {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Compressed file format unsupported.", linenumber, __LINE__);
		}
		else {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Unknown file format.", linenumber, __LINE__);
		}
		memcpy_s((void*)&headerdata, sizeof(long), (const void*)headerbuffer.float_size, sizeof(long));
		if (headerdata == XOFFILE_FORMAT_FLOAT_BITS_32) {
			mode32bit = TRUE;
		}
		else if (headerdata == XOFFILE_FORMAT_FLOAT_BITS_64) {
			mode32bit = FALSE;
			return PrintOffendingLine("\n%s\n%zu %u\n", "64 bit floating point not supported at this time.", linenumber, __LINE__);
		}
		return TRUE;
	}

	BOOL cTextXFileParser::GetTemplateName(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetTemplateName"));
#endif

		buff[0] = '\0';
		if (sfile.ch != '{') {
			while (TRUE) {
				if (GetChar() == FALSE)
					return FALSE;
				if (std::isalnum((int)sfile.ch) || sfile.ch == '_' || sfile.ch == '-' || sfile.ch == '.' || sfile.ch == '{') {
					break;
				}
				else if (IsWhiteSpace(this, (int)sfile.ch)) {
					continue;
				}
				else {
					return PrintOffendingLine("\n%s \'%c\' %s %zu %u\n", "Error unknown token", sfile.ch, "line number", linenumber, __LINE__);
				}
			}
		}
		if (sfile.ch == '{')
			return TRUE;
		else {
			std::size_t limit = blen - 1;
			
			buff[0] = sfile.ch;
			buff[1] = '\0';
			for (std::size_t i = 1; i < limit; ++i) {
				if (GetChar() == FALSE) {
					return FALSE;
				}
				if (sfile.ch == ' ' || sfile.ch == '\n') {
					break;
				}
				else if (std::isalpha((int)sfile.ch) || std::isdigit((int)sfile.ch) || sfile.ch == '.' || sfile.ch == '_' || sfile.ch == '\'' || sfile.ch == '-') {
					buff[i] = sfile.ch;
					buff[i + 1] = '\0';
				}
				else if (sfile.ch == '{') {
					return TRUE;
				}
				else {
					return PrintOffendingLine("\n%s \'%c\' %s \'{\'\n%zu %u\n", "Error unexpected token", sfile.ch, "expecting", linenumber, __LINE__);
				}
			}
			if (GetNextToken('{'))
				return TRUE;
		}
		return PrintOffendingLine("\n%s %zu %u\n", "Error buffer overload.", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetDigit(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetDigit"));
#endif

		while (TRUE) {
			if (GetChar()) {
				if (isdigit(sfile.ch)) {
					buff[0] = sfile.ch;
					buff[1] = '\0';
					return TRUE;
				}
				else if (sfile.ch == '/') {
					if (GetComment() == FALSE)
						break;
				}
				else if (sfile.ch == ' ' || sfile.ch == '\n' || sfile.ch == '\t')
					continue;
				else if (sfile.ch == '\r') {
					if (GetCarriageReturn() == FALSE)
						return FALSE;
					continue;
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetSignedDigit(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetSignedDigit"));
#endif

		while (TRUE) {
			if (GetChar()) {
				if (isdigit(sfile.ch) || sfile.ch == '-') {
					buff[0] = sfile.ch;
					buff[1] = '\0';
					return TRUE;
				}
				else if (sfile.ch == '/') {
					if (GetComment())
						continue;
					else
						break;
				}
				else if (IsWhiteSpace(this, (int)sfile.ch))
					continue;
				else if (sfile.ch == '\r') {
					if (GetCarriageReturn() == FALSE)
						return FALSE;
					continue;
				}
				else {
					return PrintOffendingLine("\n%s \'%c\' %zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
				}
			}
			else {
				return PrintOffendingLine("\n%s %zu %u\n", "Unexpected end of file ", linenumber, __LINE__);
			}
		}
		return FALSE;
	}

	BOOL cTextXFileParser::GetMatrixBody(char *buff, std::size_t blen, void *plist, DWORD row, DWORD col)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMatrixBody"));
#endif

		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		((DirectX::XMFLOAT4X4*)plist)->m[row][col] = (float)atof(buff);
		return TRUE;
	}

	BOOL cTextXFileParser::GetMatrix(char *buff, std::size_t blen, DirectX::XMFLOAT4X4 &matrix)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMatrix"));
#endif

		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE)
				return functioncalls.PrintHistoryLog();
		}
		if (Get2DArray(buff, blen, (void*)&matrix, 4, 4, &cTextXFileParser::GetMatrixBody)) {
			if (GetNextToken('}'))
				return TRUE;
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetFrameTransformMatrix(char *buff, std::size_t blen, DirectX::XMFLOAT4X4 &matrix)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetTransformMatrix"));
#endif

		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE)
				return functioncalls.PrintHistoryLog();
		}
		if (Get2DArray(buff, blen, (void*)&matrix, 4, 4, &cTextXFileParser::GetMatrixBody)) {
			if (GetNextToken(';')) {
				if (GetNextToken('}'))
					return TRUE;
			}
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetReservedWord(char *buff, std::size_t blen, char token)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetReservedWord"));
#endif
		std::size_t limit = blen - 1;
		
		for (std::size_t index = 0; index < limit; ++index) {
			if (GetChar()) {
				if (std::isalpha((int)sfile.ch) || isdigit(sfile.ch) || sfile.ch == '.' || sfile.ch == '_') {
					buff[index] = sfile.ch;
					buff[index + 1] = '\0';
				}
				else if (IsWhiteSpace(this, (int)sfile.ch) || sfile.ch == token) {
					return TRUE;
				}
				else if (sfile.ch == '\r') {
					return GetCarriageReturn();
				}
				else if (sfile.ch == '/') {
					return GetComment();
				}
				else {
					return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unknown token", sfile.ch, linenumber, __LINE__);
				}
			}
			else {
				return PrintOffendingLine("\n%s %zu %u\n", "Unexpected end of file ", linenumber, __LINE__);
			}
		}
		return PrintOffendingLine("\n%s %zu %u\n", "Error buffer overload", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetName(char *buff, std::size_t blen, char sep)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetName"));
#endif
		std::size_t limit = blen - 1;
		
		while (TRUE) {
			if (GetChar() == FALSE) {
				return PrintOffendingLine("\n%s %zu %u\n", "Error : linenumber", linenumber, __LINE__);
			}
			if (std::isalnum((int)sfile.ch) || sfile.ch == '_' || sfile.ch == '-' || sfile.ch == '.') {
				break;
			}
			else if (IsWhiteSpace(this, (int)sfile.ch)) {
				continue;
			}
			else if (sfile.ch == '}') {
				return PrintOffendingLine("\n%s %zu\n%s %u\n", "Error no string input line number :", linenumber, __FILE__, __LINE__);
			}
			else {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error unknown token", sfile.ch, linenumber, __LINE__);
			}
		}
		buff[0] = sfile.ch;
		buff[1] = '\0';
		for (std::size_t i = 1; i < limit; ++i) {
			if (GetChar()) {
				if (std::isalnum((int)sfile.ch) || sfile.ch == '.' || sfile.ch == '_' || sfile.ch == '\'' || sfile.ch == '-') {
					buff[i] = sfile.ch;
					buff[i + 1] = '\0';
				}
				else if (sfile.ch == sep) {
					return TRUE;
				}
				else if (IsWhiteSpace(this, (int)sfile.ch)) {
					if (GetNextToken(sep) == FALSE)
						break;
					return TRUE;
				}
				else if (sfile.ch == '\r') {
					if (GetCarriageReturn() == FALSE)
						break;
					if (GetNextToken('{') == FALSE)
						break;
					return TRUE;
				}
				else if (sfile.ch == '/') {
					if (GetComment() == FALSE)
						break;
					if (GetNextToken('{') == FALSE)
						break;
					else
						return TRUE;
				}
				else {
					return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unknown token", sfile.ch, linenumber, __LINE__);
				}
			}
			else {
				return PrintOffendingLine("\n%s %zu %u\n", "Unexpected end of file ", linenumber, __LINE__);
			}
		}
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetString(char *buff, std::size_t blen, char sep)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetString"));
#endif
		std::size_t limit = blen - 1;

		while (TRUE) {
			if (GetChar() == FALSE)
				return FALSE;
			if (IsWhiteSpace(this, (int)sfile.ch))
				continue;
			else if (sfile.ch == '"')
				break;
			else {
				return PrintOffendingLine("\n%s\n%zu %u\n", "Error expecting a string. ", linenumber, __LINE__);
			}
		}
		for (std::size_t i = 0; i < limit; ++i) {
			if (GetChar() == FALSE)
				return FALSE;
			if (sfile.ch == '"') {
				if (i == 0) {
					return PrintOffendingLine("\n%s\n%zu %u\n", "Error string length zero declared.", linenumber, __LINE__);
				}
				if (GetNextToken(sep) == FALSE)
					return PrintOffendingLine("|n%s \'%c\'\n%zu %u\n", "Error expecting token", sep, linenumber, __LINE__);
				return TRUE;
			}
			else if (std::isalnum((int)sfile.ch) || std::ispunct((int)sfile.ch)) {
				buff[i] = sfile.ch;
				buff[i + 1] = '\0';
			}
			else {
				if (sfile.ch == '\n' || sfile.ch == '\r')
					return PrintOffendingLine("\n%s %zu %u\n", "Error unexpected carriage return line number", linenumber, __LINE__);
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error extracting string. Unknown character",  sfile.ch, linenumber, __LINE__);
			}
		}
		return PrintOffendingLine("\n%s %zu %u\n", "Error : buffer overload.", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetColorRGBA(char *buff, std::size_t blen, DirectX::XMFLOAT4 &c)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetColorRGBA"));
#endif

		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		c.x = (float)atof(buff);
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		c.y = (float)atof(buff);
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		c.z = (float)atof(buff);
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		c.w = (float)atof(buff);
		return TRUE;
	}

	BOOL cTextXFileParser::GetColorRGB(char *buff, std::size_t blen, DirectX::XMFLOAT3 &c)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetColorRGB"));
#endif

		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		c.x = (float)atof(buff);
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		c.y = (float)atof(buff);
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		c.z = (float)atof(buff);
		return TRUE;
	}

	BOOL cTextXFileParser::GetMaterialAttributes(char *buff, std::size_t blen, ns_HoLin::sMaterial *p_material)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMaterialAttributes"));
#endif

		if (VerifyToken('{') == FALSE)
			return FALSE;
		if (strcmp(buff, "TextureFilename") == 0) {
			if (GetString(buff, blen, ';') == FALSE)
				return FALSE;
			p_material->filename = buff;
			if (GetNextToken('}') == FALSE)
				return FALSE;
			return TRUE;
		}
		return PrintOffendingLine("\n%s \'%s\'\n%zu %u\n", "Unknown identifier", buff, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetMaterialBody(char *buff, std::size_t blen, ns_HoLin::sMaterial *p_material)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMaterialBody"));
#endif

		if (GetColorRGBA(buff, blen, p_material->facecolor) == FALSE)
			return FALSE;
		if (VerifyToken(';')) {
			if (GetNextToken(';') == FALSE)
				return FALSE;
		}
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		p_material->power = (float)atof(buff);
		if (GetColorRGB(buff, blen, p_material->specularcolor) == FALSE)
			return FALSE;
		if (VerifyToken(';')) {
			if (GetNextToken(';') == FALSE)
				return FALSE;
		}
		if (GetColorRGB(buff, blen, p_material->emissivecolor) == FALSE)
			return FALSE;
		if (VerifyToken(';')) {
			if (GetNextToken(';') == FALSE)
				return FALSE;
		}
		while (TRUE) {
			if (GetNextInput(IsValidEntry) == FALSE)
				break;
			if (sfile.ch == '}')
				return TRUE;
			if (sfile.ch == '{') {
				if (GetName(buff, blen, '}') == FALSE)
					return FALSE;
				continue;
			}
			else if (std::isalpha((int)sfile.ch)) {
				buff[0] = sfile.ch;
				buff[1] = '\0';
				if (GetReservedWord(&buff[1], blen - 1, '{') == FALSE)
					return FALSE;
				if (GetMaterialAttributes(buff, blen, p_material) == FALSE)
					return FALSE;
			}
			else {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
			}
		}
		return PrintOffendingLine("\n%s %zu %u\n", "Error GetMaterialBody", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetUnsignedInteger(char *buff, std::size_t blen, void *plist)
	{
		// This function is shared by GetSkinWeightsBody
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetUnsignedInteger"));
#endif

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;

		DWORD d = (DWORD)atoi(buff);

		((std::vector<DWORD>*)plist)->emplace_back(d);
		return TRUE;
	}

	BOOL cTextXFileParser::GetMeshMaterialListAttributes(char *buff, std::size_t blen, ns_HoLin::sMesh *pmesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshMaterialListAttributes"));
#endif

		if (sfile.ch == '{') {
			if (GetName(buff, blen, '}') == FALSE)
				return FALSE;
			for (std::size_t i=0; i<pmesh->p_extra->smeshmateriallist.size(); ++i) {
				if (pmesh->p_extra->smeshmateriallist[i].name == std::string(buff))
					return TRUE;
			}
			PrintOffendingLine("\n%s%s%s%zu %u%s", "Error could not find material named \'", buff, "\'\n", linenumber, __LINE__, "\n\n");
		}
		else if (sfile.ch == 'M') {
			buff[0] = sfile.ch;
			buff[1] = '\0';
			if (GetReservedWord(&buff[1], blen - 1, '{') == FALSE)
				return FALSE;
			if (strcmp((const char*)"Material", (const char*)buff) == 0) {
				ns_HoLin::sMaterial material;

				if (GetMaterial(buff, blen, &material) == FALSE)
					return FALSE;
				pmesh->p_extra->smeshmateriallist.emplace_back(std::move(material));
				return TRUE;
			}
			else {
				PrintOffendingLine("\n%s \'%s\'\n%zu %u\n", "Unknown token", buff, linenumber, __LINE__);
			}
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetMeshMaterialListBody(char *buff, std::size_t blen, ns_HoLin::sMesh *pmesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshMaterialListBody"));
#endif
		DWORD nmaterials = 0, number_of_face_indices = 0;

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		nmaterials = (DWORD)atoi(buff);
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		number_of_face_indices = (DWORD)atoi(buff);
		if (GetArray(buff, blen, (void*)&pmesh->p_extra->material_indices, number_of_face_indices, &cTextXFileParser::GetUnsignedInteger, FALSE) == FALSE)
			return FALSE;
		if (nmaterials == 0) {
			if (GetNextToken('}') == FALSE)
				return FALSE;
		}
		for (DWORD i = 0; i < nmaterials; ++i) {
			if (GetNextInput(IsValidEntry) == FALSE)
				return FALSE;
			if (sfile.ch == '}')
				break;
			if (GetMeshMaterialListAttributes(buff, blen, pmesh) == FALSE)
				return FALSE;
		}
		if (nmaterials != pmesh->p_extra->smeshmateriallist.size()) {
			return PrintOffendingLine("\n%s %zu %s %u %s\n%zu %u\n",
				"Error expecting", pmesh->p_extra->smeshmateriallist.size(), "materials got", nmaterials,
				"materials.", linenumber, __LINE__);
		}
		if (GetNextToken('}') == FALSE)
			return FALSE;
		return TRUE;
	}

	BOOL cTextXFileParser::GetMeshMaterialList(char *buff, std::size_t blen, ns_HoLin::sMesh *pmesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshMaterialList"));
#endif

		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE)
				return FALSE;
		}
		if (GetMeshMaterialListBody(buff, blen, pmesh))
			return TRUE;
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetMaterial(char *buff, std::size_t blen, ns_HoLin::sMaterial *p_material)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMaterial"));
#endif

		memset((void*)buff, 0, blen);
		buff[0] = '\0';
		if (GetTemplateName(buff, blen) == FALSE)
			return FALSE;
		if (strlen(buff) > 0)
			p_material->name = buff;
		else {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Material template not named.", linenumber, __LINE__);
		}
		return GetMaterialBody(buff, blen, p_material);
	}

	BOOL cTextXFileParser::GetUnsignedInteger(char *buff, std::size_t blen, BOOL getnexttoken)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetUnsignedInteger"));
#endif
		std::size_t limit = blen - 1;
		
		if (GetDigit(buff, blen) == FALSE)
			return FALSE;
		buff[0] = sfile.ch;
		buff[1] = '\0';
		for (std::size_t i = 1; i < blen; ++i) {
			if (GetChar()) {
				if (isdigit(sfile.ch)) {
					buff[i] = sfile.ch;
					buff[i + 1] = '\0';
				}
				else if (IsWhiteSpace(this, (int)sfile.ch)) {
					return TRUE;
				}
				else if (sfile.ch == '\r') {
					if (GetCarriageReturn() == FALSE)
						break;
					return TRUE;
				}
				if (sfile.ch == ',' || sfile.ch == ';') {
					if (getnexttoken) {
						if (GetNextInput(IsValidSeperator) == FALSE)
							return PrintOffendingLine(NULL);
					}
					return TRUE;
				}
			}
			else {
				return PrintOffendingLine("\n%s\n%zu %u\n", "Unexpected end of file.", linenumber, __LINE__);
			}
		}
		return PrintOffendingLine("\n%s\n%zu %u\n", "Buffer overflow.", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetExponent(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetExponent"));
#endif

		if (GetSignedDigit(buff, blen)) {
			if (sfile.ch == '-' || isdigit(sfile.ch)) {
				if (GetInteger(&buff[1], blen - 1)) {
					return TRUE;
				}
			}
		}
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetFraction(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetFraction"));
#endif
		std::size_t limit = blen - 1;

		for (std::size_t i = 0; i < limit; ++i) {
			if (GetChar() == FALSE)
				return FALSE;
			if (isdigit(sfile.ch)) {
				buff[i] = sfile.ch;
				buff[i + 1] = '\0';
				continue;
			}
			else if (sfile.ch == 'e' || sfile.ch == 'E') {
				buff[i] = sfile.ch;
				buff[i + 1] = '\0';
				if (GetExponent(&buff[i + 1], (blen - i + 1)))
					return TRUE;
			}
			else if (IsWhiteSpace(this, (int)sfile.ch))
				return TRUE;
			else if (sfile.ch == '\r') {
				if (GetChar() == FALSE)
					return FALSE;
				return TRUE;
			}
			else {
				return TRUE;
			}
		}
		return PrintOffendingLine("\n%s\n%zu %u\n", "Error buffer overflow.", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetInteger(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetInteger"));
#endif
		std::size_t limit = blen - 1;

		for (std::size_t i = 0; i < limit; ) {
			if (GetChar() == FALSE)
				return FALSE;
			if (isdigit(sfile.ch)) {
				buff[i] = sfile.ch;
				buff[++i] = '\0';
				continue;
			}
			else if (sfile.ch == '.') {
				buff[i] = sfile.ch;
				buff[i + 1] = '\0';
				if (GetFraction(&buff[i + 1], (blen - i - 1)) == FALSE)
					return FALSE;
				break;
			}
			else if (sfile.ch == 'e' || sfile.ch == 'E') {
				buff[i] = sfile.ch;
				buff[i + 1] = '\0';
				if (GetExponent(&buff[i + 1], (blen - i - 1)) == FALSE)
					return FALSE;
				break;
			}
			else if (IsWhiteSpace(this, (int)sfile.ch)) {
				break;
			}
			else if (sfile.ch == '\r') {
				if (GetChar() == FALSE)
					return FALSE;
			}
			else if (sfile.ch == '/') {
				if (GetComment() == FALSE)
					return FALSE;
				break;
			}
			else
				break;
		}
		return TRUE;
	}

	BOOL cTextXFileParser::GetFloat(char *buff, std::size_t blen, BOOL getnexttoken)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetFloat"));
#endif

		if (GetSignedDigit(buff, blen)) {
			if (GetInteger(&buff[1], blen - 1)) {
				if (getnexttoken) {
					if (GetNextInput(IsValidSeperator) == FALSE)
						return FALSE;
				}
				return TRUE;
			}
		}
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetFloat(char *buff, std::size_t blen, void *plist)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetFloat"));
#endif
		float value = 0.0;

		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		value = (float)atof(buff);
		((std::vector<float>*)plist)->emplace_back(value);
		return TRUE;
	}
	
	BOOL cTextXFileParser::GetVectorBody(char *buff, std::size_t blen, void *plist)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetVectorBody"));
#endif
		DirectX::XMFLOAT3 f;

		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		f.x = (float)atof(buff);
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		f.y = (float)atof(buff);
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		f.z = (float)atof(buff);
		((std::vector<DirectX::XMFLOAT3>*)plist)->emplace_back(f);
		return TRUE;
	}
	
	BOOL cTextXFileParser::GetVector(char *buff, std::size_t blen, void *plist)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetVector"));
#endif
		if (GetNextToken('{') == FALSE)
			return FALSE;
		if (GetVectorBody(buff, blen, plist) == FALSE)
			return FALSE;
		if (GetNextToken('}') == FALSE)
			return FALSE;
		return TRUE;
	}

	BOOL cTextXFileParser::GetPolygon(char *buff, std::size_t blen, void *polygon)
	{
		// This function is shared by GetSkinWeightsBody
		// and GetPolygons
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetPolygon"));
#endif

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		((std::vector<DWORD>*)polygon)->emplace_back((DWORD)atoi(buff));
		return TRUE;
	}

	BOOL cTextXFileParser::GetPolygons(char *buff, std::size_t blen, void *polygons)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetPolygons"));
#endif
		DWORD number_of_vertices_in_polygon = 0;
		std::vector<DWORD> polygon_indices;

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		number_of_vertices_in_polygon = (DWORD)atoi(buff);
		if (GetArray(buff, blen, (void*)&polygon_indices, number_of_vertices_in_polygon, &cTextXFileParser::GetPolygon, FALSE) == FALSE)
			return FALSE;
		((std::vector<std::vector<DWORD>>*)polygons)->emplace_back(std::move(polygon_indices));
		if (sfile.ch == ';' || sfile.ch == ',') {
			return TRUE;
		}
		else {
			if (GetNextInput(IsValidSeperator) == FALSE)
				return FALSE;
			else if (sfile.ch == ';' || sfile.ch == ',') {
				return TRUE;
			}
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error unexpecting token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetFaces(char *buff, std::size_t blen, ns_HoLin::sMeshFaces &meshfaces)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetFaces"));
#endif
		DWORD number_of_faces = 0;
		std::size_t number_of_indices = 0;

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		number_of_faces = (DWORD)atoi(buff);
		if (VerifyToken(';') == FALSE)
			return FALSE;
		meshfaces.facevertexindices.reserve((std::size_t)number_of_faces);
		if (GetArray(buff, blen, (void*)&meshfaces.facevertexindices, number_of_faces, &cTextXFileParser::GetPolygons) == FALSE)
			return FALSE;
		if (sfile.ch == ';') {
			for (auto f : meshfaces.facevertexindices) {
				if (f.size() > 0)
					number_of_indices += f.size();
			}
			meshfaces.numofindices = (DWORD)number_of_indices;
			return TRUE;
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error expecting \';\', got", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetArray(char *buff, std::size_t blen, void *plist, DWORD number_of_entries, BOOL(cTextXFileParser::*pCallBackFunction)(char*, std::size_t, void*), BOOL getnextinput)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetArray"));
#endif

		for (DWORD i = 0; i < number_of_entries; ++i) {
			if ((this->*pCallBackFunction)(buff, blen, plist) == FALSE) {
				return FALSE;
			}
			if (sfile.ch == ',')
				continue;
			else if (sfile.ch == ';') {
				if (getnextinput == FALSE) {
					if ((i + 1) == number_of_entries)
						break;
					else {
						return PrintOffendingLine("\n%s %u %s %u\n%zu %u\n", "Error expected", number_of_entries, "got", (i + 1), linenumber, __LINE__);
					}
				}
				if (GetNextInput(IsValidSeperator) == FALSE)
					return FALSE;
				if (sfile.ch == ',')
					continue;
				else if (sfile.ch == ';') {
					if ((i + 1) == number_of_entries)
						break;
					else {
						return PrintOffendingLine("\n%s %zu %u\n", "Error ", linenumber, __LINE__);
					}
				}
			}
		}
		if (sfile.ch == ';' || sfile.ch == ',')
			return TRUE;
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::Get2DArray(char *buff, std::size_t blen, void *plist, DWORD rows, DWORD cols, BOOL(cTextXFileParser::*pCallBackFunction)(char*, std::size_t, void*, DWORD, DWORD))
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("Get2DArray"));
#endif

		for (DWORD i = 0; i < rows; ++i) {
			for (DWORD j = 0; j < cols; ++j) {
				if ((this->*pCallBackFunction)(buff, blen, plist, i, j) == FALSE)
					return FALSE;
				if (IsWhiteSpace(this, (int)sfile.ch)) {
					if (GetNextInput(IsValidSeperator) == FALSE) {
						return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unknown token", sfile.ch, linenumber, __LINE__);
					}
				}
				if (sfile.ch == ',')
					continue;
				if (sfile.ch == ';') {
					if (i != j) {
						return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
					}
					else
						continue;
				}
				else {
					return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error unknown token", sfile.ch, linenumber, __LINE__);
				}
			}
		}
		if (sfile.ch == ';' || sfile.ch == ',') {
			return TRUE;
		}
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetVertices(char *buff, std::size_t blen, std::vector<DirectX::XMFLOAT3> &vertices, DWORD numofvertices)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetVertices"));
#endif

		buff[0] = '\0';
		if (GetArray(buff, blen, (void*)&vertices, numofvertices, &cTextXFileParser::GetVectorBody)) {
			return TRUE;
		}
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetNormalFaceIndices(char *buff, std::size_t blen, void *plist)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetNormalFaceIndices"));
#endif
		DWORD number_of_indices_in_polygon = 0;
		std::vector<DWORD> entries;
		
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		number_of_indices_in_polygon = (DWORD)atoi(buff);
		entries.reserve((std::size_t)number_of_indices_in_polygon);
		if (GetArray(buff, blen, (void*)&entries, number_of_indices_in_polygon, &cTextXFileParser::GetPolygon, FALSE) == FALSE)
			return FALSE;
		((std::vector<std::vector<DWORD>>*)plist)->emplace_back(std::move(entries));
		return TRUE;
	}

	BOOL cTextXFileParser::GetMeshNormalsBody(char *buff, std::size_t blen, ns_HoLin::sMeshNormals *p_meshnormals)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("MeshNormalsBody"));
#endif

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;

		DWORD number_of_normals = (DWORD)atoi(buff);
		DWORD number_of_face_indices = 0;

		p_meshnormals->normals.reserve((std::size_t)number_of_normals);
		if (GetArray(buff, blen, (void*)&p_meshnormals->normals, number_of_normals, &cTextXFileParser::GetVectorBody) == FALSE)
			return FALSE;
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		number_of_face_indices = (DWORD)atoi(buff);
		p_meshnormals->facenormals.reserve((std::size_t)number_of_face_indices);
		if (GetArray(buff, blen, (void*)&p_meshnormals->facenormals, number_of_face_indices, &cTextXFileParser::GetNormalFaceIndices) == FALSE)
			return FALSE;
		return TRUE;
	}

	BOOL cTextXFileParser::GetMeshNormals(char *buff, std::size_t blen, ns_HoLin::sMeshNormals *p_meshnormals)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshNormals"));
#endif

		if (sfile.ch != '{') {
			if (GetNextInput(IsValidCharOpening) == FALSE) {
				return FALSE;
			}
			if (std::isalpha((int)sfile.ch)) {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unknown token", sfile.ch, linenumber, __LINE__);
			}
		}
		if (sfile.ch == '{') {
			if (GetMeshNormalsBody(buff, blen, p_meshnormals)) {
				if (sfile.ch == '}')
					return TRUE;
				else {
					if (GetNextInput(IsValidCharClosing)) {
						if (sfile.ch == '}')
							return TRUE;
						else {
							return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error expecting \'}\' got", sfile.ch, linenumber, __LINE__);
						}
					}
				}
			}
		}
		return PrintOffendingLine("\n%s %zu %u\n", "Error : linenumber", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetCoord2D(char *buff, std::size_t blen, void *plist)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetCoord2D"));
#endif
		DirectX::XMFLOAT2 f;

		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		f.x = (float)atof(buff);
		if (GetFloat(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		f.y = (float)atof(buff);
		((std::vector<DirectX::XMFLOAT2>*)plist)->emplace_back(f);
		return TRUE;
	}

	BOOL cTextXFileParser::GetMeshTextureCoordBody(char *buff, std::size_t blen, std::vector<DirectX::XMFLOAT2> &t)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshTextureCoordBody"));
#endif
		DWORD number_of_texture_coord = 0;

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		number_of_texture_coord = (DWORD)atoi(buff);
		t.reserve((std::size_t)number_of_texture_coord);
		if (GetArray(buff, blen, (void*)&t, number_of_texture_coord, &cTextXFileParser::GetCoord2D) == FALSE)
			return FALSE;
		return TRUE;
	}

	BOOL cTextXFileParser::GetMeshTextureCoord(char *buff, std::size_t blen, std::vector<DirectX::XMFLOAT2> &t)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshTextureCoord"));
#endif

		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE)
				return FALSE;
		}
		if (GetMeshTextureCoordBody(buff, blen, t)) {
			if (GetNextToken('}'))
				return TRUE;
		}
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetSkinWeightsBody(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetSkinWeightsBody"));
#endif
		ns_HoLin::sSkinWeights skinning;

		if (GetString(buff, blen, ';') == FALSE)
			return FALSE;
		skinning.transformNodeName = buff;
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		skinning.nWeights = (DWORD)atoi(buff);
		if (skinning.nWeights > 0) {
			if (GetArray(buff, blen, (void*)&skinning.p_vertexIndices, skinning.nWeights, &cTextXFileParser::GetUnsignedInteger, FALSE) == FALSE)
				return FALSE;
			if (VerifyToken(';') == FALSE)
				return FALSE;
			if (GetArray(buff, blen, (void*)&skinning.p_weights, skinning.nWeights, &cTextXFileParser::GetFloat, FALSE) == FALSE)
				return FALSE;
			if (VerifyToken(';') == FALSE)
				return FALSE;
		}
		if (Get2DArray(buff, blen, (void*)&skinning.matrixOffset, (DWORD)4, (DWORD)4, &cTextXFileParser::GetMatrixBody) == FALSE)
			return FALSE;
		p_mesh->p_extra->p_skininfo->p_skin_weights.emplace_back(std::move(skinning));
		if (GetNextToken(';') == FALSE)
			return FALSE;
		if (GetNextToken('}'))
			return TRUE;
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetSkinWeights(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetSkinWeights"));
#endif

		if (GetTemplateName(buff, blen)) {
			if (GetSkinWeightsBody(buff, blen, p_mesh))
				return TRUE;
		}
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetXSkinMeshHeaderBody(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetXSkinMeshHeaderBody"));
#endif

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		if (GetNextToken('}') == FALSE)
			return FALSE;
		return TRUE;
	}

	BOOL cTextXFileParser::GetXSkinMeshHeader(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetXSkinMeshHeader"));
#endif

		if (GetNextInput(IsValidEntry)) {
			if (sfile.ch == '{') {
				if (GetXSkinMeshHeaderBody(buff, blen, p_mesh))
					return TRUE;
			}
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetVertexDuplicationIndicesBody(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetVertexDuplicationIndicesBody"));
#endif

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return functioncalls.PrintHistoryLog();
		p_mesh->p_extra->sduplicates.nIndices = (DWORD)std::atoi(buff);
		if (sfile.ch != ';') {
			if (GetNextToken(';') == FALSE)
				return FALSE;
		}
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return functioncalls.PrintHistoryLog();
		if (sfile.ch != ';') {
			if (GetNextToken(';') == FALSE)
				return FALSE;
		}
		p_mesh->p_extra->sduplicates.nOriginalVertices = (DWORD)std::atoi(buff);
		if (GetArray(buff, blen, (void*)&p_mesh->p_extra->sduplicates.Indices, (DWORD)p_mesh->p_extra->sduplicates.nIndices, &cTextXFileParser::GetUnsignedInteger, FALSE) == FALSE)
			return FALSE;
		if (sfile.ch == ';') {
			if (sfile.ch != '}') {
				if (GetNextToken('}') == FALSE)
					return FALSE;
			}
		}
		else {
			return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error expecting token \';\' got", sfile.ch, linenumber, __LINE__);
		}
		return TRUE;
	}

	BOOL cTextXFileParser::GetVertexDuplicationIndices(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetVertexDuplicationIndices"));
#endif

		if (p_mesh) {
			if (sfile.ch != '{') {
				if (GetNextToken('{') == FALSE)
					return FALSE;
			}
			if (GetVertexDuplicationIndicesBody(buff, blen, p_mesh) == FALSE)
				return FALSE;
		}
		else {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Error, mesh memory not allocated.", linenumber, __LINE__);
		}
		return TRUE;
	}

	BOOL cTextXFileParser::GetIndexColor(char *buff, std::size_t blen, void *plist)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetIndexColor"));
#endif
		ns_HoLin::sIndexedColor c;

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (IsWhiteSpace(this, (int)sfile.ch)) {
			if (GetNextToken(';') == FALSE)
				return FALSE;
		}
		if (sfile.ch == ';') {
			c.index = (DWORD)std::atoi(buff);
			if (GetColorRGBA(buff, blen, c.indexColor)) {
				((std::vector<ns_HoLin::sIndexedColor>*)plist)->emplace_back(c);
				return TRUE;
			}
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetMeshVertexColorsBody(char *buff, std::size_t blen, ns_HoLin::sMesh *pmesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshVertexColorsBody"));
#endif
		DWORD nVertexColors; // should equal number of polygons in mesh

		if (pmesh == NULL)
			return PrintOffendingLine("\n%s\n%zu %u\n", "Error no mesh declared.", linenumber, __LINE__);
		if (GetUnsignedInteger(buff, blen, ';') == FALSE)
			return FALSE;
		nVertexColors = (DWORD)std::atoi(buff);
		if (pmesh->meshfaces.numofindices == nVertexColors) {
			if (GetArray(buff, blen, (void*)&pmesh->p_extra->scolors.vertexColors, nVertexColors, &cTextXFileParser::GetIndexColor, FALSE)) {
				return TRUE;
			}
			return FALSE;
		}
		return PrintOffendingLine("\n%s\n%zu %u\n", "Error number of colors and number of polygons do not match.", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetMeshVertexColors(char *buff, std::size_t blen, ns_HoLin::sMesh *pmesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshVertexColors"));
#endif

		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE)
				return FALSE;
		}
		if (GetMeshVertexColorsBody(buff, blen, pmesh) == FALSE)
			return FALSE;
		if (sfile.ch != '}') {
			if (GetNextToken('}') == FALSE)
				return FALSE;
		}
		return TRUE;
	}

	BOOL cTextXFileParser::GetMeshAttributes(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshAttributes"));
#endif

		if (std::isalpha((int)sfile.ch)) {
			buff[0] = sfile.ch;
			buff[1] = '\0';
			if (GetReservedWord(&buff[1], blen - 1, '{')) {
				if (p_mesh->p_extra == nullptr) {
					p_mesh->p_extra = new ns_HoLin::sMeshExtraAttributes;
					if (p_mesh->p_extra == NULL)
						return PrintOffendingLine("\n%s\n%zu %u\n", "Error unable to allocate memory. ", linenumber, __LINE__);
				}
				if (strcmp(buff, "MeshMaterialList") == 0) {
					if (GetMeshMaterialList(buff, blen, p_mesh))
						return TRUE;
					return PrintOffendingLine("\n%s %zu %u\n", "Error : linenumber", linenumber, __LINE__);
				}
				else if (strcmp(buff, "MeshNormals") == 0) {
					return GetMeshNormals(buff, blen, &p_mesh->p_extra->meshnormals);
				}
				else if (strcmp(buff, "Matrix4x4") == 0) {
					DirectX::XMFLOAT4X4 matrix;

					if (GetMatrix(buff, blen, matrix)) {
						p_mesh->p_extra->list_of_matrices.emplace_back(matrix);
						return TRUE;
					}
					return PrintOffendingLine("\n%s %zu %u\n", "Error : linenumber", linenumber, __LINE__);
				}
				else if (strcmp(buff, "MeshTextureCoords") == 0) {
					return GetMeshTextureCoord(buff, blen, p_mesh->p_extra->texturecoord);
				}
				else if (strcmp(buff, "MeshVertexColors") == 0) {
					return GetMeshVertexColors(buff, blen, p_mesh);
				}
				else if (strcmp(buff, "VertexDuplicationIndices") == 0) {
					return GetVertexDuplicationIndices(buff, blen, p_mesh);
				}
				else if (strcmp(buff, "SkinWeights") == 0) {
					if (p_mesh->p_extra->p_skininfo == nullptr) {
						p_mesh->p_extra->p_skininfo = new ns_HoLin::sSkinInfo;
						if (p_mesh->p_extra->p_skininfo == nullptr) {
							return PrintOffendingLine("\n%s\n%zu %u\n", "Error unable to allocate memory. ", linenumber, __LINE__);
						}
					}
					return GetSkinWeights(buff, blen, p_mesh);
				}
				else if (strcmp(buff, "XSkinMeshHeader") == 0) {
					if (p_mesh->p_extra->p_skininfo) {
						if (p_mesh->p_extra->p_skininfo->skin_header.nBones > 0) {
							return PrintOffendingLine("\n%s\n%zu %u\n", "Template XSkinMeshHeader already declared ", linenumber, __LINE__);
						}
					}
					else {
						p_mesh->p_extra->p_skininfo = new ns_HoLin::sSkinInfo;
						if (p_mesh->p_extra->p_skininfo == nullptr) {
							return PrintOffendingLine("\n%s\n%zu %u\n", "Error unable to allocate memory. ", linenumber, __LINE__);
						}
					}
					return GetXSkinMeshHeader(buff, blen, p_mesh);
				}
				else {
					return PrintOffendingLine("\n%s \'%s\'\n%zu %u\n", "Unknown word", buff, linenumber, __LINE__);
				}
			}
		}
		return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetMeshBody(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMeshBody"));
#endif
		DWORD number_of_vertices = 0;

		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
				return FALSE;
		number_of_vertices = (DWORD)atoi(buff);
		p_mesh->vertices.reserve((std::size_t)(number_of_vertices * sizeof(DirectX::XMFLOAT3)));
		if (GetVertices(buff, blen, p_mesh->vertices, number_of_vertices) == FALSE)
			return FALSE;
		if (GetFaces(buff, blen, p_mesh->meshfaces) == FALSE)
			return FALSE;
		while (TRUE) {
			if (GetNextInput(IsValidEntry) == FALSE) {
				if (endoffile) {
					return PrintOffendingLine("\n%s\n%zu %u\n", "Unexpected end of file. Possibly missing \'}\' ", linenumber, __LINE__);
				}
				return FALSE;
			}
			if (sfile.ch == '}')
				break;
			else if (std::isalpha((int)sfile.ch)) {
				if (GetMeshAttributes(buff, blen, p_mesh) == FALSE)
					return FALSE;
			}
			else if (sfile.ch == '{') {
				if (GetName(buff, blen, '}') == FALSE)
					return FALSE;
				for (std::size_t i = 0; i < xfiledata.smateriallist.size(); ++i) {
					if (xfiledata.smateriallist[i].name == std::string(buff))
						return TRUE;
				}
				return PrintOffendingLine("\n%s \'%s\'\n%zu %u%s", "Error could not find material", buff, linenumber, __LINE__);
			}
			else {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u%s", "Error, unexpected token", sfile.ch, linenumber, __LINE__);
			}
		}
		return TRUE;
	}

	BOOL cTextXFileParser::GetMesh(char *buff, std::size_t blen, ns_HoLin::sMesh *p_mesh)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetMesh"));
#endif

		memset((void*)buff, 0, blen);
		buff[0] = '\0';
		if (GetTemplateName(buff, blen) == FALSE)
			return FALSE;
		if (strlen(buff) > 0)
			p_mesh->name = buff;
		else {
			std::string name("Mesh");
			
			CreateName(buff, blen);
			sprintf_s(buff, blen, "%s_%zu", buff, xfiledata.smeshlist.number_of_meshes);
			name += buff;
			p_mesh->name = std::move(name);
		}
		return GetMeshBody(buff, blen, p_mesh);
	}

	BOOL cTextXFileParser::GetSubFrame(char *buff, std::size_t blen, ns_HoLin::sFrame *p_parentframe)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetSubFrame"));
#endif
		std::string name;

		memset((void*)buff, 0, blen);
		buff[0] = '\0';
		if (GetTemplateName(buff, blen) == FALSE)
			return FALSE;

		ns_HoLin::sSequenceOfFrames *pseq = xfiledata.sframeslist.GetLastSequence();
		ns_HoLin::sFrame *p_frame = pseq->CreateNewFrame();

		if (p_frame) {
			if (strlen(buff) == 0) {
				CreateName(buff, blen);
				sprintf_s(buff, blen, "%s_%zu", buff, p_frame->number_of_frames_created);
			}
			name = buff;
			p_frame->name = name;
			p_frame->parent_name = p_parentframe->name;
			if (name.length() > 0)
				pseq->framenames.emplace_back(name);
			if (pseq->parent_children.find(p_frame->parent_name) != pseq->parent_children.end()) {
				pseq->parent_children[p_frame->parent_name].emplace_back(p_frame->name);
			}
			else {
				pseq->parent_children.insert({p_parentframe->name, std::vector<std::string>({p_frame->name})});
			}
			if (GetFrameBody(buff, blen, p_frame) == FALSE)
				return PrintOffendingLine(NULL);
		}
		else {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Unable to allocate memory. ", linenumber, __LINE__);
		}
		return TRUE;
	}

	BOOL cTextXFileParser::GetFrameAttributes(char *buff, std::size_t blen, ns_HoLin::sFrame *p_parentframe)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetFrameAttributes"));
#endif

		if (strcmp(buff, "Frame") == 0) {
			if (GetSubFrame(buff, blen, p_parentframe))
				return TRUE;
		}
		else if (strcmp(buff, "FrameTransformMatrix") == 0) {
			buff[0] = '\0';
			if (GetFrameTransformMatrix(buff, blen, xfiledata.sframeslist.GetLastSequence()->plastframe->matrix))
				return TRUE;
		}
		else if (strcmp(buff, "Vector") == 0) {
			if (GetVector(buff, blen, (void*)&xfiledata.sframeslist.GetLastSequence()->plastframe->vectors)) {
				return TRUE;
			}
		}
		else if (strcmp(buff, "Mesh") == 0) {
			ns_HoLin::sMesh *p_mesh = xfiledata.smeshlist.CreateNewMesh();

			if (p_mesh) {
				buff[0] = '\0';
				if (GetMesh(buff, blen, p_mesh)) {
					p_parentframe->mesh.insert(p_parentframe->mesh.end(), p_mesh->name);
					return TRUE;
				}
				else
					return FALSE;
			}
			else {
				return PrintOffendingLine("\n%s\n%zu %u\n", "Unable to allocate memory.", linenumber, __LINE__);
			}
		}
		else {
			return PrintOffendingLine("\n%s \'%s\'\n%zu %u\n", "Unknown word", buff, linenumber, __LINE__);
		}
		return FALSE;
	}

	BOOL cTextXFileParser::GetFrameBody(char *buff, std::size_t blen, ns_HoLin::sFrame *pframe)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetFrameBody"));
#endif
		ns_HoLin::sSequenceOfFrames *pseq = nullptr;

		while (TRUE) {
			if (GetNextInput(IsValidEntry)) {
				if (sfile.ch == '{') {
					if (GetName(buff, blen, '}') == FALSE)
						return FALSE;
					pseq = xfiledata.sframeslist.SearchSequence(std::string(buff));
					if (pseq == nullptr) {
						if (xfiledata.smeshlist.Find(std::string(buff)) == FALSE) {
							return PrintOffendingLine("\n%s \'%s\'\n%zu %u\n", "Error could not find object reference", buff, linenumber, __LINE__);
						}
						pframe->mesh.emplace_back(std::string(buff));
					}
					else {
						if (pseq->pfirstframe) {
							pseq->pfirstframe->parent_name = xfiledata.sframeslist.GetLastSequence()->pfirstframe->name;
						}
					}
					continue;
				}
				else if (sfile.ch == '}') {
					return TRUE;
				}
				else if (std::isalpha((int)sfile.ch)) {
					buff[0] = sfile.ch;
					buff[1] = '\0';
					if (GetReservedWord(&buff[1], blen - 1, '{') == FALSE)
						return FALSE;
					if (GetFrameAttributes(buff, blen, pframe) == FALSE)
						return FALSE;
					continue;
				}
				else {
					return PrintOffendingLine("\n%s \'%c\' %zu %u%s\n", "Encountered error. Unknown token", sfile.ch, linenumber, __LINE__);
				}
			}
			else {
				if (endoffile) {
					return PrintOffendingLine("\n%s %zu %u\n", "Unexpected end of file", linenumber, __LINE__);
				}
				break;
			}
		}
		return PrintOffendingLine("\n%s \'%c\' %zu %u\n", "Error unknown token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetFrame(char *buff, std::size_t blen, ns_HoLin::sSequenceOfFrames *pseq)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetFrame"));
#endif
		std::string name;

		buff[0] = '\0';
		if (GetTemplateName(buff, blen) == FALSE)
			return FALSE;

		ns_HoLin::sFrame *pframe = pseq->CreateNewFrame();

		if (pframe != nullptr) {
			if (strlen(buff) > 0)
				name = buff;
			else {
				CreateName(buff, blen);
				sprintf_s(buff, blen, "%s_%zu", buff, pframe->number_of_frames_created);
				name = buff;
			}
			pframe->name = name;
			return GetFrameBody(buff, blen, pframe);
		}
		return PrintOffendingLine("\n%s.\n%zu %u\n", "Unable to allocate memory", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetFloatKeysBody(char *buff, std::size_t blen, void *v)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetFloatKeysBody"));
#endif
		DWORD number_of_entries = 0;
		
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		number_of_entries = (DWORD)atoi(buff);
		for (DWORD i = 0; i < number_of_entries; ++i) {
			if (GetFloat(buff, blen) == FALSE)
				return FALSE;
			if (IsWhiteSpace(this, (int)sfile.ch)) {
				if (GetNextInput(IsValidSeperator) == FALSE)
					return FALSE;
			}
			((ns_HoLin::sTimedFloatKeys*)v)->tfkeys.push_back((float)atof(buff));
			if (sfile.ch == ',')
				continue;
			else if (sfile.ch == ';') {
				if ((i + 1) == number_of_entries)
					continue;
				else {
					return PrintOffendingLine("\n%s %u %s %u\n%zu %u\n", "Error expecting", number_of_entries, "timed float keys, got", i, linenumber, __LINE__);
				}
			}
			else {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
			}
		}
		if (sfile.ch == ';')
			return TRUE;
		return PrintOffendingLine("\n%s \'%c\' %zu %u\n", "Unexpected token", sfile.ch, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetTimedFloatKeys(char *buff, std::size_t blen, void *v)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetTimedFloatKeys"));
#endif
		ns_HoLin::sTimedFloatKeys time_slot;
		
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		time_slot.time = (DWORD)atoi(buff);
		if (GetFloatKeysBody(buff, blen, (void*)&time_slot) == FALSE)
			return FALSE;
		if (GetNextToken(';') == FALSE)
			return FALSE;
		((std::vector<ns_HoLin::sTimedFloatKeys>*)v)->emplace_back(std::move(time_slot));
		return TRUE;
	}

	BOOL cTextXFileParser::GetAnimationKeyBody(char *buff, std::size_t blen, sAnimation_Data *p_anim_data)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimationKeyBody"));
#endif
		DWORD number_of_keys = 0;
		std::vector<ns_HoLin::sTimedFloatKeys> transform_data;
		
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		p_anim_data->type_of_transform = (DWORD)atoi(buff);
		if (p_anim_data->type_of_transform >= 3) {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Error transform type must be 0, 1, or 2", linenumber, __LINE__);
		}
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (VerifyToken(';') == FALSE)
			return FALSE;
		number_of_keys = (DWORD)atoi(buff);
		if (GetArray(buff, blen, (void*)&p_anim_data->transformation_data, number_of_keys, &cTextXFileParser::GetTimedFloatKeys) == FALSE)
			return FALSE;
		//p_anim_data->transformation_data = transform_data;
		return TRUE;
	}

	BOOL cTextXFileParser::GetAnimationKey(char *buff, std::size_t blen, sAnimation_Data *p_anim_data)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimationKey"));
#endif

		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE)
				return FALSE;
		}
		if (GetAnimationKeyBody(buff, blen, p_anim_data) == FALSE)
			return FALSE;
		if (sfile.ch != '}') {
			if (GetNextToken('}') == FALSE) {
				return FALSE;
			}
		}
		return TRUE;
	}

	BOOL cTextXFileParser::GetAnimationOptionsBody(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimationOptionsBody"));
#endif
		
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (sfile.ch != ';') {
			if (GetNextToken(';') == FALSE)
				return FALSE;
		}
		DWORD openclosed = (DWORD)std::atoi(buff);

		if (openclosed > 1) {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Error AnimationOptions, is either open or closed.", linenumber, __LINE__);
		}
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		if (sfile.ch != ';') {
			if (GetNextToken(';') == FALSE)
				return FALSE;
		}
		DWORD positionquality = (DWORD)std::atoi(buff);
		if (positionquality > 1) {
			return PrintOffendingLine("\n%s\n%zu %u\n", "Error AnimationOptions, position quality is either linear or spline.", linenumber, __LINE__);
		}
		if (sfile.ch != '}') {
			if (GetNextToken('}') == FALSE)
				return FALSE;
		}
		return TRUE;
	}
	
	BOOL cTextXFileParser::GetAnimationOptions(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimationOptions"));
#endif
		
		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE)
				return FALSE;
		}
		return GetAnimationOptionsBody(buff, blen);
	}

	BOOL cTextXFileParser::GetAnimationBody(char *buff, std::size_t blen, ns_HoLin::sAnimation *p_animation)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimationBody"));
#endif
		
		while (TRUE) {
			if (GetNextInput(IsValidEntry) == FALSE)
				break;
			if (sfile.ch == '{') {
				if (GetName(buff, blen, '}') == FALSE)
					break;
				if (strlen(buff) == 0) {
					return PrintOffendingLine("\n%s %zu %u\n", "Error no name entry.", linenumber, __LINE__);
				}
				std::string s(buff);
				
				if (xfiledata.sframeslist.FindFrames(s) == FALSE) {
					if (xfiledata.smeshlist.Find(s) == FALSE) {
						return PrintOffendingLine("\n%s\'%s\'\n%zu %u\n", "Could not find object reference", buff, linenumber, __LINE__);
					}
				}
				p_animation->list_of_objects.emplace_back(s);
			}
			else if (std::isalpha((int)sfile.ch)) {
				buff[0] = sfile.ch;
				buff[1] = '\0';
				if (GetReservedWord(&buff[1], blen - 1, '{') == FALSE)
					return FALSE;
				if (strcmp(buff, "AnimationKey") == 0) {
					ns_HoLin::sAnimation_Data *p_anim_data = p_animation->CreateAnimation_Data();

					if (p_anim_data == nullptr) {
						return PrintOffendingLine("\n%s %zu %u\n", "Unable to allocate memory.", linenumber, __LINE__);
					}
					if (GetAnimationKey(buff, blen, p_anim_data) == FALSE)
						return FALSE;
				}
				else if (strcmp(buff, "AnimationOptions") == 0) {
					if (GetAnimationOptions(buff, blen) == FALSE)
						return FALSE;
				}
				else {
					return PrintOffendingLine("\n%s \'%s\'\n%zu %u\n", "Unknown token", buff, linenumber, __LINE__);
				}
			}
			else if (sfile.ch == '}') {
				return TRUE;
			}
			else {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unknown token", sfile.ch, linenumber, __LINE__);
			}
		}
		return PrintOffendingLine(NULL);
	}

	BOOL cTextXFileParser::GetAnimation(char *buff, std::size_t blen, ns_HoLin::sAnimationSet *pset)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimation"));
#endif
		std::string name;
		ns_HoLin::sAnimation *p_animation = nullptr;

		buff[0] = '\0';
		memset((void*)buff, 0, blen);
		if (GetTemplateName(buff, blen) == FALSE)
			return FALSE;
		if (strlen(buff) == 0) {
			CreateName(buff, blen);
			strcat_s(buff, blen, "_Animation");
		}
		name = buff;
		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE) {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Error unexpected token", sfile.ch, linenumber, __LINE__);
			}
		}
		if (pset) {
			p_animation = pset->CreateAnimation();
		}
		else {
			p_animation = xfiledata.sanimationsetlist.CreateAnimation();
		}
		if (p_animation == nullptr) {
			return PrintOffendingLine("\n%s %zu %u\n", "Unable to allocate memory.", linenumber, __LINE__);
		}
		p_animation->name = std::move(name);
		if (GetAnimationBody(buff, blen, p_animation))
			return TRUE;
		return PrintOffendingLine("\n%s %zu %u\n", "Error : linenumber", linenumber, __LINE__);
	}
	
	BOOL cTextXFileParser::GetAnimationSetBody(char *buff, std::size_t blen, sAnimationSet *pset)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimationSetBody"));
#endif

		while (TRUE) {
			if (GetNextInput(IsValidEntry) == FALSE)
				break;
			if (std::isalpha((int)sfile.ch)) {
				buff[0] = sfile.ch;
				buff[1] = '\0';
				if (GetReservedWord(&buff[1], blen - 1, '{') == FALSE)
					return FALSE;
				if (strcmp(buff, "Animation") == 0) {
					if (GetAnimation(buff, blen, pset) == FALSE)
						break;
				}
				else {
					return PrintOffendingLine("\n%s \'%s\'\n%zu %u\n", "Error expecting \'Animation\', but got", buff, linenumber, __LINE__);
				}
			}
			else if (sfile.ch == '}')
				return TRUE;
			else {
				return PrintOffendingLine("\n%s \'%c\'\n%zu %u\n", "Unknown token", sfile.ch, linenumber, __LINE__);
			}
		}
		return PrintOffendingLine("\n%s %zu %u\n", "Error : linenumber", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetAnimationSet(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimationSet"));
#endif
		std::string name;

		buff[0] = '\0';
		memset((void*)buff, 0, blen);
		if (GetTemplateName(buff, blen) == FALSE)
			return FALSE;
		if (strlen(buff) == 0) {
			CreateName(buff, blen);
			strcat_s(buff, blen, "_AnimationSet");
		}
		name = buff;

		ns_HoLin::sAnimationSet *pset = xfiledata.sanimationsetlist.CreateAnimationSet();

		if (pset) {
			pset->name = std::move(name);
			return GetAnimationSetBody(buff, blen, pset);
		}
		return PrintOffendingLine("\n%s\n%zu %u\n", "Error allocating memory.", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetAnimTicksPerSecond(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetAnimTickPerSecond"));
#endif

		if (sfile.ch != '{') {
			if (GetNextToken('{') == FALSE)
				return FALSE;
		}
		if (GetUnsignedInteger(buff, blen) == FALSE)
			return FALSE;
		xfiledata.sanimationsetlist.animtickspersecond = (DWORD)atoi(buff);
		if (VerifyToken(';') == FALSE)
			return FALSE;
		if (GetNextToken('}') == FALSE)
			return FALSE;
		return TRUE;
	}

	BOOL cTextXFileParser::ExtractTemplates(char *buff, std::size_t blen)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("ExtractTemplate"));
#endif

		if (strcmp(buff, "Mesh") == 0) {
			ns_HoLin::sMesh *p_mesh = xfiledata.smeshlist.CreateNewMesh();
	
			if (p_mesh) {
				return GetMesh(buff, blen, p_mesh);
			}
			else {
				return PrintOffendingLine("\n%s\n%zu %u\n", "Unable to allocate memory.", linenumber, __LINE__);
			}
		}
		else if (strcmp(buff, "Frame") == 0) {
			ns_HoLin::sSequenceOfFrames *pseq = xfiledata.sframeslist.CreateNewSequence();

			if (pseq) {
				return GetFrame(buff, blen, pseq);
			}
			else {
				return PrintOffendingLine("\n%s\n%zu %u\n", "Unable to allocate memory.", linenumber, __LINE__);
			}
		}
		else if (strcmp(buff, "Material") == 0) {
			ns_HoLin::sMaterial material;

			if (GetMaterial(buff, blen, &material)) {
				xfiledata.smateriallist.emplace_back(std::move(material));
				return TRUE;
			}
			else {
				return PrintOffendingLine("\n%s\n%zu %u\n", "GetMaterial failure.", linenumber, __LINE__);
			}
		}
		else if (strcmp(buff, "AnimationSet") == 0) {
			return GetAnimationSet(buff, blen);
		}
		else if (strcmp(buff, "AnimTicksPerSecond") == 0) {
			return GetAnimTicksPerSecond(buff, blen);
		}
		else if (strcmp(buff, "Animation") == 0) {
			return GetAnimation(buff, blen);
		}
		return PrintOffendingLine("\n%s \'%s\' %zu %u\n", "Unknown tempate", buff, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::Recovery(char *buff, std::size_t blen, char expected_char, char got_char)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("Recovery"));
#endif
		std::string error_string;
		
		sprintf_s(buff, blen, "\n%s %zu\n%s \'%c\' %s \'%c\'\n", "Error : line number", linenumber, "Expecting", expected_char, "got", got_char);		
		while (sfile.ch != '\n' || sfile.ch != '\r') {
			if (GetChar() == FALSE)
				return FALSE;
		}
		if (sfile.ch == '\r') {
			if (GetChar() == FALSE) {
				return FALSE;
			}
			if (sfile.ch != '\n') {
				return FALSE;
			}
		}
		return TRUE;
	}
	
	BOOL cTextXFileParser::VerifyToken(char token)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("VerifyToken"));
#endif

		if (sfile.ch != token) {
			if (IsWhiteSpace(this, (int)sfile.ch)) {
				if (GetNextToken(token)) {
					return TRUE;
				}
			}
		}
		if (sfile.ch == token)
			return TRUE;
		return PrintOffendingLine("\n%s \'%c\' %s \'%c\' %zu %u\n\n", "Error unexpected token", sfile.ch, "expecting", token, linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetCarriageReturn()
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetCarriageReturn"));
#endif

		if (GetChar()) {
			if (sfile.ch != '\n') {
				return PrintOffendingLine("\n%s\n%zu %u%s", "Unexpected carriage return.", linenumber, __LINE__, "\n\n");
			}
		}
		else {
			return PrintOffendingLine("\n%s\n%zu %u%s", "Unexpected end of file.", linenumber, __LINE__, "\n\n");
		}
		return TRUE;
	}

	BOOL cTextXFileParser::ReadCommentChar()
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetReadCommentChar"));
#endif

		while (TRUE) {
			if (GetChar()) {
				if (sfile.ch == '\r') {
					if (GetCarriageReturn())
						return TRUE;
					else
						break;
				}
				else if (sfile.ch == '\n')
					return TRUE;
			}
			else {
				if (endoffile)
					return TRUE;
				break;
			}
		}
		return PrintOffendingLine("\n%s %zu %u\n", "Error line number", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetComment()
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetComment"));
#endif

		if (GetChar()) {
			if (sfile.ch == '/') {
				if (ReadCommentChar())
					return TRUE;
			}
			else {
				return PrintOffendingLine("\n%s%c%s\n%zu %u%s", "Unexpected token \'", sfile.ch, "\'.", linenumber, __LINE__, "\n\n");
			}
		}
		return PrintOffendingLine("\n%s\n%zu %u%s", "Unexpected end of file.", linenumber, __LINE__, "\n\n");
	}

	BOOL cTextXFileParser::GetNextInput(std::function<BOOL(int)> func)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetNextInput"));
#endif

		while (TRUE) {
			if (GetChar()) {
				if (func((int)sfile.ch))
					return TRUE;
				if (IsWhiteSpace(this, (int)sfile.ch))
					continue;
				else if (sfile.ch == '/') {
					if (GetComment())
						continue;
					else {
						break;
					}
				}
				else if (sfile.ch == '\r') {
					if (GetCarriageReturn() == FALSE) {
						break;
					}
					continue;
				}
				else {
					return PrintOffendingLine("\n%s%c%s\n%zu %u%s", "Unexpected token \'", sfile.ch, "\'.", linenumber, __LINE__, "\n\n");
				}
			}
			else {
				return PrintOffendingLine("\n%s\n%zu %u%s", "Error.", linenumber, __LINE__, "\n\n");
			}
		}
		return PrintOffendingLine("\n%s %zu %u\n", "Error line number", linenumber, __LINE__);
	}

	BOOL cTextXFileParser::GetNextToken(const char token)
	{
#ifdef FUNCTIONCALLSTACK
		ns_HoLin::sFunctionCallHistory currentfunction(std::string("GetNextToken"));
#endif

		while (TRUE) {
			if (GetChar()) {
				if (sfile.ch == token)
					return TRUE;
				if (IsWhiteSpace(this, (int)sfile.ch))
					continue;
				if (sfile.ch == '/') {
					if (GetComment())
						continue;
					else {
						return PrintOffendingLine("\n%s %zu %u\n", "Error line number", linenumber, __LINE__);
					}
				}
				else {
					return PrintOffendingLine("\n%s \'%c\' %s \'%c\' %zu %u\n", "Error unexpected token", sfile.ch, "expecting", token, linenumber, __LINE__);
				}
			}
			else {
				return PrintOffendingLine("\n%s %zu %u", "Error unexpected end of file.", linenumber, __LINE__);
			}
		}
		return PrintOffendingLine("\n%s \'%c\' %zu %u\n\n", "Error line number expecting", sfile.ch, linenumber, __LINE__);
	}
	
	void cTextXFileParser::GetOffendingLine(std::string &before, std::string &after)
	{
		DWORD index = 0, count = 0;
		
		if (trackoutput) {
			//std::cout << sfile.file_buffer;
		}
		for (index = sfile.index_of_next_char_to_read; index < sfile.page_size_in_bytes; ++index) {
			if (sfile.file_buffer[index] == '\n')
				break;
			else {
				after.append(1, sfile.file_buffer[index]);
			}
		}
		for (index = sfile.index_of_next_char_to_read; index < sfile.page_size_in_bytes; --index) {
			if (index == 0) {
				before.insert(std::begin(before), sfile.file_buffer[index]);
				for (count = (DWORD)(sfile.prevpage.size() - 1); count < sfile.page_size_in_bytes; --count) {
					if (sfile.prevpage[count] == '\n')
						break;
					before.insert(std::begin(before), sfile.prevpage[count]);
				}
				break;
			}
			before.insert(std::begin(before), sfile.file_buffer[index]);
			if (sfile.file_buffer[index] == '\n')
				break;
		}
	}

	BOOL cTextXFileParser::PrintOffendingLine(const char *p_format, ...)
	{
#ifdef FUNCTIONCALLSTACK
		if (functioncalls.history.size() == 0)
			return FALSE;
#endif
		if (serrormessages.b_has_error_msg_outputed)
			return FALSE;
		
		char *pbuff = new char[sfile.page_size_in_bytes];
		va_list arg;
		std::ofstream fout("Errorlog.txt");
		DWORD bytes = 0;
		auto clogbuf = std::clog.rdbuf();
		std::string before, after;
		
		if (fout) {
			std::clog.rdbuf(fout.rdbuf());
		}
		GetOffendingLine(before, after);
		if (p_format ) {
			va_start(arg, p_format);
			if (pbuff) {
				bytes = _vsprintf_p(pbuff, sfile.page_size_in_bytes, p_format, arg);
				std::clog << pbuff;
				std::clog << '\n' << before << " Error here " << after << '\n';
			}
		}
		functioncalls.PrintHistoryLog();
		std::clog.rdbuf(clogbuf);
		fout.close();
		if (pbuff) {
			delete[] pbuff;
			pbuff = NULL;
		}
		system("notepad.exe Errorlog.txt");
		serrormessages.b_has_error_msg_outputed = TRUE;
		return FALSE;
	}
	
	BOOL IsWhiteSpace(ns_HoLin::cTextXFileParser *p, int ch)
	{
		if (ch == ' ' || ch == '\t' || ch == '\n')
			return TRUE;
		else if (ch == '\r') {
			if (p->GetChar()) {
				if (p->sfile.ch == '\n')
					return TRUE;
			}
		}
		return FALSE;
	}

	BOOL IsValidEntry(int ch)
	{
		if (std::isalpha((int)ch) || ch == (int)'}' || ch == (int)'{')
			return TRUE;
		return FALSE;
	}

	BOOL IsValidSeperator(int ch)
	{
		if (ch == (int)',' || ch == (int)';')
			return TRUE;
		return FALSE;
	}

	BOOL IsValidCharOpening(int ch)
	{
		if (std::isalpha(ch) || ch == '{' || ch == '_')
			return TRUE;
		return FALSE;
	}
	
	BOOL IsValidCharClosing(int ch)
	{
		if (isalpha(ch) || ch == '}')
			return TRUE;
		return FALSE;
	}

	BOOL CreateName(char *buff, std::size_t blen)
	{
		char name[L_tmpnam_s];
		errno_t err = 0;
		std::size_t len = 0, temppathlength = 0, index = 0;

		GetTempPathA((DWORD)blen, buff);
		temppathlength = strlen(buff);
		err = tmpnam_s(name, L_tmpnam_s);
		if (err == 0) {
			err = strcpy_s((char*)buff, blen, (const char*)&name[temppathlength]);
			if (err == 0) {
				for (DWORD i = 0; i < strlen(buff); ++i) {
					if (buff[i] == '.')
						buff[i] = '_';
				}
				return TRUE;
			}
		}
		return FALSE;
	}
}
