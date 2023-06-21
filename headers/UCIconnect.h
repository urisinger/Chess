#include "ChessEngine.h"
#ifdef WIN32
#include <windows.h>
#endif


class UCIconnection {
public:
	UCIconnection(){
			
	}

	void Loop();
private:
	void UCIconnection::ParseGo(char* line);
	Move ParseMove(const std::string& command);
	void UCIconnection::ParsePos(char* lineIn);
	Board _board;
	ChessEngine _engine;
};