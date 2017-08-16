/*
 * W - a tiny 2D game develpoment library
 *
 * ====================
 *  GenericRetro.cpp
 * ====================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "GenericRetro.h"

struct W::_grInit {
	_grInit() {
		initGenericRetro();
	}
};
std::map<char,std::vector<W::rect>> W::GenericRetro;
W::_grInit *W::_grInitializer = new W::_grInit();

void W::initGenericRetro() {
	std::vector<rect> *v;
	
	v = &W::GenericRetro['a'];
	v->push_back(rect(position(0,0),size(10,2)));
	v->push_back(rect(position(0,2),size(2,8)));
	v->push_back(rect(position(2,4),size(6,2)));
	v->push_back(rect(position(8,2),size(2,8)));
	
	v = &W::GenericRetro['b'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,0),size(6,2)));
	v->push_back(rect(position(2,4),size(6,2)));
	v->push_back(rect(position(2,8),size(6,2)));
	v->push_back(rect(position(8,2),size(2,8)));
	
	
	v = &W::GenericRetro['c'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,0),size(7,2)));
	v->push_back(rect(position(2,8),size(7,2)));
	
	v = &W::GenericRetro['d'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(8,2),size(2,8)));
	v->push_back(rect(position(2,0),size(6,2)));
	v->push_back(rect(position(2,8),size(6,2)));
	
	v = &W::GenericRetro['e'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,0),size(7,2)));
	v->push_back(rect(position(2,4),size(5,2)));
	v->push_back(rect(position(2,8),size(7,2)));
	
	v = &W::GenericRetro['f'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,0),size(7,2)));
	v->push_back(rect(position(2,4),size(5,2)));
	
	v = &W::GenericRetro['g'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,0),size(8,2)));
	v->push_back(rect(position(2,8),size(8,2)));
	v->push_back(rect(position(8,4),size(2,4)));
	
	v = &W::GenericRetro['h'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(2,4),size(6,2)));
	
	v = &W::GenericRetro['i'];
	v->push_back(rect(position(2,0),size(2,10)));
	
	v = &W::GenericRetro['j'];
	v->push_back(rect(position(6,9),size(2,10)));
	v->push_back(rect(position(0,8),size(6,2)));
	
	v = &W::GenericRetro['k'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,4),size(5,2)));
	v->push_back(rect(position(7,0),size(2,4)));
	v->push_back(rect(position(7,6),size(2,4)));
	
	v = &W::GenericRetro['l'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,8),size(6,2)));
	
	v = &W::GenericRetro['m'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,2),size(2,2)));
	v->push_back(rect(position(4,4),size(2,2)));
	v->push_back(rect(position(6,2),size(2,2)));
	v->push_back(rect(position(8,0),size(2,10)));
	
	v = &W::GenericRetro['n'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,2),size(2,2)));
	v->push_back(rect(position(4,4),size(2,2)));
	v->push_back(rect(position(6,6),size(2,2)));
	v->push_back(rect(position(8,0),size(2,10)));
	
	v = &W::GenericRetro['o'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(2,0),size(6,2)));
	v->push_back(rect(position(2,8),size(6,2)));
	
	v = &W::GenericRetro['p'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,0),size(6,2)));
	v->push_back(rect(position(2,4),size(6,2)));
	v->push_back(rect(position(8,0),size(2,6)));
	
	v = &W::GenericRetro['q'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(2,0),size(6,2)));
	v->push_back(rect(position(2,8),size(6,2)));
	v->push_back(rect(position(6,10),size(2,2)));
	
	v = &W::GenericRetro['r'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,0),size(6,2)));
	v->push_back(rect(position(2,4),size(6,2)));
	v->push_back(rect(position(8,0),size(2,6)));
	v->push_back(rect(position(6,6),size(2,2)));
	v->push_back(rect(position(8,8),size(2,2)));
	
	v = &W::GenericRetro['s'];
	v->push_back(rect(position(0,0),size(10,2)));
	v->push_back(rect(position(0,4),size(10,2)));
	v->push_back(rect(position(0,8),size(10,2)));
	v->push_back(rect(position(0,2),size(2,2)));
	v->push_back(rect(position(8,6),size(2,2)));
	
	v = &W::GenericRetro['t'];
	v->push_back(rect(position(0,0),size(10,2)));
	v->push_back(rect(position(4,2),size(2,8)));
	                             
	v = &W::GenericRetro['u'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(2,8),size(6,2)));
	
	v = &W::GenericRetro['v'];
	v->push_back(rect(position(0,0),size(2,6)));
	v->push_back(rect(position(2,6),size(2,2)));
	v->push_back(rect(position(4,8),size(2,2)));
	v->push_back(rect(position(6,6),size(2,2)));
	v->push_back(rect(position(8,0),size(2,6)));
	
	v = &W::GenericRetro['w'];
	v->push_back(rect(position(0,0),size(2,8)));
	v->push_back(rect(position(4,0),size(2,8)));
	v->push_back(rect(position(8,0),size(2,8)));
	v->push_back(rect(position(2,8),size(2,2)));
	v->push_back(rect(position(6,8),size(2,2)));
	
	v = &W::GenericRetro['x'];
	v->push_back(rect(position(0,0),size(2,2)));
	v->push_back(rect(position(2,2),size(2,2)));
	v->push_back(rect(position(4,4),size(2,2)));
	v->push_back(rect(position(6,6),size(2,2)));
	v->push_back(rect(position(8,8),size(2,2)));
	v->push_back(rect(position(8,0),size(2,2)));
	v->push_back(rect(position(6,2),size(2,2)));
	v->push_back(rect(position(2,6),size(2,2)));
	v->push_back(rect(position(0,8),size(2,2)));
	
	v = &W::GenericRetro['y'];
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(0,0),size(2,2)));
	v->push_back(rect(position(2,2),size(2,2)));
	v->push_back(rect(position(4,4),size(4,2)));
	
	v = &W::GenericRetro['z'];
	v->push_back(rect(position(0,0),size(10,2)));
	v->push_back(rect(position(0,8),size(10,2)));
	v->push_back(rect(position(6,2),size(2,2)));
	v->push_back(rect(position(4,4),size(2,2)));
	v->push_back(rect(position(2,6),size(2,2)));
	
	v = &W::GenericRetro['0'];
	v->push_back(rect(position(4,4),size(2,2)));
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(2,0),size(6,2)));
	v->push_back(rect(position(2,8),size(6,2)));
	
	v = &W::GenericRetro['1'];
	v->push_back(rect(position(0,0),size(2,2)));
	v->push_back(rect(position(2,0),size(2,10)));
	
	v = &W::GenericRetro['2'];
	v->push_back(rect(position(0,0),size(2,4)));
	v->push_back(rect(position(2,0),size(8,2)));
	v->push_back(rect(position(8,2),size(2,2)));
	v->push_back(rect(position(2,4),size(8,2)));
	v->push_back(rect(position(0,6),size(2,4)));
	v->push_back(rect(position(2,8),size(8,2)));
	
	v = &W::GenericRetro['3'];
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(0,0),size(8,2)));
	v->push_back(rect(position(0,8),size(8,2)));
	v->push_back(rect(position(2,4),size(6,2)));
	
	v = &W::GenericRetro['4'];
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(0,0),size(2,4)));
	v->push_back(rect(position(0,4),size(8,2)));
	                             
	v = &W::GenericRetro['5'];
	v->push_back(rect(position(0,0),size(10,2)));
	v->push_back(rect(position(0,4),size(10,2)));
	v->push_back(rect(position(0,8),size(10,2)));
	v->push_back(rect(position(0,2),size(2,2)));
	v->push_back(rect(position(8,6),size(2,2)));
	
	v = &W::GenericRetro['6'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(2,0),size(8,2)));
	v->push_back(rect(position(2,4),size(8,2)));
	v->push_back(rect(position(2,8),size(8,2)));
	v->push_back(rect(position(8,6),size(2,2)));
	
	v = &W::GenericRetro['7'];
	v->push_back(rect(position(0,0),size(10, 2)));
	v->push_back(rect(position(8,2),size(2, 2 )));
	v->push_back(rect(position(6,4),size(2, 2 )));
	v->push_back(rect(position(4,6),size(2, 2 )));
	v->push_back(rect(position(2,8),size(2, 2 )));
	
	v = &W::GenericRetro['8'];
	v->push_back(rect(position(0,0),size(2,10)));
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(2,0),size(6,2)));
	v->push_back(rect(position(2,4),size(6,2)));
	v->push_back(rect(position(2,8),size(6,2)));
	
	v = &W::GenericRetro['9'];
	v->push_back(rect(position(8,0),size(2,10)));
	v->push_back(rect(position(0,0),size(8,2)));
	v->push_back(rect(position(0,4),size(8,2)));
	v->push_back(rect(position(0,2),size(2,2)));
	v->push_back(rect(position(0,8),size(8,2)));
	
	v = &W::GenericRetro['$']; /* (£, actually) */
//	v->push_back(rect(position(x+0, y+0, 10, 2, ),size()));, 0); /* ($) */
//	v->push_back(rect(position(x+0, y+4, 10, 2, ),size()));, 0);
//	v->push_back(rect(position(x+0, y+8, 10, 2, ),size()));, 0);
//	v->push_back(rect(position(x+0, y+2, 2, 2, ),size()));, 0);
//	v->push_back(rect(position(x+8, y+6, 2, 2, ),size()));, 0);
//	v->push_back(rect(position(x+4, y-1, 2, 12, ),size()));, 0);
	v->push_back(rect(position(4,0),size(6,2)));
	v->push_back(rect(position(2,2),size(2,2)));
	v->push_back(rect(position(0,4),size(8,2)));
	v->push_back(rect(position(2,6),size(2,4)));
	v->push_back(rect(position(0,8),size(10,2)));

	v = &W::GenericRetro[':'];
	v->push_back(rect(position(2,4),size(2,2)));
	v->push_back(rect(position(2,8),size(2,2)));
	                             
	v = &W::GenericRetro['!'];
	v->push_back(rect(position(2,0),size(2, 6)));
	v->push_back(rect(position(2,8),size(2, 2)));
	                            
	v = &W::GenericRetro['.'];
	v->push_back(rect(position(2,8),size(2,2)));
	
	v = &W::GenericRetro['\''];
	v->push_back(rect(position(2,0),size(2,2)));
	v->push_back(rect(position(3,2),size(2,2)));

}
