#pragma once
#include <optional>
#include <functional>
#include <tuple>

template <typename T = float>
class ImTween
{
public:
    enum TweenType
    {
        PingPong
    };

private:
    TweenType type = TweenType::PingPong;
    float speed = 0.5f;
    std::optional<std::function<void()>> onComplete;
    std::function<bool()> condition;

    std::vector<std::tuple<T, T, T*>>&& tweens = {};

    ImTween(std::vector<std::tuple<T, T, T*>>&& tweens)
    {
        this->tweens = std::move(tweens);
    }

public:
    /*
     * accepts unlimited number of values in form of std::tuple<T, T, T*> { min, max, value }
     */
    template <typename T = float, typename... Values>
    constexpr static auto Tween(Values... values) -> ImTween        //constexpr 在編譯出就將所有 function 內的運算計算好 (function 內的所有數值都是預先計算好的)
    {
        static_assert(sizeof...(values) > 0, "No values to animate");
        static_assert((std::is_same_v<std::tuple<T, T, T*>, Values> && ...), "Values must be of type std::tuple<T, T, T*>");

        std::vector<std::tuple<T, T, T*>> tweens;

        (tweens.push_back(values), ...);

        return ImTween<T>(std::move(tweens));
    }

    auto OfType(TweenType&& type) -> ImTween&
    {
        this->type = type;

        return *this;
    }

    auto Speed(float&& speed) -> ImTween&
    {
        this->speed = speed;

        return *this;
    }

    auto When(std::function<bool()>&& condition) -> ImTween&
    {
        this->condition = condition;

        return *this;
    }

    auto OnComplete(std::function<void()>&& onComplete) -> ImTween&
    {
        this->onComplete = onComplete;

        return *this;
    }

    auto Tick()
    {
        for (auto&& tween : tweens)
        {
            auto&& min = std::get<0>(tween);
            auto&& max = std::get<1>(tween);
            auto&& value = *std::get<2>(tween);

            if (this->condition() && value < max)
            {
                value += speed;

                if (value > max)
                    value = max;
            }
            else if ((this->type == TweenType::PingPong) && !this->condition() && value > min)
            {
                value -= speed;

                if (value < min)
                    value = min;
            }

            if (this->onComplete.has_value() && value == max)
                this->onComplete.value()();
        }
    }
};









