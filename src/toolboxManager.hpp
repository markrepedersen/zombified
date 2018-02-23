#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "toolboxSlot.hpp"
#include "freeze.hpp"
#include "antidote.hpp"
#include <list>

//game screen toolbox manager
class ToolboxManager
{
public:

    //method to initialize toolboxManager's toolbox and slots
    bool init(vec2 screen);

    //calls methods to render toolbox and appropriate number of slots
    void draw(const mat3& projection);

    //add a slot to player
    //void addSlot(Player player)
    bool addSlot(int player);

    //decrease a slot from player
    //void decreaseSlot(Player player)
    void decreaseSlot(int player);

    //add item to player's toolbox
    //void addItem (Player player, int itemIndex);
    int addItem(int itemIndex, int player);

    //use first item queued from player's toolbox
    //void useItem(Player player)
    int useItem(int player);

    void setListOfSlots(std::list<int>);

    std::list<int> getListOfSlot_1();
    std::list<int> getListOfSlot_2();
    
    vec2 new_tool_position(float index, int player);
    
    void move_antidoteback(int player);
    
    //bool max_slots_filled1;
    //bool max_slots_filled2;

private:
    vec2 m_screen; //screen size
    std::list<int> m_listOfSlotsPlayer1; //list of slots, 0 if empty and available. Non-zero numbers each maps to an item
    std::list<int> m_listOfSlotsPlayer2;
    std::list<ToolboxSlot> m_listOfToolboxSlots;
    
};