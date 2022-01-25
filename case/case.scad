neokey_length = 77;
neokey_width = 22;
neokey_spacing = 5;
neokey_height = neokey_spacing + 1.6;
interior_width = neokey_width;
interior_length = neokey_length;
interior_height = neokey_height;
wall_thickness = 3;

$fa=4;
$fs=0.1;

module rcube(x, y, z, r)
{
    translate([r, r, 0])
    linear_extrude(height = abs(z))
    offset(r)
    square([x - (r * 2), y - (r * 2)]);
}

module neokey_pins()
{
   // hole center spacing: 38.25 x 16.5
   x = 38.25;
   y = 16.5;
   mounting_hole_positions = [[-x/2,-y/2], [x/2,-y/2], [-x/2, y/2], [x/2, y/2]];
   translate([neokey_length/2, neokey_width/2])
   for(t = mounting_hole_positions) {
        translate(t) {
            translate([0, 0, -1]) {
                cylinder(h=neokey_spacing + 1, d=3.5);
                cylinder(h=neokey_height + 1, d=2.5);
            }
        }
   }
}

module neokey_cutout()
{
   difference()
   {
       union() {
             translate([0, 0, 5])
             rcube(neokey_length, neokey_width, 20, 1);
             translate([(neokey_length - 18) / 2, 0, 3])
             rcube(18, neokey_width, 20, 0);
             translate([0, (neokey_width - 18)/2])
             rcube(neokey_length, 18, 20, 1);
       }
   }
}

module trinket_pins()
{
    mounting_hole_positions = [[6, 20.25], [-6, 20.25]];
//    mounting_hole_positions = [[5.625, 4], [-5.625, 4], [6, 20], [-6, 20.25]];

    for(t = mounting_hole_positions) {
        translate(t) {
            translate([0, 0, -1]) {
                cylinder(h=2 + 1, d=2);
            }
        }
    }
}

module trinket_cutout() 
{
    
    difference() {
        union() {
            // main body
            translate([-8, 0])
            linear_extrude(20)
//            square([16, 27]);
            polygon(points = [
                [0, 1.5],
                [1.0, 1.5],
                [2.5, 0],
                [10, 0],
                [11.5, 1.5],
                [16, 1.5],
                [16, 28],
                [0, 28]
            ]);
            // cutouts for solder joints
            translate([5, 6, -2])
            linear_extrude(20)
            square([3, 13.25]);
            translate([-8, 6, -2])
            linear_extrude(20)
            square([3, 13.25]);
        }
        // Lap over the circuit board tab
        translate([-8, 0, 1])
        linear_extrude(20)
        square([16, 1]);
//        trinket_pins();
    }
}

// Trinket cutout in isolation:
if (0)
{
    translate([0, -40, 0])
    trinket_cutout();
}

// Test piece for the trinket cutout:
if (0)
{
    difference() {
        translate([-9, -1, -1])
        linear_extrude(1.95)
        square([18, 28]);
        trinket_cutout();
    }
}

// NeoKey cutout in isolation:
if(0)
{
    translate([wall_thickness, -40, 0])
    neokey_cutout();
}

// Test piece for the NeoKey cutout:
if (0)
{
    difference() {
        translate([-1, -1, -1])
        linear_extrude(neokey_height - 4)
        square([neokey_length + 2, neokey_width + 2]);
        translate([0, 0, -5])
        neokey_cutout();
    }
}

// Test piece for relative fit of both:
if (0)
{
    difference() {
        translate([-1, -1, -1])
        linear_extrude(neokey_height)
        square([neokey_length + 2, neokey_width + 2]);
        translate([0, 0, -1])
        neokey_cutout();
    }
}

module base()
{
    union()
    {
        difference() 
        {
            translate([-wall_thickness, -wall_thickness, -wall_thickness])
            difference()
            {
                // outer body
                rcube(
                    interior_length + (2 * wall_thickness), 
                    interior_width + (2 * wall_thickness),
                    neokey_height + wall_thickness,
                    wall_thickness);

                // lid cut
    //            color("red")
                translate([0, 0, neokey_height + (wall_thickness/2)])
                difference()
                {
                    rcube(
                        interior_length + (wall_thickness * 2) + 1, 
                        interior_width + (wall_thickness * 2) + 1,
                        wall_thickness,
                        wall_thickness);
                
                    translate([wall_thickness / 2, wall_thickness / 2, -wall_thickness])
                    rcube(
                        interior_length + wall_thickness, 
                        interior_width + wall_thickness,
                        wall_thickness * 2,
                        wall_thickness);
                }    
            }
        
        
            // NeoKey cut
            neokey_cutout();
            
            // Trinket cut
            translate([(neokey_length / 2), 0, 0])
            trinket_cutout();
        
            
            // Stemma connector cutouts
            translate([
                -wall_thickness - 1, 
                4 + 0.25, 
                neokey_height - (1.6 + 3) - 0.5])
            cube([5, 8, 6]);

            translate([
                interior_length - 1, 
                4 + 0.25, 
                neokey_height - (1.6 + 3) - 0.5])
            cube([5, 8, 6]);
        }
        
        // NeoKey pins
        neokey_pins();
        
        // Trinket pins
        translate([(neokey_length / 2), 0, 0])
        trinket_pins();
    }
}

module lid()
{
    // This is the height of the key switches, from the mounting board
    // to the underside of the grid.
    key_to_board_height = 5;
    key_spacing = 19;
    key_y = (neokey_width / 2);
    key_x = (neokey_length / 2) - (key_spacing * 1.5);
    key_locations = [
        [key_x + (19 * 0), key_y],
        [key_x + (19 * 1), key_y],
        [key_x + (19 * 2), key_y],
        [key_x + (19 * 3), key_y],
    ];
    holddown_x = key_x - (key_spacing * 0.5);
    board_hold_down_locations = [
        [holddown_x + (19 * 0), key_y],
        [holddown_x + (19 * 1), key_y],
        [holddown_x + (19 * 2), key_y],
        [holddown_x + (19 * 3), key_y],
        [holddown_x + (19 * 4), key_y],
    ];
    
    difference()
    {
        translate([-wall_thickness, -wall_thickness, -wall_thickness])
        difference()
        {
            rcube(
                interior_length + (wall_thickness * 2), 
                interior_width + (wall_thickness * 2),
                key_to_board_height + (wall_thickness/2),
                wall_thickness);
        
            translate([wall_thickness / 2, wall_thickness / 2, key_to_board_height])
            rcube(
                interior_length + wall_thickness, 
                interior_width + wall_thickness,
                10,
                wall_thickness);
    
            translate([wall_thickness, wall_thickness, wall_thickness])
            rcube(
                interior_length, 
                interior_width,
                10,
                wall_thickness);
        }
        
        for(t = key_locations) {
            translate(t) {
                translate([0, 0, -1]) {
                    cube([14, 14, 20], center = true);
                }
            }
        }
    }
    
    for (t = board_hold_down_locations) {
        translate(t) {
            translate([0, 0, 0]) {
                cylinder(h=key_to_board_height - wall_thickness, d=4);
            }
        }
    }
    
    // The amount that the hold-down needs to stick down from the top lip of the lower case
    trinket_stickdown_height = 3;
    
    // The stick-down to hold the trinket in place
    translate([((neokey_length / 2)) - 8, -wall_thickness, -wall_thickness])
    cube([16, wall_thickness, 5 + trinket_stickdown_height]);
}

base();

if (1)
{
    // Separate the lid from the bottom
    translate([0, -(5 + interior_width + wall_thickness * 2), 0])
    
    // Put the lid in place
//    translate([0, neokey_width, neokey_height + 2]) rotate([180, 0, 0])
    
    lid();
}