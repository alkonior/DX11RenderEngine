#include "RenderGraph.h"

#include <cassert>


GVM::RenderGraphNode::RenderGraphNode(EMachineCommands Command, void* Description, std::vector<Resource*>&& readDependencies, std::vector<Resource*>&& wrightDependencies):
    Command(Command), Description(Description),
    ReadDependencies(std::forward<std::vector<Resource*>>(readDependencies)),
    WrightDependencies(std::forward<std::vector<Resource*>>(wrightDependencies))
{
    for (auto& readDep : ReadDependencies)
    {
        for (auto& wrightDep : WrightDependencies)
        {
            assert(readDep != wrightDep);
        }
    }
}


bool GVM::SyncThreadBlock::TryAdd(const RenderGraphNode& Node)
{
    static std::vector<Resource*> NewReadDep;
    static std::vector<Resource*> NewWrightDep;
    NewReadDep.clear();
    NewWrightDep.clear();

    for (auto& readDep : Node.ReadDependencies)
    {
        if (ReadDependencies.contains(readDep))
            continue;
        if (WrightDependencies.contains(readDep))
            return false;
        NewReadDep.push_back(readDep);
    }

    for (auto& wrightDep : Node.WrightDependencies)
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


void GVM::RenderGraph::AddCommand(RenderGraphNode Node)
{
    switch (Node.Command)
    {
    case EMachineCommands::DRAW:
    case EMachineCommands::BEGIN_EVENT:
    case EMachineCommands::END_EVENT:
    case EMachineCommands::CLEAR_PIPELINE:
    {
        assert(lasrAddPS != -1);
        assert(Blocks[lasrAddPS].TryAdd(Node), "Draw could not be added.");
        return;
    }
    }


    bool CreateBlocFlag = true;
    for (int i = 0; i < Blocks.size(); i++)
    {
        if (Blocks[i].TryAdd(Node))
        {
            if (Node.Command == EMachineCommands::SETUP_PIPELINE)
                lasrAddPS = i;
            CreateBlocFlag = false;
            break;
        }
    }

    if (CreateBlocFlag)
    {
        Blocks.push_back({});
        Blocks[Blocks.size() - 1].TryAdd(Node);
        if (Node.Command == EMachineCommands::SETUP_PIPELINE)
            lasrAddPS = Blocks.size() - 1;
    }
}


void GVM::RenderGraph::Clear()
{
    Blocks.clear();
    lasrAddPS = -1;
}
