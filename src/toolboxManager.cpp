//Header
#include "toolboxManager.hpp"
#include <iostream>
#include <list>
#include <cmath>

namespace
{
    const size_t MAX_TOOLSLOTS = 4;
}

bool ToolboxManager::init(vec2 screen)
{
    m_screen = screen;
    //if(!m_toolbox.init({screen.x, screen.y})) return false;

    //initialize player 1 with 2 empty slots
    m_listOfSlotsPlayer1.push_back(0);
    m_listOfSlotsPlayer1.push_back(0);
    m_listOfSlotsPlayer2.push_back(0);
    m_listOfSlotsPlayer2.push_back(0);
    
    //max_slots_filled1 = false;
    //max_slots_filled2 = false;

    return true;
}

void ToolboxManager::draw(const mat3& projection)
{
    //m_toolbox.draw(projection)
    float initialOffset = 0.f;
    float offset = m_screen.x /65;
    float index = 0.f;
    
    std::list<int>::iterator it;
    for (it = m_listOfSlotsPlayer1.begin(); it != m_listOfSlotsPlayer1.end(); ++it) {
        
        initialOffset = m_screen.x/22;
        ToolboxSlot tb;
        // std::cout << offset<< std::endl;;
        // std::cout<< index;
        tb.init(initialOffset, offset, index, 580.f, *it);
        tb.draw(projection);
        //
        index++;
    }
    
    index = 0.f;
    for (it = m_listOfSlotsPlayer2.begin(); it != m_listOfSlotsPlayer2.end(); ++it) {
        
        initialOffset = m_screen.x/1.32;
        
        ToolboxSlot tb;
        // std::cout << offset<< std::endl;;
        // std::cout<< index;
        tb.init(initialOffset, offset, index, 580.f, *it);
        tb.draw(projection);
        //
        index++;
    }

}

bool ToolboxManager::addSlot(int player)
{
    if (player == 1)
    {
        if (m_listOfSlotsPlayer1.size() < MAX_TOOLSLOTS)
        {
            m_listOfSlotsPlayer1.push_back(0);
            //max_slots_filled1 = false;
            return true;
        }
    }
    if (player == 2)
    {
        if (m_listOfSlotsPlayer2.size() < MAX_TOOLSLOTS)
        {
            m_listOfSlotsPlayer2.push_back(0);
            //max_slots_filled2 = false;
            return true;
        }
    }
    
    return false;
}

void ToolboxManager::decreaseSlot()
{
    m_listOfSlotsPlayer1.pop_back();
}

int ToolboxManager::addItem(int itemIndex, int player)
{
    int count = 0;
    std::list<int>::iterator it;
    if (player ==1)
    {
        for (it = m_listOfSlotsPlayer1.begin(); it != m_listOfSlotsPlayer1.end(); it++)
        {
            //if slot doesnt have anything in it = 0
            // freeze tool = 1
            // water tool = 2
            // antidote = 3
            if (*it == 0) {
                *it = itemIndex;
                return count;
            }
            count++;
        }
        //max_slots_filled1 = true;
        //return 100;
    }
    
    if (player ==2)
    {
        for (it = m_listOfSlotsPlayer2.begin(); it != m_listOfSlotsPlayer2.end(); it++)
        {
            if (*it == 0) {
                *it = itemIndex;
                return count;
            }
            count++;
        }
        //max_slots_filled2 = true;
        //return 100;
    }
    
    return 100;
}

void ToolboxManager::useItem()
{
    m_listOfSlotsPlayer1.pop_front();
}

vec2 ToolboxManager::new_tool_position(float index, int player)
{
    float initialOffset = 0.f;
    float offset = m_screen.x /65+60;
    
    if (player == 1)
        initialOffset = m_screen.x/22;
    if (player ==2)
        initialOffset =  m_screen.x/1.32;
    
    return {(initialOffset + (offset * index ))* ViewHelper::getRatio(),
        580.f* ViewHelper::getRatio()};
}



