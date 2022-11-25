$fn=64;

difference()
{
  translate([2,2,0.1]) {
      linear_extrude(2) offset(2.4) square([111-4, 9-4]);
      translate([7,2,0]) rotate([45,0,0])  cube([14,3, 3]);
      translate([86,2,0]) rotate([45,0,0])  cube([14,3, 3]);
      translate([34,2,0]) rotate([45,0,0])  cube([39,3, 3]);
  }
  
  translate ([111 / 2, 4.5,0]) {
    translate ([-52.9/2, 0, 0]) cylinder(d=3.3, h=4);
    translate ([52.9/2, 0, 0]) cylinder(d=3.3, h=4);

    translate ([-104/2, 0, 0]) cylinder(d=3.3, h=4);
    translate ([104/2, 0, 0]) cylinder(d=3.3, h=4);

  }
}