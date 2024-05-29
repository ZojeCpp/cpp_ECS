# Entity Component System in C++

A small project I developed during the summer in this secondary account using the C++ 20 standard, made from scratch relying on meta-programming functions to deliver a product that I would have liked to see done. 

Here you can find a complete Entity Component system framework from where you can build your own project, since everything relies on templates for increased modularity. Besides that it also means that everything will be done in compilation time which greatly reduces runtime bloat.

# Component

Thanks to the use of templates the components used by the user can be anything, from the standard types to your own. The components are also stored in a Slot Map as defined by Sean Middleditch in https://web.archive.org/web/20180121142549/http://seanmiddleditch.com/data-structures-for-game-developers-the-slot-map/ which improves caching of the required data and makes it so that insertion, removal, and access are all guaranteed to take O(1) time for the best, worst, and average case. 

Finally components can be of 3 types:

* Components: Data that will be used by systems.
* Tags: Data that will be used in order to filter entities.
* Singleton Components: The data will be shared accross all entities.

# Entities

The framework relies on an entity manager which will be used to connect the components to the systems. The manager will be in charge of the creation and destruction of entities. It can be instantiated by defining 2 template parameter packs, a Component list and a Tag list, both of these rely on meta-programming functions defined by the framework to improve the framework's capabilties.

# System

In order to separate execution of the systems from the actual entities each systema must be a lambda or callable that will recive as argument an entity. Whether the function will called for an arbitrary entity depends ont the filters defined by the user.

# API
General usage case


##Define 
```cpp

using component_list = zoje::package<physics_component,render_component>
using tag_list = zoje::package<visible,can_collide,can_attack>
using singleton_components = zoje::package<player_position>
int storage_size = 10

zoje::cmp::EntityManager<component_list,tag_list,singleton_components>,size> entity_manager = {}

##CREATE WALL
auto& e = entity_manager.createEntity();
entity_manager.addComponent<physics_component>(e,physics_component{});
entity_manager.addComponent<render_component>(e,render_component{});

entity_manager.addTag<visbible,can_collide>

##CREATE_ENEMY
auto& e = entity_manager.createEntity();
entity_manager.addComponent<physics_component>(e,physics_component{});
entity_manager.addComponent<render_component>(e,render_component{});

entity_manager.addTag<visbible,can_collide,can_attack>
```
## Defining Systems

```cpp

##SYSTEM FOR ALL ENTITIES WITH CORRESPONDING COMPONENT, USED TO REDUCE COMPUTATION COST
entity_manager.foreach_cmp<physics_component>(PhysicsSystemCallable)
entity_manager.foreach_cmp<render_component>(RenderSystemCallable)

using enemy_components = zoje::package<physics_component,render_component>
using enemy_tags = zoje::package<visible,can_collide,can_attack>

entity_manager.foreach<enemy_components,enemy_tags>(EnemySystemCallable)

entity_manager.update() ##THIS FUNCTION HANDLES THE DESTRUCTION AND CREATION OF ENTITIES
```

## Deleting entities

```cpp
##TO AVOID ISSUES THAT MY ARISE FROM PROBLEMS RELATED TO DELETING ENTITIES BEFORE NEEDED USER CAN ONLY MARK ENTITIES FOR DESTRUCTION 
entity_manager.mark4destruction(e)

```
