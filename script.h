#ifndef SCRIPT_H
#define SCRIPT_H
#include <iostream>
#include "Ffmpeg/playthread.h"


class Script
{
public:
	Script(){}
	~Script();
	int ensureEnter(std::string rtsp_url);

private:
	PlayThread *p_thread = nullptr;

	bool string_compare(const std::string& s, const std::string& prefix){
		return (s.compare(0, prefix.size(), prefix) == 0);
	}
};

#endif // SCRIPT_H
