#include "ChessEngine.h"
#ifdef WIN32
#include <windows.h>
#endif


class UCIconnection {
public:
	void Loop();
private:
	void ParseGo(char* line);
	void bench(int depth);
	Move ParseMove(const std::string& command);
	void ParsePos(char* lineIn);
	Board _board;
	ChessEngine _engine;
};