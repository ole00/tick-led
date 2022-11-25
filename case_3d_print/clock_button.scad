$fn=64;

//button
translate([0,0,0]){
    cylinder(d=3.6, h=4);
    translate([0,0,-2]) cylinder(d=6, h=2);
    translate([0,0,-2-20]) cylinder(d=2.3, h=20);
    translate([0,0,-3.7-20]) cylinder(d=6, h=2.5);
}

//button clip
translate([12,0,0]) {
    length = 11;
    //length = 10 * 8;
    cube([length,9, 2]);
    translate([0,4,2]) cube([length,1.5, 0.6]);
    translate([0,-2,0]) cube([length,2, 6]);   
}
