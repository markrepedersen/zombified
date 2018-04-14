//Header
#include "toolboxManager.hpp"
#include <iostream>
#include <list>
#include <cmath>

namespace
{
    const size_t MAX_TOOLSLOTS = 8;
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
    
    toolboxInUse = false;

    return true;
}

void ToolboxManager::draw(const mat3& projection)
{
    //m_toolbox.draw(projection)
    float initialOffset = 0.f;
    float offset = (m_screen.x /65);
    float indexTop = 0.f;
    float indexBottom = 0.f;
    
//    
//    if(!toolboxInUse)
//    {
//        toolboxInUse = true;
        std::vector<int>::iterator it;
        for (it = m_listOfSlotsPlayer1.begin(); it != m_listOfSlotsPlayer1.end(); ++it) {
            
            initialOffset = m_screen.x/22;
            ToolboxSlot tb;
            // std::cout << offset<< std::endl;;
            // std::cout<< index;
            if (indexTop > 3.f)
            {
                tb.init(initialOffset, offset, indexBottom, 660.f, *it);
                indexBottom++;
            }
            else
            {
                tb.init(initialOffset, offset, indexTop, 580.f, *it); //580.f
                indexTop++;
            }
            tb.draw(projection);
        }
        
        indexTop = 0.f;
        indexBottom = 0.f;
        for (it = m_listOfSlotsPlayer2.begin(); it != m_listOfSlotsPlayer2.end(); ++it) {
            
            initialOffset = m_screen.x/1.32;
            
            ToolboxSlot tb;
            // std::cout << offset<< std::endl;;
            // std::cout<< index;
            if (indexTop > 3.f)
            {
                tb.init(initialOffset, offset, indexBottom, 660.f, *it);
                indexBottom++;
            }
            else
            {
                tb.init(initialOffset, offset, indexTop, 580.f, *it); //580.f
                indexTop++;
            }
            tb.draw(projection);
        }
        
//        toolboxInUse = false;
//    }

}

bool ToolboxManager::addSlot(int player)
{
//    std::cout << "toolbox in use in addSlot?" << toolboxInUse << std::endl;
//    if(!toolboxInUse)
//    {
//        toolboxInUse = true;
//        
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
//        toolboxInUse = false;
//    }
//    
    return false;
}

void ToolboxManager::decreaseSlot(int player)
{
//    if(!toolboxInUse)
//    {
        
//        toolboxInUse = true;
        if (player == 1)
            m_listOfSlotsPlayer1.erase(m_listOfSlotsPlayer1.begin()+antidotePos);
        
        if (player == 2)
            m_listOfSlotsPlayer2.erase(m_listOfSlotsPlayer2.begin()+antidotePos);
//    }
    toolboxInUse = false;
}

int ToolboxManager::addItem(int itemIndex, int player)
{
//    if(!toolboxInUse)
//    {
//        toolboxInUse = true;
        int count = 0;
        std::vector<int>::iterator it;
        if (player ==1)
        {
            for (it = m_listOfSlotsPlayer1.begin(); it != m_listOfSlotsPlayer1.end(); ++it)
            {
                //if slot doesnt have anything in it = 0
                // freeze tool = 1
                // water tool = 2
                // antidote = 3
                // leg = 4
                // bomb = 5
                // missile = 6
                // armour = 7
                if (*it == 0) {
                    *it = itemIndex;
                    return count;
                }
                count++;
            }
        }
        
        if (player ==2)
        {
            for (it = m_listOfSlotsPlayer2.begin(); it != m_listOfSlotsPlayer2.end(); ++it)
            {
                if (*it == 0) {
                    *it = itemIndex;
                    return count;
                }
                count++;
            }
        }
//        toolboxInUse = false;
//    }
    return 100;
}

int ToolboxManager::useItem(int player)
{
    int item_number = 0;
//    if(!toolboxInUse)
//    {
//        toolboxInUse = true;
        if (!m_listOfSlotsPlayer1.empty())
        {
            if (player == 1)
                item_number = m_listOfSlotsPlayer1.front();
        }
        
        if (!m_listOfSlotsPlayer2.empty())
        {
            if (player == 2)
                item_number = m_listOfSlotsPlayer2.front();
        }
//        toolboxInUse = false;
//    }
    return item_number;
}

vec2 ToolboxManager::new_tool_position(float index, int player)
{
    float initialOffset = 0.f;
    float offset = (m_screen.x /65+60);
    
    if (player == 1)
        initialOffset = (m_screen.x/22);
    if (player ==2)
        initialOffset =  (m_screen.x/1.32);
    
    //fprintf(stderr, "index before %f \n", index);
    if (index > 3.f)
    {
        index -= 4.f;
        //fprintf(stderr, "index after %f \n", index);
        return {(initialOffset + (offset * index ))* ViewHelper::getRatio(),
            660.f* ViewHelper::getRatio()};
    }
    else
        return {(initialOffset + (offset * index ))* ViewHelper::getRatio(),
            580.f* ViewHelper::getRatio()};
}

void ToolboxManager::move_antidoteback(int player)
{

    if (player == 1)
        m_listOfSlotsPlayer1.push_back(3);
    
    if (player == 2)
        m_listOfSlotsPlayer2.push_back(3);
    
}

std::vector<int> ToolboxManager::getListOfSlot_1()
{
    return m_listOfSlotsPlayer1;
}

std::vector<int> ToolboxManager::getListOfSlot_2()
{
    return m_listOfSlotsPlayer2;
}

void ToolboxManager::setToolboxInUse(bool inUse)
{
    this->toolboxInUse = inUse;
}

bool ToolboxManager::getToolboxInUse()
{
    return toolboxInUse;
}


void ToolboxManager::destroy()
{
    m_listOfSlotsPlayer1.clear();
    m_listOfSlotsPlayer2.clear();
    
}

