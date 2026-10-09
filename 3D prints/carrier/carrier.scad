
$fn = 100; 
// color(c = "#440000")  rotate([180, 0, 270]) translate([-40, -20, -1.5]) import("main-board.stl");

// difference() {
//     color(c = "#440000")  rotate([180, 0, 270]) translate([-40, -20, -1.5]) import("main-board.stl");
//     rotate([0, 0, 0]) translate([0, 0, -19.5]) linear_extrude(height = 28) square([20.5, 20.5], center=true);
// }


color(c = "#004400")  rotate([90, 0, 270]) translate([-137.5, 50, -33.5]) import("io-board.stl");

// color(c = "#ffff4444")  rotate([180, 0, 0]) translate([-110, -110, 13]) import("base.stl");

difference() {
    union() {
        for (r = [0, 180])
            rotate([0, 0, r]) translate([25, -12.5, -14]) linear_extrude(height = 14) square([2, 6]);

        // for (r = [0, 180])
        //     rotate([0, 0, r]) translate([25, -12.5, -12]) linear_extrude(height = 8) square([8, 6]);
        translate([0, 0, -8]) linear_extrude(height = 8) square([64, 76], center=true);
        for (r = [0, 180])
            rotate([0, 0, r]) translate([0, 35, -39]) linear_extrude(height = 39) square([64, 6], center=true);
        for (r = [0, 180])
            rotate([0, 0, r]) translate([0, -32.5, -38]) linear_extrude(height = 38) circle(d = 10);
        for (r = [0, 180])
            rotate([0, 0, r]) translate([-8, 20, -12]) linear_extrude(height = 12) square([12, 16]);
    }

    union() {
        translate([0, 0, -9]) linear_extrude(height = 20) square([49.5, 49.5], center=true);
        for (r = [0, 180])
            rotate([0, 0, r]) translate([0, 32.5, -36]) linear_extrude(height = 40) circle(d = 6.5);
        for (r = [0, 180])
            rotate([0, 0, r]) translate([-9.25, 27, -9]) linear_extrude(height = 40) square([31, 6], center=true);
        for (r = [0, 180])
            rotate([0, 0, r]) translate([0, 29, -36]) linear_extrude(height = 24) square([10, 6], center=true);

        for (r = [0, 180])
            rotate([0, 0, r]) translate([-1, 35, -47]) linear_extrude(height = 15) square([17, 20], center=true);

        rotate([0, 0, 0]) translate([-35, 0, -80]) linear_extrude(height = 100) square([10, 80], center=true);
        rotate([0, 0, 0]) translate([37, 0, -80]) linear_extrude(height = 100) square([10, 80], center=true);

        rotate([0, 0, 0]) translate([35, 0, -20]) linear_extrude(height = 14) square([10, 64], center=true);

        rotate([0, 0, 0]) translate([28, 0, -6]) linear_extrude(height = 7) square([10, 43], center=true);
        rotate([0, 0, 0]) translate([-28, 16, -4]) linear_extrude(height = 5) square([10, 10], center=true);
        rotate([0, 0, 0]) translate([-28, -.5, -6]) linear_extrude(height = 7) square([10, 12], center=true);
        rotate([0, 0, 90]) translate([-36, -18.75, -6]) linear_extrude(height = 7) square([35, 17], center=true);
        rotate([0, 0, 270]) translate([-36, 15.75, -6]) linear_extrude(height = 7) square([30, 9], center=true);


        rotate([0, 0, 0]) translate([30, 32.5, -5]) linear_extrude(height = 6) circle(d = 2.6);
        rotate([0, 0, 0]) translate([30, -32.5, -5]) linear_extrude(height = 6) circle(d = 2.6);
        rotate([0, 0, 0]) translate([-27.5, 35.5, -5]) linear_extrude(height = 6) circle(d = 2.6);
        rotate([0, 0, 0]) translate([-27.5, -35.5, -5]) linear_extrude(height = 6) circle(d = 2.6);

        rotate([0, 90, 0]) translate([36.5, 35.5, 27]) linear_extrude(height = 6) circle(d = 1.8);
        rotate([0, 90, 0]) translate([36.5, -35.5, 27]) linear_extrude(height = 6) circle(d = 1.8);
        rotate([0, 90, 0]) translate([2.5, 35.5, 27]) linear_extrude(height = 6) circle(d = 1.8);
        rotate([0, 90, 0]) translate([2.5, -35.5, 27]) linear_extrude(height = 6) circle(d = 1.8);

        for (r = [0, 180])
            rotate([0, 0, r]) translate([1.75, 23.75, -18]) linear_extrude(height = 20) circle(d = 3);
    }

}


// rotate([0, 0, 0]) translate([-8, 20, -16]) linear_extrude(height = 12) square([16, 10]);

