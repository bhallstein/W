/*
 * W - a tiny 2D game development library
 *
 * ================
 *  Positioner.h
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef W_Positioner
#define W_Positioner

#include "types.h"
#include <functional>

class LuaObj;

namespace W {
	
	namespace Corner {
		enum T {
			TopLeft, TopRight, BottomLeft, BottomRight
		};
	}
	
	namespace PosType {
		enum T {
			Fixed, Proportional
		};
	}

	
	class Positioner {
	public:
		Positioner(
			Corner::T,
			PosType::T _pos_method_x, PosType::T _pos_method_y,
			PosType::T _sizing_method_x, PosType::T _sizing_method_y,
			float _x, float _y, float _w, float _h,
			bool draggable = false
		);
		Positioner();
		Positioner(LuaObj *);
		iRect refresh(v2f container_size);
		
		void nudge(v2f delta);
		bool isDraggable() { return draggable; }
		
		static const Positioner WholeAreaPositioner;
		static const Positioner LeftHalfPositioner;
		static const Positioner RightHalfPositioner;
		static const Positioner TopHalfPositioner;
		static const Positioner BottomHalfPositioner;
		static const Positioner TopLeftQuarterPositioner;
		static const Positioner TopRightQuarterPositioner;
		static const Positioner BottomLeftQuarterPositioner;
		static const Positioner BottomRightQuarterPositioner;

		Corner::T fixed_corner;
		PosType::T pos_method_x, pos_method_y;
		PosType::T sizing_method_x, sizing_method_y;
		float corner_x, corner_y, w, h;
		bool draggable;

  private:
		iRect _p;
		
	};


  struct NewPositioner {
    typedef std::function<fRect(v2f container_size, NewPositioner p, void *positioning_data)> PosFunc;

    fRect r;
    std::vector<PosFunc> funcs;
    void *positioning_data;

    fRect operator()(v2f container_size) {
      fRect result = r;
      for (auto f : funcs) {
        result = f(container_size, *this, positioning_data);
      }
      return result;
    }

    struct Init;
    static Init *init;
  };

  namespace PositionFuncs {
    struct FromCorner {
      static fRect f(v2f container_size, NewPositioner p, void *pos_data);
      Corner::T corner;
      PosType::T pos_method_x;
      PosType::T pos_method_y;
      PosType::T sizing_method_x;
      PosType::T sizing_method_y;
    };

    struct FromCorner_Fixed {
      static fRect f(v2f container_size, NewPositioner p, void *pos_data);
      Corner::T corner;
    };

    struct VCenter {
      static fRect f(v2f container_size, NewPositioner p, void *pos_data);
    };
  }

}

#endif
