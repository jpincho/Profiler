#pragma once
#include <string>

namespace Profiler
{
void StartFrame ( void );
void EndFrame ( void );
void Start ( const std::string SectionName );
void End ( const std::string SectionName );
void Draw ( void );
}
