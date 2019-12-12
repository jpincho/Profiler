#pragma once
#include <string>
#include <chrono>
#include <vector>

namespace Profiler
{
class ProfilerSection
    {
    public:
        std::string Name;
        std::chrono::system_clock::time_point StartTimestamp, EndTimestamp;
        std::chrono::system_clock::duration AccumulatedDuration, Min, Max, LastDuration, ChildrenTime, FrameAccumulatedDuration, FrameMin, FrameMax, FrameChildrenTime;
        unsigned Count, FrameCount;
        static const ProfilerSection *CurrentlySelected;
        std::vector <ProfilerSection*> Children;
        ProfilerSection *Parent;

        ProfilerSection ( void );
        const std::string &GetName ( void ) const;
        void ClearFrameData ( void );
        void Clear ( void );
        void Start ( void );
        void Close ( void );
        ProfilerSection *AddChild ( const std::string Name );
        ProfilerSection *GetChild ( const std::string Name ) const;
        std::chrono::system_clock::duration GetChildrenTime ( const bool Global = false ) const;
        std::chrono::system_clock::duration GetAccumulatedTime ( const bool Global = false ) const;
    };

ProfilerSection *GetNewSection ( void );
}
