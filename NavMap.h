/*
 * W - a tiny 2D game development library
 *
 * ============
 *  NavMap.h
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef NavMap_H
#define NavMap_H

#include <iostream>
#include <vector>

#include "W.h"

namespace W {
	class NavNode
	{
	public:
		NavNode();
		
		// Methods
		void addNeighbour(NavNode *);
		void removeNeighbour(NavNode *);
		bool hasNeighbour(NavNode *);
		
		bool operator< (NavNode *);		// For ordering in MisterHeapy
		void setComparand(float);		// For updating by MisterHeapy
		
		// Properties
		int x, y;
		bool passable;
		std::vector<NavNode *> neighbours;		// Pointers to passable neighbour nodes. For pathfinding.
		float min_dist;
		
		NavNode *route_prev;
	};
	
	
	class NavMap
	{
	public:
		NavMap(const W::size &);
		NavMap(int _w, int _h);
		~NavMap();
		
		void makeImpassable(const rect &);	// Unlink all nodes in rect from the node network
		void makePassable(const rect &);

    void isolate(const std::vector<W::position> &); // Unlink across edge nodes, leaving interior navigable
//    void unisolate(const std::vector<W::position> &);

		void createConnection(W::position p1, W::position p2);
		void removeConnection(W::position p1, W::position p2);

		bool isPassableAt(int atX, int atY);
		bool isPassableAt(const W::position &);
		bool isPassableUnder(const W::rect &);
    bool isPassableUnder(const std::vector<W::position> &);
		
		bool getRoute(int fromX, int fromY, int toX, int toY, std::vector<position> &route);
		
		NavNode* _nodeAt(int atX, int atY);
		NavNode* _nodeAt(const W::position &);
		
		int width() { return w; }
		int height() { return h; }
		
	protected:
		// Properties
		int w, h;
		std::vector<NavNode> nodes;		// A w*h-sized array of NavNodes
		MisterHeapy<NavNode*, float> open_nodes;
		
		// Methods
		void _makeImpassable(int atX, int atY);
		void _makePassable(int atX, int atY);
		void initialize();
	};
}

#endif
