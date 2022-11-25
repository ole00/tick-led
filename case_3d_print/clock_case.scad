//############################################################
//         TICK-LED clock case
//############################################################

//Rendering details
details = 60; // [60:Preview-Quick, 360:Export-Slow]

//Export Front case
enableFront = true;

//Export Back case
enableBack = true;

//Enable Antii-scratch bumpers on the front case
enableBumpers = false;

//Overall Case scale - options for print services
caseScale = 1000; // [1000: 100% Exact printer, 996: 99.6% JLCPCB]

//Distance between front and back case
caseDistance = 22; //[22:200]

//Model rotation
caseRotation = -105; // [0:Flat, -105:Stand]

//Clock label
clockLabel = "tick-led"; //["tick-led", "tick-o-mate", "led-o-matick", "clocky", ""]

//Label size
clockLabelSize = 8;

//################################################################
module __Customizer_Limit__ () {} 

//front case wall height
wallH = 14;


//rendering edetails
// 60 / 360
$fn = details;

// rounded edge radius
edgeR = 24;

//wall thicknes
wallT = 2;

//tray Height
trayH = 2;


trayW = 120 - (edgeR * 2);
trayD = 130 - (edgeR * 2);
midCase = trayW / 2;


module outline(wall = 1) {
  difference() {
    offset( 0) children();
    offset(-wall) children();
  }
}

// extruded triangle
module exTriangle(b=1, h=1, w=1) {
    rotate(a=[0,-90,0])
    linear_extrude(height = w, center = true, convexity = 10, twist = 0)
    polygon(points=[[0,0],[h,0],[0,b]], paths=[[0,1,2]]);   
}

module mountingHole(height=6, holeR=1.6) {
    translate([0, holeR/2, 0]) linear_extrude(height = 6) circle(holeR);
    translate([0, 0, 0]) linear_extrude(height = 6) circle(holeR);
    translate([0, -holeR/2, 0]) linear_extrude(height = 6) circle(holeR);
    translate([0, -holeR, 0]) linear_extrude(height = 6) circle(holeR);
    translate([0, -holeR * 3, 0]) linear_extrude(height = 6) circle(holeR * 2);   
}

module bumperDot(size=5) {
    translate([0,0, size/3 * 2])
    difference() {
        sphere(d = size);
        translate([0,0,size/3]) cube(size, center=true);
    }
}

module bumperBar(size=5, length=10) {
    translate([0,0, size/3 * 2]) {
        translate([-length/2, 0, 0]) difference() {
            sphere(d = size);
            translate([0,0,size/3]) cube(size, center=true);
        }
        //translate([0,0, -size/3]) cube([length, size, size/3]);
        translate([-length/2,0,0]) rotate([0,90,0]) linear_extrude(height = length) circle(size/2);

        translate([length/2, 0, 0]) difference() {
            sphere(d = size);
            translate([0,0,size/3]) cube(size, center=true);
        }
    }
}


module caseFront() {
    pcbBracketU = 4.4; //height (Z) of the pcb bracket
    pcbBracket = [0.8, 0.8, 0.8, 0.9]; //light gray color
    glassBracket = [0.5, 0.5, 0.5]; //mid gray color
    rod = [0.8, 0.3, 0.3]; //red
    pcbBrackW = trayW + 2 * edgeR - 1;
    pcbBrackH = 78;
    rodPos = 26.67;
    mountHolePos = 26;
    holeR = 1.5;
    
    //mounting rods
    {
        // rod / bolt from side mounting bracket
        %translate([- rodPos - mountHolePos, - rodPos, 5]) linear_extrude(height = 18) circle(1.4);
        %translate([+ rodPos + mountHolePos, - rodPos, 5]) linear_extrude(height = 18) circle(1.4);
        %translate([- rodPos - mountHolePos, + rodPos, 5]) linear_extrude(height = 18) circle(1.4);
        %translate([+ rodPos + mountHolePos, + rodPos, 5]) linear_extrude(height = 18) circle(1.4);
    }

    //fake pcb bottom (face plate)
    %translate([0, 0, trayH + 2]) linear_extrude(height=1.6) circle(97/2);
    //fake pcb top (electronics)
    %translate([0, 0, trayH + 2 + 1.6 + 3]) linear_extrude(height=1.6) circle(97/2);
    
    //joining brackets
    {
        %translate([-56, -32,trayH + 2 + 1.6 + 3 + 2]) linear_extrude(height = 1.6) offset (1) square([112, 10]);
        %translate([-56, +22,trayH + 2 + 1.6 + 3 + 2]) linear_extrude(height = 1.6) offset (1) square([112, 10]);
    }
    
    difference() {
        translate([-midCase, -midCase, 0]){
        union() {
            // base shape
            translate([0,0,-0.3]) linear_extrude(height = trayH + 0.3) offset(edgeR) square([trayW, trayD]);
            
            //base wall
            translate([0,0,trayH]) {
                linear_extrude(wallH) {
                    outline(wallT) offset(edgeR) square([trayW, trayD]);
                }
            }
            //anti scratch bumpers
            if (enableBumpers) {
                translate([midCase , midCase -56.5, -1.2]) bumperBar(3, 34);
                translate([midCase -42, midCase + 58, -2.6]) bumperDot(8);
                translate([midCase +42, midCase + 58, -2.6]) bumperDot(8);
            }
            
            //glass bracket 
            color(glassBracket) translate([midCase, midCase, trayH]) linear_extrude(height=2) circle(53);
            

            //pcb bracket - main plate
            color(pcbBracket) translate([midCase - pcbBrackW / 2, midCase - pcbBrackH / 2 - 1, trayH + pcbBracketU]) 
                linear_extrude(height = 2) square([pcbBrackW - 0.1, pcbBrackH]);
            
 
            // Pcb bracket legs, also used to fix the hexagonal bolt head
            // and prevent its rotation during screwing.
            c1x = -26;
            c1w = 5.6; //Tolerance of the bolt hexagonal head size, acurate=5.35, tight=5.5
            c1y = -0.4;
            c1s = 3; //size of the tiny cube
            x1a = c1x + (c1w / 2);
            x1b = c1x - (c1w / 2) - c1s;
            color(pcbBracket) translate([midCase - rodPos + x1a, midCase - rodPos - 2.5, trayH - 0.1]) linear_extrude(height = pcbBracketU + 1 ) square([c1s,c1s + 3]);
            color(pcbBracket) translate([midCase - rodPos + x1b, midCase - rodPos - 2.5, trayH - 0.1]) linear_extrude(height = pcbBracketU + 1 ) square([c1s,c1s + 3]);

            c2x = 26;
            x2a = c2x + (c1w / 2);
            x2b = c2x - (c1w / 2) - c1s;
            color(pcbBracket) translate([midCase + rodPos + x2a, midCase - rodPos - 2.5, trayH - 0.1]) linear_extrude(height = pcbBracketU + 1 ) square([c1s,c1s + 3]);
            color(pcbBracket) translate([midCase + rodPos + x2b, midCase - rodPos - 2.5, trayH - 0.1]) linear_extrude(height = pcbBracketU + 1 ) square([c1s,c1s + 3]);

            color(pcbBracket) translate([midCase - rodPos + x1a, midCase + rodPos - 2.5, trayH - 0.1]) linear_extrude(height = pcbBracketU + 1 ) square([c1s,c1s + 3]);
            color(pcbBracket) translate([midCase - rodPos + x1b, midCase + rodPos - 2.5, trayH - 0.1]) linear_extrude(height = pcbBracketU + 1 ) square([c1s,c1s + 3]);

            color(pcbBracket) translate([midCase + rodPos + x2a, midCase + rodPos - 2.5, trayH - 0.1]) linear_extrude(height = pcbBracketU + 1 ) square([c1s,c1s + 3]);
            color(pcbBracket) translate([midCase + rodPos + x2b, midCase + rodPos - 2.5, trayH - 0.1]) linear_extrude(height = pcbBracketU + 1 ) square([c1s,c1s + 3]);

            
            // rod / bolt from PCB
            %translate([midCase - rodPos, midCase - rodPos, 5]) linear_extrude(height = 14) circle(1.5);
            %translate([midCase + rodPos, midCase - rodPos, 5]) linear_extrude(height = 14) circle(1.5);
            %translate([midCase - rodPos, midCase + rodPos, 5]) linear_extrude(height = 14) circle(1.5);
            %translate([midCase + rodPos, midCase + rodPos, 5]) linear_extrude(height = 14) circle(1.5);
        }
        }
        
        //glass hole
        translate([0,0,-0.4]) linear_extrude(height=3.3) circle(50.15);
        
        //glass bracket hole
        translate([0,0,-0.15 + 2 + 1]) linear_extrude(height=2) circle(97 / 2);
        
        // pcb bracket big hole
        translate([0,0,-0.15 + 2 + pcbBracketU]) linear_extrude(height=3) circle(104 / 2);

        // pcb bracket hole cutoff
        pcbBrackCutW = pcbBrackW - 30;
        translate([-pcbBrackCutW / 2, -pcbBrackH / 2 - 1.1,-0.15 + 2 + pcbBracketU])  linear_extrude(height = 3) square([pcbBrackCutW, pcbBrackH + 0.2]);

        // pcb bracket holes   
        //top Right
        translate([- rodPos - mountHolePos, - rodPos, pcbBracketU ]) mountingHole();
        // top Left
        translate([+ rodPos + mountHolePos, - rodPos, pcbBracketU ]) mountingHole();

        //bottom Right
        translate([- rodPos - mountHolePos, + rodPos, pcbBracketU ]) mountingHole();
        //bottom Left
        translate([+ rodPos + mountHolePos, + rodPos, pcbBracketU ]) mountingHole();
        
        rotate([180,0,0]) translate([0,-66 + (14-clockLabelSize)* 0.6, -0.5]) 
            linear_extrude(height=1.5) text(clockLabel, size=clockLabelSize, font="DejaVu Sans Mono:style=Bold", halign="center");
    }
}

module caseBack() {
    mountHolePos = 26;
    rodPos = 26.67;
    wall2H = 6.3;
    cavityH = 6;
    skirtT = 1.8;
    rod = [0.8, 0.3, 0.3]; //red
    skirt1 = [0.3, 0.3, 0.9]; //blue
    skirt2 = [0.3, 0.8, 0.9]; //cyan


    //mounting rods
    {
        // rod / bolt from side mounting bracket
        //%translate([- rodPos - mountHolePos, - rodPos, -wallH]) linear_extrude(height = 14) circle(1.5);
        //%translate([+ rodPos + mountHolePos, - rodPos, -wallH]) linear_extrude(height = 14) circle(1.5);
        //%translate([- rodPos - mountHolePos, + rodPos, -wallH]) linear_extrude(height = 14) circle(1.5);
        //%translate([+ rodPos + mountHolePos, + rodPos, -wallH]) linear_extrude(height = 14) circle(1.5);
    }
    union() {
    
    translate([0,1.5,-7]) rotate([15,0,0]) {
        //MCU mounting pads
        difference() {
            translate([19.5,56,-11]) linear_extrude(height=30) square([8,12]);
            union() {
            //hole
            translate([23.5,62,-7]) rotate([90,0,0]) linear_extrude(height=7) circle(1.2);
            //groove for buzzer pins
            translate([22,55.8,-3]) linear_extrude(height=10) square([3,3]);
            }
        }
        // PCB stopper cube
        translate([19.5,50.2,10])linear_extrude(height=10) square([8,4]);
        translate([26,50.2,10])linear_extrude(height=10) square([4,8]);

        difference() {
            translate([-26.5,56,-11])linear_extrude(height=30) square([8,12]);
            //hole
            translate([-22.5,62,-7]) rotate([90,0,0]) linear_extrude(height=7) circle(1.2);
        }
        // PCB stopper cube
        translate([-26.5,50.2,10])linear_extrude(height=10) square([8,4]);
        translate([-29,50.2,10])linear_extrude(height=10) square([4,8]);
        
        // button shaft lead thin
        translate([-29,46.6,13.4]) rotate([0,90,0])linear_extrude(height=10.5) square([3.4,3.6]);
        // button shaft lead thick
        translate([-29,46.6,20.5]) rotate([0,90,0])linear_extrude(height=10.5) square([4,3.6]);

        //MCU mounting PCB
        %translate([-22.5,54.3,-6])linear_extrude(height=24.2) square([46,1.6]);
        
        //MCU PCB
        %translate([-12,51,-16])linear_extrude(height=34.2) square([25,0.7]);
        
        //button
        %translate([-42,48,15]) {
            rotate([0,90,0]) translate([0,0,6.1]) linear_extrude(height = 24) circle(1.2);
            rotate([0,90,0]) translate([0,0,4.1]) linear_extrude(height = 2) circle(3);
            rotate([0,90,0]) translate([0,0,0]) linear_extrude(height = 4.6) circle(2);
        }

    }
    
    translate([-midCase, -midCase, 0]) {
    difference() {
        union() {
            translate([midCase, midCase, 0]) {
            }


            // base shape
            linear_extrude(height = trayH) offset(edgeR) square([trayW, trayD]);
            
            //base wall
            translate([0,0, -wall2H]) {
                linear_extrude(wall2H) {
                    outline(wallT) offset(edgeR) square([trayW, trayD]);
                }
            }
            //wall skirt small
            color (skirt1) translate([0,0, -wall2H + 0.001]) {
                linear_extrude(2) {
                    outline(wallT) offset(edgeR-0.5) square([trayW-0.5, trayD-0.5]);
                }
            }
            //wall skirt bottom
            color (skirt2) translate([0.25,0.25, -wall2H - 2]) {
                linear_extrude(3.5) {
                    outline(skirtT) offset(edgeR- wallT- 0.1) square([trayW-0.5, trayD-0.5]);
                }
            }

            
            //4 cavity walls - for mounting bolts
            translate([0,0, 0 - cavityH + trayH - trayH]) {
                //left 
                translate([-24, 3, 0]) linear_extrude(height = cavityH + 2) offset(5) square([12, 14]);
                translate([-24, 56, 0]) linear_extrude(height = cavityH + 2) offset(5) square([12, 14]);

                //right
                translate([24 + midCase + edgeR, 3, 0]) linear_extrude(height = cavityH + 2) offset(5) square([12, 14]);
                translate([24 + midCase + edgeR, 56, 0]) linear_extrude(height = cavityH + 2) offset(5) square([12, 14]);
                
            }

            //electronics box walls
            translate([midCase - 35, 51.95, -12.6]) rotate([15,0,0]) {
                linear_extrude(height = 30) offset(5) square([70, 51]);
            }
            //stand stair / extened pedestal
            translate([midCase - 35, 53.80, -19.5]) rotate([15,0,0]) {
                linear_extrude(height = 8) offset(5) square([70, 51]);
            }
        }

        //electronics box cavity
        translate([midCase - 33, 54.5, -15.2]) rotate([15,0,0]) {
            linear_extrude(height = 31) offset(5) square([66, 46]);
        }
        
        //electronic box wall cutter
        translate([-5,midCase +5 , -22.01]) linear_extrude(height=22) square([82,60]);
        
        //USB-C plug window
        translate([midCase - 3, midCase + 44.0, 29]) rotate([195,0,0]) linear_extrude(height = 8) offset(1.2) square([7.4, 2.2]);
        translate([midCase - 5.3, midCase + 44.7, 33.8]) rotate([195,0,0]) linear_extrude(height = 8) offset(1) square([12, 6]);
        
        //Sticker recession - for sticker size 68x22mm
        translate([midCase - 34.5, midCase + 28.5, 29.8]) rotate([195,0,0]) linear_extrude(height = 8) offset(1) square([69, 23]);
        
        //button hole for electronics
        translate([-4.1,80,20]) {
            rotate([0,90,0]) translate([0,0,0]) linear_extrude(height = 3.5) circle(2.1);
        }
        // vent holes / buzzer grille
        translate([73,74,10]) {
            rotate([0,90,0]) translate([0,0,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([0,4,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([0,8,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([0,12,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([0,16,0]) linear_extrude(height = 3.5) circle(1.1);

            rotate([0,90,0]) translate([4,2,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([4,6,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([4,10,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([4,14,0]) linear_extrude(height = 3.5) circle(1.1);

            rotate([0,90,0]) translate([-4,2,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([-4,6,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([-4,10,0]) linear_extrude(height = 3.5) circle(1.1);
            rotate([0,90,0]) translate([-4,14,0]) linear_extrude(height = 3.5) circle(1.1);
        }
        
        translate([0,0, 0.01 - cavityH + trayH]) {
           // cavity + hole
           translate([-24, 5, 0]) linear_extrude(height = cavityH) offset(5) square([10, 10]);
           color(rod) translate([- rodPos - mountHolePos + midCase, midCase - rodPos, -cavityH]) linear_extrude(height = 12) circle(2);

           // cavity + hole
           translate([-24, 58, 0]) linear_extrude(height = cavityH) offset(5) square([10, 10]);
           color(rod) translate([- rodPos - mountHolePos + midCase, midCase + rodPos, -cavityH]) linear_extrude(height = 12) circle(2);

           // cavity + hole
           translate([26 + midCase + edgeR, 5, 0]) linear_extrude(height = cavityH) offset(5) square([10, 10]);
           color(rod) translate([+ rodPos + mountHolePos + midCase, midCase - rodPos, -cavityH]) linear_extrude(height = 12) circle(2);

           // cavity + hole
           translate([26 + midCase + edgeR, 58, 0]) linear_extrude(height = cavityH) offset(5) square([10, 10]);
           color(rod) translate([+ rodPos + mountHolePos + midCase, midCase + rodPos, -cavityH]) linear_extrude(height = 12) circle(2);
        }
        
        // side cutter
        translate([-20 - edgeR -0.001, -40, - wall2H]) linear_extrude(height = wall2H + trayH + 0.1) square([20, 180]);
        translate([36 + edgeR + midCase + 0.001, -40, - wall2H]) linear_extrude(height = wall2H + trayH + 0.1) square([20, 180]);
    } //fdifference
    } //union
    }
}

// allows to fine-tune the overall size before export
worldScale = caseScale / 1000;

// these help to preview / navigate-around the model when in 'Stand' position
worldRotX = caseRotation;
worldShift = caseRotation ? -60 : 0;

// the Main() scene :)
// use '#' to higlight the object, use '%' to disable/detach the object
rotate([worldRotX,0,0]) scale(worldScale) translate([0, worldShift, worldShift]){
    if (enableFront) caseFront();
    if (enableBack) translate([0,0, caseDistance + 0.2]) caseBack();
}
