/*
 * W - a tiny 2D game development library
 *
 * ==============
 *  NavMap.cpp
 * ==============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "NavMap.h"
#include "Log.h"
#include <algorithm>
#include <tuple>

#define NAV_INFINITY 99999999

template<class T>
bool contains(const std::vector<T> &v, T p) {
  return std::find(v.begin(), v.end(), p) != v.end();
}

bool is_in_map_bounds(W::v2i p, int w, int h) {
  return p.a >= 0 && p.b >= 0 && p.a < w && p.b < h;
}


W::NavNode::NavNode() : passable(true)
{
	// hai nodey
}

void W::NavNode::addNeighbour(NavNode *n) {
	removeNeighbour(n);
	neighbours.push_back(n);
}
void W::NavNode::removeNeighbour(NavNode *n) {
	for (std::vector<NavNode*>::iterator it = neighbours.begin(); it != neighbours.end(); )
		if (*it == n) it = neighbours.erase(it);
		else it++;
}
bool W::NavNode::hasNeighbour(NavNode *n) {
  return contains(neighbours, n);
}
bool W::NavNode::operator< (NavNode *m) {
	return min_dist > m->min_dist;		// Heap orders larger items first by default. We want the opposite.
}
void W::NavNode::setComparand(float _min_dist) {
	min_dist = _min_dist;
}


W::NavMap::NavMap(v2i _sz) :
	w(_sz.a),
	h(_sz.b),
	open_nodes(_sz.a * _sz.b)
{
	initialize();
}
W::NavMap::NavMap(int _w, int _h) :
	w(_w), h(_h),
	open_nodes(w * h)
{
	initialize();
}
W::NavMap::~NavMap()
{
	// byes
}
void W::NavMap::initialize() {
	int n = w * h;
	nodes.resize(n);
	
	for (int j=0; j < h; j++) {
		for (int i=0; i < w; i++) {
			NavNode &node = nodes[j*w + i];
			node.x = i;							// NavNodes shall know their coordinates
			node.y = j;							//
			
			for (int y=j-1; y <= j+1; y++)		// Setup refs to neighbouring nodes
				for (int x=i-1; x <= i+1; x++)
					if (x == i && y == j) ;							// self is not a neighbour
					else if (x < 0 || y < 0 || x >= w || y >= h) ;	// check map boundaries
					else node.addNeighbour(&nodes[y*w + x]);
		}
	}
}

void W::NavMap::makeImpassable(const iRect &r) {
	for (int i = r.position.a; i < r.position.a + r.size.a; i++)
		for (int j = r.position.b; j < r.position.b + r.size.b; j++)
			if (i < 0 || j < 0 || i >= w || j >= h)
				throw(Exception("NavMap::makeImpassable encountered out-of-bounds coordinate."));
			else
				_makeImpassable(i, j);
}
void W::NavMap::makePassable(const iRect &r) {
	for (int i = r.position.a; i < r.position.a + r.size.a; i++)
		for (int j = r.position.b; j < r.position.b + r.size.b; j++)
			if (i < 0 || j < 0 || i >= w || j >= h)
				throw(Exception("NavMap::makePassable encountered out-of-bounds coordinate."));
			else
				_makePassable(i, j);
}

void W::NavMap::isolate(std::vector<W::v2i> plan) {
  std::vector<NavNode*> edge_nodes;

  // Sever neighbour links & collect edge_nodes
  for (auto p : plan) {
    if (!is_in_map_bounds(p, w, h)) {
      throw(Exception("NavMap::isolate encountered an out-of-bounds coordinate."));
    }

    NavNode *X = _nodeAt(p.a, p.b);
    std::vector<NavNode*> X_neighbours_to_remove;

    for (auto Y : X->neighbours) {
      if (!contains(plan, {Y->x, Y->y})) {
        X_neighbours_to_remove.push_back(Y);
      }
    }

    if (!X_neighbours_to_remove.empty()) {
      edge_nodes.push_back(X);
      for (auto Y : X_neighbours_to_remove) {
        Y->removeNeighbour(X);
        X->removeNeighbour(Y);
      }
    }
  }

  // Sever diagonal links between non-plan neighbours of edge nodes
  const std::vector<std::tuple<W::v2i, W::v2i>> diagonals = {
    { { 0,-1}, { 1, 0} },   // above & right
    { { 1, 0}, { 0, 1} },   // right & below
    { { 0, 1}, {-1, 0} },   // below & left
    { {-1, 0}, { 0,-1} },   // left & above
  };
  for (auto n : edge_nodes) {
    for (auto diag : diagonals) {
      auto p_n1 = std::get<0>(diag);
      auto p_n2 = std::get<1>(diag);
      NavNode *n1 = _nodeAt(n->x + p_n1.a, n->y + p_n1.b);
      NavNode *n2 = _nodeAt(n->x + p_n2.a, n->y + p_n2.b);

      if (!contains(plan, p_n1) && !contains(plan, p_n2)) {
        n1->removeNeighbour(n2);
        n2->removeNeighbour(n1);
      }
    }
  }
}

// TODO
//void W::NavMap::unisolate(const iRect &r) {
//
//}

void W::NavMap::createConnection(v2i p1, v2i p2) {
	NavNode *n1 = _nodeAt(p1);
	NavNode *n2 = _nodeAt(p2);
	n1->addNeighbour(n2);
	n2->addNeighbour(n1);
}
void W::NavMap::removeConnection(v2i p1, v2i p2) {
	NavNode *n1 = _nodeAt(p1);
	NavNode *n2 = _nodeAt(p2);
	n1->removeNeighbour(n2);
	n2->removeNeighbour(n1);
}

bool W::NavMap::isPassableAt(int atX, int atY) {
	return nodes[atY*w + atX].passable;
}
bool W::NavMap::isPassableAt(v2i pos) {
	return isPassableAt(pos.a, pos.b);
}
bool W::NavMap::isPassableUnder(iRect r) {
  int ox = r.position.a;
  int oy = r.position.b;
  int ow = r.size.a;
  int oh = r.size.b;
  for (int i = ox; i < ox + ow; i++) {
    for (int j = oy; j < oy + oh; j++) {
      if (!isPassableAt(i, j)) {
        return false;
      }
    }
  }
	return true;
}
bool W::NavMap::isPassableUnder(std::vector<W::v2i> v) {
  for (auto p : v) {
    if (!isPassableAt(p)) {
      return false;
    }
  }
  return true;
}

bool W::NavMap::getRoute(int fromX, int fromY, int toX, int toY, std::vector<v2i> &route) {
	route.clear();
	
	if (fromX < 0 || fromX >= w || fromY < 0 || fromY >= h || toX < 0 || toX >= w || toY < 0 || toY >= h) {
		W::log << "Navmap asked to find route to or from an out of bounds location.";
		W::log << "(from: " << fromX << "," << fromY << " to: " << toX << "," << toY << ")" << std::endl;
		return false;
	}
	
	// Navigate from A to B
	// Note: pathfinding is actually done backwards: from the destination to the start.
	// This is so we don't have to reverse the route after extracting it, since it arrives in reverse order.
	NavNode &A = nodes[w*toY + toX], &B = nodes[w*fromY + fromX];
	if (!A.passable || !B.passable)
		return false;
	
	/* Initialisation */
	int n = w * h, _i = 0;
	open_nodes.reset();
	for (int i=0; i < n; i++) {
		NavNode *node = &nodes[i];
		node->min_dist = NAV_INFINITY;				// Set nodes' min_dist to infinity
		if (node->passable)
			open_nodes.fast_push(node), _i++;		// Populate heap vector with passable nodes
	}
	A.min_dist = 0;					// Set start node's min_dist to 0
	open_nodes.reheapify();			// Re-sort heap
	
	/* Run */
	NavNode *X, *neighbour;
	float dist_via_X;
	bool route_found = false;
	while (open_nodes.size()) {
		X = open_nodes.pop();		// Pop node with lowest dist off heap
		
		if (X->min_dist == NAV_INFINITY) {
			//cout << "lowest dist node unreachable!" << endl;
			return false;		// No route is possible.
		}
		if (X == &B) {
			// cout << "route found!" << endl;
			route_found = true;
			break;
		}
		
		// Recalc neighbours' min_dists
		//cout << "recalculate neighbours' min_dists:" << endl;
		for (std::vector<NavNode*>::iterator it = X->neighbours.begin(); it != X->neighbours.end(); it++) {
			neighbour = (*it);
			dist_via_X = X->min_dist + ((neighbour->x == X->x || neighbour->y == X->y) ? 1 : 1.41421356);
			//cout << " " << neighbour->x << "," << neighbour->y << ": " << dist_via_X << " vs " << neighbour->min_dist;
			if (dist_via_X < neighbour->min_dist) {
				open_nodes.update(neighbour, dist_via_X);
				neighbour->route_prev = X;	
			}
			//cout << " (" << neighbour->min_dist << ")" << endl;
		}
	}
	if (!route_found) return false;
	
	/* Get route */
	for (X = &B; X != &A; X = X->route_prev) {
		v2i p;
		p.a = X->x, p.b = X->y;
		route.push_back(p);
	}
	route.push_back(v2i(A.x, A.y));
	
	return true;
}

void W::NavMap::_makePassable(int atX, int atY) {
	NavNode &node = nodes[atY*w + atX], *m1, *m2;
	std::vector<NavNode*> diagonalia;
	node.passable = true;
	
	// Add node back to network
	for (int j = atY-1; j <= atY+1; j++)
		for (int i = atX-1; i <= atX+1; i++)
			if (i == atX && j == atY) ;
			else if (i < 0 || i >= w || j < 0 || j >= h) ;
			else {
				nodes[j*w + i].addNeighbour(&node);
				if (i == atX || j == atY) diagonalia.push_back(&nodes[j*w + i]);
			}
	
	// Recreate nodelinks between adjacent diagonals
	for (int a=0; a < diagonalia.size(); a++) {
		m1 = diagonalia[a];
		for (int b=0; b < diagonalia.size(); b++) {
			m2 = diagonalia[b];
			if (m1->x != m2->x || m1->y != m2->y) m1->addNeighbour(m2);
		}
	}
}
void W::NavMap::_makeImpassable(int atX, int atY) {
	NavNode &node = nodes[atY*w + atX], *m1, *m2;
	std::vector<NavNode*> diagonalia;
	node.passable = false;

	// Remove node from network
	for (int j = atY-1; j <= atY+1; j++)
		for (int i = atX-1; i <= atX+1; i++)
			if (i == atX && j == atY) ;
			else if (i < 0 || i >= w || j < 0 || j >= h) ;
			else {
				nodes[j*w + i].removeNeighbour(&node);
				if (i == atX || j == atY) diagonalia.push_back(&nodes[j*w + i]);
			}
	// Sever nodelinks between adjacent diagonals
	for (int a=0; a < diagonalia.size(); a++) {
		m1 = diagonalia[a];
		for (int b=0; b < diagonalia.size(); b++) {
			m2 = diagonalia[b];
			if (m1->x != m2->x || m1->y != m2->y) m1->removeNeighbour(m2);
		}
	}
}
W::NavNode* W::NavMap::_nodeAt(int atX, int atY) {
	return &nodes[atY * w + atX];
}
W::NavNode* W::NavMap::_nodeAt(v2i p) {
	return &nodes[p.b * w + p.a];
}
