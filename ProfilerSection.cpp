#include "ProfilerSection.h"

namespace Profiler
{
static std::vector <ProfilerSection*> RecycleBin;
const ProfilerSection *ProfilerSection::CurrentlySelected = nullptr;

ProfilerSection::ProfilerSection ( void )
    {
    AccumulatedDuration = LastDuration = ChildrenTime = FrameAccumulatedDuration = FrameChildrenTime = std::chrono::system_clock::duration::zero ();
    Min = FrameMin = std::chrono::system_clock::duration::max();
    Max = FrameMax = std::chrono::system_clock::duration::min();
    Count = 0;
    Parent = nullptr;
    }

void ProfilerSection::ClearFrameData ( void )
    {
    FrameAccumulatedDuration = FrameMin = FrameMax = FrameChildrenTime = std::chrono::system_clock::duration::zero ();
    FrameCount = 0;
    for ( auto ChildIterator : Children )
        ChildIterator->ClearFrameData ();
    }

const std::string &ProfilerSection::GetName ( void ) const
    {
    return Name;
    }

void ProfilerSection::Clear ( void )
    {
    if ( Children.size () )
        {
        for ( auto ChildIterator : Children )
            ChildIterator->Clear();
        for ( auto ChildIterator : Children )
            RecycleBin.push_back ( ChildIterator );
        Children.clear();
        }
    }

void ProfilerSection::Start ( void )
    {
    StartTimestamp = std::chrono::system_clock::now();
    }

void ProfilerSection::Close ( void )
    {
    EndTimestamp = std::chrono::system_clock::now();
    LastDuration = EndTimestamp - StartTimestamp;

    Min = std::min ( LastDuration, Min );
    Max = std::max ( LastDuration, Max );
    AccumulatedDuration += LastDuration;
    if ( Parent )
        Parent->ChildrenTime += LastDuration;
    ++Count;

    FrameMin = std::min ( LastDuration, FrameMin );
    FrameMax = std::max ( LastDuration, FrameMax );
    FrameAccumulatedDuration += LastDuration;
    if ( Parent )
        Parent->FrameChildrenTime += LastDuration;
    ++FrameCount;
    }

ProfilerSection *ProfilerSection::AddChild ( const std::string Name )
    {
    ProfilerSection *NewSection = GetNewSection();
    NewSection->Name = Name;
    Children.push_back ( NewSection );
    NewSection->Parent = this;
    return NewSection;
    }

ProfilerSection *ProfilerSection::GetChild ( const std::string Name ) const
    {
    for ( auto ChildIterator : Children )
        if ( ChildIterator->Name == Name )
            return ChildIterator;
    return nullptr;
    }

std::chrono::system_clock::duration ProfilerSection::GetChildrenTime ( const bool Global ) const
    {
    std::chrono::system_clock::duration Duration = std::chrono::system_clock::duration::zero();
    for ( auto ChildIterator : Children )
        {
        if ( Global )
            Duration += ChildIterator->AccumulatedDuration;
        else
            Duration += ChildIterator->FrameAccumulatedDuration;
        }
    return Duration;
    }

std::chrono::system_clock::duration ProfilerSection::GetAccumulatedTime ( const bool Global ) const
    {
    if ( Global )
        return AccumulatedDuration;
    else
        return FrameAccumulatedDuration;
    }

ProfilerSection *GetNewSection ( void )
    {
    ProfilerSection *Result;
    if ( RecycleBin.size() )
        {
        Result = RecycleBin.back();
        RecycleBin.pop_back();
        }
    else
        {
        Result = new ProfilerSection;
        }
    return Result;
    }
}
