#pragma once

#include <functional>
#include <queue>

namespace X
{
    class RenderCommandQueue
    {
    public:
        template<typename FuncT>
        void Submit(FuncT&& func)
        {
            mCommandQueue.emplace(func);
        }

        void Execute()
        {
            while (!mCommandQueue.empty())
            {
                mCommandQueue.front()();
                mCommandQueue.pop();
            }
        }

    private:
        std::queue<std::function<void()>> mCommandQueue;
    };
}