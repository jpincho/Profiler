#include "Profiler.h"
#include "ProfilerSection.h"
#include <imgui/imgui.h>

namespace Profiler
{
static ProfilerSection RootSection;
static ProfilerSection *CurrentSection = &RootSection;

void DrawSection ( ProfilerSection *Section );

void StartFrame ( void )
    {
    RootSection.ClearFrameData();
    RootSection.Start();
    }

void EndFrame ( void )
    {
    RootSection.Close();
    }

void Start ( const std::string SectionName )
    {
    ProfilerSection *SectionToUse = nullptr;

    // Check if child already exists
    SectionToUse = CurrentSection->GetChild ( SectionName );

    // Nope, need to create and add child
    if ( SectionToUse == nullptr )
        {
        CurrentSection->AddChild ( SectionName );
        }

    SectionToUse->Start();
    CurrentSection = SectionToUse;
    }

void End ( const std::string SectionName )
    {
    assert ( SectionName == CurrentSection->GetName() );
    CurrentSection->Close();
    CurrentSection = CurrentSection->Parent;
    }

void Clear ( void )
    {
    RootSection.Clear();
    }

void Draw ( void )
    {
    if ( ImGui::Begin ( "Profiler" ) == false )
        {
        ImGui::End();
        return;
        }

    ImGui::Text ( "Accum FPS : %.02f", ( float ) RootSection.Count / RootSection.AccumulatedDuration.count() );
    ImGui::Columns ( 8 );
    ImGui::Text ( "Name" );
    ImGui::NextColumn();
    ImGui::Text ( "GMin" );
    ImGui::NextColumn();
    ImGui::Text ( "GMax" );
    ImGui::NextColumn();
    ImGui::Text ( "GAvg" );
    ImGui::NextColumn();
    ImGui::Text ( "GTotal" );
    ImGui::NextColumn();
    ImGui::Text ( "GChildren" );
    ImGui::NextColumn();
    ImGui::Text ( "GChildren %%" );
    ImGui::NextColumn();
    ImGui::Text ( "GCount" );
    ImGui::NextColumn();
    ImGui::Separator();
    DrawSection ( &RootSection );
    ImGui::Columns ();

    if ( ProfilerSection::CurrentlySelected )
        {
        static std::vector <ProfilerSection> Values;
        if ( Values.size() > 1000 )
            Values.erase ( Values.begin() );
        Values.push_back ( *ProfilerSection::CurrentlySelected );
        ImGui::PlotHistogram ( "FChildren", [] ( void*data, int index )->float
            {
            ProfilerSection *pointer = ( ProfilerSection* ) data;
            return ( float ) pointer[index].FrameChildrenTime.count();
            }, Values.data(), Values.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2 ( 0, 200 ) );


        }
    ImGui::End();
    }

void DrawSection ( ProfilerSection *Section )
    {
    bool MustDrawChildren = false;
    int Flags = ( Section->Children.size() != 0 ) ? 0 : ImGuiTreeNodeFlags_Leaf;
    if ( Section->Name.empty() )
        Flags |= ImGuiTreeNodeFlags_DefaultOpen;
    MustDrawChildren = ImGui::TreeNodeEx ( Section->Name.c_str(), Flags );
    ImGui::NextColumn();

    // Root node has special process
    if ( Section->Name.empty() )
        {
        ImGui::NextColumn();
        ImGui::NextColumn();
        ImGui::NextColumn();
        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Section->AccumulatedDuration ).count () );
        ImGui::NextColumn();
        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Section->ChildrenTime ).count () );
        ImGui::NextColumn();
        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Section->ChildrenTime ).count () * 100 / std::chrono::duration<float, std::milli> ( Section->AccumulatedDuration ).count () );
        ImGui::NextColumn();
        ImGui::NextColumn();
        }
    else
        {
        char label[32];
        sprintf ( label, "%.5f", std::chrono::duration<float, std::milli> ( Section->Min ).count () );

        if ( ImGui::Selectable ( label, Section->CurrentlySelected == Section, ImGuiSelectableFlags_SpanAllColumns ) )
            Section->CurrentlySelected = Section;

//        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Min ).count () );
        ImGui::NextColumn();
        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Section->Max ).count () );
        ImGui::NextColumn();
        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Section->AccumulatedDuration ).count () / Section->Count );
        ImGui::NextColumn();
        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Section->AccumulatedDuration ).count () );
        ImGui::NextColumn();
        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Section->ChildrenTime ).count () );
        ImGui::NextColumn();
        ImGui::Text ( "%.5f", std::chrono::duration<float, std::milli> ( Section->ChildrenTime ).count () * 100 / std::chrono::duration<float, std::milli> ( Section->AccumulatedDuration ).count () );
        ImGui::NextColumn();
        ImGui::Text ( "%u", Section->Count );
        ImGui::NextColumn();
        }

    if ( MustDrawChildren )
        {
        for ( auto ChildIterator : Section->Children )
            {
            DrawSection ( ChildIterator );
            }
        ImGui::TreePop();
        }
    }
}
