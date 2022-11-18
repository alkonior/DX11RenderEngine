#include "RenderGraph.h"

#include <cassert>

GVM::RenderGraphNode::RenderGraphNode(EMachineCommands Command, void* Description):
tuple<GVM::EMachineCommands, void*>(Command, Description)
{}

//GVM::RenderGraphNode::RenderGraphNode(EMachineCommands Command, void* Description, std::vector<Resource*>&& readDependencies, std::vector<Resource*>&& wrightDependencies):
//    Command(Command), Description(Description),
//    ReadDependencies(std::forward<std::vector<Resource*>>(readDependencies)),
//    WrightDependencies(std::forward<std::vector<Resource*>>(wrightDependencies))
//{
//    for (auto& readDep : ReadDependencies)
//    {
//        for (auto& wrightDep : WrightDependencies)
//        {
//            assert(readDep != wrightDep);
//        }
//    }
//}


bool GVM::SyncThreadBlock::TryAdd(const RenderGraphNode& Node,
const std::vector<Resource*>& readDependencies,
const std::vector<Resource*>& wrightDependencies
)
{
    static std::vector<Resource*> NewReadDep;
    static std::vector<Resource*> NewWrightDep;
    NewReadDep.clear();
    NewWrightDep.clear();

    for (auto& readDep : readDependencies)
    {
        if (ReadDependencies.contains(readDep))
            continue;
        if (WrightDependencies.contains(readDep))
            return false;
        NewReadDep.push_back(readDep);
    }

    for (auto& wrightDep : wrightDependencies)
    {
        if (ReadDependencies.contains(wrightDep))
            return false;
        if (WrightDependencies.contains(wrightDep))
            continue;
        NewWrightDep.push_back(wrightDep);
    }

    for (auto& readDep : NewReadDep)
    {
        ReadDependencies.insert(readDep);
    }
    for (auto& wrightDep : NewWrightDep)
    {
        WrightDependencies.insert(wrightDep);
    }

    Nodes.push_back(Node);
    return true;
}


void GVM::RenderGraph::AddCommand(RenderGraphNode Node,
const std::vector<Resource*>& ReadDependencies,
const std::vector<Resource*>& WrightDependencies
)
{
    switch (Node.Command())
    {
    case EMachineCommands::DRAW:
    {
        assert(lasrAddPS != -1);
        assert(Blocks[lasrAddPS].TryAdd(Node, ReadDependencies, WrightDependencies), "Draw could not be added.");
        return;
    }
    case EMachineCommands::BEGIN_EVENT:
    case EMachineCommands::END_EVENT:
    case EMachineCommands::CLEAR_PIPELINE:
    {
        if (lasrLockedBlock == Blocks.size() - 1 )
        {
            assert(Blocks[lasrLockedBlock].TryAdd(Node, ReadDependencies, WrightDependencies), "Draw could not be added.");
            return;
        }
        lasrLockedBlock = Blocks.size();
        Blocks.push_back({});
        assert(Blocks[lasrLockedBlock].TryAdd(Node, ReadDependencies, WrightDependencies), "Draw could not be added.");
        return;
    }
    }


    bool CreateBlocFlag = true;
    for (int i = lasrLockedBlock; i < Blocks.size(); i++)
    {
        if (Blocks[i].TryAdd(Node, ReadDependencies, WrightDependencies))
        {
            if (Node.Command() == EMachineCommands::SETUP_PIPELINE)
                lasrAddPS = i;
            CreateBlocFlag = false;
            break;
        }
    }

    if (CreateBlocFlag)
    {
        Blocks.push_back({});
        assert(Blocks[Blocks.size() - 1].TryAdd(Node, ReadDependencies, WrightDependencies));
        if (Node.Command() == EMachineCommands::SETUP_PIPELINE)
            lasrAddPS = Blocks.size() - 1;
    }
}


void GVM::RenderGraph::Clear()
{
    Blocks.clear();
    lasrAddPS = -1;
}
