/*
 * Copyright (c) 2007-2008 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
#if !defined(DDG_GRAPH_H)
#define DDG_GRAPH_H

#include "dyn_detail/boost/shared_ptr.hpp"
#include <set>
#include <list>
#include <queue>
#include "Annotatable.h"
#include "Instruction.h"
#include "Node.h"
#include "Absloc.h"
#include "Graph.h"

#include "DepGraphNode.h"

namespace Dyninst {
    class InstructionAPI::Instruction;
    class Edge;
    class Graph;
    class Node;

namespace DepGraphAPI {

    class Absloc;

class DDG : public Graph {
 public:
    typedef dyn_detail::boost::shared_ptr<DDG> Ptr;

    typedef std::map<Address, NodeSet> AddrMap;
    
    typedef std::set<FormalReturnNode::Ptr> FormalReturnNodeSet;
    typedef std::set<FormalParamNode::Ptr> FormalParamNodeSet;

    typedef std::set<ActualParamNode::Ptr> ActualParamNodeSet;
    typedef std::set<ActualReturnNode::Ptr> ActualReturnNodeSet;


    static Ptr createGraph() { return DDG::Ptr(new DDG()); }
    
    virtual ~DDG() {};

    const FormalParamNodeSet &formalParameterNodes() const { return formalParamNodes_; }
    const FormalReturnNodeSet &formalReturnNodes() const { return formalReturnNodes_; }
    
 private:

    DDG() {};

    // Assertion: only parameter nodes will have no in-edges,
    // by definition.
    FormalParamNodeSet formalParamNodes_;

    // Virtual nodes to represent locations defined by the function.
    FormalReturnNodeSet formalReturnNodes_;

    AddrMap callParamNodes_;
    AddrMap callReturnNodes_;
};

};
}
#endif
