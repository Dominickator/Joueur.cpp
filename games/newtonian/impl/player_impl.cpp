// DO NOT MODIFY THIS FILE
// Never try to directly create an instance of this class, or modify its member variables.
// This contains implementation details, written by code, and only useful to code



#include "../player.hpp"
#include "../../../joueur/src/base_ai.hpp"
#include "../../../joueur/src/any.hpp"
#include "../../../joueur/src/exceptions.hpp"
#include "../../../joueur/src/delta.hpp"
#include "../game_object.hpp"
#include "../job.hpp"
#include "../machine.hpp"
#include "../player.hpp"
#include "../tile.hpp"
#include "../unit.hpp"
#include "newtonian.hpp"

#include <type_traits>

namespace cpp_client
{

namespace newtonian
{


Player_::Player_(std::initializer_list<std::pair<std::string, Any&&>> init) :
    Game_object_{
        {"clientType", Any{std::decay<decltype(client_type)>::type{}}},
        {"generatorTiles", Any{std::decay<decltype(generator_tiles)>::type{}}},
        {"heat", Any{std::decay<decltype(heat)>::type{}}},
        {"internSpawn", Any{std::decay<decltype(intern_spawn)>::type{}}},
        {"lost", Any{std::decay<decltype(lost)>::type{}}},
        {"managerSpawn", Any{std::decay<decltype(manager_spawn)>::type{}}},
        {"name", Any{std::decay<decltype(name)>::type{}}},
        {"opponent", Any{std::decay<decltype(opponent)>::type{}}},
        {"physicistSpawn", Any{std::decay<decltype(physicist_spawn)>::type{}}},
        {"pressure", Any{std::decay<decltype(pressure)>::type{}}},
        {"reasonLost", Any{std::decay<decltype(reason_lost)>::type{}}},
        {"reasonWon", Any{std::decay<decltype(reason_won)>::type{}}},
        {"timeRemaining", Any{std::decay<decltype(time_remaining)>::type{}}},
        {"units", Any{std::decay<decltype(units)>::type{}}},
        {"won", Any{std::decay<decltype(won)>::type{}}},
    },
    client_type(variables_["clientType"].as<std::decay<decltype(client_type)>::type>()),
    generator_tiles(variables_["generatorTiles"].as<std::decay<decltype(generator_tiles)>::type>()),
    heat(variables_["heat"].as<std::decay<decltype(heat)>::type>()),
    intern_spawn(variables_["internSpawn"].as<std::decay<decltype(intern_spawn)>::type>()),
    lost(variables_["lost"].as<std::decay<decltype(lost)>::type>()),
    manager_spawn(variables_["managerSpawn"].as<std::decay<decltype(manager_spawn)>::type>()),
    name(variables_["name"].as<std::decay<decltype(name)>::type>()),
    opponent(variables_["opponent"].as<std::decay<decltype(opponent)>::type>()),
    physicist_spawn(variables_["physicistSpawn"].as<std::decay<decltype(physicist_spawn)>::type>()),
    pressure(variables_["pressure"].as<std::decay<decltype(pressure)>::type>()),
    reason_lost(variables_["reasonLost"].as<std::decay<decltype(reason_lost)>::type>()),
    reason_won(variables_["reasonWon"].as<std::decay<decltype(reason_won)>::type>()),
    time_remaining(variables_["timeRemaining"].as<std::decay<decltype(time_remaining)>::type>()),
    units(variables_["units"].as<std::decay<decltype(units)>::type>()),
    won(variables_["won"].as<std::decay<decltype(won)>::type>())
{
    for(auto&& obj : init)
    {
      variables_.emplace(std::make_pair(std::move(obj.first), std::move(obj.second)));
    }
}

Player_::~Player_() = default;

void Player_::resize(const std::string& name, std::size_t size)
{
    if(name == "generatorTiles")
    {
        auto& vec = variables_["generatorTiles"].as<std::decay<decltype(generator_tiles)>::type>();
        vec.resize(size);
        return;
    }
    else if(name == "units")
    {
        auto& vec = variables_["units"].as<std::decay<decltype(units)>::type>();
        vec.resize(size);
        return;
    }
    try
    {
        Game_object_::resize(name, size);
        return;
    }
    catch(...){}
    throw Bad_manipulation(name + " in Player treated as a vector, but it is not a vector.");
}

void Player_::change_vec_values(const std::string& name, std::vector<std::pair<std::size_t, Any>>& values)
{
    if(name == "generatorTiles")
    {
        using type = std::decay<decltype(generator_tiles)>::type;
        auto& vec = variables_["generatorTiles"].as<type>();
        for(auto&& val : values)
        { 
            vec[val.first] = std::static_pointer_cast<type::value_type::element_type>(get_game()->get_objects()[val.second.as<std::string>()]);
        }
        return;
    } 
    else if(name == "units")
    {
        using type = std::decay<decltype(units)>::type;
        auto& vec = variables_["units"].as<type>();
        for(auto&& val : values)
        { 
            vec[val.first] = std::static_pointer_cast<type::value_type::element_type>(get_game()->get_objects()[val.second.as<std::string>()]);
        }
        return;
    } 
    try
    {
        Game_object_::change_vec_values(name, values);
        return;
    }
    catch(...){}
    throw Bad_manipulation(name + " in Player treated as a vector, but it is not a vector.");
}

void Player_::remove_key(const std::string& name, Any& key)
{
    try
    {
        Game_object_::remove_key(name, key);
        return;
    }
    catch(...){}
    throw Bad_manipulation(name + " in Player treated as a map, but it is not a map.");
}

std::unique_ptr<Any> Player_::add_key_value(const std::string& name, Any& key, Any& value)
{
    try
    {
        return Game_object_::add_key_value(name, key, value);
    }
    catch(...){}
    throw Bad_manipulation(name + " in Player treated as a map, but it is not a map.");
}

bool Player_::is_map(const std::string& name)
{
    try
    {
        return Game_object_::is_map(name);
    }
    catch(...){}
    return false;
}

void Player_::rebind_by_name(Any* to_change, const std::string& member, std::shared_ptr<Base_object> ref)
{
   if(member == "opponent")
   { 
      to_change->as<Player>() = std::static_pointer_cast<Player_>(ref);
      return;
   }
   try
   {
      Game_object_::rebind_by_name(to_change, member, ref);
      return;
   }
   catch(...){}
   throw Bad_manipulation(member + " in Player treated as a reference, but it is not a reference.");
}


} // newtonian

} // cpp_client
