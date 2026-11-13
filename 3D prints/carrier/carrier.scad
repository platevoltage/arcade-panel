
$fn = 100; 
module right_triangle(a = 10, b = 10) {
    polygon([[0, 0], [a, 0], [0, b]]);   // right angle at origin, legs along +X and +Y
}


// color(c = "#440000")  rotate([180, 0, 270]) translate([-40, -20, -1.5]) import("main-board.stl");
// color(c = "#004400")  rotate([90, 0, 270]) translate([-137.5, 51.5, -33.5]) import("io-board.stl");

// color(c = "#ffff4444")  rotate([180, 0, 0]) translate([-110, -110, 13]) import("base.stl");






color(c = "#aaaaaaee")
difference() {
    union() {
        for (r = [0, 180])
            rotate([0, 0, r]) translate([25, -12.5, -16]) linear_extrude(height = 14) square([2, 6]);

        // for (r = [0, 180])
        //     rotate([0, 0, r]) translate([25, -12.5, -12]) linear_extrude(height = 8) square([8, 6]);

        translate([0, 0, -9]) linear_extrude(height = 9) square([64, 76], center=true); //bottom deck
        for (r = [0, 180])
            rotate([0, 0, r]) translate([0, 35, -39]) linear_extrude(height = 39) square([64, 6], center=true); //walls
        for (r = [0, 180])
            rotate([0, 0, r]) translate([0, -32.5, -38]) linear_extrude(height = 38) circle(d = 10);

        for (r = [0, 180])
            rotate([0, 0, r]) translate([-8, 20, -13]) linear_extrude(height = 12) square([12, 16]); // mount tabs

        rotate([0, 90, 0]) translate([8, -33, -30]) linear_extrude(height = 5) right_triangle(16, 16); //support triangles
        rotate([0, 90, 180]) translate([8, -33, 25]) linear_extrude(height = 5) right_triangle(16, 16); //
    }

    union() {
        translate([0, 0, -9]) linear_extrude(height = 20) square([49.5, 49.5], center=true);
        for (r = [0, 180])
            rotate([0, 0, r]) translate([0, 32.5, -36]) linear_extrude(height = 40) circle(d = 6.5); //big holes
        for (r = [0, 180])
            rotate([0, 0, r]) translate([-9.25, 27, -9]) linear_extrude(height = 40) square([31, 6], center=true);
        for (r = [0, 180])
            rotate([0, 0, r]) translate([0, 29, -36]) linear_extrude(height = 23) square([10, 6], center=true);

        for (r = [0, 180])
            rotate([0, 0, r]) translate([-1, 35, -47]) linear_extrude(height = 15) square([17, 20], center=true); //dog ear gap

        rotate([0, 0, 0]) translate([-35, 0, -80]) linear_extrude(height = 100) square([10, 80], center=true);
        rotate([0, 0, 0]) translate([37, 0, -80]) linear_extrude(height = 100) square([10, 80], center=true);

        rotate([0, 0, 0]) translate([35, 0, -20]) linear_extrude(height = 16) square([10, 64], center=true);

        rotate([0, 0, 0]) translate([28, 0, -3]) linear_extrude(height = 5) square([10, 42], center=true); //interface pins
        rotate([90, 0, 90]) translate([15.75, 1.5, -31]) linear_extrude(height = 8) offset(r = 1.5) offset(r = -1.5) square([9.25, 10], center=true); //USB-C port
        rotate([0, 0, 0]) translate([-28, 1.25, -3]) linear_extrude(height = 5) square([10, 10.5], center=true); //USB header
        rotate([0, 0, 90]) translate([-36, -18.75, -3]) linear_extrude(height = 5) square([35, 16], center=true); //6 pin right stick header
        rotate([0, 0, 270]) translate([-36, 15.5, -3]) linear_extrude(height = 5) square([30, 9], center=true); //3 Pin ring light header

        rotate([0, 0, 270]) translate([-36, -11, -1.5]) linear_extrude(height = 5) square([30, 10], center=true); //3 Pin ring light header


        rotate([0, 0, 0]) translate([30, 32.5, -5]) linear_extrude(height = 6) circle(d = 2.6); //main board screw holes
        rotate([0, 0, 0]) translate([30, -32.5, -5]) linear_extrude(height = 6) circle(d = 2.6);
        rotate([0, 0, 0]) translate([-27.5, 35.5, -5]) linear_extrude(height = 6) circle(d = 2.6);
        rotate([0, 0, 0]) translate([-27.5, -35.5, -5]) linear_extrude(height = 6) circle(d = 2.6);

        rotate([0, 90, 0]) translate([36.5, 35.5, 27]) linear_extrude(height = 6) circle(d = 1.8); //IO board screw holes
        rotate([0, 90, 0]) translate([36.5, -35.5, 27]) linear_extrude(height = 6) circle(d = 1.8);
        rotate([0, 90, 0]) translate([2.5, 35.5, 27]) linear_extrude(height = 6) circle(d = 1.8);
        rotate([0, 90, 0]) translate([2.5, -35.5, 27]) linear_extrude(height = 6) circle(d = 1.8);

        for (r = [0, 180])
            rotate([0, 0, r]) translate([1.75, 23.75, -18]) linear_extrude(height = 20) circle(d = 3); // mounting tab holes
    }

}


// rotate([0, 0, 0]) translate([-8, 20, -16]) linear_extrude(height = 12) square([16, 10]);

