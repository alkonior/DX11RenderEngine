#include "RenderGraph.h"

#include <cassert>

GVM::RenderGraphNode::RenderGraphNode(EMachineCommands Command, void* Description):
    tuple<GVM::EMachineCommands, void*>(Command, Description)
{}



bool GVM::SyncThreadBlock::TryAdd(const RenderGraphNode& Node,
    const ResourceStateTransition transitionsIn[50],
    uint8_t transitionsNum
)
{
    static std::vector<ResourceStateTransition> NewBeginTrans;
    static std::vector<ResourceStateTransition> NewEndTrans;
    NewBeginTrans.clear();
    NewEndTrans.clear();

    bool result = true;

    for (int i = 0; i < transitionsNum; i++)
    {
        
        auto curTransition = transitionsIn[i];
        uint8_t flags = curTransition.flags;
        curTransition.flags = flags & (!ResourceStateTransition::END);
        if (flags == ResourceStateTransition::DefInvalidFlag)
        {
            for (const auto& transition : *transitionsEnd)
            {
                if (curTransition == transition)
                {
                    goto end_cycle;
                }
                if (curTransition.resource == transition.resource)
                {
                    return false;
                }
            }
            NewBeginTrans.push_back(curTransition);
            NewEndTrans.push_back(curTransition);
        }
        if (flags & ResourceStateTransition::BEGIN)
        {
            for (const auto& transition : *transitionsBegin)
            {
                if (curTransition == transition)
                {
                    goto end_cycle;
                }
                if (curTransition.resource == transition.resource)
                {
                    return false;
                }
            }
            NewBeginTrans.push_back(curTransition);
        }
        curTransition.flags = flags & (!ResourceStateTransition::WRITE);
        if (flags & ResourceStateTransition::END)
        {
            for (const auto& transition : *transitionsEnd)
            {
                if (curTransition == transition)
                {
                    goto end_cycle;
                }
                if (curTransition.resource == transition.resource)
                {
                    return false;
                }
            }
                NewEndTrans.push_back(curTransition);
        }
end_cycle: {}
    }
    
    for (auto& readDep : NewBeginTrans)
    {
        transitionsBegin->push_back(readDep);
    }
    for (auto& wrightDep : NewEndTrans)
    {
        transitionsEnd->push_back(wrightDep);
    }

    GraphicsNodes.push_back(Node);
    return true;
}


void GVM::RenderGraph::AddCommand(RenderGraphNode Node,
    const std::vector<ResourceStateTransition>& transitionsIn)
{
    //switch (Node.Command())
    //{
    //case EMachineCommands::BEGIN_EVENT:
    //case EMachineCommands::END_EVENT:
    //case EMachineCommands::CLEAR_PIPELINE:
    //{
    //    if (lasrLockedBlock == Blocks.size() - 1)
    //    {
    //        assert(Blocks[lasrLockedBlock].TryAdd(Node, ReadDependencies, WrightDependencies), "Draw could not be added.");
    //        return;
    //    }
    //    lasrLockedBlock = Blocks.size();
    //    Blocks.push_back({});
    //    assert(Blocks[lasrLockedBlock].TryAdd(Node, ReadDependencies, WrightDependencies), "Draw could not be added.");
    //    return;
    //}
    //}


    if (!Blocks.rbegin()->TryAdd(Node, transitionsIn.data(), transitionsIn.size()))
    {
        TransitionPull.resize(Blocks.size()+1);
        Blocks.emplace_back(TransitionPull.rbegin()->first, TransitionPull.rbegin()->second);
        bool succses = (Blocks[Blocks.size() - 1].TryAdd(Node, transitionsIn.data(), transitionsIn.size()));
        assert(succses);
    }
}


void GVM::RenderGraph::Clear()
{
    Blocks.clear();
    for (auto& [l, r] : TransitionPull)
    {
        if (l->capacity() > 1.5*l->size())
            l->shrink_to_fit();
        if (r->capacity() > 1.5*r->size())
            r->shrink_to_fit();
        l->clear();
        r->clear();
    }
    Blocks.emplace_back(TransitionPull[0].first, TransitionPull[0].second);
}
