# Entity Component System in C++

A small project I developed during the summer in this secondary account using the C++ 20 standard. 

Here you can find a complete Entity Component system framework, from where you can build your own project, since everything relies on templates for increased modularity. Besides that it also means that every part of your implementation will be don in compilation time which greatly reduces runtime bloat.

# Component

Thanks to the use of templates the components used by the user can be anything, from the standard types to your own. The components are also stored in a Slot Map as defined by Sean Middleditch in https://web.archive.org/web/20180121142549/http://seanmiddleditch.com/data-structures-for-game-developers-the-slot-map/ which improves caching of the required data.

# Entities

The framework relies on an entity manager which will be used to connect the components to the systems. The manager will be in charge of the creation and destruction of entities. 

# System

Through the use of meta-programming tchniques applied to the use of systems the user can define which kind of components will be used for each system in compile time.

# API

