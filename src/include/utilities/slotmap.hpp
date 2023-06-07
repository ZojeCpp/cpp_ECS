#pragma once

#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>


namespace zoje
{
    /// @brief this implementation uses std::vector as the container for the slotmap,therfore allocated memory space is dynamic
    /// @tparam T data type to be stored
    template<typename T>
    struct slotmap
    {
        using key_type = std::pair<std::size_t,std::size_t>;

        slotmap()=default;

        /// @brief erases the slot that corresponds to given key, if the key is invalid throws terminate
        /// @param key must be user-key, user-key corresponds to key for indirection layer
        void erase(key_type key)
        {
            if(isValid(key))
            {
                auto last = size()-1;
                auto deletedSlotIdx = free(key.first);

                if(deletedSlotIdx==last)
                {
                    eraseEnd(key);
                }
                else
                {
                    std::iter_swap<container_itr,container_itr>(container_itr{&data_[deletedSlotIdx]},container_itr{&data_[last]});
                    index_[eraseInfo_[last]].first = deletedSlotIdx;
                    std::iter_swap<link_itr,link_itr>(link_itr{&eraseInfo_[deletedSlotIdx]},link_itr{&eraseInfo_[last]});
                
                    eraseEnd(key);
                }

            }
        }

        /// @brief checks if the given key corresponds to a valid slot
        /// @param key 
        /// @return true if there's a slot for the given key, false otherwise
        [[__nodiscard__]] bool isValid(key_type key) noexcept
        {
            if(key.first<index_.size() && index_[key.first].second == key.second) return true;
            else return false;
        }

        /// @brief add an element to the slotmap,  
        /// @param data must be rvalue 
        /// @return returns user-key to acces slot, user-key corresponds to key for indirection layer
        [[__nodiscard__]] key_type push_back(T&& data)
        {
            auto slotPosition = nextfreeSlot_;

            key_type slotAssigned = alloc();

            data_.emplace_back(std::move(data));

            slotAssigned.first = slotPosition;
            return slotAssigned;
        }


        /// @brief returns element that correspons to given key
        /// @param key 
        /// @return returns reference to the element
        [[__nodiscard__]] T& getSlotData(key_type key)
        {
            if(isValid(key))
            {
                return data_[index_[key.first].first]; 
            }
            else std::terminate();
        }

        /// @brief returns element that correspons to given key 
        /// @param key 
        /// @return returns read-only reference to the element
        [[__nodiscard__]] const T& cgetSlotData(key_type key)
        {
            if(isValid(key))
            {
                return data_[index_[key.first].first]; 
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

            nextfreeSlot_ = slot2assign.first;

            slot2assign = key_type{size(),generation_};

            generation_++;

            return slot2assign;
        }

        /// @brief pop_back of the different containers on the slotmap, in case deleted element was back of the slotmap
        /// @param key key of the element being erased
        void eraseEnd(key_type key)
        {
            if(key.first==index_.size()-1 && nextfreeSlot_==key.first)
            {
                auto lastSlot = index_.back().first;
                auto newFreeSlot = lastSlot;

                index_[lastSlot].first = nextfreeSlot_;
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

            auto dataIdx = slot2Delete.first;

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
        //size_t Capacity=10;
        using key_type = std::pair<std::size_t,std::size_t>;

        static_slotmap()
        {
            static auto counter{1};
            std::generate(index_.begin(),index_.end(), [&](){ return key_type{counter++,0};});
            std::iota(eraseInfo_.begin(),eraseInfo_.end(), 0);
        }

        /// @brief erases the slot that corresponds to given key, if the key is invalid throws terminate
        /// @param key must be user-key, user-key corresponds to key for indirection layer
        void erase(key_type key)
        {
            if(isValid(key))
            {
                auto last = size_-1;
                auto deletedSlotIdx = free(key.first);

                if(deletedSlotIdx==last)
                {
                    eraseInfo_[deletedSlotIdx] = 0;
                }
                else
                {
                    data_[deletedSlotIdx] = data_[last];
                    index_[eraseInfo_[last]].first = deletedSlotIdx;
                    eraseInfo_[deletedSlotIdx] = eraseInfo_[last];
                }
                size_--;

            }
        }

        /// @brief checks if the given key corresponds to a valid slot
        /// @param key 
        /// @return true if there's a slot for the given key, false otherwise
        [[__nodiscard__]] bool isValid(key_type key) noexcept
        {
            if(index_[key.first].second == key.second) return true;
            else return false;
        }

        /// @brief add an element to the slotmap,  
        /// @param data must be rvalue 
        /// @return returns user-key to acces slot, user-key corresponds to key for indirection layer
        [[__nodiscard__]] key_type push_back(T&& data)
        {
            auto slotPosition = nextfreeSlot_;

            key_type slotAssigned = alloc();

            data_[slotAssigned.first] = data;

            slotAssigned.first = slotPosition;
            return slotAssigned;
        }


        /// @brief returns element that correspons to given key
        /// @param key 
        /// @return returns reference to the element
        [[__nodiscard__]] T& getSlotData(key_type key)
        {
            if(isValid(key))
            {
                return data_[index_[key.first].first]; 
            }
            else std::terminate();
        }

        /// @brief returns element that correspons to given key 
        /// @param key 
        /// @return returns read-only reference to the element
        [[__nodiscard__]] const T& cgetSlotData(key_type key)
        {
            if(isValid(key))
            {
                return data_[index_[key.first].first]; 
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

            nextfreeSlot_ = slot2assign.first;

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

            auto dataIdx = slot2Delete.first;

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