#include "RenderGraph.h"

#include <cassert>

GVM::RenderGraphNode::RenderGraphNode(EMachineCommands Command, void* Description):
    tuple<GVM::EMachineCommands, void*>(Command, Description)
{}



bool GVM::SyncThreadBlock::TryAdd(
    const ResourceStateTransition transitionsIn[50],
    uint8_t transitionsNum,
    TryAddResult& result
)
{
    result.NewBeginTrans.clear();
    result.NewEndTrans.clear();

    for (int i = 0; i < transitionsNum; i++)
    {
        auto curTransition = transitionsIn[i];
        uint8_t flags = curTransition.flags;
        curTransition.flags = flags & (~ResourceStateTransition::END);
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
            curTransition.flags = flags;
            result.NewBeginTrans.push_back(curTransition);
            result.NewEndTrans.push_back(curTransition);
        }
        if (curTransition.StateTo == curTransition.StateFrom)
        {
            for (const auto& transition : *transitionsEnd)
            {
                if (curTransition.resource == transition.resource)
                {
                    if (transition.flags & ResourceStateTransition::WRITE)
                    {
                        return false;
                    }
                    if (curTransition.StateTo == transition.StateTo)
                    {
                        goto end_cycle;
                    }
                    return false;
                }
            }
            result.NewEndTrans.push_back(curTransition);
            goto end_cycle;
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
            result.NewBeginTrans.push_back(curTransition);
        }
        curTransition.flags = flags & (~ResourceStateTransition::BEGIN);
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
            result.NewEndTrans.push_back(curTransition);
        }
end_cycle: {}
    }
    return true;
}

void GVM::SyncThreadBlock::Add(const RenderGraphNode& Node, const TryAddResult& result)
{
    for (auto& readDep : result.NewBeginTrans)
    {
        transitionsBegin->push_back(readDep);
    }
    for (auto& wrightDep : result.NewEndTrans)
    {
        transitionsEnd->push_back(wrightDep);
    }

    GraphicsNodes.push_back(Node);
    
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
    static TryAddResult tryAddResult;
    
    if (Blocks.rbegin()->TryAdd(
     transitionsIn.data(),
     transitionsIn.size(),
     tryAddResult))
    {
        static TryAddResult tryAddResultSecond;
        int iterationsCount = 0;
        
        while ((iterationsCount + 1) <= MaxIterationToAdd &&
        (Blocks.rbegin()+(iterationsCount+1) != Blocks.rend()) &&
        (Blocks.rbegin()+(iterationsCount+1))->TryAdd(
            transitionsIn.data(),
            transitionsIn.size(),
            tryAddResultSecond)
           )
        {
            iterationsCount++;
            tryAddResult = std::move(tryAddResultSecond);
        }
        (Blocks.rbegin()+iterationsCount)->Add(Node, tryAddResult);
    }else
    {
        ResizeTransitionsPull(Blocks.size()+1);
        Blocks.emplace_back(TransitionsPull[Blocks.size()].first,
            TransitionsPull[Blocks.size()].second);
        bool success = (Blocks[Blocks.size() - 1].TryAdd(transitionsIn.data(), transitionsIn.size(), tryAddResult));
        assert(success);
        Blocks[Blocks.size() - 1].Add(Node, tryAddResult);
    }
 
}


void GVM::RenderGraph::Clear()
{
    Blocks.clear();
    for (auto& [l, r] : TransitionsPull)
    {
        if (l->capacity() > 1.5*l->size())
            l->shrink_to_fit();
        if (r->capacity() > 1.5*r->size())
            r->shrink_to_fit();
        l->clear();
        r->clear();
    }
    Blocks.emplace_back(TransitionsPull[0].first, TransitionsPull[0].second);
}
