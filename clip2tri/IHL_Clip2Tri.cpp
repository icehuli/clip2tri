#include "RubyUtils/RubyUtils.h"

#include "../clipper/clipper.hpp"
#include "../poly2tri/poly2tri.h"
#include "../clip2tri/clip2tri.h"
#include <vector>
#include <exception>
using namespace std;
using namespace c2t;


namespace IHL_Clip2Tri
{

	static ID IDx = rb_intern("x");
	static ID IDy = rb_intern("y");
	static ID IDz = rb_intern("z");

	VALUE pt_x(VALUE pt)
	{
		return rb_funcall(pt, IDx, 0);
	}
	VALUE pt_y(VALUE pt)
	{
		return rb_funcall(pt, IDy, 0);
	}
	VALUE pt_z(VALUE pt)
	{
		return rb_funcall(pt, IDz, 0);
	}

	class UnknownExceptionHandler
	{
		static void handler() {
			try {
				throw;
			}
			catch (std::exception& e) {
			}
			catch (...) {
			}

			throw;
		}

	public:
		std::unexpected_handler old;
		UnknownExceptionHandler(std::unexpected_handler nh = handler)
		{
			old = std::set_unexpected(nh);
		}

		~UnknownExceptionHandler()
		{
			std::set_unexpected(old);
		}
	};

	struct Point3d
	{
		double x;
		double y;
		double z;
	};

	VALUE clip2tri_triangulate(VALUE klass, VALUE polygon, VALUE holes) {

		VALUE rb_triangles = Qnil;
		try
		{

			vector<vector<Point>> holePolygons;
			vector<Point> boundingPolygon;
			vector<VALUE> cploypts;

			if (polygon == Qnil)
				return Qnil;

			long npolygon_pts = RARRAY_LEN(polygon);
			if (npolygon_pts < 3)
			{
				return Qnil;
			}

			boundingPolygon.resize(npolygon_pts);
			VALUE* polygon_ptsptr = RARRAY_PTR(polygon);
			for (int j = 0; j < npolygon_pts; j++)
			{
				boundingPolygon[j].x = NUM2DBL(rb_funcall(*polygon_ptsptr, IDx, 0));
				boundingPolygon[j].y = NUM2DBL(rb_funcall(*polygon_ptsptr, IDy, 0));
				polygon_ptsptr++;
			}

			if (npolygon_pts == 3)
			{
				cploypts.resize(3);
				for (int i = 0; i < 3; i++) //loop sub polygons
				{
					VALUE x = GetRubyInterface(boundingPolygon[i].x);
					VALUE y = GetRubyInterface(boundingPolygon[i].y);
					cploypts[i] = rb_ary_new3(2, x, y);
				}

				rb_triangles = rb_ary_new4(cploypts.size(), cploypts.data());
				return rb_triangles;
			}

			if (holes != Qnil)
			{
				long nholes = RARRAY_LEN(holes);
				holePolygons.resize(nholes);

				VALUE* holesptr = RARRAY_PTR(holes);
				for (long i = 0; i < nholes; i++)
				{
					VALUE hole_pts = *holesptr;
					long nhole_pts = RARRAY_LEN(hole_pts);
					holePolygons[i].resize(nhole_pts);
					VALUE* hole_ptsptr = RARRAY_PTR(hole_pts);
					for (int j = 0; j < nhole_pts; j++)
					{
						holePolygons[i][j].x = NUM2DBL(rb_funcall(*hole_ptsptr, IDx, 0));
						holePolygons[i][j].y = NUM2DBL(rb_funcall(*hole_ptsptr, IDy, 0));
						hole_ptsptr++;
					}
					holesptr++;
				}
			}


			vector<Point> outputTriangles;  // Every 3 points is a triangle
			clip2tri clip2tri;
			clip2tri.triangulate(holePolygons, outputTriangles, boundingPolygon);

			cploypts.resize(outputTriangles.size());
			for (int i = 0; i < outputTriangles.size(); i++) //loop sub polygons
			{
				VALUE x = GetRubyInterface(outputTriangles[i].x);
				VALUE y = GetRubyInterface(outputTriangles[i].y);
				cploypts[i] = rb_ary_new3(2, x, y);
			}

			rb_triangles = rb_ary_new4(cploypts.size(), cploypts.data());
		}
		catch (const char* e)
		{
			rb_raise(rb_eException, e);
		}
		catch (...)
		{

		}

		return rb_triangles;
	}
}

// Load this module from Ruby using:
//   require 'IHL_Clip2Tri'
#include <new.h>
extern "C"
void Init_IHL_Clip2Tri()
{
	VALUE mIHL_Clip2Tri = rb_define_module("IHL_Clip2Tri");
	//rb_define_const(mSUEX_HelloWorld, "CEXT_VERSION", GetRubyInterface("1.0.0"));
	rb_define_module_function(mIHL_Clip2Tri, "clip2tri_triangulate",
		VALUEFUNC(IHL_Clip2Tri::clip2tri_triangulate), 2);
}
