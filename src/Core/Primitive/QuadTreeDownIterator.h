/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __QUADTREEDOWNITERATOR_H__
#define	__QUADTREEDOWNITERATOR_H__

#include "QuadTree.h"

namespace degate
{
    template <typename T>
    class DownIterator : public std::iterator<std::forward_iterator_tag, T>
    {
    private:
        QuadTree<T>* node;
        bool done;

        typename std::list<T>::iterator children_iter;
        typename std::list<T>::iterator children_iter_end;

        std::list<QuadTree<T> *> open_list;

        void next_node();

    public:
        DownIterator();
        DownIterator(QuadTree<T>* node);

        virtual ~DownIterator()
        {
        }

        virtual DownIterator& operator=(const DownIterator& other);
        virtual DownIterator& operator++();
        virtual bool operator==(const DownIterator& other) const;
        virtual bool operator!=(const DownIterator& other) const;
        virtual T* operator->() const;
        virtual T operator*() const;
    };


    template <typename T>
    DownIterator<T>::DownIterator() : node(nullptr), done(true)
    {
    }

    template <typename T>
    DownIterator<T>::DownIterator(QuadTree<T>* node) :
        node(nullptr),
        done(false)
    {
        assert(node != nullptr);

        open_list.push_back(node);
        next_node();
    }

    template <typename T>
    void DownIterator<T>::next_node()
    {
        if (node == nullptr)
        {
            if (open_list.size() > 0)
            {
                node = open_list.front();
                open_list.pop_front();

                // add subtree nodes to open list
                for (typename std::vector<QuadTree<T>>::iterator it = node->subtree_nodes.begin();
                     it != node->subtree_nodes.end();
                     ++it)
                    open_list.push_back(&*it);

                // reset iterator for current
                children_iter = node->children.begin();
                children_iter_end = node->children.end();

                if (children_iter == children_iter_end)
                {
                    done = true;
                }
            }
            else
            {
                done = true;
            }
        }
    }

    template <typename T>
    DownIterator<T>& DownIterator<T>::operator++()
    {
        if (!done)
        {
            ++children_iter;
        }

        if (!done && children_iter == children_iter_end)
        {
            node = nullptr;
            next_node();
        }


        return (*this);
    }

    template <typename T>
    DownIterator<T>& DownIterator<T>::operator=(const DownIterator& other)
    {
        node = other.node;
        done = other.done;
        open_list = other.open_list;
        children_iter = other.children_iter;
        children_iter_end = other.children_iter_end;
        return (*this);
    }

    template <typename T>
    bool DownIterator<T>::operator==(const DownIterator& other) const
    {
        if (done == true && other.done == true)
            return true;
        else
            return (node == other.node &&
                children_iter == other.children_iter &&
                open_list == other.open_list &&
                done == other.done);
    }


    template <typename T>
    bool DownIterator<T>::operator!=(const DownIterator& other) const
    {
        return !(*this == other);
    }

    template <typename T>
    T* DownIterator<T>::operator->() const
    {
        return &*children_iter;
    }

    template <typename T>
    T DownIterator<T>::operator*() const
    {
        return *children_iter;
    }
}

#endif
