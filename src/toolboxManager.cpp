//Header
#include "toolboxManager.hpp"
#include <iostream>
#include <list>
#include <cmath>



bool ToolboxManager::init(vec2 screen)
{
    m_screen = screen;
    if(!m_toolbox.init({screen.x, screen.y + 150})) return false;
    
    //initialize player 1 with 2 empty slots
    m_listOfSlotsPlayer1.push_back(0);
    m_listOfSlotsPlayer1.push_back(0);

    return true;
}

void ToolboxManager::draw(const mat3& projection)
{
    m_toolbox.draw(projection);

    float initialOffset = m_screen.x/10;
    float offset = m_screen.x /30;
    float index = 0.f;
    
    std::list<int>::iterator it;
    for (it = m_listOfSlotsPlayer1.begin(); it != m_listOfSlotsPlayer1.end(); ++it) {
        ToolboxSlot tb;
        // std::cout << offset<< std::endl;;
        // std::cout<< index;
        tb.init(initialOffset, offset, index, m_screen.y + 50, *it);
        tb.draw(projection);
        //
        index++;
    }

}

void ToolboxManager::addSlot()
{
    m_listOfSlotsPlayer1.push_back(0);
}

void ToolboxManager::decreaseSlot()
{
    m_listOfSlotsPlayer1.pop_back();
}

void ToolboxManager::addItem(int itemIndex)
{
    std::list<int>::iterator it;
    for (it = m_listOfSlotsPlayer1.begin(); it != m_listOfSlotsPlayer1.end(); it++) {
        if (*it == 0) {
            *it = itemIndex;
        }
    }
}

void ToolboxManager::useItem()
{
    m_listOfSlotsPlayer1.pop_front();
}