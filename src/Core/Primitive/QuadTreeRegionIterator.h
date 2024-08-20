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

#ifndef __QUADTREEREGIONITERATOR_H__
#define __QUADTREEREGIONITERATOR_H__

// #define DEBUG_SHOW_ITER 1

#include "Core/Primitive/BoundingBox.h"
#include "Core/Utils/Iterator.h"

#include <list>

namespace degate
{
    template<typename T>
    class QuadTree;

    template<typename T>
    class RegionIterator : public iter::iterator<std::forward_iterator_tag, T>
    {
    private:
        QuadTree<T>* node;
        bool done;

        typename std::list<T>::iterator children_iter;
        typename std::list<T>::iterator children_iter_end;

        std::list<QuadTree<T>*> open_list;

        BoundingBox search_bb;

        void next_node();
        void check_next_node();
        void next_child();
        void skip_non_matching_children();

    public:
        RegionIterator();
        RegionIterator(QuadTree<T>* node, BoundingBox const& bbox);

        virtual ~RegionIterator()
        {
        }

        virtual RegionIterator& operator=(const RegionIterator& other);
        virtual RegionIterator& operator++();
        virtual bool operator==(const RegionIterator& other) const;
        virtual bool operator!=(const RegionIterator& other) const;
        virtual T* operator->() const;
        virtual T operator*() const;
    };


    /**
     * Construct an iterator end.
     */
    template<typename T>
    RegionIterator<T>::RegionIterator() : node(nullptr),
                                          done(true)
    {
    }

    template<typename T>
    RegionIterator<T>::RegionIterator(QuadTree<T>* node, BoundingBox const& bbox)
        : node(nullptr),
          done(false),
          search_bb(bbox)
    {
        assert(node != nullptr);

        open_list.push_back(node);
        next_node();
        check_next_node();
        skip_non_matching_children();
    }

    template<typename T>
    void RegionIterator<T>::next_node()
    {
        assert(node == nullptr);


        if (open_list.empty())
        {
            done = true;
        }
        else
        {
            do
            {
#ifdef DEBUG_SHOW_ITER
                debug(TM, "get head from open list");
#endif
                node = open_list.front();
                open_list.pop_front();

                // add subtree nodes to open list
                for (typename std::vector<QuadTree<T>>::iterator it = node->subtree_nodes.begin();
                     it != node->subtree_nodes.end();
                     ++it)
                {
                    if ((*it).box.intersects(search_bb))
                    {
#ifdef DEBUG_SHOW_ITER
                        debug(TM, "Put into open list: %s", (*it).get_name().c_str());
#endif
                        open_list.push_back(&*it);
                    }
                }
#ifdef DEBUG_SHOW_ITER
                debug(TM, "Current node is %s", node->get_name().c_str());
#endif
                // reset iterator for current quadtree node
                children_iter = node->children.begin();
                children_iter_end = node->children.end();

                // the quadtree might contain empty nodes
            } while (children_iter == children_iter_end && !open_list.empty());

            if (children_iter == children_iter_end && open_list.empty())
            {
                done = true;
                node = nullptr;
            }
        }
    }

    template<typename T>
    void RegionIterator<T>::check_next_node()
    {
        while (!done && children_iter == children_iter_end)
        {
            node = nullptr;
            next_node();
        }
    }

    template<typename T>
    void RegionIterator<T>::next_child()
    {
        if (!done)
        {
            check_next_node();
            ++children_iter;
            check_next_node();
        }
    }

    // if the precond is stay on a bounding-box matching child, then postcond is stay on it, too
    template<typename T>
    void RegionIterator<T>::skip_non_matching_children()
    {
#ifdef DEBUG_SHOW_ITER
        debug(TM, "in increment() done = %d, node = %p", done ? 1 : 0, node);
#endif
        while (!done &&
               !search_bb.intersects(
                       get_bbox_trait_selector<is_pointer<T>::value>::get_bounding_box_for_object(*children_iter)))
        {
#ifdef DEBUG_SHOW_ITER
            debug(TM, "iterating over children in %s", node->get_name().c_str());
#endif
            next_child();
        }
#ifdef DEBUG_SHOW_ITER
        debug(TM, "return from increment()");
#endif
    }


    template<typename T>
    RegionIterator<T>& RegionIterator<T>::operator++()
    {
#ifdef DEBUG_SHOW_ITER
        debug(TM, "++ called");
#endif
        next_child(); // one step ahead
        skip_non_matching_children();
        return (*this);
    }

    template<typename T>
    RegionIterator<T>& RegionIterator<T>::operator=(const RegionIterator& other)
    {
        node = other.node;
        done = other.done;
        open_list = other.open_list;
        children_iter = other.children_iter;
        children_iter_end = other.children_iter_end;
        return (*this);
    }

    template<typename T>
    bool RegionIterator<T>::operator==(const RegionIterator& other) const
    {
        if (done == true && other.done == true)
            return true;
        else
            return (node == other.node && children_iter == other.children_iter && open_list == other.open_list &&
                    done == other.done);
    }


    template<typename T>
    bool RegionIterator<T>::operator!=(const RegionIterator& other) const
    {
        return !(*this == other);
    }

    template<typename T>
    T* RegionIterator<T>::operator->() const
    {
        return &*children_iter;
    }

    template<typename T>
    T RegionIterator<T>::operator*() const
    {
        return *children_iter;
    }
} // namespace degate

#endif
