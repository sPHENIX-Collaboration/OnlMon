#include "triggerEnum.h"

#include <iostream>


std::string TriggerEnum::GetTriggerName(TriggerEnum::BitCodes code)
{
    auto it = TriggerEnum::TriggerNames.find(code);
    if ( it != TriggerEnum::TriggerNames.end() )
    {
        return it->second;
    }
    std::cout << "TriggerEnum::GetTriggerName: Unknown trigger code: " << code << std::endl;
    return "Unknown";
}