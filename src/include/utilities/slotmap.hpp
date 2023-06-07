#pragma once

#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <include/utilities/paramter_pack_utils.hpp>


namespace zoje
{
    /// @brief this implementation uses std::vector as the container for the slotmap,therfore allocated memory space is dynamic
    /// @tparam T data type to be stored
    template<typename T>
    struct slotmap
    {
        struct key_type {size_t index{}; size_t generation{}; };

        slotmap()=default;

        /// @brief erases the slot that corresponds to given key, if the key is invalid throws terminate
        /// @param key must be user-key, user-key corresponds to key for indirection layer
        constexpr void erase(key_type key)
        {
            if(isValid(key))
            {
                auto last = size()-1;
                auto deletedSlotIdx = free(key.index);

                if(deletedSlotIdx==last)
                {
                    eraseEnd(key);
                }
                else
                {
                    std::iter_swap<container_itr,container_itr>(container_itr{&data_[deletedSlotIdx]},container_itr{&data_[last]});
                    index_[eraseInfo_[last]].index = deletedSlotIdx;
                    std::iter_swap<link_itr,link_itr>(link_itr{&eraseInfo_[deletedSlotIdx]},link_itr{&eraseInfo_[last]});
                
                    eraseEnd(key);
                }

            }
        }

        /// @brief checks if the given key corresponds to a valid slot
        /// @param key 
        /// @return true if there's a slot for the given key, false otherwise
        [[__nodiscard__]] constexpr bool isValid(const key_type& key) noexcept
        {
            if(key.index<index_.size() && index_[key.index].generation == key.generation) return true;
            else return false;
        }

        constexpr T& operator[](const key_type& key) noexcept
        {
            assert(key.index < index_.capacity() && index_[key.index].index < data_.capacity );
            return data_[index_[key.index].index];
        }

        //TODO
        //LOOK UP HOW at() actually works
        // constexpr T& at(const key_type& key) noexcept
        // {
        //     if(key.index < index_.capacity() && index_[key.index].index < data_.capacity )
        //     {
        //         return data_[index_[key.index].index];
        //     }
        // }

        constexpr size_t capacity() noexcept { return data_.capacity();}

        constexpr void clear()
        {
            data_.clear();
            index_.clear();
            eraseInfo_.clear();
        }

        /// @brief add an element to the slotmap,  
        /// @param data must be rvalue 
        /// @return returns user-key to acces slot, user-key corresponds to key for indirection layer
        [[__nodiscard__]] constexpr key_type push_back(ppUtils::r_reference_or_l_reference_t<T> data)
        {
            auto slotPosition = nextfreeSlot_;

            key_type slotAssigned = alloc();

            data_.emplace_back(std::forward<T>(data));

            slotAssigned.index = slotPosition;
            return slotAssigned;
        }


        /// @brief returns element that correspons to given key
        /// @param key 
        /// @return returns reference to the element
        [[__nodiscard__]] constexpr T& getSlotData(key_type key)
        {
            if(isValid(key))
            {
                return data_[index_[key.index].index]; 
            }
            else std::terminate();
        }

        /// @brief returns element that correspons to given key 
        /// @param key 
        /// @return returns read-only reference to the element
        [[__nodiscard__]] constexpr const T& cgetSlotData(key_type key)
        {
            if(isValid(key))
            {
                return data_[index_[key.index].index]; 
            }
            else std::terminate();
        }

        /// @brief 
        /// @return current size of the slotmap
        [[__nodiscard__]] const size_t size() const{ return data_.size();}

        /// @brief 
        /// @return iterator to the first element on the slotmap
        [[__nodiscard__]] auto begin(){ return data_.begin();}
        /// @brief 
        /// @return iterator to the last element on the slotmap
        [[__nodiscard__]] auto end(){ return data_.end();}

        /// @brief 
        /// @return read-only iterator to the last element on the slotmap
        [[__nodiscard__]] auto cbegin() const{ return data_.cbegin();}

        /// @brief 
        /// @return read-only iterator to the last element on the slotmap
        [[__nodiscard__]] auto cend() const{ return data_.cend();}


        private:

        /// @brief allocates slot for the new element to be insterted, not meant to be used by user
        /// @return non user-key, key corresponds to the actual container of the element 
        [[__nodiscard__]] key_type alloc()
        {

            key_type& slot2assign = reference2Slot();

            eraseInfo_.emplace_back(size());

            nextfreeSlot_ = slot2assign.index;

            slot2assign = key_type{size(),generation_};

            generation_++;

            return slot2assign;
        }

        /// @brief pop_back of the different containers on the slotmap, in case deleted element was back of the slotmap
        /// @param key key of the element being erased
        void eraseEnd(key_type key)
        {
            if(key.index==index_.size()-1 && nextfreeSlot_==key.index)
            {
                auto lastSlot = index_.back().index;
                auto newFreeSlot = lastSlot;

                index_[lastSlot].index = nextfreeSlot_;
                nextfreeSlot_ = newFreeSlot;
                index_.pop_back();
            }
            
            data_.pop_back();
            eraseInfo_.pop_back();
        }
        
        /// @brief if nextfreeSlot is bigger than current slotmap increment slot count by 1, otherwise return free slot
        /// @return next free slot on the indirection layer
        [[__nodiscard__]] key_type& reference2Slot()
        {
            if(nextfreeSlot_>=index_.size())
                return index_.emplace_back(key_type{size()+1,0});
            else
                return index_[nextfreeSlot_];
        }

        /// @brief frees slot on the indirection layer 
        /// @param idxPos index for the indirection layer
        /// @return returns the freed index on the container 
        [[__nodiscard__]] size_t free(size_t idxPos)
        {   
            key_type& slot2Delete = index_[idxPos];

            auto dataIdx = slot2Delete.index;

            slot2Delete = key_type{nextfreeSlot_,0};

            nextfreeSlot_ = idxPos;

            return dataIdx;
        }

        /// @brief link between indirection layer and container
        std::vector<std::size_t> eraseInfo_{}; 
        /// @brief indirection layer, to avoid pointer dependency
        std::vector<key_type> index_{}; 
        /// @brief actual container of the inserted elements
        std::vector<T> data_;

        using container_itr = decltype(data_.begin());
        using link_itr = decltype(eraseInfo_.begin());
        using indirection_layer_itr = decltype(index_.begin());



        /// @brief begining of the indirection layer
        std::size_t nextfreeSlot_{0};
        /// @brief value added to avoid key clashing
        std::size_t generation_{1};

    };





    /// @brief this implementation uses std::array as the container for the slotmap, it's size is fixed
    /// @tparam T data type to be stored
    /// @tparam Capacity capacity of the storage
    template<typename T, auto Capacity = 10>
    struct static_slotmap
    {
        struct key_type {size_t index{}; size_t generation{}; };

        static_slotmap()
        {
            static size_t counter{1};
            std::generate(index_.begin(),index_.end(), [&](){ return key_type{counter++,size_t{0}};});
            std::iota(eraseInfo_.begin(),eraseInfo_.end(), 0);
        }

        /// @brief erases the slot that corresponds to given key, if the key is invalid throws terminate
        /// @param key must be user-key, user-key corresponds to key for indirection layer
        constexpr void erase(key_type key)
        {
            if(isValid(key))
            {
                auto last = size_-1;
                auto deletedSlotIdx = free(key.index);

                if(deletedSlotIdx==last)
                {
                    eraseInfo_[deletedSlotIdx] = 0;
                }
                else
                {
                    data_[deletedSlotIdx] = data_[last];
                    index_[eraseInfo_[last]].index = deletedSlotIdx;
                    eraseInfo_[deletedSlotIdx] = eraseInfo_[last];
                }
                size_--;

            }
        }

        /// @brief checks if the given key corresponds to a valid slot
        /// @param key 
        /// @return true if there's a slot for the given key, false otherwise
        [[__nodiscard__]] constexpr bool isValid(const key_type& key) noexcept
        {
            if(index_[key.index].generation == key.generation) return true;
            else return false;
        }

        constexpr T& operator[](const key_type& key) noexcept
        {
            return data_[index_[key.index].index];
        }

        constexpr size_t capacity() noexcept { return Capacity ;}

        constexpr void clear() noexcept 
        {
            static size_t counter{1};
            std::generate(index_.begin(),index_.end(), [&](){ return key_type{counter++,size_t{0}};});
            nextfreeSlot_ = 0;
        }

        /// @brief add an element to the slotmap,  
        /// @param data must be rvalue 
        /// @return returns user-key to acces slot, user-key corresponds to key for indirection layer
        [[__nodiscard__]] constexpr key_type push_back(ppUtils::r_reference_or_l_reference_t<T>  data)
        {
            auto slotPosition = nextfreeSlot_;

            key_type slotAssigned = alloc();

            data_[slotAssigned.index] = data;

            slotAssigned.index = slotPosition;
            return slotAssigned;
        }


        /// @brief returns element that correspons to given key
        /// @param key 
        /// @return returns reference to the element
        [[__nodiscard__]] constexpr T& getSlotData(key_type key)
        {
            if(isValid(key))
            {
                return data_[index_[key.index].index]; 
            }
            else std::terminate();
        }

        /// @brief returns element that correspons to given key 
        /// @param key 
        /// @return returns read-only reference to the element
        [[__nodiscard__]] constexpr const T& cgetSlotData(key_type key)
        {
            if(isValid(key))
            {
                return data_[index_[key.index].index]; 
            }
            else std::terminate();
        }

        /// @brief 
        /// @return current size of the slotmap
        [[__nodiscard__]] const auto size() const{ return size_;}

        /// @brief 
        /// @return iterator to the first element on the slotmap
        [[__nodiscard__]] auto begin(){ return data_.begin();}
        /// @brief 
        /// @return iterator to the last element on the slotmap
        [[__nodiscard__]] auto end(){ return data_.begin()+size_;}

        /// @brief 
        /// @return read-only iterator to the last element on the slotmap
        [[__nodiscard__]] auto cbegin() const{ return data_.cbegin();}

        /// @brief 
        /// @return read-only iterator to the last element on the slotmap
        [[__nodiscard__]] auto cend() const{ return data_.cbegin()+size_;}


        private:

        /// @brief allocates slot for the new element to be insterted, not meant to be used by user
        /// @return non user-key, key corresponds to the actual container of the element 
        [[__nodiscard__]] key_type alloc()
        {
            key_type& slot2assign = index_[nextfreeSlot_];

            eraseInfo_[size_] = size_;

            nextfreeSlot_ = slot2assign.index;

            slot2assign = key_type{size_,generation_};

            generation_++;
            size_++;

            return slot2assign;
        }
        
        /// @brief frees slot on the indirection layer 
        /// @param idxPos index for the indirection layer
        /// @return returns the freed index on the container 
        [[__nodiscard__]] size_t free(size_t idxPos)
        {   
            key_type& slot2Delete = index_[idxPos];

            auto dataIdx = slot2Delete.index;

            slot2Delete = key_type{nextfreeSlot_,0};

            nextfreeSlot_ = idxPos;

            return dataIdx;
        }

        /// @brief link between indirection layer and container
        std::array<std::size_t, Capacity> eraseInfo_{}; 
        /// @brief indirection layer, to avoid pointer dependency
        std::array<key_type, Capacity> index_{}; 
        /// @brief actual container of the inserted elements
        std::array<T, Capacity> data_;
        /// @brief begining of the indirection layer
        std::size_t nextfreeSlot_{0};
        /// @brief value added to avoid key clashing
        std::size_t generation_{1};
        std::size_t size_{0};


    };
}