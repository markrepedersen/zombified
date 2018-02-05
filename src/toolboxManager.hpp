#pragma once

#include "common.hpp"
#include "toolbox.hpp"
#include "toolboxSlot.hpp"
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
    void addSlot();

    //decrease a slot from player
    //void decreaseSlot(Player player)
    void decreaseSlot();

    //add item to player's toolbox
    //void addItem (Player player, int itemIndex);
    void addItem(int itemIndex);

    //use first item queued from player's toolbox
    //void useItem(Player player)
    void useItem();

    void setListOfSlots(std::list<int>);

    std::list<int> getListOfSlots();

private:
    vec2 m_screen; //screen size
    std::list<int> m_listOfSlotsPlayer1; //list of slots, 0 if empty and available. Non-zero numbers each maps to an item
    std::list<int> m_listOfSlotsPlayer2;
    std::list<ToolboxSlot> m_listOfToolboxSlots;
    Toolbox m_toolbox; //toolbox
};