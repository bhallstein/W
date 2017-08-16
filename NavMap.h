/*
 * NavMap.hpp - a node network providing pathfinding functionality
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
	
	
	class MappedObj;
	
	class NavMap
	{
	public:
		NavMap(const W::size &);
		NavMap(int _w, int _h);
		~NavMap();
		
		void makeImpassable(MappedObj *);	// Unlink all mappedobj nodes from the node network
		void makePassable(MappedObj *);
		
		void isolate(MappedObj *);			// Unlink only across edge nodes of mappedobj, leaving interior navigable
		void unisolate(MappedObj *);
		
		void createConnection(const W::position &p1, const W::position &p2);
		void removeConnection(const W::position &p1, const W::position &p2);
		
		bool isPassableAt(int atX, int atY);
		bool isPassableUnder(MappedObj *);
		
		bool getRoute(int fromX, int fromY, int toX, int toY, std::vector<position> *route);
		
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
