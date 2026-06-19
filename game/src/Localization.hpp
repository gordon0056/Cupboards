#pragma once

namespace game
{

inline const char* tr(const char* en, const char* ru, int lang)
{
    return (lang == 1) ? ru : en;
}

}
