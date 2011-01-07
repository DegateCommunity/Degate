/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2008, 2009, 2010 by Martin Schobert

  Degate is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  any later version.

  Degate is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with degate. If not, see <http://www.gnu.org/licenses/>.

*/

#include <degate.h>
#include <TangencyCheck.h>

/**
 * Calculate the parameter for a linear function f(x) = m*x + n.
 * @return Returns if the parameter can be calculated.
 */
bool get_line_function_for_wire(degate::Line_shptr l, double * m, double * n) {
  assert(l != NULL);
  assert(m != NULL);
  assert(n != NULL);

  int d_y = l->get_to_y() - l->get_from_y();
  int d_x = l->get_to_x() - l->get_from_x();

  if(abs(d_x) == 0) return false;
  else {
    *m = static_cast<double>(d_y) / static_cast<double>(d_x);
    *n = l->get_from_y() - l->get_from_x() * *m;
    return true;
  }
}


bool degate::check_object_tangency(Circle_shptr o1,
				   Circle_shptr o2) {

  int dx = o1->get_x() - o1->get_x();
  int dy = o2->get_y() - o1->get_y();
  return (sqrt(dx*dx + dy*dy) <= (o1->get_diameter() + o2->get_diameter()) / 2.0);
}

bool degate::check_object_tangency(Line_shptr o1,
				   Line_shptr o2) {
  double m1, n1, m2, n2;
  bool ret1 = get_line_function_for_wire(o1, &m1, &n1);
  bool ret2 = get_line_function_for_wire(o2, &m2, &n2);

  if(ret1 && ret2) {

    double xi = - (n1 - n2) / (m1 - m2);
    double yi = n1 + m1 * xi;

    return( (o1->get_from_x() - xi)*(xi - o1->get_to_x()) >= 0 &&
	    (o2->get_from_x() - xi)*(xi - o2->get_to_x()) >= 0 &&
	    (o1->get_from_y() - yi)*(yi - o1->get_to_y()) >= 0 &&
	    (o2->get_from_y() - yi)*(yi - o2->get_to_y()) >= 0);
  }
  else if(!ret1 && !ret2) {
    return o1->get_from_x() == o2->get_from_x();
  }
  else {
    Line_shptr v = ret1 ? o2 : o1;
    Line_shptr l = ret1 ? o1 : o2;

    BoundingBox const & b = v->get_bounding_box();

    if((l->get_from_x() > b.get_max_x() &&
	l->get_to_x()   > b.get_max_x()) ||
       // no intersection (line is to right of rectangle).

       (l->get_from_x() > b.get_min_x() &&
	l->get_to_x()   > b.get_min_x()) ||
       // no intersection (line is to left of rectangle).

       (l->get_from_y() > b.get_min_y() &&
	l->get_to_y()   > b.get_min_y()) ||
       // no intersection (line is above rectangle).

       (l->get_from_y() > b.get_max_y() &&
	l->get_to_y()   > b.get_max_y())
       //no intersection (line is below rectangle).
       )
      return false;
    else
      return true;

  }

  return false;
}

bool degate::check_object_tangency(Rectangle_shptr o1,
				   Rectangle_shptr o2) {
  return o1->get_bounding_box().intersects(o2->get_bounding_box());
}

bool degate::check_object_tangency(Circle_shptr o1,
				   Line_shptr o2) {
  BoundingBox const & b = o1->get_bounding_box();

  Rectangle_shptr r(new Rectangle(b.get_min_x(), b.get_max_x(),
				  b.get_min_y(), b.get_max_y()));

  return check_object_tangency(o2, r);
}

bool degate::check_object_tangency(Circle_shptr o1,
				   Rectangle_shptr o2) {
  return o1->get_bounding_box().intersects(o2->get_bounding_box());
}

bool degate::check_object_tangency(Line_shptr l,
				   Rectangle_shptr r) {


  // http://stackoverflow.com/questions/99353/how-to-test-if-a-line-segment-intersects-an-axis-aligned-rectange-in-2d

  // Let the segment endpoints be p1=(x1 y1) and p2=(x2 y2).
  // Let the rectangle's corners be (xBL yBL) and (xTR yTR).

  int x1, x2, y1, y2;

  if(l->get_from_x() < l->get_to_x()) {
    x1 = l->get_from_x();
    y1 = l->get_from_y();
    x2 = l->get_to_x();
    y2 = l->get_to_y();
  }
  else {
    x2 = l->get_from_x();
    y2 = l->get_from_y();
    x1 = l->get_to_x();
    y1 = l->get_to_y();
  }

  // F(x y) = (y2-y1)x + (x1-x2)y + (x2*y1-x1*y2)

  int dy = y2 - y1;
  int dx = x1 - x2;
  int i = x2 * y1 - x1 * y2;

  // Calculate F(x,y) for each corner of the rectangle.
  // If any of the values f[i] is 0, the corner is on the line.
  int f1 = dy * r->get_min_x() + dx * r->get_min_y() + i;
  if(f1 == 0) return true;
  int f2 = dy * r->get_min_x() + dx * r->get_max_y() + i;
  if(f2 == 0) return true;
  int f3 = dy * r->get_max_x() + dx * r->get_min_y() + i;
  if(f3 == 0) return true;
  int f4 = dy * r->get_max_x() + dx * r->get_max_y() + i;
  if(f4 == 0) return true;

  /* If all corners are "below" or "above" the line, the
     objects can't intersect. */
  if((f1 < 0 && f2 < 0 && f3 < 0 && f4 < 0) ||
     (f1 > 0 && f2 > 0 && f3 > 0 && f4 > 0)) {
    return false;
  }

  /*
    Project the endpoint onto the x axis, and check if the
    segment's shadow intersects the polygon's shadow. Repeat on the y axis:
  */
  if( (x1 > r->get_max_x() &&
       x2 > r->get_max_x()) &&
      // no intersection (line is to right of rectangle).

      !(x1 > r->get_min_x() &&
	x2 > r->get_min_x()) &&
      // no intersection (line is to left of rectangle).

      !(y1 > r->get_max_y() &&
	y2 > r->get_max_y()) &&
      // no intersection (line is above rectangle).

      !(y1 < r->get_min_y() &&
	y2 < r->get_min_y())
      //no intersection (line is below rectangle).
      ) {
    return false;
  }

  else // there is an intersection
    return true;
}

bool degate::check_object_tangency(PlacedLogicModelObject_shptr o1,
				   PlacedLogicModelObject_shptr o2) {
  if(o1 == NULL || o2 == NULL)
    throw InvalidPointerException("You passed an invalid shared pointer.");

  if(!o1->get_bounding_box().intersects(o2->get_bounding_box()))
    return false;

  Circle_shptr c1, c2;
  Line_shptr l1, l2;
  Rectangle_shptr r1, r2;

  c1 = std::tr1::dynamic_pointer_cast<Circle>(o1);
  l1 = std::tr1::dynamic_pointer_cast<Line>(o1);
  r1 = std::tr1::dynamic_pointer_cast<Rectangle>(o1);
  c2 = std::tr1::dynamic_pointer_cast<Circle>(o2);
  l2 = std::tr1::dynamic_pointer_cast<Line>(o2);
  r2 = std::tr1::dynamic_pointer_cast<Rectangle>(o2);

  if(c1 && c2)
    return check_object_tangency(c1, c2);
  else if(l1 && l2) {

    bool ret= check_object_tangency(l1, l2);
    std::cout << "Check l1/l2: " << ret << std::endl;
    return ret;
  }
  else if(r1 && r2)
    return check_object_tangency(r1, r2);

  else if(c1 && l2)
    return check_object_tangency(c1, l2);
  else if(l1 && c2)
    return check_object_tangency(c2, l1);

  else if(c1 && r2)
    return check_object_tangency(c1, r2);
  else if(r1 && c2)
    return check_object_tangency(c2, r1);


  else if(l1 && r2)
    return check_object_tangency(l1, r2);
  else if(r1 && l2)
    return check_object_tangency(l2, r1);

  assert(1==0);
}
